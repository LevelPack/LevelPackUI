#include <lpui/utils/FilePick.hpp>
#include <lpui/ui/Dialogs.hpp>

using namespace geode::prelude;

namespace lpui {
    void pickFile(
        async::TaskHolder<file::PickResult>& holder,
        file::PickMode mode,
        file::FilePickOptions options,
        std::function<void(std::filesystem::path const&)> onPicked
    ) {
        holder.spawn(file::pick(mode, std::move(options)), [onPicked = std::move(onPicked)](file::PickResult result) {
            if (result.isErr()) {
                Dialogs::showError("Cannot pick a file", result.unwrapErr());
                return;
            }
            if (!result.unwrap().has_value())
                return;
            onPicked(result.unwrap().value());
        });
    }
}
