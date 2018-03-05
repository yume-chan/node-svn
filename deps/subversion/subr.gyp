{
    "includes": [
        "./common.gypi"
    ],
    "variables": {
        "runtime%": "node"
    },
    "targets": [
        {
            "target_name": "libsvn_subr",
            "include_dirs": [
                "sqlite-amalgamation",
                "include/subr"
            ],
            "defines": [
                "alloca=_alloca"
            ],
            "sources": [
                "subversion/subversion/libsvn_subr/adler32.c",
                "subversion/subversion/libsvn_subr/atomic.c",
                "subversion/subversion/libsvn_subr/auth.c",
                "subversion/subversion/libsvn_subr/base64.c",
                "subversion/subversion/libsvn_subr/bit_array.c",
                "subversion/subversion/libsvn_subr/cache-inprocess.c",
                "subversion/subversion/libsvn_subr/cache-membuffer.c",
                "subversion/subversion/libsvn_subr/cache-memcache.c",
                "subversion/subversion/libsvn_subr/cache-null.c",
                "subversion/subversion/libsvn_subr/cache.c",
                "subversion/subversion/libsvn_subr/cache_config.c",
                "subversion/subversion/libsvn_subr/checksum.c",
                "subversion/subversion/libsvn_subr/cmdline.c",
                "subversion/subversion/libsvn_subr/compat.c",
                "subversion/subversion/libsvn_subr/compress_lz4.c",
                "subversion/subversion/libsvn_subr/compress_zlib.c",
                "subversion/subversion/libsvn_subr/config.c",
                "subversion/subversion/libsvn_subr/config_auth.c",
                "subversion/subversion/libsvn_subr/config_file.c",
                "subversion/subversion/libsvn_subr/config_win.c",
                "subversion/subversion/libsvn_subr/crypto.c",
                "subversion/subversion/libsvn_subr/ctype.c",
                "subversion/subversion/libsvn_subr/date.c",
                "subversion/subversion/libsvn_subr/debug.c",
                "subversion/subversion/libsvn_subr/deprecated.c",
                "subversion/subversion/libsvn_subr/dirent_uri.c",
                "subversion/subversion/libsvn_subr/dso.c",
                "subversion/subversion/libsvn_subr/encode.c",
                "subversion/subversion/libsvn_subr/eol.c",
                "subversion/subversion/libsvn_subr/error.c",
                "subversion/subversion/libsvn_subr/fnv1a.c",
                "subversion/subversion/libsvn_subr/gpg_agent.c",
                "subversion/subversion/libsvn_subr/hash.c",
                "subversion/subversion/libsvn_subr/io.c",
                "subversion/subversion/libsvn_subr/iter.c",
                "subversion/subversion/libsvn_subr/lock.c",
                "subversion/subversion/libsvn_subr/log.c",
                "subversion/subversion/libsvn_subr/lz4/lz4.c",
                "subversion/subversion/libsvn_subr/macos_keychain.c",
                "subversion/subversion/libsvn_subr/magic.c",
                "subversion/subversion/libsvn_subr/md5.c",
                "subversion/subversion/libsvn_subr/mergeinfo.c",
                "subversion/subversion/libsvn_subr/mutex.c",
                "subversion/subversion/libsvn_subr/nls.c",
                "subversion/subversion/libsvn_subr/object_pool.c",
                "subversion/subversion/libsvn_subr/opt.c",
                "subversion/subversion/libsvn_subr/packed_data.c",
                "subversion/subversion/libsvn_subr/path.c",
                "subversion/subversion/libsvn_subr/pool.c",
                "subversion/subversion/libsvn_subr/prefix_string.c",
                "subversion/subversion/libsvn_subr/prompt.c",
                "subversion/subversion/libsvn_subr/properties.c",
                "subversion/subversion/libsvn_subr/quoprint.c",
                "subversion/subversion/libsvn_subr/root_pools.c",
                "subversion/subversion/libsvn_subr/simple_providers.c",
                "subversion/subversion/libsvn_subr/skel.c",
                "subversion/subversion/libsvn_subr/sorts.c",
                "subversion/subversion/libsvn_subr/spillbuf.c",
                "subversion/subversion/libsvn_subr/sqlite.c",
                "subversion/subversion/libsvn_subr/sqlite3wrapper.c",
                "subversion/subversion/libsvn_subr/ssl_client_cert_providers.c",
                "subversion/subversion/libsvn_subr/ssl_client_cert_pw_providers.c",
                "subversion/subversion/libsvn_subr/ssl_server_trust_providers.c",
                "subversion/subversion/libsvn_subr/stream.c",
                "subversion/subversion/libsvn_subr/string.c",
                "subversion/subversion/libsvn_subr/subst.c",
                "subversion/subversion/libsvn_subr/sysinfo.c",
                "subversion/subversion/libsvn_subr/target.c",
                "subversion/subversion/libsvn_subr/temp_serializer.c",
                "subversion/subversion/libsvn_subr/time.c",
                "subversion/subversion/libsvn_subr/token.c",
                "subversion/subversion/libsvn_subr/types.c",
                "subversion/subversion/libsvn_subr/user.c",
                "subversion/subversion/libsvn_subr/username_providers.c",
                "subversion/subversion/libsvn_subr/utf.c",
                "subversion/subversion/libsvn_subr/utf8proc.c",
                "subversion/subversion/libsvn_subr/utf_validate.c",
                "subversion/subversion/libsvn_subr/utf_width.c",
                "subversion/subversion/libsvn_subr/validate.c",
                "subversion/subversion/libsvn_subr/version.c",
                "subversion/subversion/libsvn_subr/win32_crashrpt.c",
                "subversion/subversion/libsvn_subr/win32_crypto.c",
                "subversion/subversion/libsvn_subr/win32_xlate.c",
                "subversion/subversion/libsvn_subr/x509info.c",
                "subversion/subversion/libsvn_subr/x509parse.c",
                "subversion/subversion/libsvn_subr/xml.c"
            ],
            "conditions": [
                [
                    "runtime == 'electron'",
                    {
                        "include_dirs": [
                            "<(node_root_dir)/deps/zlib"
                        ]
                    }
                ]
            ]
        }
    ]
}
