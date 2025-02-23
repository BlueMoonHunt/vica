#pragma once
#include <string>
#include <queue>
#include <functional>

#include "base.h"
#include "event/event.h"
#include "scene.h"
#include "image.h"


struct GLFWwindow;


enum ApplicationFlag_ {
    ApplicationFlag_None = 0,
    ApplicationFlag_Minimized = 1 << 0,
    ApplicationFlag_CustomTitleBar = 1 << 1,
};

namespace vica {
    struct ApplicationSpecifications {
        const char* name = "TITLE";
        int width = 400;
        int height = 300;

        ApplicationFlag applicationFlag = ApplicationFlag_None;

        bool isInCategory(ApplicationFlag_ flag) { return applicationFlag & flag; }
    };

    class Application {
    public:
        Application(const char* title, int width, int height, ApplicationFlag flags);
        ~Application();

        Ref<Image> getImage(const std::string& name);
        bool contains_stb_supported_images(const std::filesystem::path& directory);
        void run();
        void setCustomTitleBar(std::function<void(Timestep)> func) { m_CustomTitleBar = func; }
        inline std::function<void(Timestep)> getCustomTitleBar() { return m_CustomTitleBar; }

        inline static Application& Get() { return *s_Instance; }
        inline GLFWwindow* getWindowHandle() { return m_Window; }
        ApplicationSpecifications& getSpecs() { return m_ApplicationSpecs; }
        SceneLibrary& getScenes() { return m_Scenes; }
    private:
        void init();
        void close();

        void loadImages();
        void initCallbacks();
        void onEvent(Ref<Event> e);
    private:
        static Application* s_Instance;
        GLFWwindow* m_Window;

        ApplicationSpecifications m_ApplicationSpecs;
        std::queue<Ref<Event>> m_EventQueue;
        SceneLibrary m_Scenes;
        std::unordered_map<std::string, Ref<Image>> m_Images;

        float m_LastFrameTime;
        bool m_Running = true;
        std::function<void(Timestep)> m_CustomTitleBar = nullptr;
    };

} // namespace vica