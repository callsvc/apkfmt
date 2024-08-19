#include <fstream>

#include <res/manifest.h>
namespace apkfmt::res {
    Manifest::Manifest(const std::vector<std::filesystem::path>& res) {
        auto readManifest = [&] (const std::filesystem::path& path) {
            std::fstream meta{path, std::ios::in};
            chunk.resize(file_size(path));
            meta.read(&chunk[0], static_cast<std::streamsize>(file_size(path)));

            meta.close();
        };

        for (const auto& solve : res) {
            const auto& sanitizable{solve.string()};
            if (sanitizable.find("Android") != std::string::npos) {
                readManifest(solve);
            }
        }
    }

    void Manifest::decode() {

    }
    void Manifest::save(const std::filesystem::path& output) const {
        std::ofstream meta{output, std::ios::out};
        meta.write(&chunk[0], static_cast<std::streamsize>(chunk.size()));
        meta.close();
    }
}