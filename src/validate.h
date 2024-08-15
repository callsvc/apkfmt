#pragma once
#include <filesystem>

namespace apkfmt {
    class Validate {
    public:
        Validate() = default;

        static void doChecksum(std::stringstream& stream, const std::filesystem::path& origin);
    };
}
