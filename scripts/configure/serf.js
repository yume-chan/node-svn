const fs = require("fs");
const path = require("path");

const root = path.resolve(__dirname, "../../");
const serf = path.resolve(root, "dependencies/serf");

function find(folder, extension) {
    const result = [];
    for (const name of fs.readdirSync(folder)) {
        const full = path.resolve(folder, name);
        if (fs.statSync(full).isDirectory()) {
            result.push(...find(full, extension));
        } else if (path.extname(full) === extension) {
            result.push(full);
        }
    }
    return result;
}

module.exports = function configure(platform, arch, apr) {
    const includes = [];
    const defines = []
    const sources = [];
    const references = [];

    includes.push(...apr.references);
    includes.push(root);

    defines.push("APR_DECLARE_STATIC");

    sources.push(...find(path.resolve(serf, "src"), ".c"));
    sources.push(...find(path.resolve(serf, "buckets"), ".c"));
    sources.push(...find(path.resolve(serf, "auth"), ".c"));
    sources.push(...find(path.resolve(serf, "protocols"), ".c"));

    references.push(root);

    switch (platform) {
        case "win32":
            defines.push("WIN32", "WIN32_LEAN_AND_MEAN", "NOUSER",
                "NOGDI", "NONLS", "NOCRYPT",
                "_CRT_SECURE_NO_WARNINGS",
                "_CRT_NONSTDC_NO_WARNINGS");

            defines.push("SERF_HAVE_SSPI");

            if (arch === "x64") {
                defines.push("WIN64");
            }
            break;
        default:
            throw new Error("Serf: Platform not supported!");
    }

    const configuration = {
        "target_name": "serf",
        "type": "static_library",
        "include_dirs": includes,
        "defines": defines,
        "sources": sources,
    };

    return { includes, defines, sources, references, configuration };
}
