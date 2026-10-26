#include <lpui/ui/nodes/ListFrame.hpp>
#include <lpui/utils/Create.hpp>

using namespace geode::prelude;

namespace lpui {
    bool ListFrame::init(float height) {
        if (!CCNode::init()) return false;

        auto left = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
        left->setAnchorPoint({1.f, 0.5f});
        left->setPosition({0.f + 9.f, height / 2.f});
        left->setScaleY(height / left->getContentHeight());
        this->addChild(left);

        auto right = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
        right->setAnchorPoint({0.f, 0.5f});
        right->setPosition({Width - 9.f, height / 2.f});
        right->setScaleY(height / right->getContentHeight());
        right->setFlipX(true);
        this->addChild(right);

        auto top = CCSprite::createWithSpriteFrameName("GJ_table_top_001.png");
        top->setAnchorPoint({0.5f, 0.f});
        top->setPosition({Width / 2.f, height - 11.f});
        this->addChild(top);

        auto bottom = CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
        bottom->setAnchorPoint({0.5f, 1.f});
        bottom->setPosition({Width / 2.f, 0.f + 11.f});
        this->addChild(bottom);

        this->setAnchorPoint({0.5f, 0.5f});
        this->setContentSize({Width, height});

        return true;
    }

    ListFrame* ListFrame::create(float height) {
        auto ret = new ListFrame();
        return finishCreate(ret, ret->init(height));
    }
}
