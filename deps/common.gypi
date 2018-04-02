{
    "target_defaults": {
        "cflags": [
            "-w"
        ],
        "cflags!": [
            "-Wall",
            "-Wextra"
        ],
        "msvs_settings": {
            "VCCLCompilerTool": {
                "WarningLevel": 0
            }
        },
        "type": "static_library",
        "win_delay_load_hook": "false",
        "xcode_settings": {
            "WARNING_CFLAGS": [
                "-w"
            ],
            "WARNING_CFLAGS!": [
                "-Wall",
                "-Wendif-labels",
                "-W",
                "-Wno-unused-parameter"
            ]
        }
    }
}
