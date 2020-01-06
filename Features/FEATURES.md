# Note

This is a backlog of features to add to the project. 
Formalising these features into specific tasks is done outside of the repository, and
outlines of features appear here to illustrate the direction of the project.

# Visualisations

### User Interface

Make parameters of specific visualisations available to the user via the GUI.
Provide a virtual function that writes IMGUI

## Vector fields (Visualisation/Physics Engine)

This is both a general idea, and a visualisation.

### Idea

Build a template for a vector field, drawn as an array of arrows in a grid.
A function fieldEquation(glm::vec3 pos) should be implementable
std::function<glm::vec3(glm::vec3)>.

### Mesh

The vector field generated should depend on the camera position,
so that vectors are drawn within the draw-distance, continuously.
Care should be taken so that the grid does not itself move with the camera,
only the arrows which appear within an infinite grid.
Arrows should be drawn with an array of model matrices,
moved and rotated according to the orientation of the field,
at each point. The resolution of the field should be tuneable.

### Particle trajectory

'Magnetic' fields should have normalised directions, so that particles
can be accelarated in different directions with conserved energy.

### Audio

Find ways of modifying the shape of the field based on audio input,
and adding or removing particles to/from the field.

## Fractals: Mandelbrot, Mandelbulb, Julia

Find a way of rendering fractals in 3D: Most importantly, find out how best to wind a mesh
across iteratively generated points from the above sets..

## Visualisations: Inspiration from Physics

Areas to look at to inspire new visualisations.

### Differential Geometry

Gaussian Curvature, and the Gauss-Bonnet Theorem.
Building metrics for medium range integrals over the curvature of a surface,
at a point, could yield interesting functionals.
Minimal surfaces: Local optimisations of surface area.

### Statistical Physics

Random walks, brownian motion, Langevin equation, Monte Carlo dynamics.

### Quantum Mechanics

Wavefunctions, observables and time-evolution.

# Graphics Engine Features

## Bloom

Implement the bloom effect. Two framebuffers, bright components extracted from one, then apply a gaussian blur to it and recombine with
the unchanged framebuffer.
Options should be made available in the GUI:
- Enable/disable bloom
- Set blur strength.
- Set threshold light intensity.

## Depth of field

Use the objects that the camera is looking at to determine a distance to pass to glm::lookAt(..)
Possible options in the GUI:
- Enable/disable DOF
- View-determined/slider-determined DOF
- Strength of blur

## Skyboxes

Implement a skybox.

# Audio Engine Features

## Audio file loading

Load audio files into the program, and read them into buffers.

### Transport

- Provide a transport system in the GUI:
	- Play, Pause, Stop.
	- Slider, with time played, and time remaining.

### Synchronised rendering

Two options for synchronisation:
	- Force synchronisation to framerate.
	- Real-time playing.

### Audio-Buffer handling
Provide various options of generating buffers:
	- Short, medium and long-term buffers:
	- Buffer offset: where does the current time value sit on the current sample array,
	   at the beginning, or at the middle? Provide an option to tune this.

## Multi-track recording

(Later on.)
Load multiple audio files and map them to different buffers.
This way visualisations can contain multiple audio streams so that visualisations can be affected
in different ways by different tracks.

# Physics Engine Features

## Kinetics

Force, energy, momentum and velocity, should be computable. 'Fixes' for energy/momentum conservation should be implementable.
Objects should be able to provide their own force-fields and effective ranges.
Design of a scene of physical objects should be by default separated from mesh implementations,
and the physics processing should make transformations to a set of model matrices.

## OpenCL

Use OpenCL to defer particle position/velocity computation to the GPU in paralell.

## Geometric Algebra

Incorporate GA code, generated from gaigen or otherwise, and use it to:
- Re-implement the camera class
- Implement orthogonal transformations.
- Implement fields.

### High-dimensional rendering

(Later on)
Rendering of high-dimensional objects in 3D.
One approach is the intersection of higher-dimensional polytopes,
using the world or the camera. See HighDimensionalRendering for an overview
of the mesh-generation problem.
