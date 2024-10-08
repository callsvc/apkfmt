#include <fstream>

#include <res/manifest.h>
#include <xml/decoder.h>
namespace apkfmt::res {
    Manifest::Manifest(const std::vector<std::filesystem::path>& res) {
        auto readManifest = [&] (const std::filesystem::path& path) {
            std::fstream meta{path, std::ios::in};
            content.resize(file_size(path));
            meta.read(&content[0], static_cast<std::streamsize>(file_size(path)));

            meta.close();
        };

        for (const auto& solve : res) {
            const auto& sanitizable{solve.string()};
            if (sanitizable.find("Android") == std::string::npos)
                continue;
            readManifest(solve);
            break;
        }
    }

    void Manifest::decode() {
        xml::Decoder binaryDealer{content};
        std::stringstream xml;
        binaryDealer.reconstructXml(xml);

        const auto size{xml.tellp()};
        xml.seekg(std::ios::beg);
        content.resize(size);

        xml.read(&content[0], content.size());
    }
    void Manifest::save(const std::filesystem::path& output) const {
        std::ofstream meta{output, std::ios::out};
        meta.write(&content[0], static_cast<std::streamsize>(content.size()));
        meta.close();
    }
}
