#pragma once
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include "IApplication.h"

class CoreApplication{
public:
    CoreApplication(const std::shared_ptr<IApplication>& app);
    ~CoreApplication();

    void run();
    void setTitle(const std::string&);
    void resize(int width, int height);
    void move(int x, int y);
    void setWindowIcon(const std::string& path);
    void setWindowIcon(const unsigned char* data, size_t size);

private:
    bool init();
    void initializeImGui();
    void shutdownImGui();
private:
    GLFWwindow * mWindow{};
    std::shared_ptr<IApplication> mApp;
};
