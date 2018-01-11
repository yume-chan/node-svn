{
    "targets": [
        {
            "target_name": "libsvn_ra_svn",
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
                "subversion/subversion/libsvn_ra_svn/client.c",
                "subversion/subversion/libsvn_ra_svn/cram.c",
                "subversion/subversion/libsvn_ra_svn/cyrus_auth.c",
                "subversion/subversion/libsvn_ra_svn/deprecated.c",
                "subversion/subversion/libsvn_ra_svn/editorp.c",
                "subversion/subversion/libsvn_ra_svn/internal_auth.c",
                "subversion/subversion/libsvn_ra_svn/marshal.c",
                "subversion/subversion/libsvn_ra_svn/streams.c",
                "subversion/subversion/libsvn_ra_svn/version.c",
                "subversion/subversion/libsvn_ra_svn/wrapped_sasl.c"
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
