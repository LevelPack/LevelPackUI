#pragma once
#include <concepts>
#include <type_traits>
#include <utility>

namespace lpui {
    template <class T>
    class Required {
    public:
        template <class U>
            requires (!std::same_as<std::remove_cvref_t<U>, Required>)
                && (!std::is_null_pointer_v<std::remove_cvref_t<U>>)
                && std::constructible_from<T, U&&>
        Required(U&& value) : m_value(std::forward<U>(value)) {}

        T& get() { return m_value; }
        const T& get() const { return m_value; }

        template <class... Args>
        decltype(auto) operator()(Args&&... args) const {
            return m_value(std::forward<Args>(args)...);
        }

    private:
        T m_value;
    };
}
