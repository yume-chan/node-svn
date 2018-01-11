{
    "targets": [
        {
            "target_name": "svn",
            "dependencies": [
                "deps/apr/apr.gyp:apr",
                "deps/subversion/client.gyp:libsvn_client"
            ],
            "include_dirs": [
                "src"
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
}
