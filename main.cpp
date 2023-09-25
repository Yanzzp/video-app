#include <iostream>
#include <GLFW/glfw3.h>

#include "load_frame.cpp"
#include "video_reader.h"

using namespace std;

int main() {

    GLFWwindow* window;
    if(!glfwInit()) {
        cout << "glfw init failed" << endl;
        return -1;
    }
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if(!window) {
        cout << "glfw create window failed" << endl;
        glfwTerminate();
        return -1;
    }



    VideoReaderState vr_state;
    if(!video_reader_open(&vr_state, "../test.mp4")) {
        cout << "video reader open failed" << endl;
        return -1;
    }
    const int frame_width = vr_state.width;
    const int frame_height = vr_state.height;
    uint8_t *frame_data = new uint8_t[frame_width * frame_height * 4];

    if(!video_reader_read_frame(&vr_state, frame_data)) {
        cout << "video reader read frame failed" << endl;
        return -1;
    }

    video_reader_close(&vr_state);
//    if(!load_frame1("../test.mp4", &frame_width, &frame_height, &frame_data)) {
//        cout << "load frame failed" << endl;
//        return -1;
//    }




    GLuint tex_handle;
    glfwMakeContextCurrent(window);
    glGenTextures(1, &tex_handle);
    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV,
              GL_TEXTURE_ENV_MODE,
              GL_MODULATE);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 frame_width,
                 frame_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 frame_data);

    while(!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


        int window_width, windows_height;
        glfwGetFramebufferSize(window, &window_width, &windows_height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, window_width, windows_height, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);

        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex_handle);
        glBegin(GL_QUADS);
            glTexCoord2d(0,0); glVertex2i(200,200);
        glTexCoord2d(1,0); glVertex2i(200+frame_width,200);
        glTexCoord2d(1,1); glVertex2i(200+frame_width,200+frame_height);
        glTexCoord2d(0,1); glVertex2i(200,200+frame_height);
        glEnd();
        glDisable(GL_TEXTURE_2D);

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }
    return 0;
}