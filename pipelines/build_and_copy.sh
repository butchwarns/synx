#! /bin/bash -e

# Disable shell debug output
set +x

PLUGIN="synx"

ROOT=$(cd "$(dirname "$0")/.."; pwd)
cd "$ROOT"

# Check the first argument for build type
if [ "$1" == "debug" ]; then
    BUILD_TYPE="Debug"
elif [ "$1" == "release" ]; then
    BUILD_TYPE="Release"
else
    echo "Invalid argument. Please specify 'debug' or 'release'."
    exit 1
fi

# Make folder for built plugins
cd "$ROOT/pipelines"
rm -Rf bin
mkdir bin

# Build plugin
cd "$ROOT/build"
cmake .. -GUnix\ Makefiles -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=$BUILD_TYPE
cmake --build . --config $BUILD_TYPE

# Copy plugin builds to dedicated folder
cp -R "$ROOT/build/${PLUGIN}_artefacts/$BUILD_TYPE/AU/$PLUGIN.component" "$ROOT/pipelines/bin"
cp -R "$ROOT/build/${PLUGIN}_artefacts/$BUILD_TYPE/VST3/$PLUGIN.vst3" "$ROOT/pipelines/bin"