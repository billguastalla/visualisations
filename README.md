# Introduction

Visualisations is a desktop application which can draw a number of visualisations: 3D graphical effects, Cellular Automata, and geometric features.
Emphasis is placed on the interaction with graphical features and incoming audio, and visualisations are able to respond to real-time audio
from a microphone. Individual visualisations can be encoded and written to file to mpg video files within the application.

## Platforms and Development Environments

The project is developed on a Windows 10 x64 environment, on Visual Studio 2019, version 16.1.3.
It is possible to build it on linux environments, and this should be reasonably easy,
as there is little to no platform-specific code.

## Projects

There are five projects in this repository.
The main project is Visualisations, which sits in the folder IMGUI.

To build examples of featuresUnder StandaloneFeatureDev, there are four other projects which demonstrate
features of Visualisations, as standalone projects.

The justification for this is that as a developer of your own software, you might just want to see the how video rendering
capabilities were designed in this project. If so you can go to root\StandaloneFeatureDev\RenderToFile, 
and see how FFMPEG is used in this OpenGL desktop application.

root
	\Configurations
	\Projects
		\Visualisations
		\StandaloneFeatures
			\AudioAttributes
			\HighDimensionalSandbox
			\Oscilloscope
			\RenderToFile

# Dependencies & Install Instructions

The project uses a mixture of shared and static libraries. It's advisable for contributors to build the dependencies locally,
rather than to rely on binaries found online.
For each dependency, obtain the source code, and use CMake.

In this project, in Visual Studio, the libraries and includes are maintained through project property sheets.
If you find issues with dependencies, check that the names and locations in these sheets match up with your
locally built libraries.

## Dependencies: Shared/Static Libraries

The list of link library files used in this project are listed here. To obtain them, go through the instructions 

- avcodec-58.dll
- avdevice-58.dll
- avfilter-7.dll
- avformat-58.dll
- avutil-56.dll
- portaudio_x64.dll
- postproc-55.dll
- swresample-3.dll
- swscale-5.dll

### FFMPEG (Last used version 4.1)

FFMPEG is a comprehensive tool for encoding/decoding audio and video. It is used here to write visualisations to video files.



### GLFW3 (Last used version 3.2)

GLFW is a window manager to provide contexts to draw OpenGL framebuffers, and to retrieve events from human-interface devices.

### PORTAUDIO (Last used version )

PortAudio is a general multi-platform interface to audio hardware. It is used here to record real-time audio.

## Dependencies: Header-only

