#pragma once
#include <cstdint>
#include <sstream>

namespace apkfmt {
    template <typename T> requires (std::is_trivially_copyable_v<T>)
    constexpr T readStream(std::stringstream& stream) {
        T magic;
        stream.read(reinterpret_cast<char*>(&magic), sizeof(magic));
        return magic;
    }

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;
}