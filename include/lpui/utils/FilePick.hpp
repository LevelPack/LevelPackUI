#pragma once
#include <Geode/Geode.hpp>
#include <Geode/utils/file.hpp>
#include <filesystem>
#include <functional>

namespace lpui {
    void pickFile(
        geode::async::TaskHolder<geode::utils::file::PickResult>& holder,
        geode::utils::file::PickMode mode,
        geode::utils::file::FilePickOptions options,
        std::function<void(std::filesystem::path const&)> onPicked
    );
}
