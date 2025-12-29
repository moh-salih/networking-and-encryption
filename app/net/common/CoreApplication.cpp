#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>

#include <GL/glew.h>

#include <vector>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app/net/common/CoreApplication.h"

CoreApplication::CoreApplication(const std::shared_ptr<IApplication>& app): mApp(app){
    init();
}

CoreApplication::~CoreApplication(){
    shutdownImGui();

    if(mWindow){
        glfwDestroyWindow(mWindow);
        glfwTerminate();
    }
}

bool CoreApplication::init(){
    if(!glfwInit()){
        return false;
    }

    mWindow = glfwCreateWindow(600, 600, "Chat", nullptr, nullptr);
    if(!mWindow) return false;

    glfwMakeContextCurrent(mWindow);
    glfwSwapInterval(1);


    if(glewInit() != GLEW_OK) return false;

    // Initialize ImGui
    initializeImGui();

    return true;
}

void CoreApplication::run(){
    while(!glfwWindowShouldClose(mWindow)){
        glfwPollEvents();
        if(mApp){
            mApp->update();
        }

        // New frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if(mApp) mApp->render();

        ImGui::Render();

        int width, height;
        glfwGetFramebufferSize(mWindow, &width, &height);
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(mWindow);
    }
}

void CoreApplication::setTitle(const std::string& title){
    if(mWindow) glfwSetWindowTitle(mWindow, title.c_str());
}

void CoreApplication::resize(int width, int height){
    if(mWindow) glfwSetWindowSize(mWindow, width, height);
}

void CoreApplication::move(int x, int y){
    if(mWindow) glfwSetWindowPos(mWindow, x, y);
}
void CoreApplication::setWindowIcon(const std::string& path){
    int w, h, channels;
    unsigned char* pixels = stbi_load(path.c_str(), &w, &h, &channels, 4);
    
    if (!pixels){
        printf("Failed to load icon: %s\n", path.c_str());
        return;
    }

    if (!mWindow){
        printf("Cannot set icon: window not created\n");
        stbi_image_free(pixels);
        return;
    }

    GLFWimage img;
    img.width  = w;
    img.height = h;
    img.pixels = pixels;

    glfwSetWindowIcon(mWindow, 1, &img);

    stbi_image_free(pixels);
}

void CoreApplication::setWindowIcon(const unsigned char* data, size_t size){
    int w, h, channels;

    unsigned char* pixels = stbi_load_from_memory(
        data,
        static_cast<int>(size),
        &w,
        &h,
        &channels,
        4
    );

    if (!pixels){
        printf("Failed to load icon from memory\n");
        return;
    }

    if (!mWindow){
        printf("Cannot set icon: window not created\n");
        stbi_image_free(pixels);
        return;
    }

    GLFWimage img;
    img.width  = w;
    img.height = h;
    img.pixels = pixels;

    glfwSetWindowIcon(mWindow, 1, &img);

    stbi_image_free(pixels);
}


void CoreApplication::initializeImGui(){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    if(mApp){
        mApp->applyTheme();
        mApp->initialize();
    }

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/SegoeUI.ttf", 22.0f);

    ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
    ImGui_ImplOpenGL3_Init("#version 330");    

}


void CoreApplication::shutdownImGui(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}