{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_fs_fs",
            "include_dirs": [
                "include/fs_fs"
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
            ]
        }
    ]
}
