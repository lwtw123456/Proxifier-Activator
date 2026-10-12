#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <string>

namespace keygen {

namespace detail {

inline constexpr auto crc_table = [] {
    std::array<std::uint32_t, 256> table{};
    for (std::uint32_t i = 0; i < table.size(); ++i) {
        std::uint32_t crc = i << 24;
        for (int bit = 0; bit < 8; ++bit) {
            crc = (crc << 1) ^ ((crc & 0x8000'0000u) ? 0x04C1'1DB7u : 0u);
        }
        table[i] = crc;
    }
    return table;
}();

} // namespace detail

[[nodiscard]] inline auto generate_key() -> std::string {
    using u32 = std::uint32_t;

    thread_local std::mt19937 rng{std::random_device{}()};
    const auto random = [&](u32 lo, u32 hi) {
        return std::uniform_int_distribution<u32>{lo, hi}(rng);
    };

    const u32 w0 = (random(400, 2047) << 5) | random(0, 31);
    const u32 w1 = (1199u << 16) | random(0, 65535);
    const u32 w2 = random(0, (1u << 25) - 1);

    u32 crc = 0xFFFF'FFFFu;
    for (u32 word : std::array{w0, w1, w2}) {
        for (int shift = 0; shift < 32; shift += 8) {
            const u32 byte = (word >> shift) & 0xFFu;
            crc = (crc << 8) ^ detail::crc_table[((crc >> 24) ^ byte) & 0xFFu];
        }
    }

    const u32 d = crc & 0x01FF'FFFFu;
    const u32 mix = d ^ (d << 7);

    constexpr char alphabet[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
    std::string key(29, '-');
    const auto encode = [&](u32 value, std::size_t offset, int width) {
        for (int i = 0; i < width; ++i) {
            const auto j = offset + static_cast<std::size_t>(i);
            key[j + j / 5] = alphabet[(value >> (5 * i)) & 31u];
        }
    };

    encode(w0 ^ mix ^ 0x1234'5678u, 0, 7);
    encode(w1 ^ mix ^ 0x8765'4321u, 7, 7);
    key[16] = key[2];
    encode(w2, 15, 5);
    encode(d, 20, 5);
    return key;
}

} // namespace keygen
