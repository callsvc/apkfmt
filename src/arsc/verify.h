#pragma once
#include <vector>
#include <filesystem>

namespace apkfmt::arsc {
    class Verify {
        public:
        static bool HasResources(const std::vector<std::filesystem::path>& apkdir);
    };
}
