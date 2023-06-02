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

file(GLOB FEVisualNodeSystem_SRC
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeSystem.cpp"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeSystem.h"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeArea.cpp"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeArea.h"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNode.cpp"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNode.h"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeSocket.cpp"
	"SubSystems/NodeSystem/FEVisualNodeSystem/FEVisualNodeSocket.h"
)

file(GLOB FEVisualNodeSystem_CustomNodes_SRC
	"SubSystems/NodeSystem/CustomNodes/beginNode.h"
	"SubSystems/NodeSystem/CustomNodes/beginNode.cpp"
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

file(GLOB BasicApplicationSubSystem_SRC
	"SubSystems/FEBasicApplication/FEBasicApplication.cpp"
	"SubSystems/FEBasicApplication/FEBasicApplication.h"
	"SubSystems/FEBasicApplication/FEThreadPool.cpp"
	"SubSystems/FEBasicApplication/FEThreadPool.h"
	"SubSystems/FEBasicApplication/FELog.cpp"
	"SubSystems/FEBasicApplication/FELog.h"
	"SubSystems/FEBasicApplication/FETime.cpp"
	"SubSystems/FEBasicApplication/FETime.h"
	"SubSystems/FEBasicApplication/FEUniqueID.cpp"
	"SubSystems/FEBasicApplication/FEUniqueID.h"
)

# *************** THIRD_PARTY ***************

file(GLOB imgui_SRC
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_demo.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_draw.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_impl_glfw.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_impl_opengl3.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_tables.cpp"
	"SubSystems/FEBasicApplication/ThirdParty/imgui/imgui_widgets.cpp"
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

link_directories("SubSystems/FEBasicApplication/ThirdParty/glew2/lib/$(PlatformTarget)")
link_directories("SubSystems/FEBasicApplication/ThirdParty/GLFW/lib/$(PlatformTarget)")

# add the executable
add_executable(FETestPlatform WIN32
							  ${source_SRC}
							  # *************** EDITOR ***************
							  ${FEDearImguiWrapper_SRC}
							  ${SubWindows_SRC}
							  ${FEVisualNodeSystem_SRC}
							  ${FEVisualNodeSystem_CustomNodes_SRC}
							  # *************** SUB_SYSTEM ***************
							  ${SubSystems_SRC}
							  ${BasicApplicationSubSystem_SRC}
							  # *************** THIRD_PARTY ***************
							  ${imgui_SRC}
							  ${jsoncpp_SRC}
							  ${lodepng_SRC}
)

target_link_libraries(FETestPlatform glew32s.lib)
target_link_libraries(FETestPlatform glfw3.lib)
target_link_libraries(FETestPlatform glfw3dll.lib)
target_link_libraries(FETestPlatform opengl32.lib)

source_group("Source Files" FILES ${source_SRC})
# *************** EDITOR ***************
source_group("Source Files/SubSystems/FEDearImguiWrapper" FILES ${FEDearImguiWrapper_SRC})
source_group("Source Files/Windows" FILES ${SubWindows_SRC})
source_group("Source Files/SubSystems/NodeSystem/FEVisualNodeSystem" FILES ${FEVisualNodeSystem_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes" FILES ${FEVisualNodeSystem_CustomNodes_SRC})
# *************** SUB_SYSTEM ***************
source_group("Source Files/SubSystems" FILES ${SubSystems_SRC})
source_group("Source Files/SubSystems/BasicApplication" FILES ${BasicApplicationSubSystem_SRC})
# *************** THIRD_PARTY ***************
source_group("Source Files/ThirdParty/imgui" FILES ${imgui_SRC})
source_group("Source Files/ThirdParty/jsoncpp" FILES ${jsoncpp_SRC})
source_group("Source Files/ThirdParty/lodepng" FILES ${lodepng_SRC})
# set FETestPlatform as the startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT FETestPlatform)

include_directories(
	"SubSystems/FEBasicApplication/ThirdParty/glew2/include"
	"SubSystems/FEBasicApplication/ThirdParty/"
	"SubSystems/FEBasicApplication/"
	"ThirdParty/"
)