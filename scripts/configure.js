const path = require("path");
const fs = require("fs");

function find(folder) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full));
        } else if (path.extname(full) === ".cpp") {
            result.push(full);
        }
    }
    return result;
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
            libraries: [
                "<(module_root_dir)/lib/libapr_tsvn.lib",
                "<(module_root_dir)/lib/libsvn_tsvn.lib"
            ],
            sources: find("src").map(x => path.relative(".", x)),
            configurations: {
                Release: {
                    msvs_settings: {
                        VCCLCompilerTool: {
                            ExceptionHandling: 1,
                            ObjectFile: "$(IntDir)%(RelativeDir)"
                        }
                    }
                }
            }
        },
    ]
};

fs.writeFileSync("binding.gyp", JSON.stringify(configuration, undefined, 4));
