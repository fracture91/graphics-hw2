
#ifndef __MESH_H_
#define __MESH_H_

#include "Angel.h"

using std::string;

// holds vertex list and point data to be sent to GPU
class Mesh {
	private:
		vec4* vertices;
		vec4* points;
		unsigned vertIndex;
		unsigned pointIndex;
		unsigned numPoints;
		string name;
	
	public:
		Mesh(string _name, unsigned numVertices) {
			name = _name;
			vertices = new vec4[numVertices];
			vertIndex = 0;
		}

		string getName() {
			return name;
		}

		void addVertex(vec4 vert) {
			vertices[vertIndex] = vert;
			vertIndex++;
		}

		void startTriangles(unsigned numTriangles) {
			numPoints = numTriangles * 3;
			points = new vec4[numPoints];
			pointIndex = 0;
		}

		void addTriangle(unsigned a, unsigned b, unsigned c) {
			points[pointIndex] = vertices[a]; pointIndex++;
			points[pointIndex] = vertices[b]; pointIndex++;
			points[pointIndex] = vertices[c]; pointIndex++;
		}

		unsigned getNumPoints() {
			return numPoints;
		}

		vec4* getPoints() {
			return points;
		}

		~Mesh() {
			delete vertices;
			delete points;
		}

};

#endif

