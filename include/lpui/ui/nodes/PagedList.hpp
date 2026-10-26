#pragma once
#include <Geode/Geode.hpp>
#include <optional>
#include <string>

namespace lpui {
    class ListFrame;
    class PagedListSource;

    struct PageResponse {
        cocos2d::CCArray* items = nullptr;
        std::optional<int> total;
    };

    class PagedList : public cocos2d::CCLayer {
    public:
        struct Options {
            std::string emptyText;
            int itemsPerPage = 10;
            float itemHeight = 40.f;
        };

    protected:
        PagedListSource* m_source = nullptr;

        cocos2d::CCArray* m_items = nullptr;
        geode::ListView* m_list = nullptr;
        float m_listTopY = 0.f;
        cocos2d::CCLabelBMFont* m_emptyLabel = nullptr;
        cocos2d::CCLabelBMFont* m_pageLabel = nullptr;
        geode::LoadingSpinner* m_spinner = nullptr;

        CCMenuItemSpriteExtra* m_leftArrow = nullptr;
        CCMenuItemSpriteExtra* m_rightArrow = nullptr;

        int m_page = 0;
        int m_itemsPerPage = 10;
        std::optional<int> m_total = 0;
        float m_itemHeight = 40.f;
        cocos2d::CCSize m_listSize;
        bool m_navigationEnabled = true;

        int m_lastRequestId = 0;
        std::optional<int> m_pendingRequestId;
        int m_pendingPage = 0;
        bool m_pendingKeepScroll = false;

        bool m_reloadScheduled = false;
        bool m_scheduledKeepScroll = false;

        static constexpr float ArrowOffset = 36.f;

        bool init(cocos2d::CCSize size, bool drawFrame, Options options);

        void requestPage(int page, bool keepScroll = false);
        void handleResponse(int requestId, int first, PageResponse response);
        void showItems(cocos2d::CCArray* items, int page, bool keepScroll = false);
        void updateControls();

        void onScheduledReload(float);
        void onLeft(cocos2d::CCObject*);
        void onRight(cocos2d::CCObject*);

        friend class PageRequest;

    public:
        static PagedList* create(float height, Options options);
        static PagedList* create(cocos2d::CCSize size, Options options);
        ~PagedList() override;

        void setSource(PagedListSource* source);
        void setItemsPerPage(int itemsPerPage);
        void setNavigationEnabled(bool enabled);

        void reload(bool keepScroll = false);
        void scheduleReload(bool keepScroll = false);
        void reset();
        bool goToPage(int page);
        bool goToLastPage();
        bool nextPage() { return goToPage(m_page + 1); }
        bool previousPage() { return goToPage(m_page - 1); }

        cocos2d::CCArray* getItems() const { return m_items; }
        int getCurrentPage() const { return m_page; }
        int getItemsPerPage() const { return m_itemsPerPage; }
        std::optional<int> getTotal() const { return m_total; }
        std::optional<int> getPageCount() const;
        bool hasNextPage() const;
        bool hasPreviousPage() const { return m_page > 0; }
        bool isLoading() const { return m_pendingRequestId.has_value(); }
    };

    class PageRequest {
    public:
        int first;
        int count;

        void respond(PageResponse response) const;

    private:
        geode::WeakRef<PagedList> m_list;
        int m_id;

        PageRequest(int first, int count, PagedList* list, int id)
            : first(first), count(count), m_list(list), m_id(id) {}

        friend class PagedList;
    };

    class PagedListSource {
    public:
        virtual ~PagedListSource() = default;
        virtual void loadItems(PageRequest request) = 0;
    };
}
