#include <ranges>
#include <print>

#include <holder.h>
#include <repack.h>
#include <res/ro.h>

#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
using namespace apkfmt;

struct AssociatedInOutFiles {
    std::string apkIn;
    std::string apkOut;
};

void filterApkNames(std::vector<AssociatedInOutFiles>& result,
    const std::string_view& filenames,
    const bool input = true) {
    auto associateApkNames = [&](const std::string& name) {
        for (auto& [apkIn, apkOut] : result) {
            if (apkIn.empty() && input) {
                apkIn = name;
                return;
            }
            if (apkOut.empty() && !input) {
                apkOut = name;
                return;
            }
        }
        if (input)
            result.emplace_back(name, "");
        else
            result.emplace_back("", name);
    };
    std::vector<std::string> files;
    split(files, filenames, boost::is_any_of(" ,;/"));

    for (auto& alias : files) {
        boost::trim(alias);
        if (alias.empty())
            continue;
        associateApkNames(alias);
    }
}

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

    bpo::variables_map vm;
    store(parse_command_line(argc, argv, apkOptions), vm);
    vm.notify();

    std::vector<AssociatedInOutFiles> mapper;

    filterApkNames(mapper, inputs);
    filterApkNames(mapper, outputs, false);

    for (auto& [apkIn, apkOut] : mapper) {
        auto makeAssociation = [&](std::string& unknown, const std::string& known, const bool directory = true) -> bool {
            std::filesystem::path canonical;
            if (unknown.empty()) {
                if (known.empty())
                    throw std::runtime_error("No known file specified");

                if (known.find('.') != std::string::npos)
                    canonical = directory ? known.substr(0, known.find('.')) : known;
                else
                    canonical = directory ? known : known + ".apk";
                unknown = canonical.string();
            }
            if (exists(canonical)) {
                std::print("{} already exists and won't be affected for safety reasons\n", unknown);
                return {};
            }
            return true;
        };
        if (apkIn.empty())
            makeAssociation(apkIn, apkOut, false);
        if (apkOut.empty())
            makeAssociation(apkOut, apkIn, true);
    }
    if (mapper.empty()) {
        mapper.emplace_back(holder.apk, holder.output);
    }

    for (const auto& [apkIn, apkOut] : mapper) {
        holder.apk = apkIn;
        holder.output = apkOut;

        Repack packer{holder};

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
    }

    return {};
}
