const fs = require("fs");
const path = require("path");

const normalizePath = require("./normalize-path");

const serf = path.resolve(normalizePath.root, "dependencies/serf");

/**
 * @param {string} folder
 * @param {string} extension
 * @returns {string[]}
 */
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
    includes.push(normalizePath(serf));

    // Node's OpenSSL have this
    defines.push("OPENSSL_NO_DEPRECATED");

    // Use static apr
    defines.push("APR_DECLARE_STATIC");

    // From SConsturct script
    defines.push("SERF_NO_SSL_BIO_WRAPPERS");
    defines.push("SERF_NO_SSL_X509_STORE_WRAPPERS");
    defines.push("SERF_HAVE_SSL_LOCKING_CALLBACKS");
    defines.push("SERF_HAVE_OPENSSL_ALPN");

    sources.push(...find(path.resolve(serf, "src"), ".c").map(normalizePath));
    sources.push(...find(path.resolve(serf, "buckets"), ".c").map(normalizePath));
    sources.push(...find(path.resolve(serf, "auth"), ".c").map(normalizePath));
    sources.push(...find(path.resolve(serf, "protocols"), ".c").map(normalizePath));

    references.push(normalizePath(serf));

    switch (platform) {
        case "win32":
            // From SConsturct script
            defines.push("WIN32",
                "WIN32_LEAN_AND_MEAN",
                "NOUSER",
                "NOGDI",
                "NONLS",
                "NOCRYPT",
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

    const dependencies = [];
    if (process.env.npm_config_runtime === "electron") {
        // Electron doesn't export openssl and zlib, compile from source
        includes.push("dependencies/zlib");
        dependencies.push("dependencies/openssl/openssl.gyp:openssl");
    }

    const configuration = {
        "target_name": "serf",
        "type": "static_library",
        "include_dirs": includes,
        dependencies,
        defines,
        sources,
    };

    return { includes, defines, sources, references, configuration };
}
