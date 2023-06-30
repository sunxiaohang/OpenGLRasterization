#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include <string>
#include "VertexBuffer.h"
#include "IndexBuffer.h"


enum class ShaderType {
	None = -1,
	Vertex = 0,
	Fragment = 1,
};

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

static ShaderProgramSource ParseShader(const char* filePath)
{
	std::ifstream stream(filePath);
	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::None;
	while(getline(stream, line))
	{
		if(line.find("#shader") != std::string::npos)
		{
			if(line.find("vertex") != std::string::npos)
			{
				type = ShaderType::Vertex;
			}
			else if(line.find("fragment") != std::string::npos)
			{
				type = ShaderType::Fragment;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return {ss[0].str(), ss[1].str()};
}

static int CompileShader(unsigned int shaderType, const std::string& shaderSource)
{
	unsigned int id = glCreateShader(shaderType);
	const char* src = shaderSource.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if(result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(sizeof(char) *length);
		glGetShaderInfoLog(id, length, &length, message);
		std::cout<< "Failed to compile" << (shaderType == GL_VERTEX_SHADER ? "vertex":"fragment") << "shader" << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static int CreateShader(const std::string& vertexShaderStr, const std::string& fragmentShaderStr)
{
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderStr);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderStr);
	
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glValidateProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgram;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	ShaderProgramSource source = ParseShader("BaseShader.shader");
	unsigned int shaderProgram = CreateShader(source.vertexSource, source.fragmentSource);

	float vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 3, 1,  // first Triangle
		1, 3, 2   // second Triangle
	};

	VertexBuffer vertexBuffer(vertices, 4 * 3 * sizeof(float));
	IndexBuffer indexBuffer(indices, 2 * 3);

	unsigned int vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	//specific vertices decode rule
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	GLTryCall(glUseProgram(shaderProgram));

	//clear buffer
	glBindVertexArray(0);
	GLTryCall(glUseProgram(0));

	int location = glGetUniformLocation(shaderProgram, "u_Color");
	glfwSwapInterval(1);
	//ASSERT(location != -1);
	float red = 0.0f;
	float incremenet = 0.01; 
	while (!glfwWindowShouldClose(window))// render loop
	{
		processInput(window);// input

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);// render
		glClear(GL_COLOR_BUFFER_BIT);
		glBindVertexArray(vertexArray);
		GLTryCall(glUseProgram(shaderProgram));
		indexBuffer.Bind();
		glUniform4f(location, red, 0.5f, 0.2f, 1.0f);
		if (red > 1)incremenet = -0.01;
		if (red < 0)incremenet = 0.01;
		red += incremenet;

		GLTryCall(glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, NULL));// draw our first triangle
		       
		glfwSwapBuffers(window);// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertexArray);// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteProgram(shaderProgram);

	glfwTerminate();// glfw: terminate, clearing all previously allocated GLFW resources.
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}