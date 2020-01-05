# Introduction

This is a list of features to add to the project.
Formalising these features into specific tasks.

# Visualisations

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

## Skyboxes

Implement a skybox.
<!-- 
# Audio Engine Features

## 

## Multi-track recording

# Physics Engine Features

## Kinetic objects

Force, energy, momentum and 

## OpenCL

Use OpenCL to defer particle position/velocity computation to the GPU in paralell.
-->