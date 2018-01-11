{
    "targets": [
        {
            "target_name": "libsvn_fs",
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
                "subversion/subversion/libsvn_fs/access.c",
                "subversion/subversion/libsvn_fs/deprecated.c",
                "subversion/subversion/libsvn_fs/editor.c",
                "subversion/subversion/libsvn_fs/fs-loader.c"
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
