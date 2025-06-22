#include <arsc/axml_parser.h>
namespace apkfmt::arsc {
    AxmlParser::AxmlParser(std::stringstream& stream, bpt::ptree& root) {
        stack.clear();
        source = Stream(stream);
        source->SetPos(0);

        stack.emplace_back(root);

        xmlMain = source->read<ResChunkHeader>();
        if (xmlMain.type == resXml)
            return;
        stack.clear();
    }
    void AxmlParser::Parser() {
        while (u64{*source} < xmlMain.chunkSize) {
            const Stream saved{*source};

            const auto [type, header, chunkSize]{source->get<ResChunkHeader>()};
            switch (type) {
                case resStringPool:
                    ParserStringPool();
                    break;
                case resXmlResourceMap:
                    ParserResourceMap();
                    break;
                case resXmlStartNamespace:
                    ParserStartNameSpace();
                    break;
                case resXmlEndNamespace:
                    ParserEndNameSpace();
                    break;
                case resXmlStartElement:
                    ParserStartElement();
                    break;
                case resXmlEndElement:
                    ParserEndElement();
                    break;
                case resXmlCData:
                    ParserXmlCData();
                    break;
                default: {}
            }

            source = saved;
            source->Skip(chunkSize);
        }
    }

    void AxmlParser::ParserStringPool() {
        source->read<ResChunkHeader>();

        const auto stringsCount{source->read<u32>()};
        const auto stylesCount{source->read<u32>()};
        const auto flags{source->read<u32>()};
        const auto stringsStart{source->read<u32>()};
        const auto stylesStart{source->read<u32>()};

        if (stylesCount) {
            std::vector<u32> stylesOffset;
            stylesOffset.reserve(stylesCount);
            for (u32 style{}; style < stylesCount; style++) {
                stylesOffset.emplace_back(source->read<u32>());
            }

            std::vector<u32> style;

            for (const auto& offset : stylesOffset) {
                source->SetPos(stylesStart + sizeof(ResChunkHeader) + offset);
                do {
                    auto value{source->read<u32>()};
                    if (value == 0xfffffff)
                        break;
                    style.emplace_back(value);
                } while (style.size() < 100);
            }
        }

        std::vector<u32> stringsOffset;
        stringsOffset.reserve(stringsCount);
        for (u32 string{}; string < stringsCount; string++) {
            stringsOffset.emplace_back(source->read<u32>());
        };
        strtable.clear();
        strtable.reserve(stringsCount);

        for (const auto& offset : stringsOffset) {
            std::string from;
            source->SetPos(stringsStart + sizeof(ResChunkHeader) + offset);

            if (flags & 1 << 8) {
                // We are dealing with strings in UTF-8 format
                const std::streamsize len{source->read<u8>()};
                if (len & 0x80) {
                    source->read<u8>();
                }
                from = source->GetString(len);
            } else {
                auto len{source->read<u16>()};
                if (len & 0x8000) {
                    len |= (len & 0x7fff) << 16 | source->read<u16>();
                }
                from = source->GetUtf8String(len);
            }
            strtable.emplace_back(std::move(from));
        }
    }

    void AxmlParser::ParserResourceMap() {
        const auto header{source->read<ResChunkHeader>()};
        resourcesIds.clear();

        const auto resCount{(header.chunkSize - sizeof(header)) / 4};
        for (u32 resId{}; resId < resCount; ++resId) {
            resourcesIds.emplace_back(source->read<u32>());
        }
    }
    void AxmlParser::ParserStartNameSpace() {
        source->Skip(sizeof(ResChunkHeader) + sizeof(u32) * 2);
        auto prefix{source->read<u32>()};
        auto uri{source->read<u32>()};

        stack.back().namespaces.emplace_back(uri, prefix);
    }

    void AxmlParser::ParserEndNameSpace() {
        source->Skip(sizeof(ResChunkHeader) + sizeof(u32) * 4);

        stack.back().namespaces.pop_back();
    }

    void AxmlParser::ParserStartElement() {
        using bpt = boost::property_tree::ptree::path_type;
        source->Skip(sizeof(ResChunkHeader) + sizeof(u32) * 3);

        const auto name{source->read<u32>()};
        source->Skip(sizeof(u32));
        const auto attributesCount{source->read<u16>()};
        source->Skip(sizeof(u16) * 3);

        // Creating a new ptree for the new element
        const auto& poll{strtable[name]};
        auto& elementPt{stack.back().element.add(bpt(poll, '`'), "")};

        for (const auto& [fst, snd] : stack.back().namespaces) {
            elementPt.add(
                bpt("<xmlattr>`xmlns:" + strtable[snd], '`'), strtable[fst]);
        }
        stack.emplace_back(elementPt);

        // Creates a ptree for the attributes
        for (u32 attribute{}; attribute < attributesCount; ++attribute) {
            const auto attrNs{source->read<u32>()};
            const auto attrName{source->read<u32>()};
            const auto attrRawValue{source->read<u32>()};
            const auto value{source->read<ResourceValue>()};

            std::stringstream solvedName;
            solvedName << "<xmlattr>`";
            if (attrNs != 0xffffffff) {
                u32 prefix{0xffffffff};
                auto stackIt{rbegin(stack)};
                ++stackIt;
                for (; stackIt != rend(stack); ++stackIt) {
                    const auto namespaces{stackIt->namespaces};
                    auto found{std::find_if(rbegin(namespaces), rend(namespaces), [&](const auto& ns) {
                        return ns.first == attrNs;
                    })};
                    if (found == rend(namespaces))
                        continue;

                    prefix = found->second;
                    break;
                }
                if (prefix != 0xffffffff) {
                    solvedName << strtable[prefix] + ":";
                }
            }
            if (strtable[attrName].empty()) {
                if (attrName > resourcesIds.size())
                    throw std::invalid_argument("Invalid resource id");

                solvedName << GetAttrString(resourcesIds[attrName]);
            } else {
                solvedName << strtable[attrName];
            }

            std::stringstream solvedValue;
            if (attrRawValue != 0xffffffff) {
                solvedValue << strtable[attrRawValue];
            } else {
                solvedValue << value;
            }
            try {
                elementPt.add(bpt(solvedName.str(), '`'), solvedValue.str());
            } catch ([[maybe_unused]] boost::property_tree::ptree_bad_data& except) {
            }
        }
    }

    void AxmlParser::ParserEndElement() {
        source->Skip(sizeof(ResChunkHeader) + sizeof(u32) * 4);

        stack.pop_back();
    }

    void AxmlParser::ParserXmlCData() {
        source->Skip(sizeof(ResChunkHeader) + sizeof(u32) * 2);
        const auto text{source->read<u32>()};
        source->Skip(sizeof(u32) * 2);
        stack.back().element.add("<xmltext>", strtable[text]);
    }
}
