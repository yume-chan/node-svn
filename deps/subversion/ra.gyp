{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_ra",
            "dependencies": [
                "ra_local.gyp:libsvn_ra_local",
                "ra_serf.gyp:libsvn_ra_serf",
                "ra_svn.gyp:libsvn_ra_svn"
            ],
            "sources": [
                "subversion/subversion/libsvn_ra/compat.c",
                "subversion/subversion/libsvn_ra/debug_reporter.c",
                "subversion/subversion/libsvn_ra/deprecated.c",
                "subversion/subversion/libsvn_ra/editor.c",
                "subversion/subversion/libsvn_ra/ra_loader.c",
                "subversion/subversion/libsvn_ra/util.c"
            ]
        }
    ]
}
