#include <lpui/ui/nodes/NumberSlider.hpp>
#include <lpui/utils/Create.hpp>
#include <cmath>

using namespace geode::prelude;

namespace lpui {
    template <typename T>
    T NumberSlider<T>::clamp(T value) const {
        if (value < m_minValue) return m_minValue;
        if (value > m_maxValue) return m_maxValue;
        return value;
    }

    template <typename T>
    std::string NumberSlider<T>::toString(T value) const {
        if constexpr (std::is_integral_v<T>) {
            return std::to_string(value);
        } else {
            std::string str = std::to_string(value);
            while (str.find('.') != std::string::npos && str.back() == '0') str.pop_back();
            if (!str.empty() && str.back() == '.') str.pop_back();
            return str;
        }
    }

    template <typename T>
    float NumberSlider<T>::toRatio(T value) const {
        if (m_maxValue == m_minValue) return 0.f;
        return static_cast<float>(
            (static_cast<double>(value) - static_cast<double>(m_minValue)) /
            (static_cast<double>(m_maxValue) - static_cast<double>(m_minValue))
        );
    }

    template <typename T>
    T NumberSlider<T>::fromRatio(float ratio) const {
        double min = static_cast<double>(m_minValue);
        double max = static_cast<double>(m_maxValue);
        double raw = min + static_cast<double>(ratio) * (max - min);
        if (raw <= min) return m_minValue;
        if (raw >= max) return m_maxValue;
        if constexpr (std::is_integral_v<T>) {
            return clamp(static_cast<T>(std::round(raw)));
        } else {
            return clamp(static_cast<T>(raw));
        }
    }

    template <typename T>
    void NumberSlider<T>::setValueInternal(T newValue, bool updateInput, bool updateSlider) {
        if constexpr (std::is_floating_point_v<T>) {
            if (!std::isfinite(newValue)) return;
        }

        T old = m_value;
        m_value = clamp(newValue);

        if (updateInput) {
            m_input->setString(toString(m_value));
        }
        if (updateSlider) {
            m_slider->setValue(toRatio(m_value));
        }

        if (m_value != old && m_onChange) {
            m_onChange(m_value);
        }
    }

    template <typename T>
    void NumberSlider<T>::textChanged(CCTextInputNode* node) {
        auto parsed = numFromString<T>(node->getString());
        if (parsed.isOk())
            setValueInternal(parsed.unwrap(), false, true);
    }

    template <typename T>
    void NumberSlider<T>::textInputClosed(CCTextInputNode*) {
        m_input->setString(toString(m_value));
    }

    template <typename T>
    void NumberSlider<T>::enterPressed(CCTextInputNode*) {
        m_input->defocus();
        m_input->setString(toString(m_value));
    }

    template <typename T>
    void NumberSlider<T>::onSlider(CCObject*) {
        float ratio = m_slider->getValue();
        setValueInternal(fromRatio(ratio), true, false);
    }

    template <typename T>
    bool NumberSlider<T>::init(T defaultValue, T minValue, T maxValue, Options options) {
        if (!CCMenu::init()) return false;

        if (minValue > maxValue) return false;
        if constexpr (std::is_floating_point_v<T>) {
            if (!std::isfinite(defaultValue) ||
                !std::isfinite(minValue) ||
                !std::isfinite(maxValue))
                return false;
        }


        const float width = options.width;
        m_minValue = minValue;
        m_maxValue = maxValue;
        m_onChange = std::move(options.onChange);
        m_value = clamp(defaultValue);

        this->setContentSize({width, 60.f});
        this->setAnchorPoint({.5f, .5f});
        this->ignoreAnchorPointForPosition(false);

        m_input = TextInput::create(width - 20.f, "0");
        m_input->setPosition({width * .5f, 45.f});
        m_input->setCommonFilter(std::is_floating_point_v<T> ? CommonFilter::Float : std::is_signed_v<T> ? CommonFilter::Int : CommonFilter::Uint);
        m_input->setString(toString(m_value));
        m_input->setDelegate(this);
        this->addChild(m_input);

        m_slider = Slider::create(this, menu_selector(NumberSlider::onSlider), options.sliderScale);
        m_slider->setPosition({width * .5f, 15.f});
        m_slider->setValue(toRatio(m_value));
        this->addChild(m_slider);

        return true;
    }

    template <typename T>
    NumberSlider<T>* NumberSlider<T>::create(T defaultValue, T minValue, T maxValue, Options options) {
        auto ret = new NumberSlider<T>();
        return finishCreate(ret, ret->init(defaultValue, minValue, maxValue, std::move(options)));
    }

    template class NumberSlider<int32_t>;
    template class NumberSlider<uint32_t>;
    template class NumberSlider<float>;
}
