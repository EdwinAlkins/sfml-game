#pragma once

#include <SFML/Window/Keyboard.hpp>
#include <fmt/format.h>

// Add formatter specialization for sf::Keyboard::Key
template <>
struct fmt::formatter<sf::Keyboard::Key> : fmt::formatter<int> {
    auto format(sf::Keyboard::Key key, fmt::format_context& ctx) const {
        return fmt::formatter<int>::format(static_cast<int>(key), ctx);
    }
};

template <>
struct fmt::formatter<sf::Time> : fmt::formatter<int> {
    auto format(sf::Time time, fmt::format_context& ctx) const {
        return fmt::formatter<int>::format(time.asMilliseconds(), ctx);
    }
};

template <>
struct fmt::formatter<std::atomic<bool>> : fmt::formatter<bool> {
    auto format(std::atomic<bool> value, fmt::format_context& ctx) const {
        return fmt::formatter<bool>::format(value.load(), ctx);
    }
};
