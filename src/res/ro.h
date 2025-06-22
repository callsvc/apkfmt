#pragma once
#include <filesystem>
#include <vector>

#include <res/manifest.h>
namespace apkfmt::res {
    class Ro {
        public:
        Ro() = default;
        explicit Ro(const std::filesystem::path& droidpath);

        // Groups all resource files into a specific directory
        void GroupResources() const;
        void Deobfuscate();
        void TreatManifest(const std::filesystem::path& manifest);
        void Rollback() const;
    private:
        std::filesystem::path workingdir;

        std::vector<std::filesystem::path> content;
        Manifest android;
    };
    inline const std::string groupdir{"fmtres"};
}
