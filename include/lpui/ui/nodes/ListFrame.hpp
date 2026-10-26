#pragma once
#include <Geode/Geode.hpp>

namespace lpui {
    class ListFrame : public cocos2d::CCNode {
    protected:
        bool init(float height);

    public:
        static constexpr float Width = 358.f;

        static ListFrame* create(float height);
    };
}
