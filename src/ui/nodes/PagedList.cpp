#include <lpui/ui/nodes/PagedList.hpp>
#include <lpui/ui/nodes/ListFrame.hpp>
#include <lpui/utils/Create.hpp>
#include <algorithm>

using namespace geode::prelude;

namespace lpui {
    static int pageCountFor(int total, int itemsPerPage) {
        return (std::max(0, total) + itemsPerPage - 1) / itemsPerPage;
    }

    bool PagedList::init(CCSize size, bool drawFrame, Options options) {
        if (!CCLayer::init()) return false;

        m_listSize = size;
        m_itemHeight = options.itemHeight;
        m_itemsPerPage = std::max(1, options.itemsPerPage);

        m_items = CCArray::create();
        m_items->retain();

        this->ignoreAnchorPointForPosition(false);
        this->setAnchorPoint({0.5f, 0.5f});
        this->setContentSize(m_listSize);

        auto background = CCLayerColor::create({161, 88, 44, 255}, m_listSize.width, m_listSize.height);
        this->addChild(background);

        if (drawFrame) {
            auto frame = ListFrame::create(m_listSize.height);
            frame->setZOrder(1);
            frame->setPosition(m_listSize * 0.5f);
            this->addChild(frame);
        }

        if (!options.emptyText.empty()) {
            m_emptyLabel = CCLabelBMFont::create(options.emptyText.c_str(), "bigFont.fnt");
            m_emptyLabel->setScale(0.5f);
            m_emptyLabel->setOpacity(150);
            m_emptyLabel->setPosition(m_listSize * 0.5f);
            m_emptyLabel->setZOrder(2);
            this->addChild(m_emptyLabel);
        }

        m_pageLabel = CCLabelBMFont::create("", "goldFont.fnt");
        m_pageLabel->setScale(0.5f);
        m_pageLabel->setPosition({m_listSize.width * 0.5f, -12.f});
        m_pageLabel->setZOrder(3);
        this->addChild(m_pageLabel);

        m_spinner = LoadingSpinner::create(40.f);
        m_spinner->setPosition(m_listSize * 0.5f);
        m_spinner->setZOrder(2);
        m_spinner->setVisible(false);
        this->addChild(m_spinner);

        auto buttonsMenu = CCMenu::create();
        buttonsMenu->setPosition({0.f, 0.f});
        this->addChild(buttonsMenu);

        auto rightSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        m_rightArrow = CCMenuItemSpriteExtra::create(rightSprite, this, menu_selector(PagedList::onRight));
        m_rightArrow->setPosition({m_listSize.width + ArrowOffset, m_listSize.height * 0.5f});
        buttonsMenu->addChild(m_rightArrow);

        auto leftSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        leftSprite->setFlipX(true);
        m_leftArrow = CCMenuItemSpriteExtra::create(leftSprite, this, menu_selector(PagedList::onLeft));
        m_leftArrow->setPosition({-ArrowOffset, m_listSize.height * 0.5f});
        buttonsMenu->addChild(m_leftArrow);

        showItems(CCArray::create(), 0);
        return true;
    }

    PagedList* PagedList::create(float height, Options options) {
        auto ret = new PagedList;
        return finishCreate(ret, ret->init({ListFrame::Width, height}, true, std::move(options)));
    }

    PagedList* PagedList::create(CCSize size, Options options) {
        auto ret = new PagedList;
        return finishCreate(ret, ret->init(size, false, std::move(options)));
    }

    PagedList::~PagedList() {
        CC_SAFE_RELEASE(m_items);
    }

    void PagedList::setSource(PagedListSource* source) {
        m_source = source;
        if (m_source) {
            reset();
            return;
        }

        m_pendingRequestId.reset();
        m_total = 0;
        showItems(CCArray::create(), 0);
    }

    void PagedList::setItemsPerPage(int itemsPerPage) {
        itemsPerPage = std::max(1, itemsPerPage);
        if (itemsPerPage == m_itemsPerPage) return;

        int firstVisible = m_page * m_itemsPerPage;
        m_itemsPerPage = itemsPerPage;
        requestPage(firstVisible / m_itemsPerPage);
    }

    void PagedList::setNavigationEnabled(bool enabled) {
        m_navigationEnabled = enabled;
        updateControls();
    }

    void PagedList::reload(bool keepScroll) {
        requestPage(m_page, keepScroll);
    }

    void PagedList::scheduleReload(bool keepScroll) {
        if (m_reloadScheduled) {
            m_scheduledKeepScroll = m_scheduledKeepScroll && keepScroll;
            return;
        }

        m_reloadScheduled = true;
        m_scheduledKeepScroll = keepScroll;
        this->scheduleOnce(schedule_selector(PagedList::onScheduledReload), 0.f);
    }

    void PagedList::onScheduledReload(float) {
        m_reloadScheduled = false;
        reload(m_scheduledKeepScroll);
    }

    void PagedList::reset() {
        m_total.reset();
        requestPage(0);
    }

    bool PagedList::goToPage(int page) {
        if (!m_source) return false;

        page = std::max(0, page);
        if (m_total) page = std::min(page, std::max(0, pageCountFor(*m_total, m_itemsPerPage) - 1));
        if (page == m_page && !isLoading()) return false;

        requestPage(page);
        return true;
    }

    bool PagedList::goToLastPage() {
        auto pages = getPageCount();
        if (!pages || *pages == 0) return false;
        return goToPage(*pages - 1);
    }

    std::optional<int> PagedList::getPageCount() const {
        if (!m_total) return std::nullopt;
        return pageCountFor(*m_total, m_itemsPerPage);
    }

    bool PagedList::hasNextPage() const {
        if (!m_total) return true;
        return m_page + 1 < pageCountFor(*m_total, m_itemsPerPage);
    }

    void PagedList::requestPage(int page, bool keepScroll) {
        if (!m_source) return;

        page = std::max(0, page);
        if (m_total) page = std::min(page, std::max(0, pageCountFor(*m_total, m_itemsPerPage) - 1));

        int id = ++m_lastRequestId;
        m_pendingRequestId = id;
        m_pendingPage = page;
        m_pendingKeepScroll = keepScroll;
        updateControls();

        m_source->loadItems(PageRequest(page * m_itemsPerPage, m_itemsPerPage, this, id));
    }

    void PagedList::handleResponse(int requestId, int first, PageResponse response) {
        if (m_pendingRequestId != requestId) return;
        m_pendingRequestId.reset();

        int page = m_pendingPage;
        auto items = response.items ? response.items : CCArray::create();
        int received = static_cast<int>(items->count());

        if (response.total)
            m_total = std::max(0, *response.total);
        else if (received < m_itemsPerPage)
            m_total = first + received;
        else if (m_total && *m_total < first + received)
            m_total.reset();

        if (m_total) {
            int pages = pageCountFor(*m_total, m_itemsPerPage);
            if (page > 0 && page >= pages) {
                if (pages > 0 && m_page < pages && m_page != page) {
                    updateControls();
                    return;
                }
                if (pages > 0) {
                    requestPage(pages - 1);
                    return;
                }
                page = 0;
            }
        }

        showItems(items, page, m_pendingKeepScroll && page == m_page);
    }

    void PagedList::showItems(CCArray* items, int page, bool keepScroll) {
        std::optional<float> scrolledFromTop;
        if (keepScroll && m_list)
            scrolledFromTop = m_list->m_tableView->m_contentLayer->getPositionY() - m_listTopY;

        items->retain();
        CC_SAFE_RELEASE(m_items);
        m_items = items;
        m_page = page;

        if (m_list) m_list->removeFromParent();

        auto listItems = CCArray::create();
        listItems->addObjectsFromArray(m_items);
        m_list = ListView::create(listItems, m_itemHeight, m_listSize.width, m_listSize.height);
        this->addChild(m_list);

        auto content = m_list->m_tableView->m_contentLayer;
        m_listTopY = content->getPositionY();
        if (scrolledFromTop)
            content->setPositionY(m_listTopY + std::clamp(*scrolledFromTop, 0.f, std::max(0.f, -m_listTopY)));

        updateControls();
    }

    void PagedList::updateControls() {
        const bool loading = isLoading();

        m_leftArrow->setVisible(m_navigationEnabled && !loading && hasPreviousPage());
        m_rightArrow->setVisible(m_navigationEnabled && !loading && hasNextPage());
        m_spinner->setVisible(loading);

        if (m_emptyLabel) m_emptyLabel->setVisible(!loading && m_items->count() == 0);

        auto pages = getPageCount();
        if (loading || (pages && *pages <= 1)) {
            m_pageLabel->setVisible(false);
            return;
        }

        m_pageLabel->setVisible(true);
        if (pages)
            m_pageLabel->setString(fmt::format("Page {} / {}", m_page + 1, *pages).c_str());
        else
            m_pageLabel->setString(fmt::format("Page {}", m_page + 1).c_str());
    }

    void PagedList::onLeft(CCObject*) {
        if (!m_navigationEnabled || isLoading()) return;
        previousPage();
    }

    void PagedList::onRight(CCObject*) {
        if (!m_navigationEnabled || isLoading()) return;
        nextPage();
    }

    void PageRequest::respond(PageResponse response) const {
        if (auto list = m_list.lock())
            list->handleResponse(m_id, first, std::move(response));
    }
}
