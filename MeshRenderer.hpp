
#ifndef __MESHRENDERER_H_
#define __MESHRENDERER_H_

#include <vector>
#include <algorithm>

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

		GLsizeiptr meshLength;
		GLsizeiptr boxLength;
		GLsizeiptr triangleLength;
		
		mat4 modelView;
		mat4 projection;
		vec3 translateDelta; // this will be added to modelView every time idle is called
		bool rotate;
		float theta;
		vec3 translation;
		mat4 transMat;
		mat4 rotMat;
		int lastTicks;

		int screenWidth;
		int screenHeight;

		bool showBoundingBox;
		bool showNormals;
		bool breathe;
		float normalScale;
		float normalDelta;
		float maxSize;

		void showMesh(unsigned index) {
			currentMesh = meshes[index];
			currentMeshIndex = index;
			cout << currentMesh->getName() << endl;
			
			vec4* meshPoints = currentMesh->getPoints();
			GLsizeiptr meshBytes = sizeof(meshPoints[0]) * currentMesh->getNumPoints();
			
			BoundingBox* box = currentMesh->getBoundingBox();
			vec4* boxPoints = box->getPoints();
			GLsizeiptr boxBytes = sizeof(boxPoints[0]) * box->getNumPoints();

			vec4* normals = currentMesh->getNormals();
			GLsizeiptr normalBytes = meshBytes;
			cout << "normal " << normals[0] << normals[1] << normals[2] << normals[3] << endl;

			GLsizeiptr totalBytes = meshBytes + boxBytes + normalBytes;
			glBufferData(GL_ARRAY_BUFFER, totalBytes, NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, meshBytes, meshPoints);
			glBufferSubData(GL_ARRAY_BUFFER, meshBytes, boxBytes, boxPoints);
			glBufferSubData(GL_ARRAY_BUFFER, meshBytes + boxBytes, normalBytes, normals);

			// set up vertex arrays
			GLuint posLoc = glGetAttribLocation(program, "vPosition");
			glEnableVertexAttribArray(posLoc);
			glVertexAttribPointer(posLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			
			meshLength = currentMesh->getNumPoints();
			boxLength = currentMesh->getBoundingBox()->getNumPoints();
			triangleLength = meshLength + boxLength;

			// set up normal array, which is after all triangles
			GLuint normalLoc = glGetAttribLocation(program, "normal");
			glEnableVertexAttribArray(normalLoc);
			glVertexAttribPointer(normalLoc, 4, GL_FLOAT, GL_FALSE, 0,
					BUFFER_OFFSET(triangleLength * sizeof(currentMesh->getPoints()[0])));
			
			resetState();
			glutPostRedisplay();
		}

		void resetProjection() {
			if(screenHeight == 0) {
				projection = mat4(); // don't want to divide by zero...
				return;
			}
			BoundingBox* box = currentMesh->getBoundingBox();
			cout << box->getMax() <<" "<< box->getMin() << endl;
			cout << box->getSize() << endl;
			projection = mat4()
				* Perspective(90, (float)screenWidth/screenHeight, 0.0000001, 100000)
				* LookAt(box->getMax() + box->getSize()/2, box->getMin(), vec4(0, 1, 0, 0));
			cout << projection << endl;
		}

		void resetBreatheState() {
			vec3 size = currentMesh->getBoundingBox()->getSize();
			maxSize = std::max(std::max(size[0], size[1]), size[2]);
			normalDelta = maxSize/10000;
			normalScale = 0;
		}


	public:
		MeshRenderer(vector<Mesh*> _meshes, GLuint _program) {
			meshes = _meshes;
			program = _program;
			showBoundingBox = false;
			breathe = false;
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
			resetBreatheState();
			normalScale = 0;
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
			
			if(breathe) {
				normalScale += (float)elapsed/100 * normalDelta;
				// make normalScale bounce between 1 and (1 + maxSize/100)
				if(normalScale < 0) {
					normalScale = 0;
					normalDelta = -normalDelta;
				} else if (normalScale > maxSize/100) {
					normalScale = maxSize/100;
					normalDelta = -normalDelta;
				}
				cout << "ns " << normalScale << endl;
			}

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
			if(rotate || doTranslate || breathe) {
				glutPostRedisplay();
			}
		}

		void display() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			// hook up matrices with shader
			GLuint modelLoc = glGetUniformLocationARB(program, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_TRUE, modelView);
			GLuint projLoc = glGetUniformLocationARB(program, "projection_matrix");
			glUniformMatrix4fv(projLoc, 1, GL_TRUE, projection);
			

			GLuint scaleLoc = glGetUniformLocation(program, "normal_scale");
			glUniform1f(scaleLoc, normalScale);

			GLsizeiptr pointsToDraw = triangleLength;

			if(!showBoundingBox) {
				pointsToDraw -= boxLength;
			}

			// draw triangles
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glEnable(GL_DEPTH_TEST);
			glDrawArrays(GL_TRIANGLES, 0, pointsToDraw);
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

		void toggleNormals() {
			showNormals = !showNormals;
		}

		void toggleBreathing() {
			breathe = !breathe;
			resetBreatheState();
			if(!breathe) {
				normalScale = 0;
			}
			glutPostRedisplay();
		}

};

#endif

