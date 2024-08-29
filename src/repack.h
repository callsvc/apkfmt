#pragma once
#include <holder.h>

#include <zip.h>
namespace apkfmt {
    class Repack {
    public:
        explicit Repack(const Holder& holder);

        void unpack();
        void pack() const;
    private:
        bool handleObfuscatedManifest(zip_t* entry, std::fstream& io) const;

        Holder backing;
        std::vector<u8> chunkBuffer;
        std::stringstream stream;
    };
}
