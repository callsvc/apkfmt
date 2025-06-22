#include <boost/locale.hpp>

#include <arsc/stream.h>
namespace apkfmt::arsc {
    void Stream::SetPos(const u64 pos) {
        if (data.size() < pos)
            throw std::runtime_error("Out of range");
        position = pos;
    }

    void Stream::Skip(const u64 bytes) {
        position += bytes;
        if (position > data.size())
            throw std::runtime_error("Out of range");
    }

    std::string_view Stream::GetString(const std::streamsize length) const {
        return std::string_view{&data[0], static_cast<u64>(length)};
    }
    std::string Stream::GetUtf8String(const std::streamsize length) {
        buffer.resize(length * sizeof(u16));
        memcpy(&buffer[0], &data[position], length * sizeof(u16));
        return boost::locale::conv::utf_to_utf<char>(&buffer[0], &buffer[length]);
    }
}
