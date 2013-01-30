
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
		bool rotate;
		float theta;
		vec3 translation;
		mat4 transMat;
		mat4 rotMat;
		int lastTicks;

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

		void resetProjection() {
			if(screenHeight == 0) {
				projection = mat4(); // don't want to divide by zero...
				return;
			}
			BoundingBox* box = currentMesh->getBoundingBox();
			vec3 max = box->getMax();
			vec3 min = box->getMin();
			cout << max <<" "<< min << endl;
			vec4 eye = vec4(max.x, max.y, max.z, 1);
			vec4 at = vec4(min.x, min.y, min.z, 1);
			cout << screenWidth << screenHeight << endl;
			projection = mat4()
				* Perspective(90, (float)screenWidth/screenHeight, 0.00001, 10000)
				* LookAt(eye, at, vec4(0, 1, 0, 1));
			cout << projection << endl;
		}

	public:
		MeshRenderer(vector<Mesh*> _meshes, GLuint _program) {
			meshes = _meshes;
			program = _program;
			showBoundingBox = false;
			lastTicks = 0;
			showMesh(0);
		}

		void resetState() {
			modelView = mat4();
			translateDelta = vec3();
			translation = vec3();
			rotate = false;
			theta = 0;
			transMat = rotMat = mat4();
			resetProjection();
			glutPostRedisplay();
		}

		void showPrevMesh() {
			if(currentMeshIndex == 0) {
				showMesh(meshes.size() - 1);
			} else {
				showMesh(currentMeshIndex - 1);
			}
		}

		void showNextMesh() {
			if(currentMeshIndex == meshes.size() - 1) {
				showMesh(0);
			} else {
				showMesh(currentMeshIndex + 1);
			}
		}

		void toggleBoundingBox() {
			showBoundingBox = !showBoundingBox;
			glutPostRedisplay();
		}

		void idle() {
			// scale animation delta by number of elapsed ticks
			int ticks = glutGet(GLUT_ELAPSED_TIME);
			if(lastTicks == 0) {
				lastTicks = ticks;
			}
			int elapsed = ticks - lastTicks;
			lastTicks = ticks;
			
			vec3* td = &translateDelta;
			bool doTranslate = td->x != 0 || td->y != 0 || td->z != 0;
			if(doTranslate) {
				translation += (*td) * elapsed;
				transMat = Translate(translation);
			}
			
			if(rotate) {
				// move to origin, rotate, move back
				vec4 center = currentMesh->getBoundingBox()->getCenter();
				theta += 0.25 * elapsed;
				rotMat = Translate(center) * RotateY(theta) * Translate(-center);
			}
			
			if(rotate || doTranslate) {
				modelView = mat4() * transMat * rotMat;
			}
			glutPostRedisplay();
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
			glViewport(0, 0, screenWidth, screenHeight);
			resetProjection();
		}

		void toggleTranslateDelta(unsigned offset, bool positive) {
			if((positive && translateDelta[offset] > 0)
					|| (!positive && translateDelta[offset] < 0)) {
				translateDelta[offset] = 0;
			} else {
				vec3 size = currentMesh->getBoundingBox()->getSize();
				translateDelta[offset] = positive ? size[offset]/100 : size[offset]/-100;
			}
		}

		void toggleRotate() {
			rotate = !rotate;
			glutPostRedisplay();
		}

};

#endif

