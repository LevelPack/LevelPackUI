#pragma once

namespace lpui {
    template <class T>
    T* finishCreate(T* node, bool initialized) {
        if (!initialized) {
            delete node;
            return nullptr;
        }
        node->autorelease();
        return node;
    }
}
