#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


// Screen
float screen_width = 1280.0f;
float screen_height = 720.0f;

double deltaTime = 0.0;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;

// Uniform buffer object
unsigned int uboMatrices;

float farPlane = 100.0f;
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);

bool blinn = true;
bool blinnKeyPressed;



int main() {
	// GLFW initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);  

	// Window creation
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Advanced Lighting", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Callbacks setting
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// GLAD: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}

	// Successfully loaded OpenGL
	printf("Loaded OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	std::cout << endl;


	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	stbi_set_flip_vertically_on_load(true);

	// Shaders
	Shader shader("shaders/blinnPhongShader.vert", "shaders/blinnPhongShader.frag");
	Shader lightCubeShader("shaders/lightCubeShader.vert", "shaders/lightCubeShader.frag");

	shader.use();
	shader.setFloat("material.shininess", 8.0f);
	// Point light
	shader.setVec3("pointLights[0].position", glm::vec3(0.0f, 2.0f, 0.0f));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09f);
	shader.setFloat("pointLights[0].quadratic", 0.032f);

	// configure a uniform buffer object
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.ID, "Matrices");
	glUniformBlockBinding(shader.ID, uniformBlockIndex, 0);
	unsigned int uniformBlockIndexLight = glGetUniformBlockIndex(lightCubeShader.ID, "Matrices");
	glUniformBlockBinding(lightCubeShader.ID, uniformBlockIndexLight, 0);
	// Now actually create the buffer (var already created as global)
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));
	// store the projection matrix
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	Model floor("objects/floor/floor.obj");
	Model cube("objects/cube/cube.obj");

	shader.use();
	shader.setMat4("model", glm::mat4(1.0f));
	lightCubeShader.use();
	lightCubeShader.setMat4("model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)));


	// Runtime variables

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// Per-frame time logic
	double currentTime;
	int frameCount = 0;
	double lastTime = 0.0;
	double deltaTimeSum = 0.0;
	double lastSec = 0.0;
	double renderStartTime = 1.0;
	double renderEndTime;
	double renderTimeSum = 0.0;
	// For FPS lock
	double drawingTime;
	double sleptTime = 0.0;
	double fpsTick = 1.0 / 60.0;  // Fraction of second equal to 1 frame.

	glm::mat4 view;
	glm::mat4 model = glm::mat4(1.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		currentTime = glfwGetTime();
		frameCount++;
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		deltaTimeSum += deltaTime;
		// FPS lock
		drawingTime = deltaTime - sleptTime;
		std::this_thread::sleep_for(std::chrono::duration<double>(fpsTick - drawingTime));
		sleptTime = fpsTick - drawingTime;

		if (currentTime - lastSec >= 1.0) {
			printf("FPS: %d\n", frameCount);
			printf("Avg Frame time: %10f ms\n", deltaTimeSum / frameCount * 1000);
			printf("Avg Render time: %9f ms\n", renderTimeSum / frameCount * 1000);
			cout << (blinn ? "Blinn-Phong" : "Phong") << endl;
			cout << endl;
			frameCount = 0;
			deltaTimeSum = 0.0;
			renderTimeSum = 0.0;
			lastSec = currentTime;
		}
		renderStartTime = glfwGetTime();

		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		shader.use();
		shader.setVec3("viewPos", camera.Position);
		shader.setBool("blinn", blinn);
		floor.Draw(shader);

		lightCubeShader.use();
		cube.Draw(lightCubeShader);

		glfwSwapBuffers(window);
		glfwPollEvents();

		renderEndTime = glfwGetTime();
		renderTimeSum += renderEndTime - renderStartTime;
	}

	glfwTerminate();
	return 0;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));

	projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.MovementSpeed = 10.0f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		camera.MovementSpeed = SPEED;

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	std::cout << "OpenGL window is resized: " << width << 'x' << height << std::endl;
	screen_width = static_cast<float>(width);
	screen_height = static_cast<float>(height);
	glViewport(0, 0, width, height);

	projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
