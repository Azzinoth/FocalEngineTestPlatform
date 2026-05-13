cmake_minimum_required(VERSION 3.10)

set(BUILD_TYPE "Debug and Release" CACHE STRING "Choose Build type")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# set the project name
project(FETestPlatform LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set different compiler flags depending on build type
set(CMAKE_CXX_FLAGS_DEBUG "/MP /Zi /MTd /INCREMENTAL:NO /DEBUG /Od")
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

# Force to build execution flow nodes.
set(VISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES ON CACHE BOOL "Build the Visual Node System with execution flow nodes module" FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DVISUAL_NODE_SYSTEM_BUILD_EXECUTION_FLOW_NODES")

if(NOT TARGET VisualNodeSystem)
	add_subdirectory(SubSystems/NodeSystem/VisualNodeSystem)
endif()

if(NOT TARGET FESceneGraphUI)
    add_subdirectory(SubSystems/FESceneGraphUI)
endif()

# *************** EDITOR ***************
file(GLOB FEDearImguiWrapper_SRC
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.cpp"
	"SubSystems/FEDearImguiWrapper/FEDearImguiWrapper.h"
)

file(GLOB NodeAreaWindows_SRC
	"Windows/NodeArea/NodeAreaWindow.h"
	"Windows/NodeArea/NodeAreaWindow.cpp"
	"Windows/NodeArea/NodeAreaGraphBackend.h"
	"Windows/NodeArea/NodeAreaGraphBackend.cpp"
	"Windows/NodeArea/GraphWindow.h"
	"Windows/NodeArea/GraphWindow.cpp"
	"Windows/NodeArea/NodeAreaWindowManager.h"
	"Windows/NodeArea/NodeAreaWindowManager.cpp"
)

file(GLOB SubWindows_SRC
	"Windows/ActionEditPopup.cpp"
	"Windows/ActionEditPopup.h"
	"Windows/FailedTestWindow.cpp"
	"Windows/FailedTestWindow.h"
	"Windows/PreviewWindow.h"
	"Windows/PreviewWindow.cpp"
	"Windows/TestPropertiesWindow.h"
	"Windows/TestPropertiesWindow.cpp"
	"Windows/TestsOverviewWindow.h"
	"Windows/TestsOverviewWindow.cpp"
	"Windows/TestStartPreparationsWindow.h"
	"Windows/TestStartPreparationsWindow.cpp"
	"Windows/TextInputPopup.h"
	"Windows/TextInputPopup.cpp"
	"Windows/LinkAreaSelectionPopup.h"
	"Windows/LinkAreaSelectionPopup.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_SRC
	"SubSystems/NodeSystem/CustomNodes/BeginNode.h"
	"SubSystems/NodeSystem/CustomNodes/BeginNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_Images_SRC
	"SubSystems/NodeSystem/CustomNodes/Images/ImageLiteralNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageLiteralNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageVariableNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageVariableNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageLoadNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageLoadNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageSaveNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageSaveNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageSearchNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/ImageSearchNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Images/RecognizeTextNode.h"
	"SubSystems/NodeSystem/CustomNodes/Images/RecognizeTextNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_Timing_SRC
	"SubSystems/NodeSystem/CustomNodes/Timing/SleepNode.h"
	"SubSystems/NodeSystem/CustomNodes/Timing/SleepNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Timing/TimerNode.h"
	"SubSystems/NodeSystem/CustomNodes/Timing/TimerNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_System_SRC
	"SubSystems/NodeSystem/CustomNodes/System/LaunchApplicationNode.h"
	"SubSystems/NodeSystem/CustomNodes/System/LaunchApplicationNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_Strings_SRC
	"SubSystems/NodeSystem/CustomNodes/Strings/StringLiteralNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringLiteralNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringVariableNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringVariableNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringNumberNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringNumberNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringEmptyNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringEmptyNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringIntegerNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/IsStringIntegerNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/Strings/StringEqualsNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringEqualsNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringContainsNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringContainsNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringStartsWithNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringStartsWithNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringEndsWithNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringEndsWithNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/Strings/StringLengthNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringLengthNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToIntNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToIntNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToFloatNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToFloatNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/Strings/StringConcatenateNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringConcatenateNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringSubstringNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringSubstringNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringReplaceNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringReplaceNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToUpperNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToUpperNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToLowerNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringToLowerNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringTrimNode.h"
	"SubSystems/NodeSystem/CustomNodes/Strings/StringTrimNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_FileSystem_SRC
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractFileNameNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractFileNameNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractFileExtensionNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractFileExtensionNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractDirectoryPathNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/ExtractDirectoryPathNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetAbsolutePathNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetAbsolutePathNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DoesFileExistNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DoesFileExistNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DoesDirectoryExistNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DoesDirectoryExistNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetFileSizeNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetFileSizeNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetCurrentWorkingPathNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetCurrentWorkingPathNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetTestWorkingPathNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/GetTestWorkingPathNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/FileSystem/CopyFileNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/CopyFileNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/RenameFileNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/RenameFileNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DeleteFileNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DeleteFileNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/FileSystem/CreateDirectoryNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/CreateDirectoryNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/CopyDirectoryNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/CopyDirectoryNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/RenameDirectoryNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/RenameDirectoryNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DeleteDirectoryNode.h"
	"SubSystems/NodeSystem/CustomNodes/FileSystem/DeleteDirectoryNode.cpp"
)

file(GLOB VisualNodeSystem_CustomNodes_InputSimulation_SRC
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseMoveNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseMoveNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonDownNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonDownNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonUpNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseLeftButtonUpNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseRightButtonDownNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseRightButtonDownNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseRightButtonUpNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/MouseRightButtonUpNode.cpp"

	"SubSystems/NodeSystem/CustomNodes/InputSimulation/KeyboardKeyDownNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/KeyboardKeyDownNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/KeyboardKeyUpNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/KeyboardKeyUpNode.cpp"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/TextInputNode.h"
	"SubSystems/NodeSystem/CustomNodes/InputSimulation/TextInputNode.cpp"
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
							  ${NodeAreaWindows_SRC}
							  ${VisualNodeSystem_CustomNodes_SRC}
							  ${VisualNodeSystem_CustomNodes_Images_SRC}
							  ${VisualNodeSystem_CustomNodes_Timing_SRC}
							  ${VisualNodeSystem_CustomNodes_System_SRC}
							  ${VisualNodeSystem_CustomNodes_Strings_SRC}
							  ${VisualNodeSystem_CustomNodes_FileSystem_SRC}
							  ${VisualNodeSystem_CustomNodes_InputSimulation_SRC}
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

set(Tesseract_Debug   ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tesseract/lib/Debug/tesseract55d.lib)
set(Tesseract_Release ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tesseract/lib/Release/tesseract55.lib)

set(Leptonica_Debug   ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/leptonica/lib/Debug/leptonica-1.87.0d.lib)
set(Leptonica_Release ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/leptonica/lib/Release/leptonica-1.87.0.lib)

target_link_libraries(FETestPlatform 
	PRIVATE 
		FEBasicApplication
		VisualNodeSystem
		FESceneGraphUI
	PUBLIC 
		debug	  ${Tesseract_Debug}
		optimized ${Tesseract_Release}
		debug	  ${Leptonica_Debug}
		optimized ${Leptonica_Release}
)

#target_link_libraries(FETestPlatform FEBasicApplication)
#target_link_libraries(FETestPlatform VisualNodeSystem)

if(USE_STATIC_RUNTIME)
    add_definitions(-DGLEW_STATIC)
endif()

source_group("Source Files" FILES ${source_SRC})
# *************** EDITOR ***************
source_group("Source Files/SubSystems/FEDearImguiWrapper" FILES ${FEDearImguiWrapper_SRC})
source_group("Source Files/Windows" FILES ${SubWindows_SRC})
source_group("Source Files/Windows/NodeArea" FILES ${NodeAreaWindows_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes" FILES ${VisualNodeSystem_CustomNodes_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/Images" FILES ${VisualNodeSystem_CustomNodes_Images_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/Timing" FILES ${VisualNodeSystem_CustomNodes_Timing_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/System" FILES ${VisualNodeSystem_CustomNodes_System_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/Strings" FILES ${VisualNodeSystem_CustomNodes_Strings_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/FileSystem" FILES ${VisualNodeSystem_CustomNodes_FileSystem_SRC})
source_group("Source Files/SubSystems/NodeSystem/CustomNodes/InputSimulation" FILES ${VisualNodeSystem_CustomNodes_InputSimulation_SRC})
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

add_custom_command(TARGET FETestPlatform POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tesseract/lib/$<CONFIG>/tesseract55$<$<CONFIG:Debug>:d>.dll"
		"$<TARGET_FILE_DIR:FETestPlatform>"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/leptonica/lib/$<CONFIG>/leptonica-1.87.0$<$<CONFIG:Debug>:d>.dll"
		"$<TARGET_FILE_DIR:FETestPlatform>"
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/tesseract/dependency_dlls/$<CONFIG>/"
		"$<TARGET_FILE_DIR:FETestPlatform>"
		COMMENT "Copying all other Tesseract dependency DLLs to the build directory"
)

add_custom_command(TARGET FETestPlatform PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
        -D PROJECT_VERSION_PREFIX=FETestPlatform_
        -D PROJECT_VERSION_MAJOR=0
        -D PROJECT_VERSION_MINOR=0
        -D PROJECT_VERSION_PATCH=7
        -D PROJECT_VERSION_DIR=${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo
        -P ${CMAKE_CURRENT_SOURCE_DIR}/VersionInfo/UpdateProjectVersion.cmake
)