# Utilities

Helpers for node creation, popup callbacks and file picking.

- [Callbacks](#callbacks)
- [finishCreate](#finishcreate)
- [Required](#required)
- [pickFile](#pickfile)

## Callbacks

```cpp
#include <lpui/utils/Callbacks.hpp>

template <class T>
using lpui::SaveCallback = std::function<geode::Result<>(T)>;

using lpui::DeleteCallback = std::function<geode::Result<>()>;
```

`SaveCallback<T>` accepts an edited value and returns a result. `DeleteCallback` takes no arguments and returns a result. The aliases do not manage popup behaviour themselves.

In the pattern below, the popup edits a local value and asks its owner to save or delete it. The owner handles validation and storage. The popup uses [`Dialogs::handleResult`](dialogs.md#handleresult) to show errors and closes only after `Ok`.

Popup handlers (UI setup and the factory are omitted):

```cpp
class EditNamePopup : public Popup {
public:
    struct Options {
        lpui::Required<lpui::SaveCallback<std::string>> onSave;
        lpui::Required<lpui::DeleteCallback> onDelete;
    };

protected:
    Options m_options;
    TextInput* m_input = nullptr;

    EditNamePopup(Options options) : m_options(std::move(options)) {}

    void onSaveBtn(CCObject*) {
        lpui::Dialogs::handleResult(m_options.onSave(m_input->getString()), "Failed to save", [this] {
            this->onClose(nullptr);
        });
    }

    void onDeleteBtn(CCObject*) {
        lpui::Dialogs::askConfirmDelete("item", [this] {
            lpui::Dialogs::handleResult(m_options.onDelete(), "Failed to delete", [this] {
                this->onClose(nullptr);
            });
        });
    }

    // ...
};
```

Owner side:

```cpp
auto popup = EditNamePopup::create(m_item.name, {
    .onSave = [this](std::string name) -> Result<> {
        if (name.empty())
            return Err("name can't be empty");
        m_item.name = std::move(name);
        refreshList();
        return Ok();
    },
    .onDelete = [this]() -> Result<> {
        GEODE_UNWRAP(m_storage.remove(m_item.id));
        refreshList();
        return Ok();
    },
});
popup->show();
```

## finishCreate

```cpp
#include <lpui/utils/Create.hpp>

template <class T>
T* lpui::finishCreate(T* node, bool initialized);
```

Finishes a cocos-style factory: if initialization failed, it deletes the node and returns `nullptr`. Otherwise, it calls `autorelease()` and returns the node.

It replaces the usual cocos boilerplate:

```cpp
static MyNode* create(int value) {
    auto ret = new MyNode();
    if (ret->init(value)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
```

with:

```cpp
static MyNode* create(int value) {
    auto ret = new MyNode();
    return lpui::finishCreate(ret, ret->init(value));
}
```

If a node stores options that cannot be default-constructed, pass them to its constructor, then call `init()` to build the UI. See the `Required` example below.

## Required

```cpp
#include <lpui/utils/Required.hpp>

template <class T>
class lpui::Required;
```

### Options

Most nodes in LevelPack take their settings as an `Options` struct instead of a long list of arguments:

```cpp
class LevelCell : public CCNode {
public:
    struct Options {
        std::string name;
        bool showDelete = true;
        std::function<void()> onEdit;
        std::function<void()> onDelete;
    };

    static LevelCell* create(float width, Options options);
};

auto cell = LevelCell::create(300.f, {
    .name = "Stereo Madness",
    .onEdit = [] { /* ... */ },
});
```

Designated initializers name each supplied option. Fields with defaults can be skipped, but supplied fields must follow declaration order (C++20). Adding an optional field preserves existing calls that use this form; adding a required field may need changes at the call sites.

An omitted `std::function` is empty. That is useful for optional actions, provided the caller checks it before invoking it. Calling an empty function throws `std::bad_function_call`.

### Required

`Required<T>` has no default constructor. An option declared with it must be supplied, and passing the literal `nullptr` is rejected at compile time.

```cpp
struct Options {
    std::string name;
    lpui::Required<std::function<void()>> onEdit;
    lpui::Required<std::function<void()>> onDelete;
};

auto cell = LevelCell::create(300.f, {
    .name = "Stereo Madness",
    .onEdit = [] { /* ... */ },
}); // error: onDelete is missing
```

| Method | Description |
|--------|-------------|
| `T& get()`, `const T& get() const` | the wrapped value |
| `operator()(args...)` | calls the wrapped value, so `m_options.onEdit()` works as with a plain `std::function` |

Notes:

- `Required` checks only that something was passed. An empty `std::function` variable still passes, it's not a runtime check.
- An uninitialized `Required` field prevents default construction of its containing options struct. If a node stores those options as a member, initialize that member in the constructor. Here is the factory pattern; `init(float)` builds the UI:

```cpp
class LevelCell : public CCNode {
public:
    struct Options {
        std::string name;
        lpui::Required<std::function<void()>> onEdit;
        lpui::Required<std::function<void()>> onDelete;
    };

protected:
    bool init(float width);
    Options m_options;

    LevelCell(Options options) : m_options(std::move(options)) {}

public:
    static LevelCell* create(float width, Options options) {
        auto ret = new LevelCell(std::move(options));
        return lpui::finishCreate(ret, ret->init(width));
    }
};
```

- Make required only what the node really can't work without. Optional callbacks stay plain `std::function` and are checked before calling.

## pickFile

```cpp
#include <lpui/utils/FilePick.hpp>

void lpui::pickFile(
    geode::async::TaskHolder<geode::utils::file::PickResult>& holder,
    geode::utils::file::PickMode mode,
    geode::utils::file::FilePickOptions options,
    std::function<void(std::filesystem::path const&)> onPicked
);
```

Opens the system file picker. A successful selection calls `onPicked`; cancellation does nothing. An error shows a `Cannot pick a file` message through `Dialogs::showError`. Supply a nonempty callback and copy the path if you need to keep it after the callback returns.

| Argument   | Description |
|------------|-------------|
| `holder`   | keeps the pick task; store it as a member, so the callback is dropped together with the node |
| `mode`     | `OpenFile`, `SaveFile`, ... |
| `options`  | filters, default path |
| `onPicked` | called with the picked path |

```cpp
lpui::pickFile(m_pickListener, file::PickMode::OpenFile, {.filters = {{"PNG", {"*.png"}}}}, [this](auto const& path) {
    // use path
});
```
