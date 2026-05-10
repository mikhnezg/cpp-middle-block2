#pragma once

#include "parse.hpp"
#include "types.hpp"

namespace stdx {
template <typename... Ts>
    requires(details::scannable_type<Ts> && ...)
std::expected<details::scan_result<Ts...>, details::scan_error> scan(std::string_view input, std::string_view format) {
    auto parsed = details::parse_sources<Ts...>(input, format);
    if (!parsed) {
        return std::unexpected(parsed.error());
    }
    auto [format_parts, input_parts] = *parsed;
    if (format_parts.size() != sizeof...(Ts)) {
        return std::unexpected(details::scan_error{"Number of placeholders does not match number of types"});
    }
    if (input_parts.size() != sizeof...(Ts)) {
        return std::unexpected(details::scan_error{"Number of input parts does not match number of types"});
    }

    std::tuple<std::expected<Ts, details::scan_error>...> results;
    [&]<size_t... Is>(std::index_sequence<Is...>) {
        ((std::get<Is>(results) = details::parse_value_with_format<std::tuple_element_t<Is, std::tuple<Ts...>>>(
              input_parts[Is], format_parts[Is])),
         ...);
    }(std::index_sequence_for<Ts...>{});

    bool all_ok = std::apply([](auto &&...exps) { return (exps.has_value() && ...); }, results);
    if (!all_ok) {
        details::scan_error err{""};
        std::apply([&err](auto &&...exps) { ((void)(exps.has_value() || (err = exps.error(), false)), ...); }, results);
        return std::unexpected(err);
    }

    auto values_tuple = std::apply([](auto &&...exps) { return std::make_tuple(exps.value()...); }, results);

    return details::scan_result<Ts...>{values_tuple};
}

} 
