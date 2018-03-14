{
    "target_defaults": {
        "type": "static_library",
        "win_delay_load_hook": "false",
        "cflags!": [
            "-Wall",
            "-Wextra"
        ],
        "cflags": [
            "-Wno-all",
            "-Wno-extra"
        ],
        "xcode_settings": {
            "WARNING_CFLAGS!": [
                "-Wall",
                "-Wendif-labels",
                "-W"
            ]
        },
        "msvs_settings": {
            "VCCLCompilerTool": {
                "WarningLevel": 0
            }
        }
    }
}
