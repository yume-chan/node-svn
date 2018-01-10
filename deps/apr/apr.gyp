{
    "targets": [
        {
            "target_name": "apr",
            "type": "static_library",
            "dependencies": [
                "../libexpat/expat.gyp:expat"
            ],
            "defines": [
                "APR_DECLARE_STATIC"
            ],
            "include_dirs": [
                "apr/include",
                "apr/include/private"
            ],
            "sources": [
                "apr/buckets/apr_brigade.c",
                "apr/buckets/apr_buckets.c",
                "apr/buckets/apr_buckets_alloc.c",
                "apr/buckets/apr_buckets_eos.c",
                "apr/buckets/apr_buckets_file.c",
                "apr/buckets/apr_buckets_flush.c",
                "apr/buckets/apr_buckets_heap.c",
                "apr/buckets/apr_buckets_mmap.c",
                "apr/buckets/apr_buckets_pipe.c",
                "apr/buckets/apr_buckets_pool.c",
                "apr/buckets/apr_buckets_refcount.c",
                "apr/buckets/apr_buckets_simple.c",
                "apr/buckets/apr_buckets_socket.c",
                "apr/crypto/apr_crypto.c",
                "apr/crypto/apr_md4.c",
                "apr/crypto/apr_md5.c",
                "apr/crypto/apr_passwd.c",
                "apr/crypto/apr_sha1.c",
                "apr/crypto/apr_siphash.c",
                "apr/crypto/crypt_blowfish.c",
                "apr/crypto/getuuid.c",
                "apr/crypto/uuid.c",
                "apr/dbd/apr_dbd.c",
                "apr/dbd/apr_dbd_mysql.c",
                "apr/dbd/apr_dbd_odbc.c",
                "apr/dbd/apr_dbd_oracle.c",
                "apr/dbd/apr_dbd_pgsql.c",
                "apr/dbd/apr_dbd_sqlite2.c",
                "apr/dbd/apr_dbd_sqlite3.c",
                "apr/dbm/apr_dbm.c",
                "apr/dbm/apr_dbm_berkeleydb.c",
                "apr/dbm/apr_dbm_gdbm.c",
                "apr/dbm/apr_dbm_sdbm.c",
                "apr/encoding/apr_base64.c",
                "apr/encoding/apr_escape.c",
                "apr/file_io/unix/copy.c",
                "apr/file_io/unix/fileacc.c",
                "apr/file_io/unix/filepath_util.c",
                "apr/file_io/unix/fullrw.c",
                "apr/file_io/unix/mktemp.c",
                "apr/file_io/unix/tempdir.c",
                "apr/hooks/apr_hooks.c",
                "apr/memcache/apr_memcache.c",
                "apr/memory/unix/apr_pools.c",
                "apr/misc/unix/errorcodes.c",
                "apr/misc/unix/getopt.c",
                "apr/misc/unix/otherchild.c",
                "apr/misc/unix/version.c",
                "apr/mmap/unix/common.c",
                "apr/network_io/unix/inet_ntop.c",
                "apr/network_io/unix/inet_pton.c",
                "apr/network_io/unix/multicast.c",
                "apr/network_io/unix/sockaddr.c",
                "apr/network_io/unix/socket_util.c",
                "apr/passwd/apr_getpass.c",
                "apr/poll/unix/pollcb.c",
                "apr/poll/unix/pollset.c",
                "apr/poll/unix/poll.c",
                "apr/poll/unix/select.c",
                "apr/poll/unix/wakeup.c",
                "apr/random/unix/apr_random.c",
                "apr/random/unix/sha2.c",
                "apr/random/unix/sha2_glue.c",
                "apr/dbm/sdbm/sdbm.c",
                "apr/dbm/sdbm/sdbm_hash.c",
                "apr/dbm/sdbm/sdbm_lock.c",
                "apr/dbm/sdbm/sdbm_pair.c",
                "apr/strings/apr_cpystrn.c",
                "apr/strings/apr_fnmatch.c",
                "apr/strings/apr_snprintf.c",
                "apr/strings/apr_strings.c",
                "apr/strings/apr_strnatcmp.c",
                "apr/strings/apr_strtok.c",
                "apr/strmatch/apr_strmatch.c",
                "apr/tables/apr_hash.c",
                "apr/tables/apr_tables.c",
                "apr/tables/apr_skiplist.c",
                "apr/uri/apr_uri.c",
                "apr/util-misc/apr_date.c",
                "apr/util-misc/apu_dso.c",
                "apr/util-misc/apr_queue.c",
                "apr/util-misc/apr_reslist.c",
                "apr/util-misc/apr_rmm.c",
                "apr/util-misc/apr_thread_pool.c",
                "apr/xlate/xlate.c",
                "apr/xml/apr_xml.c",
                "apr/xml/apr_xml_expat.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            },
            "all_dependent_settings": {
                "defines": [
                    "APR_DECLARE_STATIC"
                ],
                "include_dirs": [
                    "apr/include"
                ]
            },
            "conditions": [
                [
                    "OS == \"win\"",
                    {
                        "defines": [
                            "_WIN32_WINNT=0x0601"
                        ],
                        "include_dirs": [
                            "../apr-gen-test-char/win32",
                            "include/win32",
                            "include/win32/private",
                            "apr/include/arch/win32",
                            "apr/include/arch/unix"
                        ],
                        "sources": [
                            "apr/atomic/win32/apr_atomic.c",
                            "apr/dso/win32/dso.c",
                            "apr/file_io/win32/buffer.c",
                            "apr/file_io/win32/dir.c",
                            "apr/file_io/win32/filedup.c",
                            "apr/file_io/win32/filepath.c",
                            "apr/file_io/win32/filestat.c",
                            "apr/file_io/win32/filesys.c",
                            "apr/file_io/win32/flock.c",
                            "apr/file_io/win32/open.c",
                            "apr/file_io/win32/pipe.c",
                            "apr/file_io/win32/readwrite.c",
                            "apr/file_io/win32/seek.c",
                            "apr/locks/win32/proc_mutex.c",
                            "apr/locks/win32/thread_cond.c",
                            "apr/locks/win32/thread_mutex.c",
                            "apr/locks/win32/thread_rwlock.c",
                            "apr/misc/win32/apr_app.c",
                            "apr/misc/win32/charset.c",
                            "apr/misc/win32/env.c",
                            "apr/misc/win32/internal.c",
                            "apr/misc/win32/misc.c",
                            "apr/misc/win32/rand.c",
                            "apr/misc/win32/start.c",
                            "apr/misc/win32/utf8.c",
                            "apr/mmap/win32/mmap.c",
                            "apr/network_io/win32/sendrecv.c",
                            "apr/network_io/win32/sockets.c",
                            "apr/network_io/win32/sockopt.c",
                            "apr/shmem/win32/shm.c",
                            "apr/threadproc/win32/proc.c",
                            "apr/threadproc/win32/signals.c",
                            "apr/threadproc/win32/thread.c",
                            "apr/threadproc/win32/threadpriv.c",
                            "apr/time/win32/time.c",
                            "apr/time/win32/timestr.c",
                            "apr/user/win32/groupinfo.c",
                            "apr/user/win32/userinfo.c"
                        ],
                        "all_dependent_settings": {
                            "include_dirs": [
                                "include/win32",
                                "apr/include/arch/win32",
                                "apr/include/arch/unix"
                            ]
                        }
                    },
                    {
                        "include_dirs": [
                            "../apr-gen-test-char/linux",
                            "apr/include/arch/unix"
                        ],
                        "all_dependent_settings": {
                            "include_dirs": [
                                "apr/include/arch/unix"
                            ]
                        }
                    }
                ]
            ]
        }
    ]
}
