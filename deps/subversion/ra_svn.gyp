{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_ra_svn",
            "sources": [
                "subversion/subversion/libsvn_ra_svn/client.c",
                "subversion/subversion/libsvn_ra_svn/cram.c",
                "subversion/subversion/libsvn_ra_svn/cyrus_auth.c",
                "subversion/subversion/libsvn_ra_svn/deprecated.c",
                "subversion/subversion/libsvn_ra_svn/editorp.c",
                "subversion/subversion/libsvn_ra_svn/internal_auth.c",
                "subversion/subversion/libsvn_ra_svn/marshal.c",
                "subversion/subversion/libsvn_ra_svn/streams.c",
                "subversion/subversion/libsvn_ra_svn/version.c",
                "subversion/subversion/libsvn_ra_svn/wrapped_sasl.c"
            ]
        }
    ]
}
