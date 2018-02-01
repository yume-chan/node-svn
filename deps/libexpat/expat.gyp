{
    "targets": [
        {
            "target_name": "expat",
            "type": "static_library",
            "win_delay_load_hook": "false",
            "include_dirs": [
                "include",
                "libexpat/expat/lib"
            ],
            "sources": [
                "libexpat/expat/lib/loadlibrary.c",
                "libexpat/expat/lib/xmlparse.c",
                "libexpat/expat/lib/xmltok.c",
                "libexpat/expat/lib/xmlrole.c"
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "include",
                    "libexpat/expat/lib"
                ],
                "defines": [
                    "XML_STATIC"
                ]
            },
            "conditions": [
                [
                    "OS != \"win\"",
                    {
                        "include_dirs": [
                            "include/unix"
                        ],
                        "defines": [
                            "HAVE_EXPAT_CONFIG_H"
                        ],
                        "direct_dependent_settings": {
                            "include_dirs": [
                                "include/unix"
                            ],
                            "defines": [
                                "HAVE_EXPAT_CONFIG_H"
                            ]
                        }
                    }
                ]
            ]
        }
    ]
}
