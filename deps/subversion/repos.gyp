{
    "targets": [
        {
            "target_name": "libsvn_repos",
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
                "subversion/subversion/libsvn_repos/authz.c",
                "subversion/subversion/libsvn_repos/authz_info.c",
                "subversion/subversion/libsvn_repos/authz_parse.c",
                "subversion/subversion/libsvn_repos/commit.c",
                "subversion/subversion/libsvn_repos/compat.c",
                "subversion/subversion/libsvn_repos/config_file.c",
                "subversion/subversion/libsvn_repos/config_pool.c",
                "subversion/subversion/libsvn_repos/delta.c",
                "subversion/subversion/libsvn_repos/deprecated.c",
                "subversion/subversion/libsvn_repos/dump.c",
                "subversion/subversion/libsvn_repos/fs-wrap.c",
                "subversion/subversion/libsvn_repos/hooks.c",
                "subversion/subversion/libsvn_repos/list.c",
                "subversion/subversion/libsvn_repos/load-fs-vtable.c",
                "subversion/subversion/libsvn_repos/load.c",
                "subversion/subversion/libsvn_repos/log.c",
                "subversion/subversion/libsvn_repos/node_tree.c",
                "subversion/subversion/libsvn_repos/notify.c",
                "subversion/subversion/libsvn_repos/replay.c",
                "subversion/subversion/libsvn_repos/reporter.c",
                "subversion/subversion/libsvn_repos/repos.c",
                "subversion/subversion/libsvn_repos/rev_hunt.c"
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
