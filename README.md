# LevelPackUI

LevelPackUI is a static library of Geode UI components and helpers used by the LevelPack modules. Its public API lives in the `lpui` namespace.

The examples use `geode::prelude` names. Include each component’s header before using it.

| Component | Header | Description |
|-----------|--------|-------------|
| [Dialogs](docs/dialogs.md) | `lpui/ui/Dialogs.hpp` | Messages, confirmations, choices and error handling dialogs |
| [setButtonEnabled](docs/button-state.md) | `lpui/ui/ButtonState.hpp` | Disable a button and dim its image |
| [NumberStepper, NumberSlider](docs/number-inputs.md) | `lpui/ui/nodes/NumberStepper.hpp`, `lpui/ui/nodes/NumberSlider.hpp` | Numeric inputs with arrow buttons or a slider |
| [ListFrame](docs/lists.md#listframe), [PagedList](docs/lists.md#pagedlist) | `lpui/ui/nodes/ListFrame.hpp`, `lpui/ui/nodes/PagedList.hpp` | A GD list frame and a list that requests one page at a time |
| [SaveCallback, finishCreate, Required, pickFile](docs/utils.md) | `lpui/utils/Callbacks.hpp`, `lpui/utils/Create.hpp`, `lpui/utils/Required.hpp`, `lpui/utils/FilePick.hpp` | Callbacks, node creation, required options and file picking |

## Connecting the library

Add LevelPackUI after your project has created the `geode-sdk` target, then link it to your mod:

```cmake
add_subdirectory(path/to/LevelPackUI)
target_link_libraries(MyMod PRIVATE LevelPackUI)
```

The public include directory and Geode dependency are propagated by the target. LevelPackUI uses the parent project’s C++ standard setting; the LevelPack suite builds it with C++23. It also builds `LevelPackUIHeaderCheck`, which compiles each public header in isolation.

Use these components on the main thread, with GD’s standard sprite frames and fonts loaded. Node factories return autoreleased pointers; add the node to a parent or retain it if you need to keep it.
