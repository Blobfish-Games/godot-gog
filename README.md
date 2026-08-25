# Godot GOG
GDNative plugin for the GOG Galaxy SDK. Currently it only supports getting/setting Achievements. However, if you need more of the SDK exposed, it's quite easy to follow the existing implementation and add what you need.

Currently only tested on 3.6 but should be compatible with any 3.x version by downgrading the godot-cpp submodule.

## How To
Copy the `demo/addons/godot_gog` folder into your games `addons/` folder. Now if you start your game you can attach a native script to a node pointing to the `res://addons/godot_gog/godot_gog.gdns` file. 

For a more detailed setup guide, check out the demo project in `demo/`.

Linux is currently not supported by GOG Galaxy.

## Compile it yourself
Ensure you have the tooling set up that is required for compiling the Godot Engine. The same tools are used for GDNative as well. You can find the details in the official docs here: [Compiling](https://docs.godotengine.org/en/3.6/development/compiling/index.html)

Copy the `Libraries` and the `Include` folders of the GOG SDK into the `gog-sdk` folder of this repository. Only in this folder are they picked up by the compiler.

Initialize the `godot-cpp` submodule and generate the bindings of it:
```
cd godot-cpp
git submodule update --init
scons platform=(windows/osx) generate_bindings=yes target=debug
scons platform=(windows/osx) generate_bindings=yes target=release
cd ..
```

You can then build either a debug or release version with the commands:
```
scons platform=(windows/osx) target=(debug/release)
```

### Compile for Windows
Cross compilation from Linux/MacOS with MINGW doesn't work. The GOG SDK is compiled with MSVC and can't be linked against the GDExtension compiled with MINGW.
