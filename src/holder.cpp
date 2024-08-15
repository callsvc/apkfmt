#include <cstring>
#include <fstream>
#if defined(linux)
#endif

#include <holder.h>
namespace apkfmt {
    void Holder::getHolderArgs(std::vector<char*>& args) {
        for (const auto argument : args) {
            if (std::strstr(argument, ".apk"))
                apk = std::string(argument);
        }
    }
    void Holder::increaseFSz(const std::filesystem::path& stream, const u64 size) {
        if (exists(stream)) {
            resize_file(stream, size);
        }
    }
}
