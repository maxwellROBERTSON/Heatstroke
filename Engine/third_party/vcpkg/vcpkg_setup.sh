cd Engine/third_party/vcpkg

if [ ! -x "vcpkg" ]; then
    echo "Running bootstrap-vcpkg.sh..."
    ./bootstrap-vcpkg.sh
fi

# Set the VCPKG_ROOT environment variable
export VCPKG_ROOT="${PWD}"

# Add vcpkg to PATH
export PATH="$VCPKG_ROOT:$PATH"

# Install PhysX using vcpkg
echo "Installing PhysX using vcpkg..."
./vcpkg install physx

# Set VCPKG_TOOLCHAIN for use with Premake and CMake
export VCPKG_TOOLCHAIN="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

cd ../../..
