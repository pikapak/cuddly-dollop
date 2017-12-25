#include "glfwBackend.h"
#include "define_gl.h"
#include "game.h"
#include "input_manager.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	InputManager::GetInstance().Input((unsigned int)key, action);
}

GLFWManager::GLFWManager()
{
	// Init glfw
	if(!glfwInit())
	{
	    std::cout << "Failed to initialize glfw" << std::endl;
		std::getchar();
	    exit(1);
	}

	// GL Version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// Create window
	m_window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cuddly-Dollop", NULL, NULL);
	if (!m_window)
	{
		std::cout << "Window failed to be created!" << std::endl;
		std::getchar();
		exit(1);
	}

	// Set fullscreen, call again with null monitor to set windowed
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	int width, height;
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	m_screenWidth = mode->width;
	m_screenHeight = mode->height;

	glfwSetWindowMonitor(m_window, monitor, 0, 0, m_screenWidth, m_screenHeight, mode->refreshRate);

	glfwMakeContextCurrent(m_window);

	// Key callback
	glfwSetKeyCallback(m_window, key_callback);

	// GLEW init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cout << "GLEW error: " << glewGetErrorString(res) << std::endl;
		std::getchar();
		exit(1);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

	glfwSwapInterval(1);

	printf("GL version: %s\n", glGetString(GL_VERSION));

	Resize();
}

void GLFWManager::GLFWMainLoop()
{
	Game* game = new Game();
	if (!game->init(WINDOW_WIDTH, WINDOW_HEIGHT))
	{
		std::cout << "Game class init failed" << std::endl;
		std::getchar();
		exit(1);
	}

	while (!glfwWindowShouldClose(m_window))
	{
		game->renderSceneCB();
		glfwSwapBuffers(m_window);
		glfwPollEvents();
	}
}

void GLFWManager::Resize()
{
	//Setup viewport to fit the window size
	int w, h;
	glfwGetWindowSize(m_window, &w, &h);
	glViewport(0, 0, (GLsizei)(w), (GLsizei)(h));
	OrthoProjInfo::GetRegularInstance().Bottom = -(h / 2.0f);
	OrthoProjInfo::GetRegularInstance().Top = (h / 2.0f);
	OrthoProjInfo::GetRegularInstance().Left = -(w / 2.0f);
	OrthoProjInfo::GetRegularInstance().Right = (w / 2.0f);
	OrthoProjInfo::GetRegularInstance().changed = true;
}