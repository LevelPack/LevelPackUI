#include <lpui/ui/nodes/NumberStepper.hpp>
#include <lpui/ui/ButtonState.hpp>
#include <lpui/utils/Create.hpp>
#include <algorithm>
#include <cmath>

using namespace geode::prelude;

namespace lpui {
    template <typename T>
    T NumberStepper<T>::clamp(T value) const {
        if (value < m_minValue) return m_minValue;
        if (value > m_maxValue) return m_maxValue;
        return value;
    }

    template <typename T>
    std::string NumberStepper<T>::toString(T value) const {
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
    void NumberStepper<T>::updateArrowButtons() {
        setButtonEnabled(m_leftBtn, m_enabled && m_value > m_minValue);
        setButtonEnabled(m_rightBtn, m_enabled && m_value < m_maxValue);
    }

    template <typename T>
    void NumberStepper<T>::setEnabled(bool enabled) {
        m_enabled = enabled;
        if (!enabled)
            m_input->defocus();
        m_input->setEnabled(enabled);
        updateArrowButtons();
    }

    template <typename T>
    void NumberStepper<T>::setValueInternal(T newValue, bool updateInputText) {
        if constexpr (std::is_floating_point_v<T>) {
            if (!std::isfinite(newValue)) return;
        }

        T old = m_value;
        m_value = clamp(newValue);

        if (updateInputText) {
            m_input->setString(toString(m_value));
        }

        updateArrowButtons();

        if (m_value != old && m_onChange) {
            m_onChange(m_value);
        }
    }

    template <typename T>
    void NumberStepper<T>::textChanged(CCTextInputNode* node) {
        if (!m_enabled) return;
        auto parsed = numFromString<T>(node->getString());
        if (parsed.isOk())
            setValueInternal(parsed.unwrap(), false);
    }

    template <typename T>
    void NumberStepper<T>::textInputClosed(CCTextInputNode*) {
        m_input->setString(toString(m_value));
    }

    template <typename T>
    void NumberStepper<T>::enterPressed(CCTextInputNode*) {
        m_input->defocus();
        m_input->setString(toString(m_value));
    }

    template <typename T>
    void NumberStepper<T>::onArrow(CCObject* sender) {
        if (!m_enabled) return;
        int dir = sender->getTag();
        if constexpr (std::is_integral_v<T>) {
            auto next = static_cast<int64_t>(m_value) + dir * static_cast<int64_t>(m_step);
            setValueInternal(static_cast<T>(std::clamp<int64_t>(next, m_minValue, m_maxValue)), true);
        } else {
            setValueInternal(m_value + static_cast<T>(dir) * m_step, true);
        }
    }

    template <typename T>
    bool NumberStepper<T>::init(T defaultValue, T minValue, T maxValue, Options options) {
        if (minValue > maxValue) return false;
        if (!(options.step > T{0})) return false;
        if constexpr (std::is_floating_point_v<T>) {
            if (!std::isfinite(defaultValue) ||
                !std::isfinite(minValue) ||
                !std::isfinite(maxValue) ||
                !std::isfinite(options.step))
                return false;
        }

        if (!CCMenu::init()) return false;

        const float width = options.width;
        m_minValue = minValue;
        m_maxValue = maxValue;
        m_step = options.step;
        m_onChange = std::move(options.onChange);
        m_value = clamp(defaultValue);

        this->setContentSize({width, 30.f});
        this->setAnchorPoint({.5f, .5f});
        this->ignoreAnchorPointForPosition(false);

        auto leftSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        leftSpr->setFlipX(true);
        leftSpr->setScale(.4f);
        m_leftBtn = CCMenuItemSpriteExtra::create(leftSpr, this, menu_selector(NumberStepper::onArrow));
        m_leftBtn->setTag(-1);
        m_leftBtn->setPosition({12.f, 15.f});
        this->addChild(m_leftBtn);

        auto rightSpr = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
        rightSpr->setScale(.4f);
        m_rightBtn = CCMenuItemSpriteExtra::create(rightSpr, this, menu_selector(NumberStepper::onArrow));
        m_rightBtn->setTag(1);
        m_rightBtn->setPosition({width - 12.f, 15.f});
        this->addChild(m_rightBtn);

        m_input = TextInput::create(width - 44.f, "0");
        m_input->setPosition({width * .5f, 15.f});
        m_input->setCommonFilter(std::is_floating_point_v<T> ? CommonFilter::Float : std::is_signed_v<T> ? CommonFilter::Int : CommonFilter::Uint);
        m_input->setString(toString(m_value));
        m_input->setDelegate(this);
        this->addChild(m_input);

        updateArrowButtons();

        return true;
    }

    template <typename T>
    NumberStepper<T>* NumberStepper<T>::create(T defaultValue, T minValue, T maxValue, Options options) {
        auto ret = new NumberStepper<T>();
        return finishCreate(ret, ret->init(defaultValue, minValue, maxValue, std::move(options)));
    }

    template class NumberStepper<int32_t>;
    template class NumberStepper<uint32_t>;
    template class NumberStepper<float>;
}
