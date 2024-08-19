#include <functional>
#include <ranges>

#include <res/ro.h>
#include <arsc/verify.h>
#include <validate.h>
namespace apkfmt::res {
    Ro::Ro(const std::filesystem::path& androidPath)
        : workDir(androidPath) {
        std::function<void(const std::filesystem::path&)> grabEverything = [&](const std::filesystem::path& way) {
            const std::filesystem::directory_iterator walker(way);
            for (const auto& entry : walker) {
                if (entry.is_directory()) {
                    grabEverything(entry.path());
                    continue;
                }
                content.emplace_back(entry.path());
            }
        };
        grabEverything(androidPath);
    }

    void Ro::groupResources() {
        static std::array clusterFiles{
            manifestAlias
        };
        std::vector<std::string> remain;
        for (const auto& copyable : clusterFiles) {
            if (!exists(workDir / groupDir / copyable)) {
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

            std::filesystem::path destDir{workDir / groupDir};
            if (!exists(destDir)) {
                create_directories(destDir);
            }
            const auto destFile{destDir / target.filename()};
            copy_file(target, destFile);
            Validate::collideFiles(target, destFile);
            std::filesystem::remove(target);

            std::erase_if(remain, [&](const auto& moveable) {
                return moveable == target.filename();
            });
            if (remain.empty())
                break;
        }
    }
    void Ro::rollback() const {
        const std::filesystem::directory_iterator walker(workDir / groupDir);
        for (const auto& entry : walker) {
            const auto mimic{workDir / entry.path().filename()};
            if (exists(mimic)) {
                std::filesystem::remove(mimic);
            }
            copy_file(entry, mimic);
            Validate::collideFiles(entry, mimic);
        }

        remove_all(workDir / groupDir);
    }

    void Ro::deobfuscate() {
        const std::filesystem::path manifest{workDir / groupDir / manifestAlias};
        treatManifest(manifest);
    }

    void Ro::treatManifest(const std::filesystem::path& manifest) {
        if (!exists(manifest))
            return;
        std::vector<std::filesystem::path> files;
        files.push_back(manifest);
        if (arsc::Verify::hasResources(content)) {
            files.push_back(workDir / "resources.arsc");
        }

        android = Manifest(files);
        // android.decode();
        android.save(workDir / manifestAlias);
    }
}
