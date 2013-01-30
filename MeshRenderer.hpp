
#ifndef __MESHRENDERER_H_
#define __MESHRENDERER_H_

#include <vector>

#include "Mesh.hpp"

using std::vector;
using std::cout;
using std::endl;

// renders a chosen mesh from a list of meshes
class MeshRenderer {
	private:
		GLuint program;
		vector<Mesh*> meshes; // all meshes this can render
		unsigned currentMeshIndex;
		Mesh* currentMesh;
		mat4 modelView;
		mat4 projection;
		vec3 translateDelta; // this will be added to modelView every time idle is called
		bool showBoundingBox;

		int screenWidth;
		int screenHeight;

		void showMesh(unsigned index) {
			currentMesh = meshes[index];
			currentMeshIndex = index;
			cout << currentMesh->getName() << endl;
			
			vec4* meshPoints = currentMesh->getPoints();
			GLsizeiptr meshSize = sizeof(meshPoints[0]) * currentMesh->getNumPoints();
			
			BoundingBox* box = currentMesh->getBoundingBox();
			vec4* boxPoints = box->getPoints();
			GLsizeiptr boxSize = sizeof(boxPoints[0]) * box->getNumPoints();

			glBufferData(GL_ARRAY_BUFFER, meshSize + boxSize, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, meshSize, meshPoints);
			glBufferSubData(GL_ARRAY_BUFFER, meshSize, boxSize, boxPoints);

			resetState();
			glutPostRedisplay();
		}

	public:
		MeshRenderer(vector<Mesh*> _meshes, GLuint _program) {
			meshes = _meshes;
			program = _program;
			showBoundingBox = false;
			showMesh(0);
		}

		void showNextMesh() {
			if(currentMeshIndex == meshes.size() - 1) {
				showMesh(0);
			} else {
				showMesh(currentMeshIndex + 1);
			}
		}

		void resetState() {
			modelView = mat4() * RotateY(45.0f);
			translateDelta = vec3();
			glutPostRedisplay();
			// TODO
		}

		void showPrevMesh() {
			if(currentMeshIndex == 0) {
				showMesh(meshes.size() - 1);
			} else {
				showMesh(currentMeshIndex - 1);
			}
		}

		void toggleBoundingBox() {
			showBoundingBox = !showBoundingBox;
			glutPostRedisplay();
		}

		void idle() {
			vec3* td = &translateDelta;
			if(td->x != 0 || td->y != 0 || td->z != 0) {
				modelView = modelView * Translate(td->x, td->y, td->z);
				glutPostRedisplay();
			}
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			projection = Perspective(45.0, screenWidth/screenHeight, 0.1, 500.0);
			
			// hook up matrices with shader
			GLuint modelLoc = glGetUniformLocationARB(program, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelView);
			GLuint projLoc = glGetUniformLocationARB(program, "projection_matrix");
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);
			
			GLsizeiptr totalPoints = currentMesh->getNumPoints();
			if(showBoundingBox) {
				totalPoints += currentMesh->getBoundingBox()->getNumPoints();
			}

			// draw triangles
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, totalPoints);
			glDisable(GL_DEPTH_TEST); 

			// output to hardware, double buffered
			glFlush();
			glutSwapBuffers();
		}

		void reshape(int _screenWidth, int _screenHeight) {
			screenWidth = _screenWidth;
			screenHeight = _screenHeight;
		}

		void toggleTranslateDelta(unsigned offset, bool positive) {
			if((positive && translateDelta[offset] > 0)
					|| (!positive && translateDelta[offset] < 0)) {
				translateDelta[offset] = 0;
			} else {
				translateDelta[offset] = positive ? 1 : -1;
			}
		}

};

#endif

