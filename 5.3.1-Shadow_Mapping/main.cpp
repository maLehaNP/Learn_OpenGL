#define _CRT_SECURE_NO_WARNINGS
#define TIMES_SAMPLE_AMOUNT 400

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <deque>
using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/mesh.h>
#include <learnopengl/model.h>




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
bool firstMouse = true;

// Uniform buffer object
unsigned int uboMatrices;

float farPlane = 100.0f;
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);

int cursor = GLFW_CURSOR_DISABLED;
bool cursorKeyPressed;

Model floor("objects/floor/floor.obj");
Model cube("objects/cube/cube.obj");


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
	glfwSetInputMode(window, GLFW_CURSOR, cursor);
	glfwSwapInterval(1); // Enable vsync

	// Callbacks setting
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


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
	//glEnable(GL_CULL_FACE);
	//glEnable(GL_MULTISAMPLE);
	//glEnable(GL_FRAMEBUFFER_SRGB);  // Gamma correction

	stbi_set_flip_vertically_on_load(true);

	// Shaders
	//Shader shader("shaders/blinnPhongShader.vert", "shaders/blinnPhongShader.frag");
	Shader simpleDepthShader("shaders/simpleDepthShader.vert", "shaders/simpleDepthShader.frag");
	Shader debugDepthQuad("shaders/debugDepthQuad.vert", "shaders/debugDepthQuad.frag");

	// configure a uniform buffer object
	/*unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.ID, "Matrices");
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
	glBindBuffer(GL_UNIFORM_BUFFER, 0);*/

	



	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(
		GL_TEXTURE_2D,
		0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT,
		0, GL_DEPTH_COMPONENT,
		GL_FLOAT, NULL
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	float near_plane = 1.0f, far_plane = 7.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	glm::mat4 lightView = glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightSpaceMatrix = lightProjection * lightView;


	// Runtime variables
	// Per-frame time logic
	double currentTime;
	double lastTime = 0.0;
	double renderStartTime;
	double renderEndTime;
	double renderTime = 0.0;
	float delta_times[TIMES_SAMPLE_AMOUNT];
	float render_times[TIMES_SAMPLE_AMOUNT];
	int times_offset = 0;

	glm::mat4 view;
	glm::mat4 model = glm::mat4(1.0f);

	// Our ImGUI state
	bool show_demo_window = false;
	bool animate = true;
	ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


	// Render loop
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		renderStartTime = glfwGetTime();
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		delta_times[times_offset] = deltaTime * 1000;
		render_times[times_offset] = renderTime * 1000;
		times_offset = (times_offset + 1) % TIMES_SAMPLE_AMOUNT;

		glfwPollEvents();
		processInput(window);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		shader.use();
		shader.setVec3("viewPos", camera.Position);
		shader.setBool("blinn", blinn);
		floor.Draw(shader);

		lightCubeShader.use();
		cube.Draw(lightCubeShader);*/


		


		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::ColorEdit3("Clear color", (float*)&clear_color); // Edit 3 floats representing a color

		float deltaTimeAvg = 0.0f;
		float renderTimeAvg = 0.0f;
		for (int i = 0; i < TIMES_SAMPLE_AMOUNT; i++) {
			deltaTimeAvg += delta_times[i];
			renderTimeAvg += render_times[i];
		}
		deltaTimeAvg /= (float)TIMES_SAMPLE_AMOUNT;
		renderTimeAvg /= (float)TIMES_SAMPLE_AMOUNT;
		char overlay[32];
		sprintf(overlay, "mov avg %f", deltaTimeAvg);
		ImGui::PlotLines("Frame time", delta_times, TIMES_SAMPLE_AMOUNT, times_offset, overlay, 0.0f, 20.0f, ImVec2(0, 100));
		sprintf(overlay, "mov avg %f", renderTimeAvg);
		ImGui::PlotLines("Render time", render_times, TIMES_SAMPLE_AMOUNT, times_offset, overlay, 0.0f, 5.0f, ImVec2(0, 100));

		ImGui::End();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		renderEndTime = glfwGetTime();
		renderTime = renderEndTime - renderStartTime;

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


// renders the 3D scene
// --------------------
void renderScene(Shader& shader)
{
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f, 1.0f, 5.0f));
	shader.setMat4("model", model);
	floor.Draw(shader);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.setMat4("model", model);
	cube.Draw(shader);
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
	if (cursor == GLFW_CURSOR_DISABLED) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !cursorKeyPressed) {
		//glfwSetWindowShouldClose(window, true);
		if (cursor == GLFW_CURSOR_DISABLED)
			cursor = GLFW_CURSOR_NORMAL;
		else if (cursor == GLFW_CURSOR_NORMAL) {
			cursor = GLFW_CURSOR_DISABLED;
			firstMouse = true;
		}
		glfwSetInputMode(window, GLFW_CURSOR, cursor);
		cursorKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
	{
		cursorKeyPressed = false;
	}

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
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	printf("OpenGL window is resized: %d x %d\n", width, height);
	screen_width = static_cast<float>(width);
	screen_height = static_cast<float>(height);
	glViewport(0, 0, width, height);

	projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
