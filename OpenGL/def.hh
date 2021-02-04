#include <cstdio>
#include <cstdlib>

#include <fstream>
#include <sstream>
#include <string>

#include <array>

#define GLM_MESSAGES
#define GLM_FORCE_INLINE
#define GLM_FORCE_SSE2

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Window
{
    const int x =SDL_WINDOWPOS_CENTERED;
    const int y = SDL_WINDOWPOS_CENTERED;
    const int w = 1280, h = 800;

    const uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
};

struct Cube
{
    const GLfloat vertices[24] = {
        // front
        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // back
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
        -1.0,  1.0, -1.0
    };
    
    const GLfloat colors[24] = {
         // a color for each vertex
         1.0, 0.0, 1.0,
         0.0, 1.0, 0.0,
         1.0, 0.0, 1.0,
         1.0, 1.0, 1.0,
         // back face
         1.0, 1.5, 0.5,
         0.0, 1.0, 1.0,
         1.5, 0.5, 1.0,
         1.0, 1.0, 1.0,
    };
    
    // Vertex Buffer Objects
    GLuint vbo, vbo_colors;    
    // Index Buffer Object
    // This way we can define some number of vertices
    // if we want to use them for other sides that share them
    GLuint ibo_elements;

    const GLushort elements[36] = {
         // front face
         0, 1, 2,
         2, 3, 0,
         // right
         1, 5, 6,
         6, 2, 1,
         // back
         7, 6, 5,
         5, 4, 7,
         // left
         4, 0, 3,
         3, 7, 4,
         // bottom
         4, 5, 1,
         1, 0, 4,
         // top
         3, 2, 6,
         6, 7, 3,
    };
};

struct Attributes
{
      GLint coord, v_color;
      std::array<const char*, 2> name { {"coord", "v_color"} };
};

struct Uniform
{
    GLint fade, mat_transform, mvp;
    std::array<const char*, 3> name { {"fade", "mat_transform", "mvp"} };
};

Window Win;
Cube cube;
Attributes attrib;
Uniform uniform;

GLuint program;

//GLfloat fogColor[4] = {0.0, 0.0, 0.0, 1.0};

// load shaders, generate & bind buffers etc.
static bool init(const char *vtxShader_filename, const char *fragShader_filename)
{    
    glGenBuffers(1, &cube.ibo_elements);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ibo_elements);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube.elements), cube.elements, GL_STATIC_DRAW);

    glGenBuffers(1, &cube.vbo_colors);
    glBindBuffer(GL_ARRAY_BUFFER, cube.vbo_colors);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube.colors), cube.colors, GL_STATIC_DRAW);

    GLint compile_ok = GL_FALSE, link_ok = GL_FALSE;

    std::ifstream vtxShader_file;
    vtxShader_file.open(vtxShader_filename, std::ifstream::in);
    
    std::string vtxShader_str;
    const char *vtxShader_src;
    
    if (vtxShader_file.is_open())
    {
        std::basic_stringstream<char> ss;
        ss << vtxShader_file.rdbuf();
        
        vtxShader_str = ss.str();
        vtxShader_src = vtxShader_str.c_str();
    }
    else
    {
        std::fprintf(stderr, "Cannot find file %s!\n", vtxShader_filename);
        std::exit(EXIT_FAILURE);
    }
    
    vtxShader_file.close();

    GLuint vtxShader = glCreateShader(GL_VERTEX_SHADER);
    
    glShaderSource(vtxShader, 1, &vtxShader_src, NULL);
    glCompileShader(vtxShader);
    glGetShaderiv(vtxShader, GL_COMPILE_STATUS, &compile_ok);
    
    if (!compile_ok)
    {
        std::fputs("Vertex shader error!", stderr);
        return false;      
    }
    
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER); 
    
    std::ifstream fragShader_file;
    fragShader_file.open(fragShader_filename, std::ifstream::in);
       
    std::string fragShader_str;
    const char *fragShader_src;
    
    if (fragShader_file.is_open())
    {
        std::basic_stringstream<char> ss;
        ss << fragShader_file.rdbuf();
        
        fragShader_str = ss.str();
        fragShader_src = fragShader_str.c_str();
        
        fragShader_file.close();
    }
    else
    {
        std::fprintf(stderr, "Cannot find file %s!\n", fragShader_filename);
        std::exit(EXIT_FAILURE);
    }
    
    glShaderSource(fragShader, 1, &fragShader_src, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &compile_ok);
    
    if (!compile_ok)
    {
        std::fputs("Fragment shader error!", stderr);
        return false;
    }

    program = glCreateProgram();
      
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);
    
    glLinkProgram(program);
    
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    
    if (!link_ok)
    {
        std::fputs("Linking failed!\n", stderr);
        return false;
    }
    
    attrib.coord   = glGetAttribLocation(program, std::get<0>(attrib.name));
    attrib.v_color = glGetAttribLocation(program, std::get<1>(attrib.name));

    uniform.fade = glGetUniformLocation(program, std::get<0>(uniform.name));
    //uniform.mat_transform = glGetUniformLocation(program, std::get<1>(uniform.name));
    uniform.mvp = glGetUniformLocation(program, std::get<2>(uniform.name));
        
    if (attrib.coord == -1)
    {
        std::fputs("Could not bind attribute coord!", stderr);
        return false;
    }
    
    if (attrib.v_color == -1)
    {
        std::fputs("Coult not bind attribute v_color!", stderr);
        return false;
    }

    if (uniform.fade == -1)
    {
        std::fputs("Could not bind uniform fade!", stderr);
        return false;
    }

/*    
    if (uniform.mat_transform == -1)
    {
        std::fputs("Could not bind uniform mat_transformation!", stderr);
        return false;
    }
*/
    
    if (uniform.mvp == -1)
    {
        std::fputs("Could not bind uniform mvp!", stderr);
        return false;
    }
    
    return true;
}

/*
void fogInit(GLint x, GLint y, GLsizei width, GLsizei height)
{
    GLenum fog_mode = GL_EXP2;
    GLfloat fog_start = 0.0, fog_end = 1.0;
    GLfloat fog_density = 1.0;
    
    glFogi(GL_FOG_MODE, fog_mode);
    glHint(GL_FOG_HINT, GL_NICEST);
    
    glFogf(GL_FOG_DENSITY, fog_density);
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end); 
    
    glFogfv(GL_FOG_COLOR, fogColor);
    
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
}
*/

static inline void render(SDL_Window *win)
{    
    glClearColor(0.7f, 0.5f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.vbo);
    glEnableVertexAttribArray(attrib.coord);
    glVertexAttribPointer(attrib.coord, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ibo_elements);
    
    int size;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    glDrawElements(GL_TRIANGLE_FAN, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

    glEnableVertexAttribArray(attrib.v_color);    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.vbo_colors);
    glVertexAttribPointer(attrib.v_color, 3, GL_FLOAT, GL_FALSE, 0, 0);

//    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glDisableVertexAttribArray(attrib.coord);
    glDisableVertexAttribArray(attrib.v_color);
    
    SDL_GL_SwapWindow(win);
}

/*
static void stencilTest(SDL_Window *win)
{
//    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_FALSE);
    glStencilFunc(GL_NEVER, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    
    glStencilMask(0xFF);
    
    render(win);
        
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);
    glStencilMask(0x00);
    
    glStencilFunc(GL_EQUAL, 0, 0xFF);
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    
    render(win);
}
*/

static inline float aspectaxis()
{
    float outputzoom = 1.0f;
    float aspectorigin = 16.0f / 9.0f; // aspect ratio
    int aspectconstraint = 1;
    
    switch (aspectconstraint)
    {
        case 1:
            if ((Win.w / Win.h) < aspectorigin)
                outputzoom *= static_cast<float>((Win.w / Win.h) / aspectorigin);
            else outputzoom *= static_cast<float>(aspectorigin / aspectorigin);
            break;
        case 2:
            outputzoom *= static_cast<float>((Win.w / Win.h) / aspectorigin);
            break;
       default:
            outputzoom *= static_cast<float>(aspectorigin / aspectorigin);
    }
    
    return outputzoom;
}

static inline float recalculateFOV()
{
    return 2.0f * glm::atan(glm::tan(glm::radians(45.0f / 2.0f)) / aspectaxis());
}

static inline void programLogic(void)
{    
    GLfloat fade = sinf(SDL_GetTicks() / 697.0) + 0.01;
    glUniform1f(uniform.fade, fade); 
    
    GLfloat delta = (SDL_GetTicks() / 1000.0) * 180; // 180 degree angle rotation
        
    static const glm::vec3 xaxis(1, 0, 0);
    static const glm::vec3 yaxis(0, 1, 0);    
    static const glm::vec3 zaxis(0, 0, 1);

    // rotate around the x, y or z axis
    // do matrix transformations
    // const glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, fade)) * glm::rotate(glm::mat4(0.5f), glm::radians(delta), glm::smoothstep(zaxis, yaxis, xaxis));
    
    const glm::mat4 anim = glm::rotate(glm::mat4(1.0), glm::radians(delta), glm::smoothstep(zaxis, yaxis, xaxis));
      
    const glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
    const glm::mat4 projection = glm::perspective(recalculateFOV(), 1.0f * Win.w / Win.h, 0.1f, 10.0f);
    const glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
    const glm::mat4 mvp = projection * view * model;
    
    glUniformMatrix4fv(uniform.mvp, 1, GL_FALSE, glm::value_ptr(mvp));
    
//    glUniformMatrix4fv(uniform.fade, 1, GL_FALSE, glm::value_ptr(transform));
//    glUniformMatrix4fv(uniform.mat_transform, 1, GL_FALSE, glm::value_ptr(transform));
//    fogInit(0, 0, Win.w, Win.h);
}