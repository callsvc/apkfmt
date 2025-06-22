#pragma once
#include <holder.h>

#include <zip.h>
namespace apkfmt {
    class Repack {
    public:
        explicit Repack(const Holder& holder);

        void Unpack();
        void Pack() const;
    private:
        bool HandleObfuscatedManifest(zip_t* entry, std::fstream& io) const;

        Holder backing;
        std::vector<u8> chunkBuffer;
        std::stringstream stream;
    };
}
