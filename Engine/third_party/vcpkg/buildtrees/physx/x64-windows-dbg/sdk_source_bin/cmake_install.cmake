# Install script for directory: C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/source/compiler/cmake

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/install/vc17win64/PhysX")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation/windows" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsMathIntrinsics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsIntrinsics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsAoS.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsInlineAoS.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsTrigConstants.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsInclude.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/windows/PxWindowsFPU.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXFoundation_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/foundation" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxFoundation.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAssert.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxFoundationConfig.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMathUtils.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAlignedMalloc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAllocatorCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxProfiler.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAoS.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAlloca.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAllocator.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxArray.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxAtomic.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBasicTemplates.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBitMap.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBitAndData.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBitUtils.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBounds3.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxBroadcast.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxConstructor.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxErrorCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxErrors.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxFlags.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxFPU.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxInlineAoS.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxIntrinsics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxHash.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxHashInternals.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxHashMap.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxHashSet.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxInlineAllocator.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxInlineArray.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxPinnedArray.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMathIntrinsics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMutex.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxIO.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMat33.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMat34.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMat44.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMath.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxMemory.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxPlane.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxPool.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxPreprocessor.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxQuat.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxPhysicsVersion.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSortInternals.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSimpleTypes.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSList.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSocket.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSort.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxStrideIterator.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxString.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSync.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxTempAllocator.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxThread.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxTransform.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxTime.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxUnionCast.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxUserAllocated.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxUtilities.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVec2.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVec3.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVec4.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecMath.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecMathAoSScalar.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecMathAoSScalarInline.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecMathSSE.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecQuat.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxVecTransform.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/foundation/PxSIMDHelpers.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/gpu" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/gpu/PxGpu.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/gpu/PxPhysicsGpu.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cudamanager" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cudamanager/PxCudaContextManager.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cudamanager/PxCudaContext.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cudamanager/PxCudaTypes.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common/windows" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/windows/PxWindowsDelayLoadHook.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysX_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysX_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxActor.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxActorData.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxAggregate.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationJointReducedCoordinate.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationLink.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationReducedCoordinate.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationTendon.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationTendonData.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArticulationMimicJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxBroadPhase.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxClient.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxConeLimitedConstraint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxConstraint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxConstraintDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxContact.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxContactModifyCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableAttachment.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableElementFilter.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableBody.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableBodyFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableSurface.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableSurfaceFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableVolume.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableVolumeFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeletionListener.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxFEMParameter.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxFiltering.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxForceMode.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxImmediateMode.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxLockedData.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxNodeIndex.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleBuffer.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleGpu.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleSolverType.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleSystem.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleSystemFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPBDParticleSystem.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPhysics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPhysicsAPI.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPhysicsSerialization.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPhysXConfig.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPruningStructure.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxQueryFiltering.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxQueryReport.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxRigidActor.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxRigidBody.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxRigidDynamic.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxRigidStatic.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxScene.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSceneDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSceneLock.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSceneQueryDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSceneQuerySystem.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxShape.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSimulationEventCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSimulationStatistics.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSoftBody.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSoftBodyFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSparseGridParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxVisualizationParameter.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxIsosurfaceExtraction.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSmoothing.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxAnisotropy.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleNeighborhoodProvider.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxArrayConverter.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxSDFBuilder.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxResidual.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDirectGPUAPI.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableSkinning.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxBaseMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableSurfaceMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxDeformableVolumeMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxFEMMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxFEMSoftBodyMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxParticleMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxPBDMaterial.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxMaterial.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/common" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxBase.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxCollection.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxCoreUtilityTypes.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxInsertionCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxMetaData.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxMetaDataFlags.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxPhysXCommonConfig.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxProfileZone.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxRenderBuffer.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxRenderOutput.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxSerialFramework.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxSerializer.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxStringTable.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxTolerancesScale.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/common/PxTypeInfo.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/pvd" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/pvd/PxPvdSceneClient.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/pvd/PxPvd.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/pvd/PxPvdTransport.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/omnipvd" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/omnipvd/PxOmniPvd.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/collision" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/collision/PxCollisionDefs.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/solver" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/solver/PxSolverDefs.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/PxConfig.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCharacterKinematic_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCharacterKinematic_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/characterkinematic" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxBoxController.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxCapsuleController.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxController.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxControllerBehavior.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxControllerManager.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxControllerObstacles.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/characterkinematic/PxExtended.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCommon_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geometry" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxBoxGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxCapsuleGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxConvexMesh.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxConvexMeshGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxCustomGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxConvexCoreGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryInternal.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryHit.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryQuery.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryQueryFlags.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGeometryQueryContext.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxHeightField.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxHeightFieldDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxHeightFieldFlag.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxHeightFieldGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxHeightFieldSample.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxMeshQuery.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxMeshScale.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxPlaneGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxReportCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxSimpleTriangleMesh.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxSphereGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTriangle.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTriangleMesh.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTriangleMeshGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxBVH.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxBVHBuildStrategy.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTetrahedron.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTetrahedronMesh.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxTetrahedronMeshGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxParticleSystemGeometry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geometry/PxGjkQuery.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/geomutils" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geomutils/PxContactBuffer.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/geomutils/PxContactPoint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCooking_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cooking" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxBVH33MidphaseDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxBVH34MidphaseDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/Pxc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxConvexMeshDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxCooking.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxCookingInternal.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxMidphaseDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxTriangleMeshDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxTetrahedronMeshDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxBVHDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxTetrahedronMeshDesc.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/cooking/PxSDFDesc.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXExtensions_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXExtensions_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxBinaryConverter.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxBroadPhaseExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxCollectionExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxConvexMeshExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxCudaHelpersExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDefaultAllocator.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDefaultCpuDispatcher.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDefaultErrorCallback.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDefaultSimulationFilterShader.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDefaultStreams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDeformableSurfaceExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDeformableVolumeExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxExtensionsAPI.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxMassProperties.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRaycastCCD.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRepXSerializer.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRepXSimpleType.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRigidActorExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRigidBodyExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSceneQueryExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSceneQuerySystemExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxCustomSceneQuerySystem.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSerialization.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxShapeExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSimpleFactory.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSmoothNormals.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSoftBodyExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxStringTableExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxTriangleMeshExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxTetrahedronMeshExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRemeshingExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxTriangleMeshAnalysisResult.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxTetrahedronMeshAnalysisResult.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxTetMakerExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxGjkQueryExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxCustomGeometryExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSamplingExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxConvexCoreExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDeformableSkinningExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxParticleClothCooker.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxParticleExt.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/extensions" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxConstraintExt.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxContactJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxD6Joint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxD6JointCreate.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxDistanceJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxContactJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxFixedJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxGearJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRackAndPinionJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxJointLimit.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxPrismaticJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxRevoluteJoint.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/extensions/PxSphericalJoint.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/filebuf" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/filebuf/PxFileBuf.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXVehicle_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXVehicle_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleDrive.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleDrive4W.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleDriveNW.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleDriveTank.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleNoDrive.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleSDK.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleShaders.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleTireFriction.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleUpdate.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleUtil.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleUtilControl.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleUtilSetup.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleUtilTelemetry.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle/PxVehicleWheels.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXVehicle2_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXVehicle2_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXVehicle2_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXVehicle2_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleAPI.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleComponent.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleComponentSequence.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleLimits.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/PxVehicleMaths.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/braking" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/braking/PxVehicleBrakingFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/braking/PxVehicleBrakingParams.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/commands" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/commands/PxVehicleCommandHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/commands/PxVehicleCommandParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/commands/PxVehicleCommandStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/drivetrain" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/drivetrain/PxVehicleDrivetrainComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/drivetrain/PxVehicleDrivetrainFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/drivetrain/PxVehicleDrivetrainHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/drivetrain/PxVehicleDrivetrainParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/drivetrain/PxVehicleDrivetrainStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/physxActor" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxActor/PxVehiclePhysXActorComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxActor/PxVehiclePhysXActorFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxActor/PxVehiclePhysXActorHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxActor/PxVehiclePhysXActorStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/physxConstraints" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxConstraints/PxVehiclePhysXConstraintStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/physxRoadGeometry" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/physxRoadGeometry/PxVehiclePhysXRoadGeometryState.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/rigidBody" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/rigidBody/PxVehicleRigidBodyComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/rigidBody/PxVehicleRigidBodyFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/rigidBody/PxVehicleRigidBodyParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/rigidBody/PxVehicleRigidBodyStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/roadGeometry" TYPE FILE FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/roadGeometry/PxVehicleRoadGeometryState.h")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/steering" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/steering/PxVehicleSteeringFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/steering/PxVehicleSteeringParams.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/suspension" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/suspension/PxVehicleSuspensionComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/suspension/PxVehicleSuspensionFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/suspension/PxVehicleSuspensionParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/suspension/PxVehicleSuspensionStates.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/suspension/PxVehicleSuspensionHelpers.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/tire" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/tire/PxVehicleTireComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/tire/PxVehicleTireFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/tire/PxVehicleTireHelpers.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/tire/PxVehicleTireParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/tire/PxVehicleTireStates.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/wheel" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/wheel/PxVehicleWheelComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/wheel/PxVehicleWheelFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/wheel/PxVehicleWheelParams.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/wheel/PxVehicleWheelStates.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/wheel/PxVehicleWheelHelpers.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/vehicle2/pvd" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/pvd/PxVehiclePvdComponents.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/pvd/PxVehiclePvdFunctions.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/vehicle2/pvd/PxVehiclePvdHelpers.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXPvdSDK_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXPvdSDK_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXTask_static_64.pdb")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE FILE OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXTask_static_64.pdb")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/task" TYPE FILE FILES
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/task/PxCpuDispatcher.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/task/PxTask.h"
    "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/include/task/PxTaskManager.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXFoundation_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXFoundation_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXFoundation_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXFoundation.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXFoundation.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXFoundation.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXFoundation.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysX_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysX_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysX_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysX_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysX.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysX.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysX.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysX.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCharacterKinematic_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCharacterKinematic_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCharacterKinematic.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCharacterKinematic.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCharacterKinematic.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCharacterKinematic.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXPvdSDK_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXPvdSDK_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXPvdSDK.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXPvdSDK.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXPvdSDK.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXPvdSDK.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCommon_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCommon_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCommon_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCommon.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCommon.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCommon.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCommon.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCooking_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXCooking_64.dll")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE SHARED_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXCooking_64.dll")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCooking.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCooking.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCooking.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXCooking.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXExtensions_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXExtensions_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXExtensions.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXExtensions.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXExtensions.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXExtensions.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXVehicle_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXVehicle_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXVehicle2_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXVehicle2_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXVehicle2_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXVehicle2_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle2.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle2.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle2.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXVehicle2.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/debug" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/debug/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/checked" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/checked/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/profile" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/profile/PhysXTask_static_64.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin/win.x86_64.vc143.md/release" TYPE STATIC_LIBRARY FILES "C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/src/hysx-5.5.0-86566ce8b0.clean/physx/bin/win.x86_64.vc143.md/release/PhysXTask_static_64.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXTask.dir/install-cxx-module-bmi-debug.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Cc][Hh][Ee][Cc][Kk][Ee][Dd])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXTask.dir/install-cxx-module-bmi-checked.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Pp][Rr][Oo][Ff][Ii][Ll][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXTask.dir/install-cxx-module-bmi-profile.cmake" OPTIONAL)
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    include("C:/dev/Heatstroke/Engine/third_party/vcpkg/buildtrees/physx/x64-windows-dbg/sdk_source_bin/CMakeFiles/PhysXTask.dir/install-cxx-module-bmi-release.cmake" OPTIONAL)
  endif()
endif()

