{
    "targets": [
        {
            "target_name": "apr-iconv",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "include_dirs": [
                "apr-iconv/include"
            ],
            "sources": [
                "dependencies/apr-iconv/lib/iconv.c",
                "dependencies/apr-iconv/lib/iconv_ces.c",
                "dependencies/apr-iconv/lib/iconv_ces_euc.c",
                "dependencies/apr-iconv/lib/iconv_ces_iso2022.c",
                "dependencies/apr-iconv/lib/iconv_int.c",
                "dependencies/apr-iconv/lib/iconv_module.c",
                "dependencies/apr-iconv/lib/iconv_uc.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            }
        }
    ]
}
