#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"

using namespace std;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);



// Settings
float screen_width = 1280.0f;
float screen_height = 720.0f;

// Timings
double deltaTime = 0.0;
double lastTime = 0.0;

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;
bool firstMouse = true;

// Flashlight
bool calcSpotLight = false;



int main() {
	// GLFW initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window creation
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Blending", NULL, NULL);
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

	// Querying OpenGL maximum supporter values
	int glIntVal;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glIntVal);
	std::cout << "Maximum texture size supported: " << glIntVal << std::endl;
	/*glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &glIntVal);
	std::cout << "GL_MAX_VIEWPORT_DIMS: " << glIntVal << std::endl;*/  // Lead to "Run-Time Check Failure #2 - Stack around the variable 'glIntVal' was corrupted."
	glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &glIntVal);
	std::cout << "GL_MAX_3D_TEXTURE_SIZE: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &glIntVal);
	std::cout << "GL_MAX_ELEMENTS_VERTICES: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &glIntVal);
	std::cout << "GL_MAX_ELEMENTS_INDICES: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX, &glIntVal);
	std::cout << "GL_MAX: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &glIntVal);
	std::cout << "GL_MAX_DRAW_BUFFERS: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &glIntVal);
	std::cout << "Maximum nr of vertex attributes supported: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &glIntVal);
	std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &glIntVal);
	std::cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &glIntVal);
	std::cout << "GL_MAX_RENDERBUFFER_SIZE: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_SAMPLES, &glIntVal);
	std::cout << "GL_MAX_SAMPLES: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &glIntVal);
	std::cout << "GL_MAX_TEXTURE_BUFFER_SIZE: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &glIntVal);
	std::cout << "GL_MAX_RECTANGLE_TEXTURE_SIZE: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &glIntVal);
	std::cout << "GL_MAX_FRAGMENT_INPUT_COMPONENTS: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &glIntVal);
	std::cout << "GL_MAX_SERVER_WAIT_TIMEOUT: " << glIntVal << std::endl;
	std::cout << endl;

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Build and compile shader
	Shader lightingShader("shader.vert", "shader.frag");
	Shader lightCubeShader("light_cube.vert", "light_cube.frag");
	Shader singleColorShader("shaderSingleColor.vert", "shaderSingleColor.frag");


	// load models
	// -----------
	Model ourModel("backpack/backpack.obj");

	cout << "Total number of meshes: " << ourModel.meshes.size() << endl;
	cout << "Total number of textures loaded: " << ourModel.textures_loaded.size() << endl;
	int nVertSum = 0;
	for (Mesh mesh : ourModel.meshes) {
		nVertSum += mesh.vertices.size();
	}
	cout << "Total number of vertices: " << nVertSum << endl;
	std::cout << endl;

	float vertices[] = {
		// positions
		-0.5f, -0.5f, -0.5f,  // back left down
		 0.5f, -0.5f, -0.5f,  // back right down
		 0.5f,  0.5f, -0.5f,  // back right up
		-0.5f,  0.5f, -0.5f,  // back left up

		-0.5f, -0.5f,  0.5f,  // front left down
		 0.5f, -0.5f,  0.5f,  // front right down
		 0.5f,  0.5f,  0.5f,  // front right up
		-0.5f,  0.5f,  0.5f,  // front left up

		-0.5f,  0.5f,  0.5f,  // left up front
		-0.5f,  0.5f, -0.5f,  // left up back
		-0.5f, -0.5f, -0.5f,  // left down back
		-0.5f, -0.5f,  0.5f,  // left down front

		 0.5f,  0.5f,  0.5f,  // right u f
		 0.5f,  0.5f, -0.5f,  // right u b
		 0.5f, -0.5f, -0.5f,  // right d b
		 0.5f, -0.5f,  0.5f,  // right d f

		-0.5f, -0.5f, -0.5f,  // down l b
		 0.5f, -0.5f, -0.5f,  // down r b
		 0.5f, -0.5f,  0.5f,  // down r f
		-0.5f, -0.5f,  0.5f,  // down l f

		-0.5f,  0.5f, -0.5f,  // up l b
		 0.5f,  0.5f, -0.5f,  // up r b
		 0.5f,  0.5f,  0.5f,  // up r f
		-0.5f,  0.5f,  0.5f  // up l f
	};
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.5f, -3.0f)
	};

	vector<Vertex> cube_vertices;
	for (int i = 0; i < 3 * 4 * 6; i += 3) {
		Vertex vertex;
		vertex.Position = glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]);
		cube_vertices.push_back(vertex);
	}
	vector<unsigned int> indices = {
		0, 1, 2,
		0, 3, 2,

		4, 5, 6,
		4, 7, 6,

		8, 9, 10,
		8, 11, 10,

		12, 13, 14,
		12, 15, 14,

		16, 17, 18,
		16, 19, 18,

		20, 21, 22,
		20, 23, 22
	};
	vector<Texture> textures;

	Mesh cube(cube_vertices, indices, textures);

	float planeVertices[] = {
		// positions         // Normals         // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, 0.0f,  5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
		-5.0f, 0.0f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
		-5.0f, 0.0f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
		 5.0f, 0.0f, -5.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
	};

	vector<Vertex> plane_vertices;
	for (int i = 0; i < 8 * 4; i += 8) {
		Vertex vertex;
		vertex.Position = glm::vec3(planeVertices[i], planeVertices[i + 1], planeVertices[i + 2]);
		vertex.Normal = glm::vec3(planeVertices[i + 3], planeVertices[i + 4], planeVertices[i + 5]);
		vertex.TexCoords = glm::vec2(planeVertices[i + 6], planeVertices[i + 7]);
		plane_vertices.push_back(vertex);
	}

	vector<unsigned int> quadIndices = {
		0, 1, 2,
		0, 2, 3
	};

	textures.clear();
	Texture texture;
	unsigned int textureId = TextureFromFile("bathroom_floor_tile_texture.jpg", ".");
	cout << "Floor texture id: " << textureId << endl;
	std::cout << endl;
	texture.id = textureId;
	texture.type = "texture_diffuse";
	texture.path = "bathroom_floor_tile_texture.jpg";
	textures.push_back(texture);
	texture.id = TextureFromFile("bathroom_floor_tile_texture_specular.jpg", ".");
	texture.type = "texture_specular";
	texture.path = "bathroom_floor_tile_texture_specular.jpg";
	textures.push_back(texture);

	Mesh floor(plane_vertices, quadIndices, textures);

	vector<glm::vec3> vegetation;
	vegetation.push_back(glm::vec3(-2.0f, 0.0f, -0.48f));
	vegetation.push_back(glm::vec3(2.0f, 0.0f, 0.51f));
	vegetation.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	vegetation.push_back(glm::vec3(2.0f, 0.0f, -0.6f));

	float quad[] = {
		// positions          // normals         // texture Coords
		 0.5f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
		-0.5f,  0.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
		-0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
		 0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f
	};

	vector<Vertex> quadVerticesVec;
	for (int i = 0; i < 8 * 4; i += 8) {
		Vertex vertex;
		vertex.Position = glm::vec3(quad[i], quad[i + 1], quad[i + 2]);
		vertex.Normal = glm::vec3(quad[i + 3], quad[i + 4], quad[i + 5]);
		vertex.TexCoords = glm::vec2(quad[i + 6], quad[i + 7]);
		quadVerticesVec.push_back(vertex);
	}

	textures.clear();
	texture.id = TextureFromFile("grass.png", ".");
	texture.type = "texture_diffuse";
	texture.path = "grass.png";
	textures.push_back(texture);
	texture.id = TextureFromFile("grass_specular.png", ".");
	texture.type = "texture_specular";
	texture.path = "grass_specular.png";
	textures.push_back(texture);

	Mesh grass(quadVerticesVec, quadIndices, textures);

	textures.clear();
	texture.id = TextureFromFile("blending_transparent_window.png", ".");
	texture.type = "texture_diffuse";
	texture.path = "blending_transparent_window.png";
	textures.push_back(texture);
	texture.id = TextureFromFile("blending_transparent_window_specular.png", ".");
	texture.type = "texture_specular";
	texture.path = "blending_transparent_window_specular.png";
	textures.push_back(texture);

	Mesh transpWindow(quadVerticesVec, quadIndices, textures);

	
	// Shader configuration --------------------------------------------------------

	lightingShader.use();
	lightingShader.setFloat("material.shininess", 32.0f);

	lightingShader.setVec3("viewPos", camera.Position);

	// Direct light
	lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);  // Light pointing downwards
	lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// Point light 1
	lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
	lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("pointLights[0].constant", 1.0f);
	lightingShader.setFloat("pointLights[0].linear", 0.09f);
	lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
	// Point light 2
	lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
	lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("pointLights[1].constant", 1.0f);
	lightingShader.setFloat("pointLights[1].linear", 0.09f);
	lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
	// Point light 3
	lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
	lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("pointLights[2].constant", 1.0f);
	lightingShader.setFloat("pointLights[2].linear", 0.09f);
	lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
	// Point light 4
	lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
	lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("pointLights[3].constant", 1.0f);
	lightingShader.setFloat("pointLights[3].linear", 0.09f);
	lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
	// Spot light
	lightingShader.setVec3("spotLight.position", camera.Position);
	lightingShader.setVec3("spotLight.direction", camera.Front);
	lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	lightingShader.setFloat("spotLight.constant", 1.0f);
	lightingShader.setFloat("spotLight.linear", 0.09f);
	lightingShader.setFloat("spotLight.quadratic", 0.032f);

	lightingShader.setBool("calcSpotLight", calcSpotLight);
	

	// Runtime variables
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	double currentTime;
	double deltaTimeSum = 0.0;
	double lastSec = 0.0f;
	int frameCount = 0;
	double drawingTime;
	double sleptTime = 0.0;
	double fpsTick = 1.0 / 60.0;  // Fraction of second equal to 1 frame.
	//std::cout << "FPS tick: " << fpsTick * 1000 << " ms" << std::endl;
	double renderStartTime;
	double renderEndTime;
	double renderTime = 0.0;
	double renderTimeSum = 0.0;

	const float radius = 4.0f;

	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model = glm::mat4(1.0f);
	lightingShader.setMat4("model", model);

	std::map<float, glm::vec3> sorted;

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	

	// Render loop

	while (!glfwWindowShouldClose(window)) {
		// Per-frame time logic
		currentTime = glfwGetTime();
		frameCount++;

		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
		deltaTimeSum += deltaTime;

		drawingTime = deltaTime - sleptTime;
		std::chrono::milliseconds s{ static_cast<int>((fpsTick - drawingTime) * 1000) };
		std::this_thread::sleep_for(s);
		sleptTime = fpsTick - drawingTime;

		if (currentTime - lastSec >= 1.0) {
			printf("Camera pos: %f, %f, %f\n", camera.Position.x, camera.Position.y, camera.Position.z);

			std::cout << "FPS: " << frameCount << std::endl;
			std::cout << "Avg Frame time: " << deltaTimeSum / frameCount * 1000 << " ms" << std::endl;
			std::cout << "Avg Render time: " << renderTime * 1000 << " ms" << std::endl;
			std::cout << std::endl;
			frameCount = 0;
			deltaTimeSum = 0.0;
			renderTimeSum = 0.0;
			lastSec = currentTime;
		}
		renderStartTime = glfwGetTime();


		processInput(window);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		// View/projection transformations
		projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, 100.0f);
		view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);
		// World transformation
		model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// Circle around the scene over time
		pointLightPositions[0].x = sin(glfwGetTime()) * radius;
		pointLightPositions[0].z = cos(glfwGetTime()) * radius;
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);

		lightingShader.setBool("calcSpotLight", calcSpotLight);
		if (calcSpotLight) {
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
		}

		lightingShader.use();

		// Draw floor
		floor.Draw(lightingShader);

		// Draw backpack
		// Move up
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.75f, 0.0f));
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);

		// Draw grass
		/*for (unsigned int i = 0; i < vegetation.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, vegetation[i]);
			lightingShader.setMat4("model", model);
			grass.Draw(lightingShader);
		}*/
		// Draw grass in right order
		sorted.clear();
		for (unsigned int i = 0; i < vegetation.size(); i++)
		{
			float distance = glm::length(camera.Position - vegetation[i]);
			sorted[distance] = vegetation[i];
		}
		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, it->second);
			lightingShader.setMat4("model", model);
			grass.Draw(lightingShader);
		}

		// Draw 4 light objs
		for (unsigned int i = 0; i < 4; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			lightCubeShader.setMat4("model", model);
			cube.Draw(lightCubeShader);
		}

		// Draw window
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 1.75f, 2.0f));
		lightingShader.setMat4("model", model);
		transpWindow.Draw(lightingShader);

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);


		glfwSwapBuffers(window);
		glfwPollEvents();

		renderEndTime = static_cast<float>(glfwGetTime());
		renderTime = renderEndTime - renderStartTime;
		renderTimeSum += renderTime;
	}
}



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
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
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		calcSpotLight = 1;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		calcSpotLight = 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	std::cout << "OpenGL window is resized: " << width << 'x' << height << std::endl;
	screen_width = static_cast<float>(width);
	screen_height = static_cast<float>(height);
	glViewport(0, 0, width, height);
}
