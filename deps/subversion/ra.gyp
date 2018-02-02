{
    "targets": [
        {
            "target_name": "libsvn_ra",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr",
                "../libexpat/expat.gyp:expat",
                "ra_local.gyp:libsvn_ra_local",
                "ra_serf.gyp:libsvn_ra_serf",
                "ra_svn.gyp:libsvn_ra_svn"
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
                "subversion/subversion/libsvn_ra/compat.c",
                "subversion/subversion/libsvn_ra/debug_reporter.c",
                "subversion/subversion/libsvn_ra/deprecated.c",
                "subversion/subversion/libsvn_ra/editor.c",
                "subversion/subversion/libsvn_ra/ra_loader.c",
                "subversion/subversion/libsvn_ra/util.c"
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
