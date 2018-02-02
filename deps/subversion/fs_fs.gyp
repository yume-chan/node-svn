{
    "targets": [
        {
            "target_name": "libsvn_fs_fs",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr",
                "../libexpat/expat.gyp:expat"
            ],
            "include_dirs": [
                "subversion/subversion/include",
                "subversion/subversion/include/private",
                "include/fs_fs"
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
                "subversion/subversion/libsvn_fs_fs/cached_data.c",
                "subversion/subversion/libsvn_fs_fs/caching.c",
                "subversion/subversion/libsvn_fs_fs/dag.c",
                "subversion/subversion/libsvn_fs_fs/dump-index.c",
                "subversion/subversion/libsvn_fs_fs/fs.c",
                "subversion/subversion/libsvn_fs_fs/fs_fs.c",
                "subversion/subversion/libsvn_fs_fs/hotcopy.c",
                "subversion/subversion/libsvn_fs_fs/id.c",
                "subversion/subversion/libsvn_fs_fs/index.c",
                "subversion/subversion/libsvn_fs_fs/load-index.c",
                "subversion/subversion/libsvn_fs_fs/lock.c",
                "subversion/subversion/libsvn_fs_fs/low_level.c",
                "subversion/subversion/libsvn_fs_fs/pack.c",
                "subversion/subversion/libsvn_fs_fs/recovery.c",
                "subversion/subversion/libsvn_fs_fs/rep-cache.c",
                "subversion/subversion/libsvn_fs_fs/rev_file.c",
                "subversion/subversion/libsvn_fs_fs/revprops.c",
                "subversion/subversion/libsvn_fs_fs/stats.c",
                "subversion/subversion/libsvn_fs_fs/temp_serializer.c",
                "subversion/subversion/libsvn_fs_fs/transaction.c",
                "subversion/subversion/libsvn_fs_fs/tree.c",
                "subversion/subversion/libsvn_fs_fs/util.c",
                "subversion/subversion/libsvn_fs_fs/verify.c"
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
