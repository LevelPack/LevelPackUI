#pragma once
#include <Geode/Result.hpp>
#include <functional>

namespace lpui {
    template <class T>
    using SaveCallback = std::function<geode::Result<>(T)>;

    using DeleteCallback = std::function<geode::Result<>()>;
}
