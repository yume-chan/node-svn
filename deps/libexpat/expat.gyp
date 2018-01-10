{
    "targets": [
        {
            "target_name": "expat",
            "type": "static_library",
            "include_dirs": [
                "include",
                "libexpat/expat/lib"
            ],
            "sources": [
                "libexpat/expat/lib/loadlibrary.c",
                "libexpat/expat/lib/xmlparse.c",
                "libexpat/expat/lib/xmlrole.c",
                "libexpat/expat/lib/xmltok.c",
                "libexpat/expat/lib/xmltok_impl.c",
                "libexpat/expat/lib/xmltok_ns.c"
            ],
            "configurations": {
                "Release": {
                    "defines": [
                        "NDEBUG"
                    ]
                }
            },
            "direct_dependent_settings": {
                "include_dirs": [
                    "include",
                    "libexpat/expat/lib"
                ],
                "defines": [
                    "XML_STATIC"
                ]
            }
        }
    ]
}
