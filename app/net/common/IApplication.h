#pragma once

class IApplication{
public:
    virtual void render() = 0;
    virtual void update() = 0;
    virtual void applyTheme() = 0;
    virtual void initialize() = 0;
};