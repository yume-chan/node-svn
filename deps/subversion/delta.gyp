{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_delta",
            "sources": [
                "subversion/subversion/libsvn_delta/branch.c",
                "subversion/subversion/libsvn_delta/branch_compat.c",
                "subversion/subversion/libsvn_delta/branch_migrate.c",
                "subversion/subversion/libsvn_delta/branch_nested.c",
                "subversion/subversion/libsvn_delta/branch_repos.c",
                "subversion/subversion/libsvn_delta/cancel.c",
                "subversion/subversion/libsvn_delta/compat.c",
                "subversion/subversion/libsvn_delta/compose_delta.c",
                "subversion/subversion/libsvn_delta/debug_editor.c",
                "subversion/subversion/libsvn_delta/default_editor.c",
                "subversion/subversion/libsvn_delta/deprecated.c",
                "subversion/subversion/libsvn_delta/depth_filter_editor.c",
                "subversion/subversion/libsvn_delta/editor.c",
                "subversion/subversion/libsvn_delta/element.c",
                "subversion/subversion/libsvn_delta/path_driver.c",
                "subversion/subversion/libsvn_delta/svndiff.c",
                "subversion/subversion/libsvn_delta/text_delta.c",
                "subversion/subversion/libsvn_delta/version.c",
                "subversion/subversion/libsvn_delta/xdelta.c"
            ]
        }
    ]
}
