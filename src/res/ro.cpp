#include <functional>
#include <ranges>

#include <res/ro.h>
#include <arsc/verify.h>
#include <validate.h>
namespace apkfmt::res {
    Ro::Ro(const std::filesystem::path& droidpath)
        : workingdir(droidpath) {
        std::function<void(const std::filesystem::path&)> grabEverything = [&](const std::filesystem::path& way) {
            for (const std::filesystem::directory_iterator walker(way); const auto& entry : walker) {
                if (entry.is_directory()) {
                    grabEverything(entry.path());
                    continue;
                }
                content.emplace_back(entry.path());
            }
        };
        grabEverything(droidpath);
    }

    void Ro::GroupResources() const {
        static std::array clusterFiles{
            manifestalias
        };
        std::vector<std::string> remain;
        for (const auto& copyable : clusterFiles) {
            if (!exists(workingdir / groupdir / copyable)) {
                remain.push_back(copyable);
            }
        }
        if (remain.empty())
            return;

        for (const auto& target : content) {
            bool chosen{};
            for (const auto& picked : clusterFiles) {
                if (target.filename() == picked)
                    chosen = true;
            }
            if (!chosen)
                continue;

            std::filesystem::path destDir{workingdir / groupdir};
            if (!exists(destDir)) {
                create_directories(destDir);
            }
            const auto destFile{destDir / target.filename()};
            copy_file(target, destFile);
            Validate::CollideFiles(target, destFile);
            std::filesystem::remove(target);

            std::erase_if(remain, [&](const auto& moveable) {
                return moveable == target.filename();
            });
            if (remain.empty())
                break;
        }
    }
    void Ro::Rollback() const {
        for (const std::filesystem::directory_iterator walker(workingdir / groupdir); const auto& entry : walker) {
            const auto mimic{workingdir / entry.path().filename()};
            if (exists(mimic)) {
                std::filesystem::remove(mimic);
            }
            copy_file(entry, mimic);
            Validate::CollideFiles(entry, mimic);
        }

        remove_all(workingdir / groupdir);
    }

    void Ro::Deobfuscate() {
        const std::filesystem::path manifest{workingdir / groupdir / manifestalias};
        TreatManifest(manifest);
    }

    void Ro::TreatManifest(const std::filesystem::path& manifest) {
        if (!exists(manifest))
            return;
        std::vector<std::filesystem::path> files;
        files.push_back(manifest);
        if (arsc::Verify::HasResources(content)) {
            files.push_back(workingdir / "resources.arsc");
        }

        android = Manifest(files);
        android.Decode();
        android.Export(workingdir / manifestalias);
    }
}
