cmake_minimum_required(VERSION 3.1)

# set default build to WIN32/X86
set(CMAKE_VS_PLATFORM_NAME_DEFAULT Win32)
set(CMAKE_GENERATOR_PLATFORM Win32)
set(CMAKE_GENERATOR_TOOLSET host=x86)

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

file(GLOB FEEditorNodeSystem_SRC
	"SubSystems/NodeSystem/FEEditorNodeSystem.cpp"
	"SubSystems/NodeSystem/FEEditorNodeSystem.h"
	"SubSystems/NodeSystem/FEEditorNodeArea.cpp"
	"SubSystems/NodeSystem/FEEditorNodeArea.h"
	"SubSystems/NodeSystem/FEEditorNode.cpp"
	"SubSystems/NodeSystem/FEEditorNode.h"
	"SubSystems/NodeSystem/FEEditorNodeSocket.cpp"
	"SubSystems/NodeSystem/FEEditorNodeSocket.h"
)

file(GLOB FEEditorNodeSystem_StandardNodes_SRC
	"SubSystems/NodeSystem/StandardNodes/beginNode.h"
	"SubSystems/NodeSystem/StandardNodes/beginNode.cpp"
	"SubSystems/NodeSystem/StandardNodes/combinedActionNode.h"
	"SubSystems/NodeSystem/StandardNodes/combinedActionNode.cpp"
	"SubSystems/NodeSystem/StandardNodes/endNode.h"
	"SubSystems/NodeSystem/StandardNodes/endNode.cpp"
	"SubSystems/NodeSystem/StandardNodes/globalActionNode.h"
	"SubSystems/NodeSystem/StandardNodes/globalActionNode.cpp"
	"SubSystems/NodeSystem/StandardNodes/regionNode.h"
	"SubSystems/NodeSystem/StandardNodes/regionNode.cpp"
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
	"SubSystems/FETPScreen.cpp"
	"SubSystems/FETPScreen.h"
	"SubSystems/TestManager.cpp"
	"SubSystems/TestManager.h"
)

# *************** THIRD_PARTY ***************

file(GLOB imgui_SRC
	"ThirdParty/imgui/imgui.cpp"
	"ThirdParty/imgui/imgui_demo.cpp"
	"ThirdParty/imgui/imgui_draw.cpp"
	"ThirdParty/imgui/imgui_impl_glfw.cpp"
	"ThirdParty/imgui/imgui_impl_opengl3.cpp"
	"ThirdParty/imgui/imgui_tables.cpp"
	"ThirdParty/imgui/imgui_widgets.cpp"
)

file(GLOB jsoncpp_SRC
	"ThirdParty/jsoncpp/json_reader.cpp"
	"ThirdParty/jsoncpp/json_tool.h"
	"ThirdParty/jsoncpp/json_value.cpp"
	"ThirdParty/jsoncpp/json_valueiterator.inl"
	"ThirdParty/jsoncpp/json_writer.cpp"
)

file(GLOB lodepng_SRC
	"ThirdParty/lodepng/lodepng.cpp"
	"ThirdParty/lodepng/lodepng.h"
)

file(GLOB source_SRC
	"FETestPlatform.cpp"
	"FETestPlatform.h"
	"main.cpp"
)

link_directories("ThirdParty/glew2/lib")
link_directories("ThirdParty/GLFW/lib")

# add the executable
add_executable(FETestPlatform WIN32
							  ${source_SRC}
							  # *************** EDITOR ***************
							  ${FEDearImguiWrapper_SRC}
							  ${SubWindows_SRC}
							  ${FEEditorNodeSystem_SRC}
							  ${FEEditorNodeSystem_StandardNodes_SRC}
							  # *************** SUB_SYSTEM ***************
							  ${SubSystems_SRC}
							  # *************** THIRD_PARTY ***************
							  ${imgui_SRC}
							  ${jsoncpp_SRC}
							  ${lodepng_SRC}
)

target_link_libraries(FETestPlatform glew32.lib)
target_link_libraries(FETestPlatform glfw3.lib)
target_link_libraries(FETestPlatform glfw3dll.lib)
target_link_libraries(FETestPlatform opengl32.lib)

source_group("Source Files" FILES ${source_SRC})
# *************** EDITOR ***************
source_group("Source Files/SubSystems/FEDearImguiWrapper" FILES ${FEDearImguiWrapper_SRC})
source_group("Source Files/Windows" FILES ${SubWindows_SRC})
source_group("Source Files/SubSystems/NodeSystem" FILES ${FEEditorNodeSystem_SRC})
source_group("Source Files/SubSystems/NodeSystem/StandardNodes" FILES ${FEEditorNodeSystem_StandardNodes_SRC})
# *************** SUB_SYSTEM ***************
source_group("Source Files/SubSystems" FILES ${SubSystems_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/imgui" FILES ${imgui_SRC})
source_group("Source Files/ThirdParty/jsoncpp" FILES ${jsoncpp_SRC})
source_group("Source Files/ThirdParty/lodepng" FILES ${lodepng_SRC})
# set FETestPlatform as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FETestPlatform)

include_directories(
	"ThirdParty/glew2/include"
	"ThirdParty/"
)