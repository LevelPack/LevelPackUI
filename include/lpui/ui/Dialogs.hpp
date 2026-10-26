#pragma once
#include <Geode/Result.hpp>
#include <functional>
#include <optional>
#include <string>
#include <string_view>

namespace lpui::Dialogs {
    void showMessage(std::string_view title, std::string const& text, std::optional<float> width = std::nullopt);
    void showError(std::string_view context, std::string const& err);
    void askSaveChanges(std::function<geode::Result<>()> onSave, std::string_view errorContext, std::function<void()> onClose);
    void askConfirm(std::string_view title, std::string_view text, std::string_view confirmLabel, std::function<void()> onConfirm);
    void askConfirmDelete(std::string_view what, std::function<void()> onConfirm, std::string_view details = {});
    void askChoice(
        std::string_view title, std::string_view text, std::string_view firstLabel, std::string_view secondLabel,
        std::function<void(bool secondChosen)> onChoice, bool persistent = false
    );
    void handleResult(geode::Result<> res, std::string_view errorContext, std::function<void()> onOk);
}
