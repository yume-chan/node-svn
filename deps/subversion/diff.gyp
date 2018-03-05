{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_diff",
            "sources": [
                "subversion/subversion/libsvn_diff/binary_diff.c",
                "subversion/subversion/libsvn_diff/deprecated.c",
                "subversion/subversion/libsvn_diff/diff.c",
                "subversion/subversion/libsvn_diff/diff3.c",
                "subversion/subversion/libsvn_diff/diff4.c",
                "subversion/subversion/libsvn_diff/diff_file.c",
                "subversion/subversion/libsvn_diff/diff_memory.c",
                "subversion/subversion/libsvn_diff/diff_tree.c",
                "subversion/subversion/libsvn_diff/lcs.c",
                "subversion/subversion/libsvn_diff/parse-diff.c",
                "subversion/subversion/libsvn_diff/token.c",
                "subversion/subversion/libsvn_diff/util.c"
            ]
        }
    ]
}
