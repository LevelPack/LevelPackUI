#include <lpui/ui/Dialogs.hpp>
#include <Geode/Geode.hpp>
#include <Geode/ui/PopupManager.hpp>

using namespace geode::prelude;

namespace lpui::Dialogs {
    namespace {
        ManagedPopup quickPopup(
            std::string_view title, std::string text, std::string_view firstLabel, std::string_view secondLabel,
            geode::Function<void(FLAlertLayer*, bool)> onChoice, bool cancelledByEscape = true
        ) {
            auto popup = createQuickPopup(
                std::string(title).c_str(), std::move(text), std::string(firstLabel).c_str(), std::string(secondLabel).c_str(),
                std::move(onChoice), false, cancelledByEscape
            );
            return PopupManager::get().manage(popup);
        }
    }

    void showMessage(std::string_view title, std::string const& text, std::optional<float> width) {
        PopupManager::get().alert(std::string(title), text, "OK", nullptr, width.value_or(PopupManager::DEFAULT_WIDTH)).showQueue();
    }

    void showError(std::string_view context, std::string const& err) {
        PopupManager::get().alert("Error", fmt::format("{}: {}", context, err), "OK").showQueue();
    }

    void askSaveChanges(std::function<Result<>()> onSave, std::string_view errorContext, std::function<void()> onClose) {
        quickPopup(
            "Save changes?", "Do you want to save your changes?", "Discard", "Save",
            [onSave = std::move(onSave), errorContext = std::string(errorContext), onClose = std::move(onClose)](FLAlertLayer*, bool save) {
                if (save)
                    handleResult(onSave(), errorContext, onClose);
                else
                    onClose();
            }
        ).showQueue();
    }

    void askConfirm(std::string_view title, std::string_view text, std::string_view confirmLabel, std::function<void()> onConfirm) {
        quickPopup(
            title, std::string(text), "Cancel", confirmLabel,
            [onConfirm = std::move(onConfirm)](FLAlertLayer*, bool confirmed) {
                if (confirmed)
                    onConfirm();
            }
        ).showQueue();
    }

    void askConfirmDelete(std::string_view what, std::function<void()> onConfirm, std::string_view details) {
        auto question = fmt::format("Are you sure you want to delete this {}?", what);
        quickPopup(
            fmt::format("Delete {}", what),
            details.empty() ? question : fmt::format("{}\n{}", details, question),
            "Cancel", "Delete",
            [onConfirm = std::move(onConfirm)](FLAlertLayer*, bool confirmed) {
                if (confirmed)
                    onConfirm();
            }
        ).showQueue();
    }

    void askChoice(
        std::string_view title, std::string_view text, std::string_view firstLabel, std::string_view secondLabel,
        std::function<void(bool secondChosen)> onChoice, bool persistent
    ) {
        auto popup = quickPopup(
            title, std::string(text), firstLabel, secondLabel,
            [onChoice = std::move(onChoice)](FLAlertLayer*, bool secondChosen) {
                onChoice(secondChosen);
            },
            false
        );
        popup.setPersistent(persistent);
        popup.showQueue();
    }

    void handleResult(Result<> res, std::string_view errorContext, std::function<void()> onOk) {
        if (res.isErr()) {
            showError(errorContext, res.unwrapErr());
            return;
        }
        onOk();
    }
}
