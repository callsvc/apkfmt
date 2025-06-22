#pragma once

#include <vector>
#include <filesystem>
namespace apkfmt::res {
    class Manifest {
    public:
        Manifest() = default;
        explicit Manifest(const std::vector<std::filesystem::path>& res);

        void Decode();
        void Export(const std::filesystem::path& output) const;
    private:
        std::vector<char> content;
    };
    inline const std::string manifestalias{"AndroidManifest.xml"};
}