{
    "targets": [
        {
            "target_name": "svn",
            "include_dirs": [
                "include/apr",
                "include/svn",
                "src"
            ],
            "libraries": [
                "<(module_root_dir)/lib/libapr_tsvn.lib",
                "<(module_root_dir)/lib/libsvn_tsvn.lib"
            ],
            "sources": [
                "src\\cpp\\client.cpp",
                "src\\cpp\\svn_error.cpp",
                "src\\node\\async_client.cpp",
                "src\\node\\export.cpp",
                "src\\node\\node_client.cpp"
            ],
            "configurations": {
                "Release": {
                    "msvs_settings": {
                        "VCCLCompilerTool": {
                            "ExceptionHandling": 1
                        }
                    }
                }
            }
        }
    ]
}