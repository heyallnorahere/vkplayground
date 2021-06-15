#pragma once
#include "application.h"
class vkplayground_app : public vkplayground::application {
public:
    vkplayground_app();
protected:
    virtual void load() override;
    virtual void update() override;
    virtual void render() override;
    virtual void unload() override;
};