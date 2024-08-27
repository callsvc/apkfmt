#include <algorithm>
#include <fstream>
#include <functional>
#include <ranges>

#include <repack.h>
#include <validate.h>
namespace apkfmt {
    Repack::Repack(const Holder& holder) : backing(holder) {
        if (!is_directory(holder.output)) {
            create_directory(holder.output);
        }
        if (!is_regular_file(holder.apk))
            std::terminate();

        Validate::doChecksum(stream, holder.apk);
    }

    void Repack::handleObfuscatedManifest(zip_t* entry) const {
        u16 compressionMethod;
        const std::string& apk{backing.apk};
        std::fstream input{apk, std::ios::in | std::ios::binary};
        input.seekg(zip_entry_header_offset(entry) + 0x8);
        input.read(reinterpret_cast<char*>(&compressionMethod), sizeof(compressionMethod));

        if (compressionMethod == 0x8) {
        }
    }

    void Repack::unpack() {
        const std::string input{backing.apk};
        const std::string output{backing.output};

        const auto zip{zip_open(&input[0], 0, 'r')};
        auto totalEntries{zip_entries_total(zip)};

        for (decltype(totalEntries) entry{}; entry < totalEntries; ++entry) {
            auto entryPath{backing.output};
            zip_entry_openbyindex(zip, entry);

            entryPath.append(zip_entry_name(zip));
            if (zip_entry_isdir(zip)) {
                if (!is_directory(entryPath))
                    create_directory(entryPath);
                continue;
            }
            const auto ioSize{zip_entry_size(zip)};
            std::fstream io{entryPath};
            if (!is_regular_file(entryPath)) {
                create_directories(entryPath.parent_path());
                io.open(entryPath, std::ios::out | std::ios::trunc);

                Holder::increaseFSz(entryPath, ioSize);
            }

            if (chunkBuffer.size() < ioSize)
                chunkBuffer.resize(ioSize);
            if (entryPath.filename() == "AndroidManifest.xml") {
                handleObfuscatedManifest(zip);
            }
            zip_entry_noallocread(zip, &chunkBuffer[0], ioSize);
            // ReSharper disable once CppRedundantCastExpression
            io.write(reinterpret_cast<char*>(&chunkBuffer[0]), static_cast<std::streamsize>(ioSize));
            if (io.is_open()) {
                io.close();
            }
            zip_entry_close(zip);
        }
        zip_close(zip);
    }

    void Repack::pack() const {
        constexpr auto zipLevelStore{0};
        std::string output{backing.apk.stem()};
        output += "-compressed.apk";
        const auto zip{zip_open(output.c_str(), zipLevelStore, 'w')};

        std::function<void(const std::filesystem::path&)> walk = [&](const auto& path) {
            const auto walker{std::filesystem::directory_iterator(path)};
            auto filter{walker | std::views::drop(0)};
            if (path == backing.output)
                filter = {walker | std::views::drop(1)};
            for (const auto& entry : filter) {
                const std::string rfs{entry.path()};
                auto iterator{std::find_if(rfs.begin(), rfs.end(), [](auto it) { return it == '/'; })};
                std::advance(iterator, 1);
                if (!entry.is_regular_file()) {
                    walk(entry);
                }
                zip_entry_open(zip, iterator.base());
                zip_entry_fwrite(zip, rfs.c_str());
                zip_entry_close(zip);
            }
        };
        walk(backing.output);

        zip_close(zip);
    }
}
