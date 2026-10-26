#pragma once
#include <Geode/Geode.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

namespace lpui {
    template <typename T>
    class NumberSlider : public cocos2d::CCMenu, public TextInputDelegate {
        static_assert(std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, float>);

    public:
        struct Options {
            std::function<void(T)> onChange;
            float width = 140.f;
            float sliderScale = 0.7f;
        };

    protected:
        geode::TextInput* m_input = nullptr;
        Slider* m_slider = nullptr;

        T m_value{};
        T m_minValue{};
        T m_maxValue{};
        std::function<void(T)> m_onChange;

        T clamp(T value) const;
        std::string toString(T value) const;
        float toRatio(T value) const;
        T fromRatio(float ratio) const;
        void setValueInternal(T newValue, bool updateInput, bool updateSlider);

        void textChanged(CCTextInputNode* node) override;
        void textInputClosed(CCTextInputNode*) override;
        void enterPressed(CCTextInputNode*) override;

        void onSlider(cocos2d::CCObject*);

        bool init(T defaultValue, T minValue, T maxValue, Options options);

    public:
        static NumberSlider* create(T defaultValue, T minValue, T maxValue, Options options);

        T getValue() const { return m_value; }
        void setValue(T value) { setValueInternal(value, true, true); }
    };

    extern template class NumberSlider<int32_t>;
    extern template class NumberSlider<uint32_t>;
    extern template class NumberSlider<float>;
}
