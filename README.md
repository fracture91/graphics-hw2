Andrew Hurle

CS 4731 - Computer Graphics

Homework 2


PLY file renderer, applies transformations to meshes.

The program is linked against whatever files are present on the machine.
The Zoo Lab machines contain the needed library files, so they're not
included here.

Running
=====

To compile and run on Linux:

1. `make && ./hw2`


To compile and run on Windows (Zoo Lab machines, tested on FLA21-08):

1. Open "Visual Studio Command Prompt (2010)" from the start menu
2. `cd C:\wherever\these\files\are`
3. `nmake /f Win-Makefile`
4. `hw2.exe`

Notes
=====

hw2.cpp is the main file, but it doesn't do much besides the normal
OpenGL setup goop.  PLYReader.hpp is responsible for reading the PLY
files - again, pretty straightforward.  Most of the magic happens in
Mesh.hpp and MeshRenderer.hpp.  There is a Mesh struct, which contains
all of the parsed data, a BoundingBox, normal vectors, and normal lines.
Some of the math is delegated to the BoundingBox, which calculates the
appropriate size for the box.

MeshRenderer contains a set of Meshes and cycles through them.  The
initial viewing point is set as the max corner of the bounding box, and
the camera is looking at the min point.  I thought this was a better
angle for viewing the meshes to start with.  Consequently, the z and x
axes both point out of the screen a bit.

I seem to have some sort of problem with calculating normals, like
they're pointing the wrong way or at a constant point or something.
Consequently, the breathing animation and normal lines look messed up.
However, the cow.ply mesh seems to do a pretty good job with it, along
with a few others.

Some of the very small meshes show up in a strange initial position and
clip earlier than desired.  Tried to debug this with Xuan but couldn't
figure it out.


