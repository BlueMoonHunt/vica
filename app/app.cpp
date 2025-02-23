#include "application.h"
#include "appTheme.h"
#include <imgui.h>

class MainScene : public vica::Scene {
public:
    MainScene() : Scene("Main Scene", true) {}

    virtual void onUIRender(vica::Timestep ts) override {
        ImGui::Text("Welcome to the VICA Application!");
        ImGui::Text("Here are some things you can do:");
        ImGui::BulletText("Create and manage scenes.");
        ImGui::BulletText("Add various UI elements to your scenes.");
        ImGui::BulletText("Customize the application theme.");
    }
};

int main() {
    vica::Application app("Co Clock", 900, 600, ApplicationFlag_CustomTitleBar);
    setAppTheme();

    auto& scenes = app.getScenes();
    scenes.add(CreateRef<MainScene>());
    
    app.run();
    return 0;
}