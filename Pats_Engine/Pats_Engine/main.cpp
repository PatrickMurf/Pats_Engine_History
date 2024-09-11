#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//Include vector stuff
#include <glm/glm.hpp>  // I assume this helps setup/required for using glm.
#include <glm/vec3.hpp> // 3-vector
#include <glm/vec4.hpp> // 4-vector
#include <glm/mat4x4.hpp>   // Something to do with glm matrixes
#include <glm/gtc/matrix_transform.hpp> // Not sure what this is, but I assume it's important for drawing/manipulating shapes

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

// Libraries for stuff
#include <iostream>
#include <fstream>
#include <string>

struct sVertex
{
    glm::vec3 pos;
    glm::vec3 col;
} Vertex;

/*  Old verticies chart
sVertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};
*/

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec3 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

    


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

#pragma region Camera_stuff

glm::vec3 viewerCam = glm::vec3(0.0, 0.0, 4.0f);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    const float CAMERA_MOVE_SPEED = 0.1f;

    if (key == GLFW_KEY_A)
    {
        viewerCam.x -= CAMERA_MOVE_SPEED;
    }

    if (key == GLFW_KEY_D)
    {
        viewerCam.x += CAMERA_MOVE_SPEED;
    }

    if (key == GLFW_KEY_W)
    {
        viewerCam.z -= CAMERA_MOVE_SPEED;
    }

    if (key == GLFW_KEY_S)
    {
        viewerCam.z += CAMERA_MOVE_SPEED;
    }
}
#pragma endregion Camera_stuff

int main(void)
{
#pragma region Model_loading_stuff

    // Setup file paths
    std::ifstream plyBunnyFile("assets/models/bun_zipper_res3.ply");
    //std::ifstream plyBunnyFile("assets/models/lobster.ply");

    // Setup read assets
    std::string fileCursor = "";

#pragma region File_processing-Header_data

    // Sift for vertex count
    while (fileCursor != "vertex")
    {
        plyBunnyFile >> fileCursor;
    }
    // Get vertex count
    int fileVertexCount = 0;
    plyBunnyFile >> fileVertexCount;

    // Sift for face count (aka: number of triangles)
    while (fileCursor != "face")
    {
        plyBunnyFile >> fileCursor;
    }
    // Get face count
    int fileFaceCount = 0;
    plyBunnyFile >> fileFaceCount;

    // Sift for end of header
    while (fileCursor != "end_header")
    {
        plyBunnyFile >> fileCursor;
    };

#pragma endregion File_processing-Header_data
#pragma region File_processing-vertex_data

    // Structures used in processing
    struct sPlyFileVertex
    {
        float x, y, z, confidence, intensity;
        //float x, y, z;
    };
    struct sTriangle
    {
        unsigned int vertexIndex_0;
        unsigned int vertexIndex_1;
        unsigned int vertexIndex_2;
    };

    // Special readers for the file
    sPlyFileVertex* pPlyFileVertices = new sPlyFileVertex[fileVertexCount];
    sTriangle* pPlyTriangles = new sTriangle[fileFaceCount];

    // Start scooping the vertices data
    for (unsigned scoopCursor = 0; scoopCursor != fileVertexCount; scoopCursor++)
    {
        plyBunnyFile >> pPlyFileVertices[scoopCursor].x;
        plyBunnyFile >> pPlyFileVertices[scoopCursor].y;
        plyBunnyFile >> pPlyFileVertices[scoopCursor].z;
        plyBunnyFile >> pPlyFileVertices[scoopCursor].confidence;
        plyBunnyFile >> pPlyFileVertices[scoopCursor].intensity;
    }

    // Start scooping the face/triangle data
    for (unsigned scoopCursor = 0; scoopCursor != fileFaceCount; scoopCursor++)
    {
        int rubbishData = 0;  // <-- used because there's some human only data

        plyBunnyFile >> rubbishData;
        plyBunnyFile >> pPlyTriangles[scoopCursor].vertexIndex_0;
        plyBunnyFile >> pPlyTriangles[scoopCursor].vertexIndex_1;
        plyBunnyFile >> pPlyTriangles[scoopCursor].vertexIndex_2;
    }

#pragma endregion File_processing-Vertex_data

   #pragma endregion Model_loading_stuff

    unsigned int numOfDrawVertices = fileFaceCount * 3;

    //Declaring verticies as pointer / static array.
    sVertex* pVerticies = new sVertex[numOfDrawVertices];

    /*
    pVerticies[0] = { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } };
    pVerticies[1] = { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } };
    pVerticies[2] = { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } };
    */

    // Loading all the file's data into a usebale form
    unsigned int vertexIndex = 0;
    for (unsigned int triangleIndex = 0; triangleIndex != fileFaceCount; triangleIndex++)
    {
        // Point 1 of the triangle
        pVerticies[vertexIndex + 0].pos.x = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_0].x;
        pVerticies[vertexIndex + 0].pos.y = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_0].y;
        pVerticies[vertexIndex + 0].pos.z = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_0].z;
        pVerticies[vertexIndex + 0].col.r = 1.0f;
        pVerticies[vertexIndex + 0].col.g = 1.0f;
        pVerticies[vertexIndex + 0].col.b = 1.0f;

        // Point 2 of the triangle
        pVerticies[vertexIndex + 1].pos.x = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_1].x;
        pVerticies[vertexIndex + 1].pos.y = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_1].y;
        pVerticies[vertexIndex + 1].pos.z = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_1].z;
        pVerticies[vertexIndex + 1].col.r = 1.0f;
        pVerticies[vertexIndex + 1].col.g = 1.0f;
        pVerticies[vertexIndex + 1].col.b = 1.0f;

        // Point 3 of the triangle
        pVerticies[vertexIndex + 2].pos.x = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_2].x;
        pVerticies[vertexIndex + 2].pos.y = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_2].y;
        pVerticies[vertexIndex + 2].pos.z = pPlyFileVertices[pPlyTriangles[triangleIndex].vertexIndex_2].z;
        pVerticies[vertexIndex + 2].col.r = 1.0f;
        pVerticies[vertexIndex + 2].col.g = 1.0f;
        pVerticies[vertexIndex + 2].col.b = 1.0f;

        // Move on to next set
        vertexIndex += 3;
    }



    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    int byte_size_of_vertex_array = sizeof(sVertex) * numOfDrawVertices ;

    glBufferData(GL_ARRAY_BUFFER,
                    byte_size_of_vertex_array,    /*sizeof(vertices),*/
                    pVerticies,
                    GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(
        vpos_location,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(
        vcol_location,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(sVertex),
        (void*)offsetof(sVertex, col));

    while (!glfwWindowShouldClose(window))
    {
        float screenRatio;  //I think that's what this is...?
        int width, height;
        glm::mat4 m, p, v, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        screenRatio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        m = glm::mat4(1.0f);

        glm::mat4 rotateZ =
            glm::rotate(glm::mat4(1.0f),
                0.0f,
                glm::vec3(0.0f, 0.0, 1.0f));

        m = m * rotateZ;

        p = glm::perspective(0.6f,
            screenRatio,
            0.1f,
            1000.0f);
            
        v = glm::mat4(1.0f);


        /*
        mat4x4 m, p, mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float)glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        */
        /*
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        */

        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

        v = glm::lookAt(
            viewerCam,
            cameraTarget,
            upVector);

        mvp = p * v * m;
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)&mvp);
        glBindVertexArray(vertex_array);
        glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);
        glDrawArrays(GL_TRIANGLES, 0, numOfDrawVertices);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}