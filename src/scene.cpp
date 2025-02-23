#include "scene.h"
#include <GLFW/glfw3.h>
#include "application.h"
#include <print>
#include <imgui.h>

namespace vica {

    void SceneLibrary::add(Ref<Scene> scene) {
        if (m_Scenes.find(scene->getName()) == m_Scenes.end()) {
            m_Scenes.emplace(scene->getName(), scene);
            if (!m_ActiveScene)
                m_ActiveScene = scene;
        }
    }

    void SceneLibrary::show(const std::string& name) {
        if (m_Scenes.find(name) != m_Scenes.end()) {
            m_ActiveScene = m_Scenes[name];

            auto& app = Application::Get();
            GLFWwindow* window = app.getWindowHandle();
            if (app.getWindowHandle()) {

                if (!m_ActiveScene->isResizable()) {
                    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_FALSE);
                    glfwSetWindowSize(window, m_ActiveScene->getWidth(), m_ActiveScene->getHeight());
                }
                else
                    glfwSetWindowAttrib(window, GLFW_RESIZABLE, GLFW_TRUE);

                glfwSetWindowTitle(window, m_ActiveScene->getName().append(" - ").append(app.getSpecs().name).c_str());
            }
        }
    }

    void customTitleBar(Timestep ts) {
        ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        auto& app = Application::Get();
        auto window = app.getWindowHandle();

        ImGui::BeginGroup(); // Start a group for the title bar

        float titleBarHeight = ImGui::GetFrameHeight();
        ImVec2 titleBarPos = ImGui::GetCursorScreenPos();
        ImVec2 titleBarSize = ImVec2(ImGui::GetWindowSize().x, titleBarHeight);

        drawList->AddRectFilled(titleBarPos, { titleBarPos.x + titleBarSize.x, titleBarPos.y + titleBarSize.y }, IM_COL32(0, 0, 0, 0));

        // **Button Settings**
        float buttonSize = titleBarHeight * 0.8f;
        float buttonPadding = style.FramePadding.x;
        float x = ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - buttonPadding - buttonSize;
        float y = titleBarPos.y + (titleBarHeight - buttonSize) / 2.0f;

        ImGui::SetCursorScreenPos({ titleBarPos.x + style.FramePadding.x, titleBarPos.y });

        // **Icon Button (Example: App Logo)**
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));

        ImGui::ImageButton("favicon", (ImTextureID)app.getImage("fav.png")->getID(), { buttonSize * 5, buttonSize * 2 });
        
        ImGui::PopStyleColor(3);


        ImGui::EndGroup();

        // Move window content below title bar
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + titleBarHeight);
    }


    void Scene::onUpdate(Timestep ts) {
        auto& app = Application::Get();

        if (app.getSpecs().isInCategory(ApplicationFlag_CustomTitleBar) && m_ShowCustomeTitleBar) {
            if (app.getCustomTitleBar())
                app.getCustomTitleBar()(ts);
            else
                customTitleBar(ts);
        }

        onUIRender(ts);
    }

} // namespace vica