#include <holder.h>
#include <repack.h>

#include <res/ro.h>

#include <boost/program_options.hpp>
using namespace apkfmt;
auto main(const i32 argc, char** argv) -> i32 {
    namespace bpo = boost::program_options;
    Holder holder;
    bpo::options_description apkOptions("Usage options");

    std::string outputs;
    std::string inputs;
    apkOptions.add_options()
        ("unpack", "Decompress the specified packages")
        ("input", bpo::value(&inputs), "Specify the path for the input files/directories")
        ("output", bpo::value(&outputs), "Specify the naming format for the directories or output files")
        ("pack", "Package the specified directory");

    std::vector<char*> args;
    for (i32 argument{}; argument < argc; argument++)
        args.push_back(argv[argument]);

    holder.getHolderArgs(args);
    Repack packer{holder};

    bpo::variables_map vm;
    store(parse_command_line(argc, argv, apkOptions), vm);

    if (vm.contains("unpack")) {
        packer.unpack();
        res::Ro readOnlyDir{holder.output};
        readOnlyDir.groupResources();
        readOnlyDir.deobfuscate();
    }

    if (vm.contains("pack")) {
        res::Ro readOnlyDir{holder.output};
        readOnlyDir.rollback();
        packer.pack();
    }

    return {};
}
