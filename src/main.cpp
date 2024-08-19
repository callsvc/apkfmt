#include <holder.h>
#include <repack.h>

#include <res/ro.h>
using namespace apkfmt;
auto main(const i32 argc, char** argv) -> i32 {
    Holder holder;

    std::vector<char*> args;
    for (i32 argument{}; argument < argc; argument++)
        args.push_back(argv[argument]);

    holder.getHolderArgs(args);
    Repack repack{holder};

    repack.unpack();
    res::Ro readOnlyOutput{holder.output};
    readOnlyOutput.groupResources();
    // repack.pack();

    return {};
}
