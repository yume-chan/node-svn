const path = require("path");
const fs = require("fs");

function find(folder, ext) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full, ext));
        } else if (path.extname(full) === ext) {
            result.push(full);
        }
    }
    return result;
}

const platform = process.platform;
const arch = process.arch;
const base = `lib/svn/${platform}/${arch}`;
const libs = [];
switch (platform) {
    case "win32":
        libs.push(
            `<(module_root_dir)/${base}/libapr_tsvn.lib`,
            `<(module_root_dir)/${base}/libsvn_tsvn.lib`,
        );
        break;
    case "linux":
        libs.push(
            `<(module_root_dir)/${base}/libapr-1.so`,
            `<(module_root_dir)/${base}/libsvn_client-1.so`,
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
                `include/${platform}/apr`,
                `include/${platform}/svn`,
                "src",
            ],
            libraries: libs,
            sources: find("src", ".cpp").map(x => path.relative(".", x).replace(/\\/g, "/")),
            cflags_cc: [
                "-std=gnu++17",
                "-fexceptions",
            ],
            "cflags_cc!": [
                "-fno-rtti",
            ],
            xcode_settings: {
                GCC_ENABLE_CPP_EXCEPTIONS: "YES",
                CLANG_CXX_LIBRARY: "libc++",
                MACOSX_DEPLOYMENT_TARGET: "10.7",
            },
            msvs_settings: {
                VCCLCompilerTool: {
                    AdditionalOptions: [
                        "/std:c++17"
                    ],
                    ExceptionHandling: 1,
                }
            }
        },
    ]
};

fs.writeFileSync("binding.gyp", JSON.stringify(configuration, undefined, 4));
