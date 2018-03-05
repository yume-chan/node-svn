{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_fs_x",
            "include_dirs": [
                "include/fs_x"
            ],
            "sources": [
                "subversion/subversion/libsvn_fs_x/batch_fsync.c",
                "subversion/subversion/libsvn_fs_x/cached_data.c",
                "subversion/subversion/libsvn_fs_x/caching.c",
                "subversion/subversion/libsvn_fs_x/changes.c",
                "subversion/subversion/libsvn_fs_x/dag.c",
                "subversion/subversion/libsvn_fs_x/dag_cache.c",
                "subversion/subversion/libsvn_fs_x/fs.c",
                "subversion/subversion/libsvn_fs_x/fs_id.c",
                "subversion/subversion/libsvn_fs_x/fs_x.c",
                "subversion/subversion/libsvn_fs_x/hotcopy.c",
                "subversion/subversion/libsvn_fs_x/id.c",
                "subversion/subversion/libsvn_fs_x/index.c",
                "subversion/subversion/libsvn_fs_x/lock.c",
                "subversion/subversion/libsvn_fs_x/low_level.c",
                "subversion/subversion/libsvn_fs_x/noderevs.c",
                "subversion/subversion/libsvn_fs_x/pack.c",
                "subversion/subversion/libsvn_fs_x/recovery.c",
                "subversion/subversion/libsvn_fs_x/rep-cache.c",
                "subversion/subversion/libsvn_fs_x/reps.c",
                "subversion/subversion/libsvn_fs_x/rev_file.c",
                "subversion/subversion/libsvn_fs_x/revprops.c",
                "subversion/subversion/libsvn_fs_x/string_table.c",
                "subversion/subversion/libsvn_fs_x/temp_serializer.c",
                "subversion/subversion/libsvn_fs_x/transaction.c",
                "subversion/subversion/libsvn_fs_x/tree.c",
                "subversion/subversion/libsvn_fs_x/util.c",
                "subversion/subversion/libsvn_fs_x/verify.c"
            ]
        }
    ]
}
