#pragma once
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

        static std::string getAttrString(u16 index);
    };
}
