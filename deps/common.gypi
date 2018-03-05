{
    "target_defaults": {
        "type": "static_library",
        "win_delay_load_hook": "false",
        "cflags!": [
            "-Wall",
            "-Wextra"
        ],
        "xcode_settings": {
            "WARNING_CFLAGS": [
                "-Wall",
                "-Wendif-labels",
                "-W"
            ]
        }
    }
}
