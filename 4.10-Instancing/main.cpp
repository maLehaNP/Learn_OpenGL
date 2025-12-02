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
//unsigned int loadCubemap(vector<std::string> faces);


// Screen
float screen_width = 1920.0f;
float screen_height = 1080.0f;

double deltaTime = 0.0;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;

// Uniform buffer object
unsigned int uboMatrices;

float farPlane = 1000.0f;

glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), screen_width / screen_height, 0.1f, farPlane);;



int main() {
	// GLFW initialization
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Window creation
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Instancing", NULL, NULL);
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
	glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &glIntVal);
	std::cout << "GL_MAX_ELEMENTS_VERTICES: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &glIntVal);
	std::cout << "Maximum nr of vertex attributes supported: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &glIntVal);
	std::cout << "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS: " << glIntVal << std::endl;
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &glIntVal);
	std::cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS: " << glIntVal << std::endl;


	// Configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Shaders
	//Shader shader("shaders/shader.vert", "shaders/shader.frag");
	Shader shader("shaders/rockShader.vert", "shaders/rockShader.frag");
	Shader instanceShader("shaders/instanceShader.vert", "shaders/rockShader.frag");

	// Calculating offsets
	/*glm::vec2 translations[100];
	int index = 0;
	float offset = 0.1f;
	for (int y = -10; y < 10; y += 2)
	{
		for (int x = -10; x < 10; x += 2)
		{
			glm::vec2 translation;
			translation.x = (float)x / 10.0f + offset;
			translation.y = (float)y / 10.0f + offset;
			translations[index++] = translation;
		}
	}*/
	// Transfer the data to the uniform array
	/*shader.use();
	for (unsigned int i = 0; i < 100; i++)
	{
		shader.setVec2("offsets[" + to_string(i) + "]", translations[i]);
	}*/

	// Objects data
	/*float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};
	unsigned int quadVAO, quadVBO, instanceVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(2, 1);

	glBindVertexArray(0);*/

	stbi_set_flip_vertically_on_load(true);

	Model planet("objects/planet/planet.obj");
	Model rock("objects/rock/rock.obj");
	unsigned int rockTextureId = rock.meshes[0].textures[0].id;
	
	// configure a uniform buffer object
	unsigned int uniformBlockIndex = glGetUniformBlockIndex(shader.ID, "Matrices");
	glUniformBlockBinding(shader.ID, uniformBlockIndex, 0);
	unsigned int uniformBlockIndexInstance = glGetUniformBlockIndex(shader.ID, "Matrices");
	glUniformBlockBinding(instanceShader.ID, uniformBlockIndexInstance, 0);
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

	unsigned int amount = 100000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed	
	float radius = 150.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	// vertex buffer object
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

	for (unsigned int i = 0; i < rock.meshes.size(); i++)
	{
		unsigned int VAO = rock.meshes[i].VAO;
		glBindVertexArray(VAO);
		// vertex attributes
		std::size_t vec4Size = sizeof(glm::vec4);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}


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

		/*shader.use();
		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
		glBindVertexArray(0);*/

		// draw planet
		shader.use();
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		shader.setMat4("model", model);
		planet.Draw(shader);

		// draw meteorites
		/*for (unsigned int i = 0; i < amount; i++)
		{
			shader.setMat4("model", modelMatrices[i]);
			rock.Draw(shader);
		}*/
		instanceShader.use();
		glBindTexture(GL_TEXTURE_2D, rockTextureId);
		for (unsigned int i = 0; i < rock.meshes.size(); i++)
		{
			glBindVertexArray(rock.meshes[i].VAO);
			glDrawElementsInstanced(
				GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount
			);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();

		renderEndTime = glfwGetTime();
		renderTimeSum += renderEndTime - renderStartTime;
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	/*glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);*/

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
