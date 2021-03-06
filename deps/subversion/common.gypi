{
    "includes": [
        "../common.gypi"
    ],
    "target_defaults": {
        "conditions": [
            [
                "OS == 'win'",
                {
                    "include_dirs": [
                        "include/win"
                    ]
                }
            ],
            [
                "OS == 'mac'",
                {
                    "include_dirs": [
                        "include/unix"
                    ]
                }
            ],
            [
                "OS == 'linux'",
                {
                    "include_dirs": [
                        "include/linux"
                    ]
                }
            ]
        ],
        "configurations": {
            "Release": {
                "defines": [
                    "NDEBUG"
                ]
            }
        },
        "defines": [
            "SVN_HAVE_MEMCACHE",
            "SVN_INTERNAL_LZ4",
            "SVN_HAVE_SERF",
            "SVN_LIBSVN_RA_LINKS_RA_SERF",
            "SVN_SQLITE_INLINE",
            "SVN_INTERNAL_UTF8PROC"
        ],
        "dependencies": [
            "../apr/apr.gyp:apr",
            "../libexpat/expat.gyp:expat"
        ],
        "direct_dependent_settings": {
            "include_dirs": [
                "subversion/subversion/include"
            ]
        },
        "include_dirs": [
            "subversion/subversion/include",
            "subversion/subversion/include/private"
        ]
    }
}
