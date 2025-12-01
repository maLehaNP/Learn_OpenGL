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
//#include <learnopengl/mesh.h>
//#include <learnopengl/model.h>

//#include "shader.h"
//#include "mesh.h"
//#include "model.h"

using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadCubemap(vector<std::string> faces);


// Screen
float screen_width = 1280.0f;
float screen_height = 720.0f;

// Time
double deltaTime = 0.0;
double lastTime = 0.0;

// Camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;



int main() {

}
