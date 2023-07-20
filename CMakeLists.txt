cmake_minimum_required(VERSION 3.1)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(FETestPlatform LANGUAGES CXX)

# set different compiler flags depending on build type
set(CMAKE_CXX_FLAGS_DEBUG "/MP /ZI /W3 /Od /MDd /JMC /sdl /FC /D WIN32 /D IMGUI_IMPL_OPENGL_LOADER_GLEW /D _WINDOWS /D _DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/MP /W3 /O2 /Oi /Gy /sdl /FC /GL /OPT:REF /OPT:ICF /D WIN32 /D IMGUI_IMPL_OPENGL_LOADER_GLEW /D NDEBUG /D _WINDOWS")

# Turn on the ability to create folders to organize projects (.vcproj)
# It creates "CMakePredefinedTargets" folder by default and adds CMake
# defined projects like INSTALL.vcproj and ZERO_CHECK.vcproj
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(NOT TARGET VisualNodeSystem)
	add_subdirectory(SubSystems/NodeSystem/VisualNodeSystem)
endif()

# *************** EDITOR ***************
file(GLOB FEDearImguiWrapper_SRC
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.cpp"
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
)

file(GLOB SubWindows_SRC
	"Windows/actionEditPopup.cpp"
	"Windows/actionEditPopup.h"
	"Windows/failedTestWindow.cpp"
	"Windows/failedTestWindow.h"
	"Windows/nodeRegionWindow.h"
	"Windows/nodeRegionWindow.cpp"
	"Windows/previewWindow.h"
	"Windows/previewWindow.cpp"
	"Windows/screenshootEditor.h"
	"Windows/screenshootEditor.cpp"
	"Windows/testEditorWindow.h"
	"Windows/testEditorWindow.cpp"
	"Windows/testPropertiesWindow.h"
	"Windows/testPropertiesWindow.cpp"
	"Windows/testsOverviewWindow.h"
	"Windows/testsOverviewWindow.cpp"
	"Windows/testStartPreparationsWindow.h"
	"Windows/testStartPreparationsWindow.cpp"
	"Windows/textInputPopup.h"
	"Windows/textInputPopup.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_SRC
	"SubSystems/NodeSystem/CustomNodes/basicLogicNode.h"
	"SubSystems/NodeSystem/CustomNodes/basicLogicNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/boolNode.h"
	"SubSystems/NodeSystem/CustomNodes/boolNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/intNode.h"
	"SubSystems/NodeSystem/CustomNodes/intNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/floatNode.h"
	"SubSystems/NodeSystem/CustomNodes/floatNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/vec2Node.h"
	"SubSystems/NodeSystem/CustomNodes/vec2Node.cpp"
	"SubSystems/NodeSystem/CustomNodes/vec2AddNode.h"
	"SubSystems/NodeSystem/CustomNodes/vec2AddNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/branchNode.h"
	"SubSystems/NodeSystem/CustomNodes/branchNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/sequenceNode.h"
	"SubSystems/NodeSystem/CustomNodes/sequenceNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/timerNode.h"
	"SubSystems/NodeSystem/CustomNodes/timerNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/sleepNode.h"
	"SubSystems/NodeSystem/CustomNodes/sleepNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/beginNode.h"
	"SubSystems/NodeSystem/CustomNodes/beginNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/mouseMoveNode.h"
	"SubSystems/NodeSystem/CustomNodes/mouseMoveNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/mouseLeftButtonDown.h"
	"SubSystems/NodeSystem/CustomNodes/mouseLeftButtonDown.cpp"
	"SubSystems/NodeSystem/CustomNodes/mouseLeftButtonUp.h"
	"SubSystems/NodeSystem/CustomNodes/mouseLeftButtonUp.cpp"
	"SubSystems/NodeSystem/CustomNodes/imageNode.h"
	"SubSystems/NodeSystem/CustomNodes/imageNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/imageSearchNode.h"
	"SubSystems/NodeSystem/CustomNodes/imageSearchNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/combinedActionNode.h"
	"SubSystems/NodeSystem/CustomNodes/combinedActionNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/endNode.h"
	"SubSystems/NodeSystem/CustomNodes/endNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/globalActionNode.h"
	"SubSystems/NodeSystem/CustomNodes/globalActionNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/regionNode.h"
	"SubSystems/NodeSystem/CustomNodes/regionNode.cpp"
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

target_link_libraries(FETestPlatform VisualNodeSystem)

target_link_libraries(FETestPlatform glew32s.lib)
target_link_libraries(FETestPlatform glfw3.lib)
target_link_libraries(FETestPlatform glfw3dll.lib)
target_link_libraries(FETestPlatform opengl32.lib)

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
	"ThirdParty/"
)