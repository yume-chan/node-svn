{
    "includes": [
        "../common.gypi"
    ],
    "targets": [
        {
            "conditions": [
                [
                    "OS != \"win\"",
                    {
                        "defines": [
                            "HAVE_EXPAT_CONFIG_H"
                        ],
                        "direct_dependent_settings": {
                            "defines": [
                                "HAVE_EXPAT_CONFIG_H"
                            ],
                            "include_dirs": [
                                "include/unix"
                            ]
                        },
                        "include_dirs": [
                            "include/unix"
                        ]
                    }
                ]
            ],
            "direct_dependent_settings": {
                "defines": [
                    "XML_STATIC"
                ],
                "include_dirs": [
                    "include",
                    "libexpat/expat/lib"
                ]
            },
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
            "target_name": "expat",
        }
    ]
}
