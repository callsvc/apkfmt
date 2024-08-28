#pragma once
#include <optional>
#include <vector>
#include <format>

#include <boost/property_tree/ptree.hpp>

#include <types.h>
#include <arsc/stream.h>
namespace apkfmt::arsc {
    namespace bpt = boost::property_tree;

    struct ResourceValue {
        static constexpr auto resComplexUnitPx{0};
        static constexpr auto resComplexUnitDip{1};
        static constexpr auto resComplexUnitSp{2};
        static constexpr auto resComplexUnitPt{3};
        static constexpr auto resComplexUnitIn{4};
        static constexpr auto resComplexUnitMm{5};
        static constexpr auto resComplexUnitFraction{0};
        static constexpr auto resComplexUnitParent{1};

        u16 size;
        u8 res0;
        u8 dataType;
        u32 data;

        enum ResourceType : u16 {
            Null = 0,
            Reference = 1,
            Attribute = 2,
            String = 3,
            Float = 4,
            Dimension = 5,
            Fraction = 6,
            DynamicReference = 7,

            FirstInt = 0x10,
            IntDec = 0x10,
            IntHex = 0x11,
            IntBoolean = 0x12,

            FirstColorInt = 0x1c,
            IntColorArgb8 = 0x1c,
            IntColorRgb8 = 0x1d,
            IntColorArgb4 = 0x1e,
            IntColorRgb4 = 0x1f,

            LastColorInt = 0x1f,
            LastInt = 0x1f
        };

        friend std::ostream& operator<<(std::ostream& os, const ResourceValue& resource) {
            auto printComplex = [&](const bool frac) {
                constexpr auto mantissaMul{1.f / (1 << 8)};
                constexpr std::array radixMul{
                    mantissaMul * 1.f,
                    mantissaMul * 1.f / (1 << 7),
                    mantissaMul * 1.f / (1 << 15),
                    mantissaMul * 1.f / (1 << 23),
                };
                const auto value{static_cast<i32>(resource.data & 0xffffff00) * radixMul[frac]};
                if (frac) {
                    os << value * 100;
                    const auto fracMode{(resource.data & 0x7)};
                    if (!fracMode) {
                        os << "%";
                    } else if (fracMode == 1) {
                        os << "%p";
                    }
                } else {
                    os << value;
                    static std::array<std::string, 6> units{
                        "px", "dip", "sp", "pt", "in", "mm"
                    };
                    if ((resource.data & 0x7) < units.size())
                        os << units[resource.data & 0x7];
                }
            };
            float format{};
            switch (resource.dataType) {
                case Null:
                    os << "null";
                    break;
                case Float:
                    std::memcpy(&format, &resource.data, sizeof(float));
                    os << format;
                    break;
                case Dimension:
                    printComplex(false);
                    break;
                case Fraction:
                    printComplex(true);
                    break;
                case IntDec:
                    os << std::dec << resource.data;
                    break;
                case IntBoolean:
                    os << (resource.data ? "true" : "false");
                    break;
                default:
                    os << std::format("type {}/{}", resource.dataType, resource.size);
            }
            return os;
        }
    };

    class AxmlParser {
    public:
        static constexpr u16 resNull{0x0001};
        static constexpr u16 resStringPool{0x0001};

        static constexpr u16 resTable{0x0002};

        static constexpr u16 resXml{0x0003};
        static constexpr u16 resXmlFirstChunk{0x0100};
        static constexpr u16 resXmlStartNamespace{0x100};
        static constexpr u16 resXmlEndNamespace{0x0101};
        static constexpr u16 resXmlStartElement{0x0102};
        static constexpr u16 resXmlEndElement{0x0103};
        static constexpr u16 resXmlCData{0x0104};
        static constexpr u16 resXmlLastChunk{0x017f};
        static constexpr u16 resXmlResourceMap{0x0180};

        static constexpr u16 resTablePackage{0x0200};
        static constexpr u16 resTableType{0x0201};
        static constexpr u16 resTableTypeSpec{0x0202};
        static constexpr u16 resTableLibrary{0x0203};

        AxmlParser(std::stringstream& stream, bpt::ptree& root);
        std::string getAttrString(u16 index);

        void parser();
    private:
        void parserStringPool();
        void parserResourceMap();
        void parserStartNameSpace();
        void parserEndNameSpace();
        void parserStartElement();
        void parserEndElement();
        void parserXmlCData();

        std::optional<Stream> source;
        std::vector<std::string> attributes;

        struct ResChunkHeader {
            u16 type;
            u16 header;
            u32 chunkSize;
        };
        static_assert(sizeof(ResChunkHeader) == 8, "Invalid ResChunkHeader");
        struct XmlStackItem {
            explicit XmlStackItem(bpt::ptree& pt) : element(pt) {}

            std::vector<std::pair<u32, u32>> namespaces;
            bpt::ptree& element;
        };

        std::vector<XmlStackItem> stack;
        std::vector<std::string> strTab;
        std::vector<u32> resourcesIds;
        ResChunkHeader xmlMain;
    };
}
