#pragma once
#include <vector>
#include <filesystem>

namespace apkfmt::arsc {
    class Verify {
        public:
        static bool hasResources(const std::vector<std::filesystem::path>& apkDir);
    };
}
