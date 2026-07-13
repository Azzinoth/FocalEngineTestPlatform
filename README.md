# Focal Engine Test Platform

A visual, node-based test automation tool for the [Focal Engine](https://github.com/Azzinoth/FocalEngine) ecosystem. Because of the graphical nature of the engine, unit tests alone are insufficient, this platform evaluates visual output and UI functionality by capturing the screen, searching for user defined images, recognizing text, and simulating user input. Test logic is defined as node graphs, no code required.

⚠️ **Early development**: Not ready for general use. Expect breaking changes, missing features, and rough edges.

![Focal Engine Test Platform](https://github.com/Azzinoth/FocalEngineTestPlatform/blob/media/FocalEngineTestPlatform_Window.png)

## Key Features

- **Node-based test authoring**: Tests are built as visual graphs using the [Visual Node System](https://github.com/Azzinoth/VisualNodeSystem).
- **Multi-graph organization**: Test logic can be split across multiple node areas, each holding a user-defined part of the test. A hierarchical overview, built with the [Focal Engine Scene Graph UI](https://github.com/Azzinoth/FESceneGraphUI), shows the whole structure, with indicators for parent/child relationships between areas, the presence of links between node areas, and whether a window is currently open for an area.
- **Screen capture**: Take screenshots of a specified monitor or parts of the screen.
- **Image search**: Locate a template image on screen.
- **Text recognition**: OCR via Tesseract.
- **Input simulation**: Mouse movement/clicks, keyboard keys, and text input.
- **Input recording and replay**: Record real user input and replay it later. A recording can be a part of a test graph, combined with other logical steps.
- **Test management UI**: Tests overview, test properties, failure inspection.

## Focal Engine Ecosystem

The Focal Engine project consists of modular components that work together to provide a complete development environment:

[Basic Application Module](https://github.com/Azzinoth/FEBasicApplication) - A foundation layer for OpenGL and ImGui applications that provides essential utilities including time measurement, thread pooling, logging, TCP networking, and profiling capabilities.

[Visual Node System](https://github.com/Azzinoth/VisualNodeSystem) - A framework for creating visual node-based interfaces with features like zoom, reroute nodes, group comments, and JSON serialization, ideal for material editors and visual scripting.

[Focal Engine](https://github.com/Azzinoth/FocalEngine) - The engine with all core functionality.

[Focal Engine Scene Graph UI](https://github.com/Azzinoth/FESceneGraphUI) - A reusable scene graph UI component, that provides a hierarchical tree view for visualizing and interacting with scene graph nodes.

[Focal Engine Editor](https://github.com/Azzinoth/FocalEngineEditor) - A comprehensive editor for the engine.

Focal Engine Test Platform (this repository) - A visual testing tool for the engine and its ecosystem.

## Building the Project for Visual Studio (Windows)

```bash
# Initialize a new Git repository
git init

# Add the remote repository
git remote add origin https://github.com/Azzinoth/FocalEngineTestPlatform

# Pull the contents of the remote repository
git pull origin master

# Initialize and update submodules
git submodule update --init --recursive

# Generate the build files using CMake
# Will work in Windows PowerShell
cmake CMakeLists.txt
```

## Third Party Licenses

This project uses the following third-party libraries:

1) **tesseract**: This library is licensed under Apache 2.0 License. The full license text can be found at [tesseract's GitHub repository](https://github.com/tesseract-ocr/tesseract/blob/main/LICENSE).

2) **leptonica**: This library is licensed under the BSD 2-Clause License. The full license text can be found at [leptonica's GitHub repository](https://github.com/DanBloomberg/leptonica/blob/master/leptonica-license.txt).

3) **lodepng**: This library is licensed under the zlib License. The full license text can be found at [lodepng's GitHub repository](https://github.com/lvandeve/lodepng/blob/master/LICENSE).
