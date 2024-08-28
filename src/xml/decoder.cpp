#include <boost/property_tree/xml_parser.hpp>

#include <xml/decoder.h>
#include <arsc/axml_parser.h>
namespace apkfmt::xml {
    Decoder::Decoder(std::vector<char>& buffer) : content(buffer) {
        encoded.write(&content[0], content.size());
        encoded.seekg(std::ios::beg);

        if (encoded.peek() == 0x3) {
            arsc::AxmlParser axml(encoded, root);
            axml.parser();
        } else {
            read_xml(encoded, root, boost_pt::xml_parser::trim_whitespace);
        }
    }

    void Decoder::reconstructXml(std::stringstream& output) const {
        if (encoded.view().empty()) {
            throw std::runtime_error("Empty xml string");
        }
        const boost_pt::xml_writer_settings<std::string> settings(' ', 2);
        write_xml(output, root, settings);
    }
}
