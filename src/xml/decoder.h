#pragma once
#include <memory>
#include <sstream>
#include <vector>

namespace apkfmt::xml {
    class Decoder {
    public:
        explicit Decoder(std::vector<char>& buffer);

        void reconstructXml(std::stringstream& output) const;
    private:
        std::vector<char>& content;
        std::stringstream fixed;
    };
}
