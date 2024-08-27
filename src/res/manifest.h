#pragma once

#include <vector>
#include <filesystem>
namespace apkfmt::res {
    class Manifest {
    public:
        Manifest() = default;
        explicit Manifest(const std::vector<std::filesystem::path>& res);

        void decode();
        void save(const std::filesystem::path& output) const;
    private:
        std::vector<char> chunk;
    };
    inline const std::string manifestAlias{"AndroidManifest.xml"};
}