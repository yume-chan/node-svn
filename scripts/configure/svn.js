const fs = require("fs");
const path = require("path");

const normalizePath = require("./normalize-path");

const vcxproj = require("./vcxproj");

const root = path.resolve(__dirname, "../../");
const subversion = path.resolve(root, "dependencies/subversion");

module.exports = async function configure(platform, arch, name, apr) {
    fs.copyFileSync(path.resolve(subversion, "subversion/svn_private_config.hw"), path.resolve(root, "dependencies/include/svn_private_config.h"));

    switch (platform) {
        case "win32":
            const svn = "dependencies/subversion/build/win32/vcnet-vcproj";
            const { includes, defines, sources } = await vcxproj(svn, `libsvn_${name}.vcxproj`, `Release|${arch === "x64" ? "x64" : "Win32"}`);
            includes.push(...apr.references);

            const references = path.resolve(subversion, "subversion/include");

            const configuration = {
                "target_name": `libsvn_${name}`,
                "type": "static_library",
                "include_dirs": includes,
                "defines": defines,
                "sources": sources,
            };

            return { includes, defines, sources, references, configuration };
        default:
            throw new Error("Subversion: Platform not supported!");
    }
}
