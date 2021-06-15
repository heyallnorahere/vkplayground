#pragma once
class vkplayground : public libplayground::vk::application {
public:
    vkplayground();
protected:
    virtual void load() override;
    virtual void update() override;
    virtual void render() override;
    virtual void unload() override;
};