# Heatstroke
A multiplayer fps engine

To run on windows:
> premake5 vs2022

or

> premake5 gmake2
make

To run on linux:
Make current directory == Heatstroke root
> export LD_LIBRARY_PATH=$(pwd)/Engine/third_party/libsodium-1.0.20/libsodium-build/lib:$(pwd)/Engine/third_party/vcpkg/installed/x64-linux/tools:$LD_LIBRARY_PATH

> ./premake5 gmake2

> make -j
