#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform float normal_scale;

in vec4 vPosition;
in vec4 normal;

void main() {
	// PROTIP #3
	// can extend to gl_Position = projection * camera * models * vertex
	// you may wish to view camera motion as a transform on the projection (projection * camera) * models ...
	// this could be thought of as moving the observer in space
	// or you may view camera motion as transforming all objects in the world while the observer remains still
	// projection * (camera * models) * ...
	// notice mathematically there is no difference
	// however in rare circumstances the order of the transform may affect the numerical stability
	// of the overall projection
	vec4 breathedPoint = vPosition + (normal_scale * normal);
	gl_Position = projection_matrix*model_matrix*breathedPoint;
}
