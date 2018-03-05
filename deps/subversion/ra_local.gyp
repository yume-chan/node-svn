{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_ra_local",
            "sources": [
                "subversion/subversion/libsvn_ra_local/ra_plugin.c",
                "subversion/subversion/libsvn_ra_local/split_url.c"
            ]
        }
    ]
}
