#pragma once

#include <charconv>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#include "types.hpp"

namespace stdx::details {
template <typename T>
std::expected<T, scan_error> parse_value_with_format(std::string_view input, std::string_view fmt) {
    if (fmt == "d") {
        if constexpr (std::is_integral_v<std::remove_cvref_t<T>> && std::is_signed_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse signed integer"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in integer input"});
            }
            return static_cast<T>(value);
        } else {
            return std::unexpected(scan_error{"Type mismatch: expected signed integer for 'd'"});
        }
    } else if (fmt == "u") {
        if constexpr (std::is_integral_v<std::remove_cvref_t<T>> && std::is_unsigned_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse unsigned integer"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in unsigned integer input"});
            }
            return static_cast<T>(value);
        } else {
            return std::unexpected(scan_error{"Type mismatch: expected unsigned integer for 'u'"});
        }
    } else if (fmt == "f") {
        if constexpr (std::is_floating_point_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse floating point"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in floating point input"});
            }
            return static_cast<T>(value);
        } else {
            return std::unexpected(scan_error{"Type mismatch: expected floating point for 'f'"});
        }
    } else if (fmt == "s") {
        if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
            return std::string(input);
        } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string_view>) {
            return input;
        } else {
            return std::unexpected(scan_error{"Type mismatch: expected string or string_view for 's'"});
        }
    } else if (fmt.empty()) {
        if constexpr (std::is_integral_v<std::remove_cvref_t<T>> && std::is_signed_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse signed integer"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in integer input"});
            }
            return static_cast<T>(value);
        } else if constexpr (std::is_integral_v<std::remove_cvref_t<T>> && std::is_unsigned_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse unsigned integer"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in unsigned integer input"});
            }
            return static_cast<T>(value);
        } else if constexpr (std::is_floating_point_v<std::remove_cvref_t<T>>) {
            std::remove_cvref_t<T> value;
            auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
            if (ec != std::errc{}) {
                return std::unexpected(scan_error{"Failed to parse floating point"});
            }
            if (ptr != input.data() + input.size()) {
                return std::unexpected(scan_error{"Extra characters in floating point input"});
            }
            return static_cast<T>(value);
        } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>) {
            return std::string(input);
        } else if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string_view>) {
            return input;
        } else {
            return std::unexpected(scan_error{"Unsupported type for empty format"});
        }
    } else {
        return std::unexpected(scan_error{"Unknown format specifier"});
    }
}

template <typename... Ts>
std::expected<std::pair<std::vector<std::string_view>, std::vector<std::string_view>>, scan_error>
parse_sources(std::string_view input, std::string_view format) {
    std::vector<std::string_view> format_parts; 
    std::vector<std::string_view> input_parts;
    size_t start = 0;
    while (true) {
        size_t open = format.find('{', start);
        if (open == std::string_view::npos) {
            break;
        }
        size_t close = format.find('}', open);
        if (close == std::string_view::npos) {
            break;
        }

        if (open > start) {
            std::string_view between = format.substr(start, open - start);
            auto pos = input.find(between);
            if (input.size() < between.size() || pos == std::string_view::npos) {
                return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
            }
            if (start != 0) {
                input_parts.emplace_back(input.substr(0, pos));
            }

            input = input.substr(pos + between.size());
        }

        format_parts.push_back(format.substr(open + 1, close - open - 1));
        start = close + 1;
    }

    if (start < format.size()) {
        std::string_view remaining_format = format.substr(start);
        auto pos = input.find(remaining_format);
        if (input.size() < remaining_format.size() || pos == std::string_view::npos) {
            return std::unexpected(scan_error{"Unformatted text in input and format string are different"});
        }
        input_parts.emplace_back(input.substr(0, pos));
        input = input.substr(pos + remaining_format.size());
    } else {
        input_parts.emplace_back(input);
    }
    return std::pair{format_parts, input_parts};
}

}