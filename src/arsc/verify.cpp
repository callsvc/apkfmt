#include <arsc/verify.h>
namespace apkfmt::arsc {
    bool Verify::HasResources(const std::vector<std::filesystem::path>& apkdir) {
        for (const auto& entry : apkdir) {
            if (entry.filename() == "resources.arsc")
                return true;
        }
        return {};
    }
}