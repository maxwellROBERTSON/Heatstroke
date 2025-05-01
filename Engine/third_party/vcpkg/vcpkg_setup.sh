cd Engine/third_party/vcpkg

if [ ! -x "vcpkg" ]; then
    echo "Running bootstrap-vcpkg.sh..."
    ./bootstrap-vcpkg.sh
fi

# Set the VCPKG_ROOT environment variable
export VCPKG_ROOT="${PWD}"

# Add vcpkg to PATH
export PATH="$VCPKG_ROOT:$PATH"

INSTALLED_PHYSX=$(./vcpkg list | grep "physx")

# Check if physx is installed
if [ -z "$INSTALLED_physx" ]; then
    echo "physx is not installed. Installing..."
    ./vcpkg install physx
else
    echo "physx is already installed:"
    echo "$INSTALLED_PHYSX"
fi

INSTALLED_OPENAL_SOFT=$(./vcpkg list | grep "openal-soft")

# Check if openal-soft is installed
if [ -z "$INSTALLED_OPENAL_SOFT" ]; then
    echo "openal-soft is not installed. Installing..."
    ./vcpkg install openal-soft
else
    echo "openal-soft is already installed:"
    echo "$INSTALLED_OPENAL_SOFT"
fi

INSTALLED_LIBSNDFILE=$(./vcpkg list | grep "libsndfile")

# Check if libsndfile is installed
if [ -z "$INSTALLED_LIBSNDFILE" ]; then
    echo "libsndfile is not installed. Installing..."
    ./vcpkg install libsndfile
else
    echo "libsndfile is already installed:"
    echo "$INSTALLED_LIBSNDFILE"
fi

# Check if vulkan is installed
if command -v vulkaninfo &>/dev/null; then
    echo "Vulkan is installed."
else
    echo "Vulkan SDK is not installed."
    read -p "Do you want to install Vulkan SDK using vcpkg? (Y:N):" choice
    case "$choice" in
        [Yy]*)
            echo "Installing Vulkan SDK using vcpkg..."
            ./vcpkg install vulkan
            echo "Vulkan SDK has been installed successfully!"
            ;;
        *)
            echo "Vulkan SDK installation skipped."
            ;;
    esac
fi

# Set VCPKG_TOOLCHAIN for use with Premake and CMake
export VCPKG_TOOLCHAIN="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

cd ../../..
