#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace stdx::details {

struct scan_error {
    std::string message;
};

template <typename T>
concept scannable_type =
    std::is_same_v<std::remove_cvref_t<T>, int8_t> || std::is_same_v<std::remove_cvref_t<T>, int16_t> ||
    std::is_same_v<std::remove_cvref_t<T>, int32_t> || std::is_same_v<std::remove_cvref_t<T>, int64_t> ||
    std::is_same_v<std::remove_cvref_t<T>, uint8_t> || std::is_same_v<std::remove_cvref_t<T>, uint16_t> ||
    std::is_same_v<std::remove_cvref_t<T>, uint32_t> || std::is_same_v<std::remove_cvref_t<T>, uint64_t> ||
    std::is_same_v<std::remove_cvref_t<T>, float> || std::is_same_v<std::remove_cvref_t<T>, double> ||
    std::is_same_v<std::remove_cvref_t<T>, std::string> || std::is_same_v<std::remove_cvref_t<T>, std::string_view>;
template <typename... Ts>
struct scan_result {
    std::tuple<Ts...> values_;

    auto values() const { return values_; }
};

}
