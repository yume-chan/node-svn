{
    "targets": [
        {
            "target_name": "libsvn_diff",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr",
                "../libexpat/expat.gyp:expat"
            ],
            "include_dirs": [
                "subversion/subversion/include",
                "subversion/subversion/include/private"
            ],
            "defines": [
                "SVN_HAVE_MEMCACHE",
                "SVN_INTERNAL_LZ4",
                "SVN_HAVE_SERF",
                "SVN_LIBSVN_RA_LINKS_RA_SERF",
                "SVN_SQLITE_INLINE",
                "SVN_INTERNAL_UTF8PROC"
            ],
            "sources": [
                "subversion/subversion/libsvn_diff/binary_diff.c",
                "subversion/subversion/libsvn_diff/deprecated.c",
                "subversion/subversion/libsvn_diff/diff.c",
                "subversion/subversion/libsvn_diff/diff3.c",
                "subversion/subversion/libsvn_diff/diff4.c",
                "subversion/subversion/libsvn_diff/diff_file.c",
                "subversion/subversion/libsvn_diff/diff_memory.c",
                "subversion/subversion/libsvn_diff/diff_tree.c",
                "subversion/subversion/libsvn_diff/lcs.c",
                "subversion/subversion/libsvn_diff/parse-diff.c",
                "subversion/subversion/libsvn_diff/token.c",
                "subversion/subversion/libsvn_diff/util.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            },
            "conditions": [
                [
                    "OS == \"win\"",
                    {
                        "include_dirs": [
                            "include/win"
                        ]
                    }
                ]
            ]
        }
    ]
}
