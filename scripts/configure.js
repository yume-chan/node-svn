const path = require("path");
const fs = require("fs");
const util = require("util");

const vcxproj = require("./configure/vcxproj");
const dsp = require("./configure/dsp");
const serf = require("./configure/serf");
const svn = require("./configure/svn");

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

const platform = process.argv[2] || process.platform;
const arch = process.argv[3] || process.arch;
const root = path.resolve(__dirname, "..");

try {
    fs.mkdirSync("dependencies/include");
} catch (err) { }

async function configure_expat() {
    const { includes, defines, sources } = await vcxproj("dependencies/libexpat/expat/lib", "expat_static.vcxproj", "Release|Win32");
    includes.push(path.resolve(root, "dependencies/libexpat/expat/lib"));

    const references = [];
    references.push(path.resolve(root, "dependencies/libexpat/expat/lib"));

    const configuration =
        {
            "target_name": "expat",
            "type": "static_library",
            "include_dirs": includes,
            "defines": defines,
            "sources": sources
        };

    return { includes, defines, sources, references, configuration };
}

function configure_apr(expat) {
    fs.copyFileSync(path.resolve(root, "dependencies/apr-gen-test-char", platform, "apr_escape_test_char.h"), path.resolve(root, "dependencies/include/apr_escape_test_char.h"));

    const h = path.resolve(root, "dependencies/include/apr.h");

    // Modify APR_HAVE_IPV6 to 0
    // Or it cannot find `IF_NAMESIZE`
    fs.copyFileSync(path.resolve(root, "dependencies/apr", "include/apr.hw"), h);
    let content = fs.readFileSync(h, "utf-8");
    content = content.replace(/APR_HAVE_IPV6\s+1/, "APR_HAVE_IPV6 0");
    fs.writeFileSync(h, content);

    fs.copyFileSync(path.resolve(root, "dependencies/apr", "include/apu_want.hw"), path.resolve(root, "dependencies/include/apu_want.h"));
    fs.copyFileSync(path.resolve(root, "dependencies/apr", "include/private/apu_select_dbm.hw"), path.resolve(root, "dependencies/include/apu_select_dbm.h"));
    const { includes, defines, sources } = dsp("dependencies/apr", "apr.dsp", `apr - ${arch === "x64" ? "x64" : "Win32"} Release`);
    includes.push(...expat.references);
    includes.push(path.resolve(root, "dependencies/include"));

    const references = [];
    references.push(...expat.references);
    references.push(path.resolve(root, "dependencies/include"));
    references.push(path.resolve(root, "dependencies/apr/include"));

    const configuration = {
        "target_name": "apr",
        "type": "static_library",
        "include_dirs": includes,
        "defines": defines,
        "sources": sources
    };

    return { includes, defines, sources, references, configuration };
}

function configure_apr_iconv() {
    return dsp("dependencies/apr-iconv", "apriconv.dsp", `apriconv - ${arch === "x64" ? "x64" : "Win32"} Release`);
}

async function main() {
    const expat = await configure_expat();
    expat.references = ["dependencies/libexpat/expat/lib"];

    const apr = configure_apr(expat);

    const apr_iconv = configure_apr_iconv();

    const serf_conf = serf(platform, arch, apr);

    const client = await svn(platform, arch, "client", apr);
    const diff = await svn(platform, arch, "diff", apr);
    const delta = await svn(platform, arch, "delta", apr);
    const svn_fs = await svn(platform, arch, "fs", apr);
    const fs_util = await svn(platform, arch, "fs_util", apr);
    const fs_fs = await svn(platform, arch, "fs_fs", apr);
    const fs_x = await svn(platform, arch, "fs_x", apr);
    const ra = await svn(platform, arch, "ra", apr);
    const ra_local = await svn(platform, arch, "ra_local", apr);
    const ra_svn = await svn(platform, arch, "ra_svn", apr);

    const ra_serf = await svn(platform, arch, "ra_serf", apr);
    ra_serf.includes.push(...serf_conf.references);

    const repos = await svn(platform, arch, "repos", apr);
    const subr = await svn(platform, arch, "subr", apr);
    subr.defines.push("XML_STATIC");
    const wc = await svn(platform, arch, "wc", apr);

    const configuration = {
        "targets": [
            apr.configuration,
            {
                "target_name": "apr-iconv",
                "type": "static_library",
                "include_dirs": apr_iconv.includes.concat(apr.includes),
                "defines": apr_iconv.defines,
                "sources": apr_iconv.sources
            },
            {
                "target_name": "expat",
                "type": "static_library",
                "include_dirs": expat.includes,
                "defines": expat.defines,
                "sources": expat.sources
            },
            serf_conf.configuration,
            client.configuration,
            diff.configuration,
            delta.configuration,
            svn_fs.configuration,
            fs_fs.configuration,
            fs_util.configuration,
            fs_x.configuration,
            ra.configuration,
            ra_local.configuration,
            ra_serf.configuration,
            ra_svn.configuration,
            repos.configuration,
            subr.configuration,
            wc.configuration,
            {
                "target_name": "svn",
                "dependencies": [
                    "apr",
                    "expat",
                    "serf",
                    "libsvn_client",
                    "libsvn_diff",
                    "libsvn_delta",
                    "libsvn_fs",
                    "libsvn_fs_fs",
                    "libsvn_fs_util",
                    "libsvn_fs_x",
                    "libsvn_ra",
                    "libsvn_ra_local",
                    "libsvn_ra_svn",
                    "libsvn_ra_serf",
                    "libsvn_repos",
                    "libsvn_subr",
                    "libsvn_wc",
                ],
                "include_dirs": [
                    ...apr.includes,
                    ...client.includes,
                    "src"
                ],
                "defines": [
                    "APR_DECLARE_STATIC"
                ],
                "sources": [
                    "src/cpp/client.cpp",
                    "src/cpp/svn_error.cpp",
                    "src/node/async_client.cpp",
                    "src/node/depth.cpp",
                    "src/node/export.cpp",
                    "src/node/node_client.cpp",
                    "src/node/node_kind.cpp",
                    "src/node/revision_kind.cpp",
                    "src/node/status_kind.cpp"
                ],
                "libraries": [
                    "ws2_32.lib",
                    "Mincore.lib"
                ],
                "cflags_cc": [
                    "-std=gnu++17",
                    "-fexceptions"
                ],
                "cflags_cc!": [
                    "-fno-rtti"
                ],
                "xcode_settings": {
                    "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                    "CLANG_CXX_LIBRARY": "libc++",
                    "MACOSX_DEPLOYMENT_TARGET": "10.7"
                },
                "msvs_settings": {
                    "VCCLCompilerTool": {
                        "AdditionalOptions": [
                            "/std:c++17"
                        ],
                        "DisableSpecificWarnings": [
                            "4005"
                        ],
                        "ExceptionHandling": 1
                    }
                }
            }
        ]
    };

    fs.writeFileSync("binding.gyp", JSON.stringify(configuration, undefined, 4));
}
main();
