{
    "targets": [
        {
            "target_name": "libsvn_client",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr",
                "../libexpat/expat.gyp:expat",
                "subr.gyp:libsvn_subr",
                "delta.gyp:libsvn_delta",
                "diff.gyp:libsvn_diff",
                "ra.gyp:libsvn_ra",
                "fs.gyp:libsvn_fs",
                "fs_fs.gyp:libsvn_fs_fs",
                "fs_x.gyp:libsvn_fs_x",
                "fs_util.gyp:libsvn_fs_util",
                "repos.gyp:libsvn_repos",
                "wc.gyp:libsvn_wc"
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
                "subversion/subversion/libsvn_client/add.c",
                "subversion/subversion/libsvn_client/blame.c",
                "subversion/subversion/libsvn_client/cat.c",
                "subversion/subversion/libsvn_client/changelist.c",
                "subversion/subversion/libsvn_client/checkout.c",
                "subversion/subversion/libsvn_client/cleanup.c",
                "subversion/subversion/libsvn_client/cmdline.c",
                "subversion/subversion/libsvn_client/commit.c",
                "subversion/subversion/libsvn_client/commit_util.c",
                "subversion/subversion/libsvn_client/compat_providers.c",
                "subversion/subversion/libsvn_client/conflicts.c",
                "subversion/subversion/libsvn_client/copy.c",
                "subversion/subversion/libsvn_client/copy_foreign.c",
                "subversion/subversion/libsvn_client/ctx.c",
                "subversion/subversion/libsvn_client/delete.c",
                "subversion/subversion/libsvn_client/deprecated.c",
                "subversion/subversion/libsvn_client/diff.c",
                "subversion/subversion/libsvn_client/diff_local.c",
                "subversion/subversion/libsvn_client/diff_summarize.c",
                "subversion/subversion/libsvn_client/export.c",
                "subversion/subversion/libsvn_client/externals.c",
                "subversion/subversion/libsvn_client/import.c",
                "subversion/subversion/libsvn_client/info.c",
                "subversion/subversion/libsvn_client/iprops.c",
                "subversion/subversion/libsvn_client/list.c",
                "subversion/subversion/libsvn_client/locking_commands.c",
                "subversion/subversion/libsvn_client/log.c",
                "subversion/subversion/libsvn_client/merge.c",
                "subversion/subversion/libsvn_client/merge_elements.c",
                "subversion/subversion/libsvn_client/mergeinfo.c",
                "subversion/subversion/libsvn_client/mtcc.c",
                "subversion/subversion/libsvn_client/patch.c",
                "subversion/subversion/libsvn_client/prop_commands.c",
                "subversion/subversion/libsvn_client/ra.c",
                "subversion/subversion/libsvn_client/relocate.c",
                "subversion/subversion/libsvn_client/repos_diff.c",
                "subversion/subversion/libsvn_client/resolved.c",
                "subversion/subversion/libsvn_client/revert.c",
                "subversion/subversion/libsvn_client/revisions.c",
                "subversion/subversion/libsvn_client/shelve.c",
                "subversion/subversion/libsvn_client/status.c",
                "subversion/subversion/libsvn_client/switch.c",
                "subversion/subversion/libsvn_client/update.c",
                "subversion/subversion/libsvn_client/upgrade.c",
                "subversion/subversion/libsvn_client/url.c",
                "subversion/subversion/libsvn_client/util.c",
                "subversion/subversion/libsvn_client/version.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            },
            "direct_dependent_settings": {
                "include_dirs": [
                    "subversion/subversion/include"
                ]
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
