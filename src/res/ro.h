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
        void deobfuscate();
        void treatManifest(const std::filesystem::path& manifest);
        void rollback() const;
    private:
        std::filesystem::path workDir;

        std::vector<std::filesystem::path> content;
        Manifest android;
    };
    inline const std::string groupDir{"fmtres"};
}
