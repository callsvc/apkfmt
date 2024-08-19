#pragma once

#include <filesystem>
#include <vector>

#include <res/manifest.h>
namespace apkfmt::res {
    class Ro {
        public:
        Ro() = default;
        explicit Ro(const std::filesystem::path& androidPath);
        // Groups all resource files into a specific directory
        void groupResources();
    private:
        std::filesystem::path workDir;

        std::vector<std::filesystem::path> content;
        Manifest androidManifest;
    };
}
