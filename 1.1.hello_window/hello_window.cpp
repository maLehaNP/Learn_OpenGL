#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



/* Whenever the window changes in size, GLFW calls this function and fills in the proper arguments for you to process. */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)  // (if it's not pressed, glfwGetKey returns GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
}



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // tell GLFW we want to explicitly use the core-profile


    // glfw window creation
    // --------------------
    // This window object holds all the windowing data and is required by most of GLFW's other functions. 
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);  // we tell GLFW to make the context of our window the main context on the current thread


    // glad: load all OpenGL function pointers
    // ---------------------------------------
    // GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function
    // We pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific.
    // GLFW gives us glfwGetProcAddress that defines the correct function based on which OS we're compiling for.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  // We do have to tell GLFW we want to call this function on every window resize by registering it


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // Specify the color to clear the screen with. The glClearColor function is a state-setting function.


    // Render loop
    // An iteration of the render loop is more commonly called a frame.
    while (!glfwWindowShouldClose(window))  // The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close.
    {
        // Input
        processInput(window);

        // Rendering commands here
        glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer. glClear is a state-using function in that it uses the current state to retrieve the clearing color from.

        // Check and call events and swap the buffers
        glfwSwapBuffers(window);  // The glfwSwapBuffers will swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window) that is used to render to during this render iteration and show it as output to the screen. 
        glfwPollEvents();  // The glfwPollEvents function checks if any events are triggered (like keyboard input or mouse movement events), updates the window state, and calls the corresponding functions (which we can register via callback methods).
    }


    // As soon as we exit the render loop we would like to properly clean/delete all of GLFW's resources that were allocated.
    glfwTerminate();
    return 0;
}
