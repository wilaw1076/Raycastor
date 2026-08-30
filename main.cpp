#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
using namespace std;

#include "map.h"
#include "player.h"
#include "raycaster.h"
#include "render3d.h"

// Window size
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const float PI = 3.14159265f;
const float FOV = PI / 2.0f;
const int NUM_RAYS = 120;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

void processInput(GLFWwindow* window, Player& player, float deltaTime);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycastor Project", NULL, NULL);
    if(window == NULL) //If window creation fails, print error and terminate GLFW
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //If GLAD fails to initialize, print error and terminate GLFW
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    framebuffer_size_callback(window, width, height);

    int success;
    char infoLog[512];

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); //drawimg the shape
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
  
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //pixel
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
  
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    Player player;
    float lastFrame = 0.0f;

    //MAP Buffer Object binding and compiling
    std::vector<float> mapVertices;
    std::vector<unsigned int> mapIndices;
    generateMapGeometry(mapVertices, mapIndices);

    unsigned int mapVBO, mapVAO, mapEBO; 
    glGenVertexArrays(1, &mapVAO);
    glGenBuffers(1, &mapVBO);
    glGenBuffers(1, &mapEBO);

    glBindVertexArray(mapVAO);

    glBindBuffer(GL_ARRAY_BUFFER, mapVBO);
    glBufferData(GL_ARRAY_BUFFER, mapVertices.size() * sizeof(float), mapVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mapIndices.size() * sizeof(unsigned int), mapIndices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Player Buffer Object binding and compiling 
    std::vector<float> playerVertices;
    generatePlayerGeometry(player, playerVertices, 32);

    unsigned int playerVBO, playerVAO;
    glGenVertexArrays(1, &playerVAO);
    glGenBuffers(1, &playerVBO);

    glBindVertexArray(playerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
    glBufferData(GL_ARRAY_BUFFER, playerVertices.size() * sizeof(float), playerVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Direction Buffer Object binding and compiling
    std::vector<float> directionVertices;
    generateDirectionGeometry(player, directionVertices);

    unsigned int directionVAO, directionVBO;
    glGenVertexArrays(1, &directionVAO);
    glGenBuffers(1, &directionVBO);

    glBindVertexArray(directionVAO);

    glBindBuffer(GL_ARRAY_BUFFER, directionVBO);

    glBufferData(GL_ARRAY_BUFFER, directionVertices.size() * sizeof(float), directionVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Raycast Buffer Object binding and compiling
    std::vector<float> rayVertices;
    std::vector<RayHit> rayHits;
    float startAngle = player.angle - FOV / 2;
    float angleStep = FOV / (NUM_RAYS-1);

    for(int i = 0; i < NUM_RAYS; i++)
    {
        float rayAngle = startAngle + i * angleStep;

        RayHit rayHit = castRayDDA(player, rayAngle);
        rayHits.push_back(rayHit);

        if(rayHit.hit)
        {
            generateRayGeometry(player, rayHit, rayVertices);
        }
    }
    
    unsigned int rayVBO, rayVAO;
    glGenVertexArrays(1, &rayVAO);
    glGenBuffers(1, &rayVBO);

    glBindVertexArray(rayVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
    glBufferData(GL_ARRAY_BUFFER, rayVertices.size() * sizeof(float), rayVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Wall Buffer Object binding and compiling
    std::vector<float> wallVertices;
    const int initialViewWidth = width / 2;
    const int initialViewHeight = height; 
    generate3DWallGeometry(player, rayHits, wallVertices, startAngle, angleStep, FOV, initialViewWidth, initialViewHeight);

    unsigned int wallVAO, wallVBO;
    glGenVertexArrays(1, &wallVAO);
    glGenBuffers(1, &wallVBO);

    glBindVertexArray(wallVAO);

    glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
    glBufferData(GL_ARRAY_BUFFER, wallVertices.size() * sizeof(float), wallVertices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while(!glfwWindowShouldClose(window)) //Main render loop
    {
        int framebufferWidth;
        int framebufferHeight;

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

        int halfWidth = framebufferWidth / 2;

        //LEFT 2D VIEW

        int mapViewportSize;
        if (halfWidth < framebufferHeight)
        {
            mapViewportSize = halfWidth;
        }
        else
        {
            mapViewportSize = framebufferHeight;
        }
        int mapXOffset = (halfWidth - mapViewportSize) / 2;
        int mapYOffset = (framebufferHeight - mapViewportSize) / 2;

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, player, deltaTime); //Process input

        //Player Regenerator
        playerVertices.clear();
        generatePlayerGeometry(player, playerVertices, 32);
        glBindBuffer(GL_ARRAY_BUFFER, playerVBO);
        glBufferData(GL_ARRAY_BUFFER, playerVertices.size() * sizeof(float), playerVertices.data(), GL_DYNAMIC_DRAW);

        //Direction Indicate Regenerator
        directionVertices.clear();
        generateDirectionGeometry(player, directionVertices);
        glBindBuffer(GL_ARRAY_BUFFER, directionVBO);
        glBufferData(GL_ARRAY_BUFFER, directionVertices.size() * sizeof(float), directionVertices.data(), GL_DYNAMIC_DRAW);

        //Raycast Regenerator
        rayVertices.clear();
        rayHits.clear();
        float startAngle = player.angle - FOV / 2;
        float angleStep = FOV / (NUM_RAYS-1);

        for(int i = 0; i < NUM_RAYS; i++)
        {
            float rayAngle = startAngle + i * angleStep;

            RayHit rayHit = castRayDDA(player, rayAngle);
            rayHits.push_back(rayHit);

            if(rayHit.hit)
            {
                generateRayGeometry(player, rayHit, rayVertices);
            }
        }
        glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
        glBufferData(GL_ARRAY_BUFFER, rayVertices.size() * sizeof(float), rayVertices.data(), GL_DYNAMIC_DRAW);

        //Wall Regenerator
        wallVertices.clear();
        const int viewWidth = framebufferWidth / 2;
        const int viewHeight = framebufferHeight;
        generate3DWallGeometry(player, rayHits, wallVertices, startAngle, angleStep, FOV, viewWidth, viewHeight);
        glBindBuffer(GL_ARRAY_BUFFER, wallVBO);
        glBufferData(GL_ARRAY_BUFFER, wallVertices.size() * sizeof(float), wallVertices.data(), GL_DYNAMIC_DRAW);

        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);

        glViewport(mapXOffset,mapYOffset, mapViewportSize, mapViewportSize);

        //Map render
        glBindVertexArray(mapVAO); //draw this config 
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mapIndices.size()), GL_UNSIGNED_INT, 0); //execs the draw

        //Player render
        glBindVertexArray(playerVAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, static_cast<GLsizei>(playerVertices.size() / 3));

        //Direction Indicator render
        glBindVertexArray(directionVAO);
        glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(directionVertices.size() / 3));

        //Raycast render
        if(!rayVertices.empty())
        {
           glBindVertexArray(rayVAO);
           glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(rayVertices.size() / 3)); 
        }
        glViewport(halfWidth, 0, framebufferWidth - halfWidth, framebufferHeight);

        //Wall render
        glBindVertexArray(wallVAO);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(wallVertices.size() / 3));

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //Set polygon mode to line
        
        glBindVertexArray(0);

        glfwSwapBuffers(window); //Swap the front and back buffers
        glfwPollEvents(); //Poll for and process events
    }
    glfwTerminate(); //Terminate GLFW
    return 0;
}

void processInput(GLFWwindow* window, Player& player, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        movePlayerForward(player, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        movePlayerBackward(player, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        rotatePlayerLeft(player, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        rotatePlayerRight(player, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}