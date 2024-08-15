#pragma once
#include <holder.h>
namespace apkfmt {
    class Repack {
    public:
        explicit Repack(const Holder& holder);
        void unpack();
        void pack() const;

    private:
        Holder backing;
        std::vector<u8> chunkBuffer;
        std::stringstream stream;
    };
}
