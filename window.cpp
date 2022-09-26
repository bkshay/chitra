// This file is a part of  chitra (चित्र)
// Copyright (C) 2022  akshay bansod <akshay.bnsod@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "window.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

bool isGladInited = false,  isGlfwInited = false;

window::window(windowConfig& cfg){

    if(!isGlfwInited) isGlfwInited = glfwInit();

    // setup monitor
    int monitorCount;
    GLFWmonitor* targetMon = glfwGetMonitors(&monitorCount)
        [ (cfg.monitorIndex < monitorCount) ? cfg.monitorIndex : 0 ];

    // use display resoulution for height and width
    if(!cfg.height || !cfg.width)
        glfwGetMonitorWorkarea(targetMon, NULL, NULL, &cfg.width, &cfg.height);

    // create window
    m_win = glfwCreateWindow(
        m_width = cfg.width, 
        m_height = cfg.height, 
        cfg.title, 
        (cfg.setFullscreen) ? targetMon : NULL,
        NULL
    );
    
    // switch context
    GLFWwindow* prevCtx = glfwGetCurrentContext();
    glfwMakeContextCurrent(m_win);

    // initialize glad
    if(!isGladInited) isGladInited = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // setup input callbacks
    switchHandler(cfg.handler);
    glfwSetKeyCallback(m_win, glfwKeyCallback);
    glfwSetDropCallback(m_win, glfwDropCallback);
    glfwSetWindowSizeCallback(m_win, glfwResizeCallback);
    glfwSetCursorPosCallback(m_win, glfwCursorPosCallback);
    glfwSetWindowCloseCallback(m_win, glfwWindowCloseCallback);
    glfwSetMouseButtonCallback(m_win, glfwMouseButtonCallback);

    glfwMakeContextCurrent(prevCtx);
};


window::inputHandler* window::switchHandler(inputHandler* handler){

    handler->display = this;
    inputHandler* prev = m_handler;
    m_handler = handler;
    glfwSetWindowUserPointer(m_win, handler);
    return prev;
};


window::~window(){
    glfwDestroyWindow(m_win);
};



// static window callbacks
void window::glfwCursorPosCallback(GLFWwindow* win, double x, double y){
    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);

    // update handler dataset
    handler.dposX = x - handler.posX;
    handler.dposY = y - handler.posY;
    handler.posX = x;
    handler.posY = y;

    handler.cursorUpdate();
};

void window::glfwKeyCallback(GLFWwindow* win, int key, int scancode, int action, int mods){
    if(action == GLFW_REPEAT) return;

    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);
    handler.keyStateUpdate(key, action);
};

void window::glfwWindowCloseCallback(GLFWwindow* win){
    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);
    handler.close();
};

void window::glfwResizeCallback(GLFWwindow* win, int width, int height){
    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);
    handler.width = width;
    handler.height = height;

    handler.resized();
};

void window::glfwMouseButtonCallback(GLFWwindow* win, int button, int action, int mods){

    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);
    handler.mouseButtonUpdate(button, action);
};


void window::glfwDropCallback(GLFWwindow* win, int path_count, const char* paths[]){
    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);

    for(int i=0; i < path_count; i++){
        handler.dropInput(paths[i]);
    };
};

void window::glfwScrollCallback(GLFWwindow* win, double x, double y){
    inputHandler& handler = *(inputHandler*) glfwGetWindowUserPointer(win);

    handler.dScrollX = x - handler.scrollX;
    handler.dScrollY = y - handler.scrollY;
    handler.scrollX = x;
    handler.scrollY = y;

    handler.cursorUpdate();
};
