#include <lpui/ui/ButtonState.hpp>

using namespace geode::prelude;

namespace lpui {
    static void enableCascadeColor(CCNode* node) {
        if (auto rgba = typeinfo_cast<CCNodeRGBA*>(node))
            rgba->setCascadeColorEnabled(true);
        if (auto children = node->getChildren()) {
            for (auto child : CCArrayExt<CCNode*>(children))
                enableCascadeColor(child);
        }
    }

    void setButtonEnabled(CCMenuItemSpriteExtra* button, bool enabled) {
        button->setEnabled(enabled);

        auto image = button->getNormalImage();
        if (!image) return;
        enableCascadeColor(image);
        if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(image))
            rgba->setColor(enabled ? ccColor3B{255, 255, 255} : ccColor3B{110, 110, 110});
    }
}
