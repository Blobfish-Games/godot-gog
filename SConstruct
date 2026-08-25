#!/usr/bin/env python
import os
import sys


libname = "godot_gog"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

# add GOG SDK to scons
env.Append(CPPPATH="gog-sdk/Include/")
env.Append(LIBPATH="gog-sdk/Libraries/")
if env['platform'] == "windows":
     env.Append(LIBS="Galaxy64")
if env['platform'] == "linux":
     env.Append(LIBS="libGalaxy64")
if env['platform'] == "macos":
    env.Append(LIBS="libGalaxy")
    # Mac specific handling of rpath to find shared libraries located in the same folder as libgodot_gog.dylib
    env.Append(LINKFLAGS=["-rpath", "@loader_path/."])

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

copy = env.Install("{}/addons/{}/{}/".format(projectdir, libname, env["platform"]), library)

default_args = [library, copy]
Default(*default_args)
