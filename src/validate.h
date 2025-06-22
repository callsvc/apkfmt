#pragma once
#include <filesystem>

namespace apkfmt {
    class Validate {
    public:
        Validate() = default;

        static void DoChecksum(std::stringstream& stream, const std::filesystem::path& origin);
        static void CollideFiles(const std::filesystem::path& dest, const std::filesystem::path& src);
    };
}
