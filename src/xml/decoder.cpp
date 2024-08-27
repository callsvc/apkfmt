#include <xml/decoder.h>

#include <arsc/axml_parser.h>
namespace apkfmt::xml {
    Decoder::Decoder(std::vector<char>& buffer) : content(buffer) {
        for (const auto& ordered : content) {
            fixed.put(ordered);
        }
        fixed.seekg(std::ios::beg);
        [[maybe_unused]] arsc::AxmlParser parser;
        auto test{arsc::AxmlParser::getAttrString(100)};
    }

    void Decoder::reconstructXml(std::stringstream& output) const {
        output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        if (fixed.view().empty()) {
        }
    }
}
