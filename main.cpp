#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include<iostream>
#include <filesystem>
#include <random>
#include <ctime>
#include <vector>


#include<render.h>


void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

float* moveVertex(float x,float y,float m) {
    static float vertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
       -0.5f, -0.5f, 0.0f,  // bottom left
       -0.5f,  0.5f, 0.0f   // top left 
    };
    vertices[0] += x; // 修改第一个顶点的x坐标
	vertices[1] += y; // 修改第一个顶点的y坐标
	vertices[0] += m; // 修改第二个顶点的x坐标
	vertices[3] += m; // 修改第四个顶点的x坐标
	vertices[6]  += m; // 修改第三个顶点的x坐标
	vertices[9] += m; // 修改第四个顶点的y坐标
    return vertices;
}

//glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
//glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool enable = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// 网格参数
float unitLength = 1.0f;  // 单位长度
int gridSize = 5;        // 网格大小（从原点向每个方向延伸的单位数）


void generateCoordinateAxes(std::vector<float>& vertices) {
    // X轴 (红色)
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { 5.0f, 0.0f, 0.0f });

    // Y轴 (绿色)
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { 0.0f, 5.0f, 0.0f });

    // Z轴 (蓝色)
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { 0.0f, 0.0f, 5.0f });
}

// 生成虚线网格顶点数据
void generateGridLines(std::vector<float>& vertices) {
    float extent = gridSize * unitLength;

    // 生成网格线
    for (int i = -gridSize; i <= gridSize; ++i) {
        float pos = i * unitLength;

        // XZ平面网格
        vertices.insert(vertices.end(), { -extent, 0.0f, pos });
        vertices.insert(vertices.end(), { extent, 0.0f, pos });

        vertices.insert(vertices.end(), { pos, 0.0f, -extent });
        vertices.insert(vertices.end(), { pos, 0.0f, extent });

        //// XY平面网格
        //vertices.insert(vertices.end(), { -extent, pos, 0.0f });
        //vertices.insert(vertices.end(), { extent, pos, 0.0f });

        //vertices.insert(vertices.end(), { pos, -extent, 0.0f });
        //vertices.insert(vertices.end(), { pos, extent, 0.0f });

        //// YZ平面网格
        //vertices.insert(vertices.end(), { 0.0f, -extent, pos });
        //vertices.insert(vertices.end(), { 0.0f, extent, pos });

        //vertices.insert(vertices.end(), { 0.0f, pos, -extent });
        //vertices.insert(vertices.end(), { 0.0f, pos, extent });
    }
}

// 设置VAO和VBO
void setupBuffers(unsigned int& VAO, unsigned int& VBO, const std::vector<float>& vertices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}




int main()
{
    //std::cout << "当前工作目录: " << std::filesystem::current_path() << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

	Shader ourShader("shader/shadervs.txt", "shader/shaderfs.txt");
    
    Shader shaderProgram("C:\\opengl\\OpenGLTest\\shader\\shadervs.txt", "C:\\opengl\\OpenGLTest\\shader\\axis.fs.txt");

    // 生成顶点数据
    std::vector<float> axesVertices;
    std::vector<float> gridVertices;

    generateCoordinateAxes(axesVertices);
    generateGridLines(gridVertices);

    ourShader.use();

    /*
    std::default_random_engine e(time(0)); // 使用当前时间作为种子
    std::uniform_int_distribution<int> u(1, 100); // 定义一个均匀分布
    std::default_random_engine f(time(0)); // 使用当前时间作为种子
    std::uniform_int_distribution<int> v(-200, 200); // 定义一个均匀分布

    */

	

	PolyCurve curve({ 0.0f, 1.0f, -2.0f, 3.0f });
	PolyCurve curve2({ 0.0f });
	PolyCurve curve3({ 1.0f, -1.0f });
	RenderManager manager;
	manager.AddCurve(curve);
	manager.AddCurve(curve2);
	manager.AddCurve(curve3);



    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
// --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

     //   glBindBuffer(GL_ARRAY_BUFFER, VBO);
      //  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), moveVertex(x/12000,y/12000,m/1200), GL_STATIC_DRAW);
	//	x = x + u(e)/100.0f-0.501;
	//	y = y + v(f) / 100.0f-0.002;
     //   int tmp = u(e);


      //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
      //  glEnableVertexAttribArray(0);
        // draw our first triangle
      //  glUseProgram(shaderProgram);
      //  glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        //glDrawArrays(GL_TRIANGLES, 0, 6);
      //  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0); // no need to unbind it every time 

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
      //  glUseProgram(shaderProgram);
      // 
      //  LineRender(cases).Draw();
	//	test.Draw();

    //    ourShader.use();
        float t = glfwGetTime();
        //std::cout << glfwGetTimerValue() << std::endl;
        glm::mat4 trans = glm::mat4(1.0f);
      //  trans = glm::rotate(trans, glm::radians(1.0f), glm::vec3(0.0, 1.0, 0.0));
        //std::cout << (float)glfwGetTime() << std::endl;
        //trans = glm::scale(trans, glm::vec3(sin(t), cos(t), 0.5));

        ourShader.setMat4("transform", trans);

        //view matrix
        const float radius = 8.0f;
        float camX = sin(t) * radius;
        float camZ = cos(t) * radius;
        //glm::mat4 view;
        
        //view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

        //view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);
        
        //projection matrix
        //glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        manager.Render();

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
 
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
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

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (enable) {
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        enable = !enable;
    }
}


// 绘制坐标轴
void drawCoordinateAxes(unsigned int axesVAO,unsigned int id) {
    glUseProgram(id);
    glBindVertexArray(axesVAO);

    // 设置线宽
    glLineWidth(3.0f);

    // 绘制X轴 (红色)
    glm::mat4 model = glm::mat4(1.0f);
    unsigned int modelLoc = glGetUniformLocation(id, "model");
    unsigned int colorLoc = glGetUniformLocation(id, "lineColor");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // 红色
    glDrawArrays(GL_LINES, 0, 2);

    // 绘制Y轴 (绿色)
    glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); // 绿色
    glDrawArrays(GL_LINES, 2, 2);

    // 绘制Z轴 (蓝色)
    glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); // 蓝色
    glDrawArrays(GL_LINES, 4, 2);
}

// 绘制虚线网格
void drawGrid(unsigned int gridVAO, int vertexCount, unsigned int id) {
    glUseProgram(id);
    glBindVertexArray(gridVAO);

    // 设置线宽和虚线模式
    glLineWidth(1.0f);

    // 启用线段虚线模式
   // glEnable(GL_LINE_STIPPLE);
    //glLineStipple(1, 0x00FF); // 虚线模式

    // 设置网格颜色 (灰色)
    unsigned int colorLoc = glGetUniformLocation(id, "lineColor");
    glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);

    glm::mat4 model = glm::mat4(1.0f);
    unsigned int modelLoc = glGetUniformLocation(id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_LINES, 0, vertexCount);

    //glDisable(GL_LINE_STIPPLE);
}