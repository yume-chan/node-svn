{
    "includes": [
        "./common.gypi"
    ],
    "targets": [
        {
            "target_name": "libsvn_ra_serf",
            "dependencies": [
                "../serf/serf.gyp:serf"
            ],
            "sources": [
                "subversion/subversion/libsvn_ra_serf/blame.c",
                "subversion/subversion/libsvn_ra_serf/blncache.c",
                "subversion/subversion/libsvn_ra_serf/commit.c",
                "subversion/subversion/libsvn_ra_serf/eagain_bucket.c",
                "subversion/subversion/libsvn_ra_serf/get_deleted_rev.c",
                "subversion/subversion/libsvn_ra_serf/get_file.c",
                "subversion/subversion/libsvn_ra_serf/get_lock.c",
                "subversion/subversion/libsvn_ra_serf/getdate.c",
                "subversion/subversion/libsvn_ra_serf/getlocations.c",
                "subversion/subversion/libsvn_ra_serf/getlocationsegments.c",
                "subversion/subversion/libsvn_ra_serf/getlocks.c",
                "subversion/subversion/libsvn_ra_serf/inherited_props.c",
                "subversion/subversion/libsvn_ra_serf/list.c",
                "subversion/subversion/libsvn_ra_serf/lock.c",
                "subversion/subversion/libsvn_ra_serf/log.c",
                "subversion/subversion/libsvn_ra_serf/merge.c",
                "subversion/subversion/libsvn_ra_serf/mergeinfo.c",
                "subversion/subversion/libsvn_ra_serf/multistatus.c",
                "subversion/subversion/libsvn_ra_serf/options.c",
                "subversion/subversion/libsvn_ra_serf/property.c",
                "subversion/subversion/libsvn_ra_serf/replay.c",
                "subversion/subversion/libsvn_ra_serf/request_body.c",
                "subversion/subversion/libsvn_ra_serf/sb_bucket.c",
                "subversion/subversion/libsvn_ra_serf/serf.c",
                "subversion/subversion/libsvn_ra_serf/stat.c",
                "subversion/subversion/libsvn_ra_serf/stream_bucket.c",
                "subversion/subversion/libsvn_ra_serf/update.c",
                "subversion/subversion/libsvn_ra_serf/util.c",
                "subversion/subversion/libsvn_ra_serf/util_error.c",
                "subversion/subversion/libsvn_ra_serf/xml.c"
            ]
        }
    ]
}
