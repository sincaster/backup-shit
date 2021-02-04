#include <GL/glew.h>
#include <GL/freeglut.h>

#include <SDL2/SDL.h>

#include "def.hh"

#include <cstdio>
#include <cstdlib>

int main(int, char **)
{
    glewExperimental = GL_TRUE;
    
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Demo", 
        Win.x, Win.y,
        Win.h, Win.w,
        Win.flags);        
          
    SDL_GL_CreateContext(win);
    
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    GLenum glew_status = glewInit();
    
    if (glew_status != GLEW_OK)
    {
        std::fprintf(stderr, "glew_init() failed! -> %s\n", glewGetErrorString(glew_status));
        std::exit(EXIT_FAILURE);
    }
    
    
//    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_FOG);
    
    glShadeModel(GL_FLAT);
    
    glDisable(GL_DITHER);
    
    if (!init("shader.glsl", "shader.frag")) std::exit(EXIT_FAILURE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    SDL_Event ev;
    for (; ev.type != SDL_QUIT; SDL_PollEvent(&ev))
    {	
        if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            glViewport(0, 0, ev.window.data1, ev.window.data2);
            
        programLogic();
        render(win);
        //stencilTest(win);
    }
    
    // free resources    
    glDeleteProgram(program);
    std::puts("destroyed program");
    //glDeleteBuffers(1, &triangle.vbo);
    //glDeleteBuffers(1, &triangle.vbo_colors);
    glDeleteBuffers(1, &cube.vbo);
    glDeleteBuffers(1, &cube.vbo_colors);
    glDeleteBuffers(1, &cube.ibo_elements);
    std::puts("destroyed vbo(s)");
}