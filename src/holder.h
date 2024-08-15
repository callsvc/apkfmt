#pragma once
#include <filesystem>
#include <vector>

#include <types.h>
namespace apkfmt {
    class Holder {
    public:
        Holder() = default;
        void getHolderArgs(std::vector<char*>& args);

        static void increaseFSz(const std::filesystem::path& stream, u64 size);

        std::filesystem::path apk;
        std::filesystem::path output{"ApkOut"};
    };
}
