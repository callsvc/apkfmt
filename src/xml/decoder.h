#pragma once
#include <sstream>
#include <vector>

#include <boost/property_tree/ptree.hpp>
namespace apkfmt::xml {
    namespace boost_pt = boost::property_tree;

    class Decoder {
    public:
        explicit Decoder(std::vector<char>& buffer);

        void reconstructXml(std::stringstream& output) const;
    private:
        std::vector<char>& content;
        std::stringstream encoded;

        boost_pt::ptree root;
    };
}
