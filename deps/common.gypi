{
    "target_defaults": {
        "type": "static_library",
        "win_delay_load_hook": "false",
        "cflags!": [
            "-Wall",
            "-Wextra"
        ],
        "cflags": [
            "-w"
        ],
        "xcode_settings": {
            "WARNING_CFLAGS!": [
                "-Wall",
                "-Wendif-labels",
                "-W",
                "-Wno-unused-parameter"
            ],
            "WARNING_CFLAGS": [
                "-w"
            ]
        },
        "msvs_settings": {
            "VCCLCompilerTool": {
                "WarningLevel": 0
            }
        }
    }
}
