{
    "targets": [
        {
            "target_name": "libsvn_wc",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr",
                "../libexpat/expat.gyp:expat"
            ],
            "include_dirs": [
                "subversion/subversion/include",
                "subversion/subversion/include/private",
                "include/wc"
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
                "subversion/subversion/libsvn_wc/adm_crawler.c",
                "subversion/subversion/libsvn_wc/adm_files.c",
                "subversion/subversion/libsvn_wc/adm_ops.c",
                "subversion/subversion/libsvn_wc/ambient_depth_filter_editor.c",
                "subversion/subversion/libsvn_wc/cleanup.c",
                "subversion/subversion/libsvn_wc/conflicts.c",
                "subversion/subversion/libsvn_wc/context.c",
                "subversion/subversion/libsvn_wc/copy.c",
                "subversion/subversion/libsvn_wc/crop.c",
                "subversion/subversion/libsvn_wc/delete.c",
                "subversion/subversion/libsvn_wc/deprecated.c",
                "subversion/subversion/libsvn_wc/diff_editor.c",
                "subversion/subversion/libsvn_wc/diff_local.c",
                "subversion/subversion/libsvn_wc/entries.c",
                "subversion/subversion/libsvn_wc/externals.c",
                "subversion/subversion/libsvn_wc/info.c",
                "subversion/subversion/libsvn_wc/lock.c",
                "subversion/subversion/libsvn_wc/merge.c",
                "subversion/subversion/libsvn_wc/node.c",
                "subversion/subversion/libsvn_wc/old-and-busted.c",
                "subversion/subversion/libsvn_wc/props.c",
                "subversion/subversion/libsvn_wc/questions.c",
                "subversion/subversion/libsvn_wc/relocate.c",
                "subversion/subversion/libsvn_wc/revert.c",
                "subversion/subversion/libsvn_wc/revision_status.c",
                "subversion/subversion/libsvn_wc/status.c",
                "subversion/subversion/libsvn_wc/translate.c",
                "subversion/subversion/libsvn_wc/tree_conflicts.c",
                "subversion/subversion/libsvn_wc/update_editor.c",
                "subversion/subversion/libsvn_wc/upgrade.c",
                "subversion/subversion/libsvn_wc/util.c",
                "subversion/subversion/libsvn_wc/wc_db.c",
                "subversion/subversion/libsvn_wc/wc_db_pristine.c",
                "subversion/subversion/libsvn_wc/wc_db_update_move.c",
                "subversion/subversion/libsvn_wc/wc_db_util.c",
                "subversion/subversion/libsvn_wc/wc_db_wcroot.c",
                "subversion/subversion/libsvn_wc/wcroot_anchor.c",
                "subversion/subversion/libsvn_wc/workqueue.c"
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
