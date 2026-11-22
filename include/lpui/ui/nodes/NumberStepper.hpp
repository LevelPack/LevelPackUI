#pragma once
#include <Geode/Geode.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

namespace lpui {
    template <typename T>
    class NumberStepper : public cocos2d::CCMenu {
        static_assert(std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, float>);

    public:
        struct Options {
            T step = 1;
            std::function<void(T)> onChange;
            float width = 100.f;
        };

    protected:
        geode::TextInput* m_input = nullptr;
        CCMenuItemSpriteExtra* m_leftBtn = nullptr;
        CCMenuItemSpriteExtra* m_rightBtn = nullptr;

        T m_value{};
        T m_minValue{};
        T m_maxValue{};
        T m_step{};
        std::function<void(T)> m_onChange;

        T clamp(T value) const;
        std::string toString(T value) const;
        void updateArrowButtons();
        void setValueInternal(T newValue, bool updateInputText);

        void onTextChanged(std::string const& text);
        void onInputFocus(geode::TextInput::FocusState state);

        void onArrow(cocos2d::CCObject* sender);

        bool init(T defaultValue, T minValue, T maxValue, Options options);

    public:
        static NumberStepper* create(T defaultValue, T minValue, T maxValue, Options options);

        T getValue() const { return m_value; }
        void setValue(T value) { setValueInternal(value, true); }
        void setEnabled(bool enabled) override;
    };

    extern template class NumberStepper<int32_t>;
    extern template class NumberStepper<uint32_t>;
    extern template class NumberStepper<float>;
}
