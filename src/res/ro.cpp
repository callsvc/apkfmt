#include <functional>
#include <res/ro.h>

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
        const std::string groupDir{"FmtRes"};

        static std::array<std::string, 1> clusterFiles{
            "AndroidManifest.xml"
        };

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
            copy_file(target, destDir / target.filename());
            std::filesystem::remove(target);
        }
    }
}
