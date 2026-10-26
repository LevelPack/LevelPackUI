# Button state

- [setButtonEnabled](#setbuttonenabled)

## setButtonEnabled

```cpp
#include <lpui/ui/ButtonState.hpp>

void lpui::setButtonEnabled(CCMenuItemSpriteExtra* button, bool enabled);
```

Disables clicks and dims the button when `enabled` is `false`. Pass `true` to enable it again. `button` must be non-null.

The helper sets the normal image’s tint to `{110, 110, 110}` when disabled and white when enabled. It enables cascading color on the image’s subtree. It doesn't save or restore an earlier tint; images without `CCRGBAProtocol` support are not tinted.

```cpp
auto menu = CCMenu::create();
this->addChild(menu);

auto button = CCMenuItemExt::createSpriteExtra(ButtonSprite::create("click"), [](auto btn) {
    lpui::setButtonEnabled(btn, false);
});
menu->addChild(button);
```

| enabled | disabled |
|:-------:|:--------:|
| ![enabled](../previews/enabled_btn.png) | ![disabled](../previews/disabled_btn.png) |
