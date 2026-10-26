# Number inputs

Use `NumberStepper` for an input with arrow buttons, or `NumberSlider` for an input with a slider underneath. Both support `int32_t`, `uint32_t` and `float`:

```cpp
auto input = lpui::NumberStepper<int32_t>::create(5, 0, 10, {});
```

Both clamp finite values to the supplied range. Typing updates the stored value as soon as the text parses; incomplete or invalid text leaves the previous value unchanged. Closing the input or pressing Enter replaces the text with the stored value’s display string. `onChange` runs only when the value changes, including changes through `setValue()`. It is not called during creation.

For `float`, the display uses `std::to_string` with trailing zeros removed. On the current toolchain this prints six decimal places before trimming, so small values may display as `0`. Use `getValue()` to read the stored value.

- [NumberStepper](#numberstepper)
- [NumberSlider](#numberslider)
- [Which one to use](#which-one-to-use)

## NumberStepper

```cpp
#include <lpui/ui/nodes/NumberStepper.hpp>

static lpui::NumberStepper<T>* lpui::NumberStepper<T>::create(T defaultValue, T minValue, T maxValue, Options options);
```

The left arrow subtracts `step`; the right arrow adds it. Values stop at the supplied bounds. Content size is `width × 30`, with anchor point `{0.5, 0.5}`.

| Argument       | Description |
|----------------|-------------|
| `defaultValue` | initial value, clamped to `[minValue, maxValue]` |
| `minValue`     | minimum value |
| `maxValue`     | maximum value |
| `options`      | see the table below |

| Option     | Description |
|------------|-------------|
| `step`     | how much one arrow click changes the value, `1` by default |
| `onChange` | called with the new value every time it changes |
| `width`    | node width, `100` by default. The input is `width - 44` wide |

At either bound, the arrow pointing out of the range is disabled with [`setButtonEnabled`](button-state.md).

`create()` returns `nullptr` if `minValue > maxValue` or `step <= 0`. For `float`, the initial value, bounds and step must also be finite. Later attempts to set `NaN` or infinity are ignored. An equal minimum and maximum is allowed; both arrows are disabled.

Choose `width > 44` to leave positive space for the text input.

| Method                 | Description |
|------------------------|-------------|
| `T getValue() const`   | returns the current value |
| `void setValue(T value)` | sets the value (clamped), updates the input and the arrows, calls `onChange` if the value changed |

```cpp
auto stepper = lpui::NumberStepper<int32_t>::create(1, 1, 10, {
    .onChange = [](int32_t value) {
        log::info("value: {}", value);
    }
});
this->addChild(stepper);
```

At `minValue` the left arrow is disabled:

![IntStepper](../previews/intStepperInput.png)

`NumberStepper<float>` with a custom step:

```cpp
auto stepper = lpui::NumberStepper<float>::create(0.5f, 0.f, 1.f, {
    .step = 0.1f,
    .onChange = [](float value) {
        log::info("opacity: {}", value);
    }
});
this->addChild(stepper);
```

![FloatStepper](../previews/floatStepperInput.png)

## NumberSlider

```cpp
#include <lpui/ui/nodes/NumberSlider.hpp>

static lpui::NumberSlider<T>* lpui::NumberSlider<T>::create(T defaultValue, T minValue, T maxValue, Options options);
```

The text input and slider control the same stored value. Content size is `width × 60`, with anchor point `{0.5, 0.5}`.

| Argument       | Description |
|----------------|-------------|
| `defaultValue` | initial value, clamped to `[minValue, maxValue]` |
| `minValue`     | minimum value, slider's left end |
| `maxValue`     | maximum value, slider's right end |
| `options`      | see the table below |

| Option        | Description |
|---------------|-------------|
| `onChange`    | called with the new value every time it changes |
| `width`       | node width, `140` by default. The input is `width - 20` wide |
| `sliderScale` | slider scale, `0.7` by default |

Typing a valid number updates the slider immediately. Moving the slider updates the input; for integer types, the calculated value is rounded to the nearest integer. The thumb itself moves continuously.

`create()` returns `nullptr` for `minValue > maxValue`. For `float`, the initial value and bounds must be finite, and later attempts to set `NaN` or infinity are ignored. Equal bounds are allowed and give a fixed value.

Choose `width > 20` and a positive `sliderScale`. The scale controls the slider’s size independently of the input width.

| Method                 | Description |
|------------------------|-------------|
| `T getValue() const`   | returns the current value |
| `void setValue(T value)` | sets the value (clamped), updates the input and the slider, calls `onChange` if the value changed |

```cpp
auto slider = lpui::NumberSlider<int32_t>::create(50, 0, 100, {
    .onChange = [](int32_t value) {
        log::info("value: {}", value);
    }
});
this->addChild(slider);
```

![IntSlider](../previews/intSliderInput.png)

`NumberSlider<float>` with custom width:

```cpp
auto slider = lpui::NumberSlider<float>::create(1.35f, 0.f, 2.f, {
    .onChange = [](float value) {
        log::info("speed: {}", value);
    },
    .width = 200.f
});
this->addChild(slider);
```

![FloatSlider](../previews/longFloatSliderInput.png)

## Which one to use

| | NumberStepper | NumberSlider |
|-|---------------|--------------|
| size | `width x 30`, compact | `width x 60`, twice as tall |
| good for | small ranges and exact values: stars, coins, counts | wide ranges where the rough position matters: scale, speed, anchors |
| has `step` | yes | no, the slider is continuous (rounded for integer types) |
