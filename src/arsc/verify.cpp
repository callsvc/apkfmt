#include <arsc/verify.h>

namespace apkfmt::arsc {
    bool Verify::hasResources(const std::vector<std::filesystem::path>& apkDir) {
        for (const auto& entry : apkDir) {
            if (entry.filename() == "resources.arsc")
                return true;
        }
        return {};
    }
}