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

# Check if PhysX is installed
if [ -z "$INSTALLED_PHYSX" ]; then
    echo "PhysX is not installed. Installing..."
    ./vcpkg install physx
else
    echo "PhysX is already installed:"
    echo "$INSTALLED_PHYSX"
fi

INSTALLED_OPENAL=$(./vcpkg list | grep "openal-soft")

# Check if PhysX is installed
if [ -z "$INSTALLED_OPENAL" ]; then
    echo "OpenAL is not installed. Installing..."
    ./vcpkg install openal-soft
else
    echo "OpenAL is already installed:"
    echo "$INSTALLED_OPENAL"
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
