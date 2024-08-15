#include <algorithm>
#include <fstream>
#include <functional>
#include <ranges>

#include <zip.h>

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
            auto ioSize{zip_entry_size(zip)};
            std::fstream io{entryPath};
            if (!is_regular_file(entryPath)) {
                create_directories(entryPath.parent_path());
                io.open(entryPath, std::ios::out | std::ios::trunc);

                Holder::increaseFSz(entryPath, ioSize);
            }

            chunkBuffer.resize(ioSize);
            zip_entry_noallocread(zip, chunkBuffer.data(), chunkBuffer.size());
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
        output += "-Zipped.apk";
        const auto zip{zip_open(output.c_str(), zipLevelStore, 'w')};

        std::function<void(const std::filesystem::path&)> walk = [&](const auto& path) {
            const auto walker{std::filesystem::directory_iterator(path)};
            auto filter{walker | std::views::drop(0)};
            if (path == backing.output)
                filter = {walker | std::views::drop(1)};
            for (const auto& entry : filter) {
                const std::string rfs{entry.path()};
                auto iterator{std::find_if(rfs.begin(), rfs.end(), [](auto it) { return it == '/'; })};
                iterator.operator++();
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
