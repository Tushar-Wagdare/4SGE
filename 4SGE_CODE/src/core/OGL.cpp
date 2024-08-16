#ifndef UNICODE
#define UNICODE
#endif


///*** HEADERS ***///
// Standard C Headers
#include<stdio.h>
#include<stdlib.h>
// My Headers
#include "..\..\include\core\window.hpp"
#include "..\..\include\core\windowCallback.hpp"
//*** OpenGL Headers 
#include<C:/glew-2.1.0/include/GL/glew.h>
#include<gl/GL.h>
#include"..\..\include\math\vmath.h"


//*** Name space ***///
using namespace vmath;


//*** link With OpenGL Librar ***///
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")


//*** Global Function Declaration ***///
void display(void);
void update(void);
int initialize(void);


//*** Global Variables Declaration ***///
// Enums 
enum
{
	AMC_ATTRIBUTE_POSITION  = 0,
	AMC_ATTRIBUTE_COLOR
};
// PP Variables
GLuint shaderProgramObject  = 0;
GLuint vao                  = 0;
GLuint vbo_position         = 0;
GLuint vbo_color            = 0;
GLuint mvpMatrixUniform     = 0;
mat4 perspectiveProjectionMatrix;
// Window Variables
FILE* gpFILE                = NULL;
Window* g_myWindow;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{ 
	//*** Variable declaratrion ***///
	void (*render[2])(void) = {display, update};


	//*** Code ***///
	g_myWindow = new Window(WndProc);
	g_myWindow->createWindow(TEXT("4SGE"), TEXT("4SGE : Tushar Wagdare"));


	// Log file Creation
    if (fopen_s(&gpFILE, "..\\log\\Log.txt", "w") != 0)
    {
        MessageBox(NULL, TEXT("Log Create Hou Shakat Nahi"), TEXT("Error"), MB_OK || MB_ICONERROR);
        exit(0);
    }
	else
	{
    	fprintf(gpFILE, "Program Started Successfully\n\n");
	}


	// OpenGL Initialization
	if(initialize() < 0)
    {
        fprintf(gpFILE, "initialize() failed..\n");
        exit(0);
    }
	else
	{
		fprintf(gpFILE, "initialize() successfull..\n");
	}


	return(g_myWindow->initGameLoop(render));
}



int initialize(void)
{
	//*** Function Declarations ***///
	void resize(int,int);
	void uninitialize(void);


	///*** Code ***///
	if(g_myWindow->switchToRenderingContext() < 0)
	{
		fprintf(gpFILE,"switchToRenderingContext() Failed\n\n");
		return(-1);
	}


	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFILE,"glewInit() Failed !\n\n");
		return(-6);
	}
	

	///*** Shade Code ***///
	// Vertex Shader
	const GLchar* vertexShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"in vec4 aColor;" \
		"out vec4 oColor;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"oColor = aColor;" \
		"}";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);
	GLint status        = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog   = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE,"Vertex Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE,"********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	 

	// Fragment Shader
	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"in vec4 oColor;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = oColor;" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE,"Fragment Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE,"********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}


	// Shader Program
	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");

	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE,"Shader Program Linking Error Log:%s\n", szInfoLog);
				fprintf(gpFILE,"********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}


	// Get Shader Uniform Location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");


	// Declare Position And Color Arrays
	const GLfloat triangle_position[] = 
	{
		0.0f,1.0f,0.0f,
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f 
	};

	const GLfloat triangle_color[] = 
	{ 
		1.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,
		0.0f,0.0f,1.0f 
	};


	// VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);


	// VBO for Position 
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// VBO for color
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	

	// Enabling Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);


	// Set the Clear Color of Window To Blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	// initialize orthographicProjectionMatrix
	perspectiveProjectionMatrix = vmath::mat4::identity();


	// Warmup call
	resize(WIN_WIDTH, WIN_HIGHT);


	return(0);
}



void resize(int width, int height)
{
	//*** Code ***///
	if (height <= 0)
	{
		height = 1;
	}

	// Set perspective projection matrix
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);


	// Set Viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}



void display(void)
{
	//*** Code ***///
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);


	// Transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;


	// push above mvp int vertex shader's mvp uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(g_myWindow->getDeviceContext());
}



void update(void)
{
	//*** Code ***///
}



void uninitialize(void)
{
	///*** Code ***///
	// Free Shader Program Object
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}

		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}


	// Delete VBO Color
	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;
	}


	// Delete VBO Position
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position  = 0;
	}


	// Delete VAO
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
}


