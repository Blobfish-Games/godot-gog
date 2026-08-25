# Godot GOG
GDExtension for the GOG Galaxy SDK. Currently it only supports getting/setting Achievements and getting Overlay information. However, if you need more of the SDK exposed, it's quite easy to follow the existing implementation and add what you need.

Currently only tested on 4.7 but should be compatible with any 4.x version by downgrading the godot-cpp submodule.

## How To
Copy the `demo/addons/godot_gog` folder into your games `addons/` folder.
Now if you open your game, you should have a new node type called GOG that you can use. For a more detailed setup guide, check out the demo project in `demo/`.

## Linux Caveats
Linux is currently not supported by GOG. However, the editor freezes repeatedly if no library for Linux is provided (see https://github.com/godotengine/godot/issues/105615). To avoid this, the addon is compiled for Linux as well but it can't be used for any interactions with GOG.

The official libGalaxy64.so file has the executable stack flag bit set. This results in the Godot editor failing to load the extension:
```
ERROR: Can't open dynamic library: Error: libGalaxy64.so: cannot enable executable stack as shared object requires: Invalid argument.
```
To avoid this, clear the flag on `libGalaxy64.so` with `execstack -c libGalaxy64.so` or use the one provided in `demo/addons/godot_gog/linux/` which has it already cleared.

## Compile it yourself
Ensure you have the tooling set up that is required for compiling the Godot Engine. The same tools are used for GDExtensions as well. You can find the details in the official docs here: [Building from source](https://docs.godotengine.org/en/stable/engine_details/development/compiling/)

Copy the `Libraries` and the `Include` folders of the GOG SDK into the `gog-sdk` folder of this repository. Only in this folder are they picked up by the compiler.

You can build the debug and release version with these commands:
```
scons platform=(windows/linux/macos) target=template_debug
scons platform=(windows/linux/macos) target=template_release
```

### Compile for Windows
Cross compilation from Linux/MacOS with MINGW doesn't work. The GOG SDK is compiled with MSVC and can't be linked against the GDExtension compiled with MINGW.
