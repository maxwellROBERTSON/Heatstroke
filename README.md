# Masters_Group_Project_Network
A initial creation of a networking solution to be used in my Master's group project using yojimbo.

<!-- To run on windows: -->
./premake5 vs2022
<!-- or -->
./premake5 gmake2
make

<!-- To run on linux: -->
export LD_LIBRARY_PATH=$[PATH_TO_HEATSTROKE_ROOT_DIRECTORY]/Engine/third_party/libsodium-stable/include_lib/lib
<!-- or when current directory == Heatstroke root -->
export LD_LIBRARY_PATH=$(pwd)/Engine/third_party/libsodium-stable/include_lib/lib

./premake5 gmake2
make