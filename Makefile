hw2: hw2.cpp vshader1.glsl fshader1.glsl Angel.h CheckError.h mat.h vec.h\
		textfile.h textfile.cpp InitShader.cpp Mesh.hpp PLYReader.hpp
	g++ hw2.cpp -g -Wall -lglut -lGL -lGLEW -o hw2

clean:
	rm hw2

