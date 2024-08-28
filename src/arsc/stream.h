#pragma once
#include <fstream>
#include <cstring>
#include <vector>

#include <types.h>
namespace apkfmt::arsc {
    class Stream {
    public:
        explicit Stream(std::stringstream& io) {
            data = io.view();
            position = io.tellg();
        }

        template <typename T>
        requires std::is_trivially_copyable_v<T>
        auto get() const {
            T object;
            memcpy(&object, &data[position], sizeof(T));
            return object;
        }
        template <typename T>
        auto read() {
            auto object{get<T>()};
            position += sizeof(object);
            return object;
        }
        explicit operator u64() const {
            return position;
        }

        void setPos(u64 pos);
        void skip(u64 bytes);

        std::string_view getString(std::streamsize length) const;
        std::string getUtf8String(std::streamsize length);
    private:
        std::vector<u16> buffer;

        u64 position{};
        std::string_view data;
    };
}
