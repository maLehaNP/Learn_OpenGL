#define _CRT_SECURE_NO_WARNINGS
#define TIMES_SAMPLE_AMOUNT 400

#include <iostream>
#include <string>
#include <vector>

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
void adjustPerspective();
void renderScene(Shader& shader, Model& floor, Model& cube);
void renderScene(Shader& shader, Model& cube);


// Screen
float screen_width = 1920.0f;
float screen_height = 1080.0f;

double deltaTime = 0.0;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 0.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

unsigned int uboMatrices;  // Uniform Buffer Object

float farPlane = 100.0f;
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);

int cursor = GLFW_CURSOR_DISABLED;
bool cursorKeyPressed;



int main() {
	// GLFW
	// ----
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);  

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Advanced Lighting", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, cursor);
	glfwSwapInterval(1); // VSync

	// Callbacks setting
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// ImGui setup
	// -----------
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();


	// GLAD: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to init GLAD" << std::endl;
		return -1;
	}
	printf("Loaded OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	std::cout << std::endl;


	// Global settings
	// ---------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_FRAMEBUFFER_SRGB);  // Gamma correction

	//stbi_set_flip_vertically_on_load(true);


	// Shaders
	// -------
	Shader shader("shaders/shader.vert", "shaders/shader.frag");
	Shader depthShader("shaders/depthShader.vert", "shaders/depthShader.geom", "shaders/depthShader.frag");
	Shader lightShader("shaders/lightShader.vert", "shaders/lightShader.frag");

	// Configure a uniform buffer object
	unsigned int UBI = glGetUniformBlockIndex(shader.ID, "Matrices");
	glUniformBlockBinding(shader.ID, UBI, 0);
	unsigned int LightUBI = glGetUniformBlockIndex(lightShader.ID, "Matrices");
	glUniformBlockBinding(lightShader.ID, LightUBI, 0);
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, uboMatrices);
	adjustPerspective();


	// Models
	// ------
	Model floor("objects/floor/floor.obj");
	Model cube("objects/cube/cube.obj");
	Model lightCube("objects/lightCube/lightCube.obj");


	// configure depth map FBO
	// -----------------------
	// Framebuffer object for rendering the depth map
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	// create a cubemap
	unsigned int depthCubemap;
	glGenTextures(1, &depthCubemap);

	// assign each of the single cubemap faces a 2D depth-valued texture image:
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL
		);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	// Since we're going to use a geometry shader, that allows us to render to all faces in a single pass,
	// we can directly attach the cubemap as a framebuffer's depth attachment
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// Light space transform
	// ---------------------
	//glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);
	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
	float near = 1.0f;
	float far = 25.0f;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);  // FOV must be 90!

	std::vector<glm::mat4> shadowTransforms = {
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)),
		shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0))
	};


	// shader configuration
	// --------------------
	depthShader.use();
	for (int i = 0; i < shadowTransforms.size(); i++)
		depthShader.setMat4("shadowMatrices[" + to_string(i) + "]", shadowTransforms[i]);
	depthShader.setVec3("lightPos", lightPos);
	depthShader.setFloat("far_plane", far);

	shader.use();
	shader.setInt("diffuseTexture", 0);
	shader.setInt("depthMap", 1);

	shader.setVec3("lightPos", lightPos);
	shader.setVec3("viewPos", camera.Position);
	shader.setFloat("far_plane", far);
	

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
	ImVec4 clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	bool show_metrics_window = false;
	int samples = 5;
	float offset = 0.1f;


	// Render loop
	// -----------
	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		// --------------------
		renderStartTime = glfwGetTime();
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		delta_times[times_offset] = deltaTime * 1000;
		render_times[times_offset] = renderTime * 1000;
		times_offset = (times_offset + 1) % TIMES_SAMPLE_AMOUNT;


		// Pre-render processing
		// ---------------------
		glfwPollEvents();
		processInput(window);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		depthShader.use();

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//renderScene(depthShader, floor, cube);
		renderScene(depthShader, cube);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0, 0, screen_width, screen_height);  // reset viewport
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		// 2. render scene as normal using the generated depth/shadow map  
		// --------------------------------------------------------------
		view = camera.GetViewMatrix();
		glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		shader.use();
		shader.setVec3("viewPos", camera.Position);
		shader.setInt("samples", samples);
		shader.setFloat("offset", offset);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
		//renderScene(depthShader, floor, cube);
		renderScene(shader, cube);

		lightShader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.25f));
		lightShader.setMat4("model", model);
		lightCube.Draw(lightShader);
		

		// ImGui
		// -----
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();

		ImGui::Begin("Debug");                           // Create a window called "Hello, world!" and append into it.

		ImGui::ColorEdit3("Clear color", (float*)&clear_color);  // Edit 3 floats representing a color

		ImGui::SliderInt("samples", &samples, 0, 6);
		ImGui::SliderFloat("offset", &offset, 0.0f, 0.2f);

		ImGui::Checkbox("Show Metrics Window", &show_metrics_window);

		float deltaTimeAvg = 0.0f;
		float renderTimeAvg = 0.0f;
		for (int i = 0; i < TIMES_SAMPLE_AMOUNT; i++) {
			deltaTimeAvg += delta_times[i];
			renderTimeAvg += render_times[i];
		}
		deltaTimeAvg /= (float)TIMES_SAMPLE_AMOUNT;
		renderTimeAvg /= (float)TIMES_SAMPLE_AMOUNT;
		char overlay[32];
		sprintf(overlay, "mov avg %f ms", deltaTimeAvg);
		ImGui::PlotLines("Frame time", delta_times, TIMES_SAMPLE_AMOUNT, times_offset, overlay, 0.0f, 20.0f, ImVec2(0, 100));
		sprintf(overlay, "mov avg %f ms", renderTimeAvg);
		ImGui::PlotLines("Render time", render_times, TIMES_SAMPLE_AMOUNT, times_offset, overlay, 0.0f, 5.0f, ImVec2(0, 100));

		ImGui::Text("Camera Pos   (%.2f, %.2f, %.2f)", camera.Position.x, camera.Position.y, camera.Position.z);
		ImGui::Text("Camera Front (%.2f, %.2f, %.2f)", camera.Front.x, camera.Front.y, camera.Front.z);
		ImGui::Text("Camera Up    (%.2f, %.2f, %.2f)", camera.Up.x, camera.Up.y, camera.Up.z);
		ImGui::Text("Camera Right (%.2f, %.2f, %.2f)", camera.Right.x, camera.Right.y, camera.Right.z);
		ImGui::Text("Camera Yaw %.2f", camera.Yaw);
		ImGui::Text("Camera Pitch %.2f", camera.Pitch);
		ImGui::Text("Camera MovementSpeed %.2f", camera.MovementSpeed);
		ImGui::Text("Camera Zoom %.2f", camera.Zoom);
		ImGui::Text("");
		ImGui::Text("Light Pos (%.2f, %.2f, %.2f)", lightPos.x, lightPos.y, lightPos.z);

		ImGui::End();

		if (show_metrics_window)
			ImGui::ShowMetricsWindow();

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		renderEndTime = glfwGetTime();  // Includes ImGui rendering time (+~0.4 ms)
		renderTime = renderEndTime - renderStartTime;

		glfwSwapBuffers(window);
	}

	// Cleanup
	// -------
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


// renders the 3D scene
// --------------------
void renderScene(Shader& shader, Model& floor, Model& cube)
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
	model = glm::translate(model, glm::vec3(2.0f, 0.5f, 1.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.5f, 2.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.25));
	shader.setMat4("model", model);
	cube.Draw(shader);
}

void renderScene(Shader& shader, Model& cube)
{
	// room cube
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(5.0f));
	shader.setMat4("model", model);
	glDisable(GL_CULL_FACE); // note that we disable culling here since we render 'inside' the cube instead of the usual 'outside' which throws off the normal culling methods.
	shader.setInt("reverse_normals", 1); // A small little hack to invert normals when drawing cube from the inside so lighting still works.
	cube.Draw(shader);
	shader.setInt("reverse_normals", 0); // and of course disable it
	glEnable(GL_CULL_FACE);
	// cubes
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.setMat4("model", model);
	cube.Draw(shader);
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	model = glm::scale(model, glm::vec3(0.75f));
	shader.setMat4("model", model);
	cube.Draw(shader);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));

	adjustPerspective();
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

	adjustPerspective();
}

void adjustPerspective() {
	projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
