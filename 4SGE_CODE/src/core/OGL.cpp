#ifndef UNICODE
#define UNICODE
#endif

///*** Standard C Headers ***
#include<stdio.h>
#include<stdlib.h>

///*** My Headers ***
#include "..\..\include\core\window.hpp"
#include "..\..\include\core\windowCallback.hpp"

//*** OpenGL Headers ***//
//#include<gl/glew.h>
#include<C:/glew-2.1.0/include/GL/glew.h>
#include<gl/GL.h>
#include"..\..\include\math\vmath.h"
//*** Name space ***
using namespace vmath;

//*** link With OpenGL Library//
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "OpenGL32.lib")

///PP Variables
GLuint shaderProgramObject  = 0;
GLuint vao                  = 0;
GLuint vbo_position         = 0;
GLuint vbo_color            = 0;
GLuint mvpMatrixUniform     = 0;
mat4 perspectiveProjectionMatrix;//in vmath.h

Window myWindow;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//*** Local vaiables *** 

	//*** Code ***
	//*** Log file creation ***
	if (fopen_s(&gpFILE, "..\\log\\Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log Create Hou Shakat Nahi"), TEXT("Error"), MB_OK || MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFILE, "Program Started Successfully\n\n");



	return(myWindow.initGameLoop());
}


int Window::initialize(void)
{
	//*** Function Declarations ***
	void resize(int,int);
	void uninitialize(void);

	//01 -  Initialization Of PFD 
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	
	pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion   = 1;
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits   = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits  = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;


	//02 - Get The DC
	ghdc = GetDC(myWindow.getWindowHandle());
		if (ghdc == NULL)
		{
			fprintf(gpFILE, "GetDC Failed !!!\n\n");
			return(-1);
		}


	//03 - Tell OS Hya PFD shi match honara PFD de
	iPexelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPexelFormatIndex == 0)
	{
		fprintf(gpFILE, "ChoosepixelFormat() Failed\n\n");
		return(-2);
	}
	

	//04 - Set Obtained Pixel Format
	if (SetPixelFormat(ghdc, iPexelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFILE, "SetPixelFormat() Failed\n\n");
		return(-3);
	}


	//05 - Tell WGL Bridging library To Give OpenGL Copatible DC From 'ghdc'
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFILE, "wglCreateContex() Failed\n\n");
		return(-4);
	}


	//06 - Now 'ghdc' End Its Roll And Give Controll To 'ghrc'
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFILE, "wglMakeCurrent() failed\n\n");
		return(-5);
	}


	//Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFILE, "glewInit() Failed !\n\n");
		return(-6);
	}
	

	//----------------------------------------------------------------------------------------//
	//Vertex Shader
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

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);///Creates Empty Shader Object
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);///

	glCompileShader(vertexShaderObject);///Return NoneZero Value on Successfull Compiation
	GLint status        = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog   = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);//EC step 1
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFILE, "Vertex Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}
	 

	//Fragment Shader
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
				fprintf(gpFILE, "Fragment Shader Compilation Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	//Shader Program
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
				fprintf(gpFILE, "Shader Program Linking Error Log:%s\n", szInfoLog);
				fprintf(gpFILE, "********************************************************\n");
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	//Get Shader Uniform Location
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

	//Declare Position And Color Arrays
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

	//VAO
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//VBO for Position 
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_position), triangle_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for color
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_color), triangle_color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//
	glBindVertexArray(0);
	//-------------------------------------------------------------------------------------//

	//07 - 2nd step Enabling Depth
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0f);


	//08 - Set the Clear Color of Window To Blue
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//initialize orthographicProjectionMatrix
	perspectiveProjectionMatrix = vmath::mat4::identity();


	//Warmup call
	resize(WIN_WIDTH, WIN_HIGHT);


	return(0);
}



void resize(int width, int height)
{
	//*** Code ***
	if (height <= 0)
	{
		height = 1;
	}

	//Set perspective projection matrix
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
}



void Window::display(void)
{
	//*** Code ***
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);

	//Transformation
	mat4 modelViewMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//push above mvp int vertex shader's mvp uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	glUseProgram(0);

	SwapBuffers(ghdc);
}



void Window::update(void)
{
	//*** Code ***
}



void uninitialize(void)
{
	//Free Shader Program Object
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


	//Delete VBO Color
	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
		vbo_color = 0;
	}


	//Delete VBO Position
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position  = 0;
	}


	//Delete VAO
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}
	


	//*** Code ***
	if (myWindow.getWindowFullscreenStatus() == TRUE)
	{
		myWindow.ToggleFullscreen();
		myWindow.setWindowFullscreenStatus(FALSE);
	}


	//*** Make The hdc As Current DC ***//
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}


	//*** Destroy Rendering Contex***
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}


	//*** Release HDC ***
	if (ghdc)
	{
		ReleaseDC(myWindow.getWindowHandle(), ghdc);
		ghdc = NULL;
	}


	//*** Destroy Window ***
	if (myWindow.getWindowHandle())
	{
		DestroyWindow(myWindow.getWindowHandle());
	}


	//*** Close Log File ***
	if (gpFILE)
	{
		fprintf(gpFILE, "\nProgram Ended Successfully\n");
		fclose(gpFILE);
		gpFILE = NULL;
	}
}


