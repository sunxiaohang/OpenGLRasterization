#include "Utils.h"
#include "Shader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int main()
{
	// glfw: initialize and configure
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

	VertexArray vertexArray;
	VertexBufferLayout layout;
	layout.Push<float>(3);
	VertexBuffer vertexBuffer(vertices, 4 * 3 * sizeof(float));
	IndexBuffer indexBuffer(indices, 2 * 3);
	vertexArray.AddBuffer(vertexBuffer, layout);

	Renderer renderer;
	Shader shader("BaseShader.shader");
	glfwSwapInterval(1);
	//ASSERT(location != -1);
	float red = 0.0f;
	float incremenet = 0.01; 
	while (!glfwWindowShouldClose(window))// render loop
	{
		processInput(window);// input
		renderer.Clear();
		renderer.Draw(vertexArray, indexBuffer, shader);
		shader.SetUniform4f("u_Color", red, 0.5f, 0.2f, 1.0f);
		if (red > 1)incremenet = -0.01;
		if (red < 0)incremenet = 0.01;
		red += incremenet;		       
		glfwSwapBuffers(window);// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwPollEvents();
	}
	vertexArray.UnBind();
	vertexBuffer.UnBind();
	indexBuffer.UnBind();
	shader.UnBind();
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