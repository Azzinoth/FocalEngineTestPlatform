cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(FETestPlatform LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set different compiler flags depending on build type
set(CMAKE_CXX_FLAGS_DEBUG "/MP /Zi /MTd /INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF /Od")
set(CMAKE_CXX_FLAGS_RELEASE "/MP /MT")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT TARGET FEBasicApplication)
  add_subdirectory(SubSystems/FEBasicApplication)
endif()

# Force your path to Dear ImGui.
set(DEAR_IMGUI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/SubSystems/FEBasicApplication/ThirdParty/imgui CACHE PATH "Path to Dear ImGui directory." FORCE)

# Force to build standard nodes.
set(VISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES ON CACHE BOOL "Build the Visual Node System with standard nodes module" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DVISUAL_NODE_SYSTEM_BUILD_STANDARD_NODES")

if(NOT TARGET VisualNodeSystem)
	add_subdirectory(SubSystems/NodeSystem/VisualNodeSystem)
endif()

# *************** EDITOR ***************
file(GLOB FEDearImguiWrapper_SRC
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.cpp"
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
)

file(GLOB SubWindows_SRC
	"Windows/ActionEditPopup.cpp"
	"Windows/ActionEditPopup.h"
	"Windows/FailedTestWindow.cpp"
	"Windows/FailedTestWindow.h"
	"Windows/NodeRegionWindow.h"
	"Windows/NodeRegionWindow.cpp"
	"Windows/PreviewWindow.h"
	"Windows/PreviewWindow.cpp"
	"Windows/TestEditorWindow.h"
	"Windows/TestEditorWindow.cpp"
	"Windows/TestPropertiesWindow.h"
	"Windows/TestPropertiesWindow.cpp"
	"Windows/TestsOverviewWindow.h"
	"Windows/TestsOverviewWindow.cpp"
	"Windows/TestStartPreparationsWindow.h"
	"Windows/TestStartPreparationsWindow.cpp"
	"Windows/TextInputPopup.h"
	"Windows/TextInputPopup.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_SRC
	"SubSystems/NodeSystem/CustomNodes/BeginNode.h"
	"SubSystems/NodeSystem/CustomNodes/BeginNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/TimerNode.h"
	"SubSystems/NodeSystem/CustomNodes/TimerNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/SleepNode.h"
	"SubSystems/NodeSystem/CustomNodes/SleepNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/ImageNode.h"
	"SubSystems/NodeSystem/CustomNodes/ImageNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/ImageSearchNode.h"
	"SubSystems/NodeSystem/CustomNodes/ImageSearchNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/MouseMoveNode.h"
	"SubSystems/NodeSystem/CustomNodes/MouseMoveNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/MouseLeftButtonDown.h"
	"SubSystems/NodeSystem/CustomNodes/MouseLeftButtonDown.cpp"
	"SubSystems/NodeSystem/CustomNodes/MouseLeftButtonUp.h"
	"SubSystems/NodeSystem/CustomNodes/MouseLeftButtonUp.cpp"

	"SubSystems/NodeSystem/CustomNodes/TextInputNode.h"
	"SubSystems/NodeSystem/CustomNodes/TextInputNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/LunchApplicationNode.h"
	"SubSystems/NodeSystem/CustomNodes/LunchApplicationNode.cpp"
)

# *************** SUB_SYSTEM ***************

file(GLOB SubSystems_SRC
	"SubSystems/FECoreIncludes.h"
	"SubSystems/FEFileSystem.cpp"
	"SubSystems/FEFileSystem.h"
	"SubSystems/FEPTActionSystem.cpp"
	"SubSystems/FEPTActionSystem.h"
	"SubSystems/FETest.cpp"
	"SubSystems/FETest.h"
	"SubSystems/FETPAction.cpp"
	"SubSystems/FETPAction.h"
	"SubSystems/FETPImage.cpp"
	"SubSystems/FETPImage.h"
	"SubSystems/FETPInput.cpp"
	"SubSystems/FETPInput.h"
	"SubSystems/FETPComputeShaderCompare.cpp"
	"SubSystems/FETPComputeShaderCompare.h"
	"SubSystems/FETPScreenCapture.cpp"
	"SubSystems/FETPScreenCapture.h"
	"SubSystems/FETPScreen.cpp"
	"SubSystems/FETPScreen.h"
	"SubSystems/TestManager.cpp"
	"SubSystems/TestManager.h"
)

# *************** THIRD_PARTY ***************

file(GLOB lodepng_SRC
	"ThirdParty/lodepng/lodepng.cpp"
	"ThirdParty/lodepng/lodepng.h"
)

file(GLOB source_SRC
	"FETestPlatform.cpp"
	"FETestPlatform.h"
	"main.cpp"
)

link_directories(${GLEW_LIB_DIR})
link_directories(${GLFW_LIB_DIR})

# add the executable
add_executable(FETestPlatform WIN32
							  ${source_SRC}
							  # *************** EDITOR ***************
							  ${FEDearImguiWrapper_SRC}
							  ${SubWindows_SRC}
							  ${VisualNodeSystem_CustomNodes_SRC}
							  # *************** SUB_SYSTEM ***************
							  ${SubSystems_SRC}
							  # *************** THIRD_PARTY ***************
							  ${lodepng_SRC}
)

# This will ensure the manifest is used when linking the executable.
set_target_properties(FETestPlatform PROPERTIES
                      LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" 
                      /MANIFEST:embed
                      /MANIFESTINPUT:\"${CMAKE_SOURCE_DIR}/FETestPlatform.manifest\"")

target_link_libraries(FETestPlatform FEBasicApplication)
target_link_libraries(FETestPlatform VisualNodeSystem)

if(USE_STATIC_RUNTIME)
    add_definitions(-DGLEW_STATIC)
endif()

source_group("Source Files" FILES ${source_SRC})
# *************** EDITOR ***************
source_group("Source Files/SubSystems/FEDearImguiWrapper" FILES ${FEDearImguiWrapper_SRC})
source_group("Source Files/Windows" FILES ${SubWindows_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes" FILES ${VisualNodeSystem_CustomNodes_SRC})
# *************** SUB_SYSTEM ***************
source_group("Source Files/SubSystems" FILES ${SubSystems_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/lodepng" FILES ${lodepng_SRC})

# set the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FETestPlatform)

include_directories(
	${GLEW_INCLUDE_DIR}
	${BASICAPP_THIRDPARTY_DIR}
	${BASICAPP_DIR}
	${VISUAL_NODE_SYSTEM_THIRDPARTY_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/SubSystems/FEBasicApplication/ThirdParty/imgui
	"ThirdParty/"
)