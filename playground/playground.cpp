#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <vector>
#include <GL/glew.h>

#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include "common/shader.hpp"

#define CAMERA_RADIUS 5
#define CAMERA_HEIGHT 1
#define CAMERA_VIEW_HEIGHT 1
#define RADIAN_SPEED 0.001
#define GENERATING_SPEED 30

GLfloat valueByAbs(GLfloat abs) {
	GLfloat result = rand() % (int)(200*abs + 1) / 100.0 - abs;
	std::cout << "valueByAbs(" << abs << ") => " << result << std::endl;
	return result;
}

GLfloat probNeg(GLfloat value) {
	GLfloat result = rand() % 2 ? value : -value;
	std::cout << "probNeg(" << value << ") => " << result << std::endl;
	return result;
}

GLfloat absYbyX(GLfloat x) {
	GLfloat result = sqrt(1 - pow(x, 2));
	std::cout << "absYbyX(" << x << ") => " << result << std::endl;
	return result;
}

GLfloat absZbyXY(GLfloat x, GLfloat y) {
	GLfloat result = sqrt(1 - pow(x, 2) - pow(y, 2));
	std::cout << "absZbyXY(" << x << ", " << y << ") => " << result << std::endl;
	return result;
}

std::vector<GLfloat> pickle_vertex_buffer_data;
std::vector<GLfloat> g_color_buffer_data;

std::vector<GLfloat> generatePoint() {
	std::vector<GLfloat> result;

	GLfloat x = valueByAbs(1.0);
	GLfloat y = valueByAbs(absYbyX(x));
	GLfloat z = probNeg(absZbyXY(x, y));

	result.push_back(x);
	result.push_back(y);
	result.push_back(z);

	g_color_buffer_data.push_back((x + 1) / 2);
	g_color_buffer_data.push_back((y + 1) / 2);
	g_color_buffer_data.push_back((z + 1) / 2);

	std::cout << "point x: " << x << " y: " << y <<  " z: " << z << std::endl << std::endl;
	return result;
}

void generatePickle() {
	std::vector<GLfloat> point;
	point = generatePoint();
	pickle_vertex_buffer_data.insert(pickle_vertex_buffer_data.end(), point.begin(), point.end());
	point = generatePoint();
	pickle_vertex_buffer_data.insert(pickle_vertex_buffer_data.end(), point.begin(), point.end());
	point = generatePoint();
	pickle_vertex_buffer_data.insert(pickle_vertex_buffer_data.end(), point.begin(), point.end());
	std::cout << "done new pickle" << std::endl << std::endl;

}

int main( void )
{
	srand (time(NULL));
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 PickleModel     = glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 0.0f));

	generatePickle();
	
	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);

	GLuint pickleertexbuffer;
	glGenBuffers(1, &pickleertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, pickleertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pickle_vertex_buffer_data.size(), &pickle_vertex_buffer_data[0], GL_STATIC_DRAW);


	float x;
	float z;

	float radian = 0;
	
	do{	
		x = CAMERA_RADIUS*sin(radian);
		z = CAMERA_RADIUS*cos(radian);
		radian += RADIAN_SPEED;

		// Camera matrix
		glm::mat4 View       = glm::lookAt(
									glm::vec3(x, CAMERA_HEIGHT, z), // Camera is at (4,3,-3), in World Space
									glm::vec3(0, CAMERA_VIEW_HEIGHT, 0), // and looks at the origin
									glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
							);
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*g_color_buffer_data.size(), &g_color_buffer_data[0], GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

	// std::cout << "RAD  " << radian << " yss: " << (long int)(radian * 10000) % 1000 << std::endl;

		if ((int)(radian / RADIAN_SPEED) % (int)(1 / RADIAN_SPEED / GENERATING_SPEED) == 0) {
			generatePickle();
			glBindBuffer(GL_ARRAY_BUFFER, pickleertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pickle_vertex_buffer_data.size(), &pickle_vertex_buffer_data[0], GL_STATIC_DRAW);
		}
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, pickleertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glm::mat4 MVPPickle       = Projection * View * PickleModel;
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVPPickle[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, pickle_vertex_buffer_data.size());

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &pickleertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

