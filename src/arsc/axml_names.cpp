#include <filesystem>
#include <fstream>
#include <vector>

#include <arsc/axml_parser.h>
namespace apkfmt::arsc {
    std::string AxmlParser::getAttrString(u16 index) {
        [[unlikely]] if (attributes.empty()) {
            std::filesystem::path attrs{"android_attributes_list.txt"};
            if (std::getenv("APKFMT_ATTRS_PATH") != nullptr) {
                attrs = std::getenv("APKFMT_ATTRS_PATH");
            }
            if (exists(attrs)) {
                std::ifstream read{attrs};
                std::vector<char> maxAttrName(100);
                do {
                    read.getline(&maxAttrName[0], maxAttrName.size());
                    attributes.emplace_back(&maxAttrName[0]);
                } while (read.gcount());
            }
        }
        index -= 0x1010000;
        if (index > attributes.size()) {
            throw std::runtime_error("Invalid attribute index");
        }
        return attributes[index];
    }
}
