#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

// Include stb_easy_font with implementation
#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

void drawText(float x, float y, const char *text, float r, float g, float b) {
    // Set text color
    glColor3f(r, g, b);
    
    // Allocate vertex buffer
    static char buffer[99999];
    
    // Generate text vertices - returns number of quads
    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));
    
    // Debug output
    printf("Generated %d quads for text: %s\n", num_quads, text);
    
    // Only try to render if we have vertices
    if (num_quads > 0) {
        // Draw the text using vertex arrays
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void drawRedRectangle(int x, int y) {
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x+100, y);
    glVertex2f(x+100, y+20);
    glVertex2f(x, y+20);
    glEnd();
}

int main(int argc, char** argv) {
    if (!glfwInit()) {
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "Text Rendering Demo", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        glfwTerminate();
        printf("GLEW problem: %s\n", glewGetErrorString(err));
        return -1;
    }
    
    // Set up orthographic projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 800, 600, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable blending for text
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw red rectangle
        drawRedRectangle(50, 50);
        
        // Draw text in different colors and positions
        drawText(50, 100, "Hello, World!", 0.0, 0.0, 1.0); // Blue text
        drawText(50, 150, "OpenGL Text Rendering", 0.0, 1.0, 0.0); // Green text
        drawText(50, 200, "With stb_easy_font", 1.0, 1.0, 0.0); // Yellow text
        
        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
        
        // Handle ESC key to exit
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    
    glfwTerminate();
    return 0;
}


// g++ -I/opt/homebrew/include -L/opt/homebrew/lib -lGLEW -lglfw -framework OpenGL stb_test.cpp