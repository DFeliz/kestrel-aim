# kestrel-aim

Console-only Windows screen capture + computer-vision object tracking backend.

- Capture: DXGI Desktop Duplication (DDA), DirectX 11.
- Detection: BGRA purple-hue pixel classifier, 8-connectivity clustering.
- Aim logic: concentric radial multiplier zones with dot-product gating.
- Interface: stdin/stdout + config.ini.

## Build

    cmake -B build -A x64 -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

Binary: build\Release\kestrel_aim.exe

## Console commands

    set <key> <value>
    show
    save [path]
    load [path]
    on | off
    fov <w> <h>
    quit
