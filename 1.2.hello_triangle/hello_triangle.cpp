#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);



// Simple vertex shader
const char* vertexShaderSource =
    "#version 330 core\n"                                    // Declaration of shader version
    "layout (location = 0) in vec3 aPos;\n"                  // Declare all the input vertex attributes in the vertex shader
    "void main()\n"
    "{\n"
    "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"  // To set the output of the vertex shader we have to assign the position data to the predefined gl_Position variable which is a vec4 behind the scenes.
    "}\0";


// Simple fragment shader
const char* fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"  // Output variable and that is a vector of size 4 that defines the final color output that we should calculate ourselves.
    "void main()\n"
    "{\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"  // Assign a vec4 to the color output as an orange color with an alpha value of 1.0 (1.0 being completely opaque).
    "}\0";



int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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


    // build and compile our shader program
    // ------------------------------------
    // In order for OpenGL to use the shader it has to dynamically compile it at run-time from its source code.

    // Vertex shader ------------------------------------------------------------------------------

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // Attach the shader source code to the shader object and compile the shader
    glShaderSource(
        vertexShader,         // the shader object to compile
        1,                    // specifies how many strings we're passing as source code
        &vertexShaderSource,  // the actual source code of the vertex shader
        NULL
    );
    glCompileShader(vertexShader);
    // Checking for compile-time errors is accomplished as follows:
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader ------------------------------------------------------------------------------

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader program ---------------------------------------------------------------------------------------

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // Attaching the shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // and linking them
    glLinkProgram(shaderProgram);
    // Check
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    glUseProgram(shaderProgram);  // Activating program object
    // Every shader and rendering call after glUseProgram will now use this program object (and thus the shaders).

    // Deleting the shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Vertex input -------------------------------------------------------------------------------------------

    // Because we want to render a single triangle we want to specify a total of three vertices with each vertex having a 3D position.
    // We define them in normalized device coordinates (the visible region of OpenGL) in a float array:
    //float vertices[] = {
    //    -0.5f, -0.5f, 0.0f,  // vertex 1 (left)
    //     0.5f, -0.5f, 0.0f,  // vertex 2 (right)
    //     0.0f,  0.5f, 0.0f   // vertex 3 (top)
    //};

    // Rectangle
    //float vertices[] = {
    //     0.5f,  0.5f, 0.0f,  // top right
    //     0.5f, -0.5f, 0.0f,  // bottom right
    //    -0.5f, -0.5f, 0.0f,  // bottom left
    //    -0.5f,  0.5f, 0.0f   // top left 
    //};
    //unsigned int indices[] = {  // note that we start from 0!
    //    0, 1, 3,   // first triangle
    //    1, 2, 3    // second triangle
    //};

    // Element Buffer Objects (for rectangle)
    /*unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

    // 2 triangles
    float vertices[] = {
        // triangle 1
        -0.75f, -0.75f, 0.0f,
        -0.25f, -0.75f, 0.0f,
        -0.5f,  -0.25f, 0.0f,
         0.25f,  0.25f, 0.0f,
         0.75f,  0.25f, 0.0f,
         0.5f,   0.75f, 0.0f
    };

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    // Vertex Buffer Object
    unsigned int VBO;  // Will store buffer ID
    glGenBuffers(1, &VBO);  // Generating 1 buffer and assigning it's ID to our variable

    glBindBuffer(GL_ARRAY_BUFFER, VBO);  // Binding buffer to the GL_ARRAY_BUFFER buffer type of a vertex buffer object

    // Copies the previously defined vertex data into the buffer's memory
    glBufferData(
        GL_ARRAY_BUFFER,   // the type of the buffer we want to copy data into
        sizeof(vertices),  // specifies the size of the data (in bytes) we want to pass to the buffer
        vertices,          // the actual data we want to send
        GL_STATIC_DRAW     // The fourth parameter specifies how we want the graphics card to manage the given data. This can take 3 forms:
                           // * GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
                           // * GL_STATIC_DRAW: the data is set only once and used many times.
                           // * GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    );
    /* The position data of the triangle does not change, is used a lot, and stays the same for every render call so its usage type should best be GL_STATIC_DRAW.
     * If, for instance, one would have a buffer with data that is likely to change frequently, a usage type of GL_DYNAMIC_DRAW ensures the graphics card will place the data in memory that allows for faster writes.
     */

    // Linking Vertex Attributes ----------------------------------------------------------------------------

    /* Our vertex buffer data is formatted as follows:
     * - The position data is stored as 32-bit (4 byte) floating point values.
     * - Each position is composed of 3 of those values.
     * - There is no space (or other values) between each set of 3 values. The values are tightly packed in the array.
     * - The first value in the data is at the beginning of the buffer.
     */

    // With this knowledge we can tell OpenGL how it should interpret the vertex data (per vertex attribute)

    glVertexAttribPointer(
        0,                  // specifies which vertex attribute we want to configure
        3,                  // specifies the size of the vertex attribute. The vertex attribute is a vec3 so it is composed of 3 values.
        GL_FLOAT,           // specifies the type of the data
        GL_FALSE,           // specifies if we want the data to be normalized
        3 * sizeof(float),  // is known as the stride and tells us the space between consecutive vertex attributes.
                            // Since the next set of position data is located exactly 3 times the size of a float away we specify that value as the stride.
        (void*)0            // This is the offset of where the position data begins in the buffer.
    );
    glEnableVertexAttribArray(0);  // Enabling the vertex attribute giving the vertex attribute location as argument

    // Drawing an object in OpenGL would now look something like this:
    /*
     * // 0. copy our vertices array in a buffer for OpenGL to use
     * glBindBuffer(GL_ARRAY_BUFFER, VBO);
     * glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     * // 1. then set the vertex attributes pointers
     * glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
     * glEnableVertexAttribArray(0);  
     * // 2. use our shader program when we want to render an object
     * glUseProgram(shaderProgram);
     * // 3. now draw the object 
     * someOpenGLFunctionThatDrawsOurTriangle();
     */


    // Vertex Array Object ----------------------------------------------------------------------------------

    /* A vertex array object stores the following:
     * - Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
     * - Vertex attribute configurations via glVertexAttribPointer.
     * - Vertex buffer objects associated with vertex attributes by calls to glVertexAttribPointer.
     *  
     * 
     * unsigned int VAO;
     * glGenVertexArrays(1, &VAO);
     * 
     * 
     * // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
     * // 1. bind Vertex Array Object
     * glBindVertexArray(VAO);
     * // 2. copy our vertices array in a buffer for OpenGL to use
     * glBindBuffer(GL_ARRAY_BUFFER, VBO);
     * glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     * // 3. then set our vertex attributes pointers
     * glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
     * glEnableVertexAttribArray(0);  
     * 
     * [...]
     *
     * // ..:: Drawing code (in render loop) :: ..
     * // 4. draw the object
     * glUseProgram(shaderProgram);
     * glBindVertexArray(VAO);
     * someOpenGLFunctionThatDrawsOurTriangle();
     */


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);


    // Uncomment this call to draw in wireframe polygons. Default is glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    // Render loop
    //---------------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Input
        // -----
        processInput(window);

        // Render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);  // Specify the color to clear the screen with.
        glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer.
        
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // draw our first triangle
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // Draw rectangle
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        //glBindVertexArray(0);

        // Draw 2 triangles
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //---------------------------------------------------------------------------------------------------------


    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);


    // As soon as we exit the render loop we would like to properly clean/delete all of GLFW's resources that were allocated.
    glfwTerminate();
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
