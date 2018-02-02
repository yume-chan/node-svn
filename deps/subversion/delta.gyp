{
    "targets": [
        {
            "target_name": "libsvn_delta",
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
                "subversion/subversion/libsvn_delta/branch.c",
                "subversion/subversion/libsvn_delta/branch_compat.c",
                "subversion/subversion/libsvn_delta/branch_migrate.c",
                "subversion/subversion/libsvn_delta/branch_nested.c",
                "subversion/subversion/libsvn_delta/branch_repos.c",
                "subversion/subversion/libsvn_delta/cancel.c",
                "subversion/subversion/libsvn_delta/compat.c",
                "subversion/subversion/libsvn_delta/compose_delta.c",
                "subversion/subversion/libsvn_delta/debug_editor.c",
                "subversion/subversion/libsvn_delta/default_editor.c",
                "subversion/subversion/libsvn_delta/deprecated.c",
                "subversion/subversion/libsvn_delta/depth_filter_editor.c",
                "subversion/subversion/libsvn_delta/editor.c",
                "subversion/subversion/libsvn_delta/element.c",
                "subversion/subversion/libsvn_delta/path_driver.c",
                "subversion/subversion/libsvn_delta/svndiff.c",
                "subversion/subversion/libsvn_delta/text_delta.c",
                "subversion/subversion/libsvn_delta/version.c",
                "subversion/subversion/libsvn_delta/xdelta.c"
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
                    },
                    {
                        "include_dirs": [
                            "include/unix"
                        ]
                    }
                ]
            ]
        }
    ]
}
