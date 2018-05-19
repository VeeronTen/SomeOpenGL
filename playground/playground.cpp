// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <vector>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <common/shader.hpp>

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

std::vector<GLfloat> generatePoint() {
	std::vector<GLfloat> result;

	GLfloat x = valueByAbs(1.0);
	GLfloat y = valueByAbs(absYbyX(x));
	GLfloat z = probNeg(absZbyXY(x, y));

	result.push_back(x);
	result.push_back(y);
	result.push_back(z);
	std::cout << "point x: " << x << " y: " << y <<  " z: " << z << std::endl;
	return result;
}

void generatePickle() {
	std::vector<GLfloat> point;
	point = generatePoint();
	pickle_vertex_buffer_data.insert(pickle_vertex_buffer_data.end(), point.begin(), point.end());
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
	glm::mat4 Model      = glm::mat4(1.0f);
	glm::mat4 Transform2     = glm::translate(glm::mat4(), glm::vec3(-3.0f, 0.0f, 0.0f));
	glm::mat4 PickleModel     = glm::translate(glm::mat4(), glm::vec3(0.0f, 2.0f, 0.0f));
	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		 1.0f,-1.0f, 1.0f
	};

	static GLfloat g_color_buffer_data[12*3*3];

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	GLuint pickleertexbuffer;
	glGenBuffers(1, &pickleertexbuffer);
	generatePickle();
	glBindBuffer(GL_ARRAY_BUFFER, pickleertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pickle_vertex_buffer_data.size(), &pickle_vertex_buffer_data[0], GL_STATIC_DRAW);

	#define RADIUS 3
	#define SPEED 0.0005

	float x;
	float z;

	float radian = 0;
	
	do{	
		x = RADIUS*sin(radian);
		z = RADIUS*cos(radian);
		radian += SPEED;

		// Camera matrix
		glm::mat4 View       = glm::lookAt(
									glm::vec3(x, 5, z), // Camera is at (4,3,-3), in World Space
									glm::vec3(0,2,0), // and looks at the origin
									glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
							);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		float colorCoeff = fmod(radian, 2);
		colorCoeff = colorCoeff > 1 ? colorCoeff - 1 : 1 - colorCoeff;
		for (int i = 0; i < 12*3 ; i++){
			g_color_buffer_data[3*i+0] = g_vertex_buffer_data[3*i+0] < 0 ? 0 : g_vertex_buffer_data[3*i+0] * colorCoeff;
			g_color_buffer_data[3*i+1] = g_vertex_buffer_data[3*i+1] < 0 ? 0 : g_vertex_buffer_data[3*i+1] * colorCoeff;
			g_color_buffer_data[3*i+2] = g_vertex_buffer_data[3*i+2] < 0 ? 0 : g_vertex_buffer_data[3*i+2] * colorCoeff;
		}
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

	glm::mat4 Model2 = glm::rotate(
		Transform2,
		radian,
		glm::vec3(0.0f, 1.0f, 0.0f)
	);

		glm::mat4 MVP2        = Projection * View * Model2; // Remember, matrix multiplication is the other way around
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);


		if ((int)(radian * 8) % 8 == 0 ) {
			generatePickle();
			glBindBuffer(GL_ARRAY_BUFFER, pickleertexbuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pickle_vertex_buffer_data.size(), &pickle_vertex_buffer_data[0], GL_STATIC_DRAW);
		}
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
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

