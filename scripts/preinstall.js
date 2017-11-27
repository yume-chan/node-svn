const path = require("path");
const fs = require("fs");

function find(folder, ext) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full));
        } else if (path.extname(full) === ext) {
            result.push(full);
        }
    }
    return result;
}

const platform = process.platform;
const arch = process.arch;
const libs = [];
switch (platform) {
    case "win32":
        const base = `lib/svn/${platform}/${arch}`;
        libs.push(
            `<(module_root_dir)/${base}/libapr_tsvn.lib`,
            `<(module_root_dir)/${base}/libsvn_tsvn.lib`,
        );
        break;
    default:
        throw new Error("platform not supported.");
}

const configuration = {
    targets: [
        {
            target_name: "svn",
            include_dirs: [
                "include/apr",
                "include/svn",
                "src",
            ],
            libraries: libs,
            sources: find("src", ".cpp").map(x => path.relative(".", x).replace(/\\/g, "/")),
            configurations: {
                Release: {
                    msvs_settings: {
                        VCCLCompilerTool: {
                            AdditionalOptions: [
                                "/std:c++17"
                            ],
                            ExceptionHandling: 1,
                        }
                    }
                }
            }
        },
    ]
};

fs.writeFileSync("binding.gyp", JSON.stringify(configuration, undefined, 4));
