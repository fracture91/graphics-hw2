
#ifndef __MESH_H_
#define __MESH_H_

#include "Angel.h"

using std::string;
using std::cout;
using std::endl;

class BoundingBox {
	private:
		vec3 min, max;
		vec4* vertices;
		vec4* points; // points describing triangles of bounding box
		unsigned numPoints;
		unsigned pointsIndex;
		bool dirty;

	public:
		BoundingBox(vec4 initialPoint) {
			vertices = new vec4[8]; // cube has 8 corners
			numPoints = 3 * 2 * 6; // 3 points per tri, 2 tri per face, 6 faces
			points = new vec4[numPoints];
			for(int i = 0; i < 3; i++) {
				max[i] = min[i] = initialPoint[i];
			}
		}

		void addContainedVertex(vec4 vert) {
			for(int i = 0; i < 3; i++) {
				if(vert[i] < min[i]) {
					min[i] = vert[i];
					dirty = true;
				}
				if(vert[i] > max[i]) {
					max[i] = vert[i];
					dirty = true;
				}
			}
		}

		void quad(int a, int b, int c, int d) {
			points[pointsIndex] = vertices[a]; pointsIndex++;
			points[pointsIndex] = vertices[b]; pointsIndex++;
			points[pointsIndex] = vertices[c]; pointsIndex++;
			points[pointsIndex] = vertices[a]; pointsIndex++;
			points[pointsIndex] = vertices[c]; pointsIndex++;
			points[pointsIndex] = vertices[d]; pointsIndex++;
		}

		vec4* getPoints() {
			if(!dirty) {
				return points;
			}

			// create vertices based on min and max
			vertices[0] = vec4(min.x, min.y, max.z, 1);
			vertices[1] = vec4(min.x, max.y, max.z, 1);
			vertices[2] = vec4(max.x, max.y, max.z, 1);
			vertices[3] = vec4(max.x, min.y, max.z, 1);
			vertices[4] = vec4(min.x, min.y, min.z, 1);
			vertices[5] = vec4(min.x, max.y, min.z, 1);
			vertices[6] = vec4(max.x, max.y, min.z, 1);
			vertices[7] = vec4(max.x, min.y, min.z, 1);

			// add to points to make faces
			pointsIndex = 0;
			quad(1, 0, 3, 2);
			quad(2, 3, 7, 6);
			quad(3, 0, 4, 7);
			quad(6, 5, 1, 2);
			quad(4, 5, 6, 7);
			quad(5, 4, 0, 1);

			dirty = false;
			return points;
		}

		unsigned getNumPoints() {
			return numPoints;
		}

		vec3 getSize() {
			return max - min;
		}

		vec3 getMin() {
			return vec3(min);
		}

		vec3 getMax() {
			return vec3(max);
		}

		~BoundingBox() {
			delete vertices;
			delete points;
		}
};

// holds vertex list and point data to be sent to GPU
class Mesh {
	private:
		vec4* vertices;
		vec4* points;
		unsigned vertIndex;
		unsigned pointIndex;
		unsigned numPoints;
		string name;
		BoundingBox* box;
	
	public:
		Mesh(string _name, unsigned numVertices) {
			name = _name;
			vertices = new vec4[numVertices];
			vertIndex = 0;
			box = NULL;
		}

		string getName() {
			return name;
		}

		void addVertex(vec4 vert) {
			vertices[vertIndex] = vert;
			vertIndex++;
			if(box == NULL) {
				box = new BoundingBox(vert);
			} else {
				box->addContainedVertex(vert);
			}
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

		BoundingBox* getBoundingBox() {
			return box;
		}

		~Mesh() {
			delete vertices;
			delete points;
			if(box != NULL) {
				delete box;
			}
		}

};

#endif

