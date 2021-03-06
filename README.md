![alt text][Screen1]

# Introduction

Visualisations is a desktop application which can draw a number of visualisations: 3D graphical effects, Cellular Automata, and geometric features.
Emphasis is placed on the interaction between 3D graphics and real-time audio, and visualisations are able to respond to real-time audio
from a microphone. Individual visualisations can be encoded and written to file to mpg video files within the application.

## Disclaimer

This is a personal project for implementing audio-visual ideas. The development process, when it happens, is fast
and loose: unlike for projects that are built as part of a team and relied on by a user base, here it is a secondary priority to organise the source 
in the interest of clarity, stability, speed and security.

## Running

Installers for releases are made available for Windows 10 x64,
and the binaries are available [here](https://billguastalla.com/visualisations/binaries/).

Installers are produced using [NSIS](https://nsis.sourceforge.io/Main_Page).

<video width="1280" height="720" controls>
  <source src="https://billguastalla.com/visualisations/samples/Visualisations%20v0.4.mp4" type="video/mp4">
</video>

## Platforms and Development Environments

The project is developed on a Windows 10 x64 environment, on Visual Studio 2019, version 16.1.3.
It is possible to build it on linux environments, and this should be reasonably easy,
as there is little to no platform-specific code.

<!--  -->

## Projects in this Repository

The directory of the main project is in Projects\Visualisations.

root<br/>
  |----- Configurations<br/>
  |----- Projects<br/>
  |-----|----- StandaloneFeatures<br/>
  |-----|-----|----- AudioAttributes<br/>
  |-----|-----|----- HighDimensionalSandbox<br/>
  |-----|-----|----- Oscilloscope<br/>
  |-----|-----|----- RenderToFile<br/>
  |-----|----- Visualisations<br/>
  |----- {...}<br/>
  

Under Projects\StandaloneFeatures\, there are four other projects.
These exist to demonstrate specific features used or to be used in the main project, as standalone projects.

One justification for this is that as a developer of your own software, you might just want to see the how video rendering
capabilities were designed in this project. If so you can go to root\StandaloneFeatureDev\RenderToFile, 
and see how FFMPEG is used in this OpenGL desktop application.

![alt text][Screen2]

# Dependencies & Install Instructions

The project uses a mixture of shared and static libraries. It's advisable for contributors to build the dependencies locally,
rather than to rely on binaries found online.
For each dependency, obtain the source code, use CMake to set up a build environment for the dependency-project, and then build it.

In this project, in Visual Studio, the libraries and includes are maintained through project property sheets.
If you find issues with dependencies, check that the names and locations in these sheets match up with your
locally built libraries.

## Dependencies: Shared/Static Libraries

The list of link library files used in this project are listed here.

- avcodec-58.dll
- avdevice-58.dll
- avfilter-7.dll
- avformat-58.dll
- avutil-56.dll
- portaudio_x64.dll
- postproc-55.dll
- swresample-3.dll
- swscale-5.dll

Below are descriptions of each library used, and links to find their sources.

### FFMPEG (Last used version 4.1)

[FFMPEG](https://www.ffmpeg.org/) is a comprehensive tool for encoding/decoding audio and video. It is used here to write visualisations to video files.

### GLFW3 (Last used version 3.2)

[GLFW](https://www.glfw.org/) is a window manager to provide contexts to draw OpenGL framebuffers, and to retrieve events from human-interface devices.

### PORTAUDIO (Last used version 1246720)

[PortAudio](http://www.portaudio.com/) is a general multi-platform interface to audio hardware. It is used here to record real-time audio.

## Dependencies: Header-only

### GLM (Last Used Version: 0.9)

[GLM](https://glm.g-truc.net/0.9.9/index.html) is a mathematics and vector library for use with OpenGL.
One of the reasons it is useful is because it aligns data types on the CPU closely with the native types of the GLSL shading language.

### IMGUI (Last Used Version: 1.6.2)

[IMGUI](https://github.com/ocornut/imgui) is an Immediate-Mode-GUI system. In Visualisations, it is library which produces the window manager within the OpenGL viewport.

### STB-Image (Last Used Version: 2.1.9)

[STB Image](https://github.com/nothings/stb) is a C/C++ image loading library, as a single header, stb_image.h.

# Unit Tests

Unit tests are written in a project called UnitTests, under the main solution.
The framework for unit testing used here is called [Catch2](https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md).
To add new unit tests, look at TestSuite.cpp.

[Screen1]: https://billguastalla.com/visualisations/screens/Screen1.png "Mesh Surface Oscilloscope"
[Screen2]: https://billguastalla.com/visualisations/screens/Screen2.png "Point Clouds"

# Credits & Licensing

The author expresses gratitude to the writers and contributors of each library that has been used by Visualisations.

This project owes a huge amount to the [OpenGL tutorial](https://learnopengl.com/) provided by [Joey De Vries](https://twitter.com/JoeyDeVriez). 
A number of OpenGL-based components in this project are derived from these OpenGL tutorials. The author of this project is grateful to Joey's contributions to
computer graphics.

Currently, Visualisations itself is not licensed. If you would like to use the program, send me an email.
My email is my github username @gmail.com.

The components that are modified works from Joey De Vries, are licensed under Creative Commons 4.0 Non-commercial Attribution,
and are labelled as such at the top of the file. (see LICENSE-CC4.0NA.md)

