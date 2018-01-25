{
    "variables": {
        "runtime%": "node"
    },
    "targets": [
        {
            "target_name": "serf",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "dependencies": [
                "../apr/apr.gyp:apr"
            ],
            "defines": [
                "OPENSSL_NO_DEPRECATED",
                "SERF_NO_SSL_BIO_WRAPPERS",
                "SERF_NO_SSL_X509_STORE_WRAPPERS",
                "SERF_HAVE_SSL_LOCKING_CALLBACKS",
                "SERF_HAVE_OPENSSL_ALPN"
            ],
            "include_dirs": [
                "serf"
            ],
            "sources": [
                "serf/src/config_store.c",
                "serf/src/context.c",
                "serf/src/deprecated.c",
                "serf/src/incoming.c",
                "serf/src/logging.c",
                "serf/src/outgoing.c",
                "serf/src/outgoing_request.c",
                "serf/src/pump.c",
                "serf/src/ssltunnel.c",
                "serf/buckets/aggregate_buckets.c",
                "serf/buckets/allocator.c",
                "serf/buckets/barrier_buckets.c",
                "serf/buckets/brotli_buckets.c",
                "serf/buckets/buckets.c",
                "serf/buckets/bwtp_buckets.c",
                "serf/buckets/chunk_buckets.c",
                "serf/buckets/copy_buckets.c",
                "serf/buckets/dechunk_buckets.c",
                "serf/buckets/deflate_buckets.c",
                "serf/buckets/event_buckets.c",
                "serf/buckets/fcgi_buckets.c",
                "serf/buckets/file_buckets.c",
                "serf/buckets/headers_buckets.c",
                "serf/buckets/hpack_buckets.c",
                "serf/buckets/http2_frame_buckets.c",
                "serf/buckets/iovec_buckets.c",
                "serf/buckets/limit_buckets.c",
                "serf/buckets/log_wrapper_buckets.c",
                "serf/buckets/mmap_buckets.c",
                "serf/buckets/prefix_buckets.c",
                "serf/buckets/request_buckets.c",
                "serf/buckets/response_body_buckets.c",
                "serf/buckets/response_buckets.c",
                "serf/buckets/simple_buckets.c",
                "serf/buckets/socket_buckets.c",
                "serf/buckets/split_buckets.c",
                "serf/auth/auth.c",
                "serf/auth/auth_basic.c",
                "serf/auth/auth_digest.c",
                "serf/auth/auth_spnego.c",
                "serf/auth/auth_spnego_gss.c",
                "serf/auth/auth_spnego_sspi.c",
                "serf/protocols/fcgi_protocol.c",
                "serf/protocols/fcgi_stream.c",
                "serf/protocols/http2_protocol.c",
                "serf/protocols/http2_stream.c",
                "src/ssl_buckets.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            },
            "direct_dependent_settings": {
                "defines": [
                    "OPENSSL_NO_DEPRECATED",
                    "SERF_NO_SSL_BIO_WRAPPERS",
                    "SERF_NO_SSL_X509_STORE_WRAPPERS",
                    "SERF_HAVE_SSL_LOCKING_CALLBACKS",
                    "SERF_HAVE_OPENSSL_ALPN"
                ],
                "include_dirs": [
                    "serf"
                ]
            },
            "conditions": [
                [
                    "runtime == 'electron'",
                    {
                        "dependencies": [
                            "../openssl/openssl.gyp:openssl"
                        ],
                        "include_dirs": [
                            "<(node_root_dir)/deps/zlib"
                        ]
                    }
                ],
                [
                    "OS == 'win'",
                    {
                        "defines": [
                            "SERF_HAVE_SSPI"
                        ]
                    }
                ]
            ]
        }
    ]
}
