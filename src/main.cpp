#include <holder.h>
#include <repack.h>

#include <res/ro.h>

#include <boost/program_options.hpp>
using namespace apkfmt;
auto main(const i32 argc, char** argv) -> i32 {
    namespace bpo = boost::program_options;
    Holder holder;
    bpo::options_description apkOptions("Usage options");
    apkOptions.add_options()
        ("unpack", "")
        ("pack", "");

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
