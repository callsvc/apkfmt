#pragma once
#include <vector>

#include <types.h>
namespace apkfmt::arsc {
    enum ResourceType : u16 {
        Null = 0,
        StringPool = 1,
        Xml = 3
    };

    class AxmlParser {
    public:
        AxmlParser();
        std::vector<std::string> attributes;
        std::string getAttrString(u16 index);
    };
}
