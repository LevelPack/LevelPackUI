#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <asp/fs/Path.hpp>
#include <functional>

namespace lpui {
    void pickFile(
        geode::async::TaskHolder<geode::utils::file::PickResult>& holder,
        geode::utils::file::PickMode mode,
        geode::utils::file::FilePickOptions options,
        std::function<void(asp::fs::Path const&)> onPicked
    );
}
