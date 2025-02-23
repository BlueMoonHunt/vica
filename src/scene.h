#pragma once
#include <string>
#include <unordered_map>
#include <functional>

#include "base.h"
#include "timestep.h"

namespace vica {


    class Scene {
    public:
        Scene(const std::string& name, bool showCustomTitleBar = false)
            : m_Name(name), m_Resizable(true), m_ShowCustomeTitleBar(showCustomTitleBar) {
        };

        Scene(const std::string& name, int width, int height, bool showCustomTitleBar = false)
            : m_Name(name), m_Width(width), m_Height(height), m_Resizable(false), m_ShowCustomeTitleBar(showCustomTitleBar) {
        };

        virtual ~Scene() = default;

        void onUpdate(Timestep ts);

        virtual void onUIRender(Timestep ts) {};

        std::string getName() { return m_Name; }
        const int getHeight() const { return m_Height; }
        const int getWidth() const { return m_Width; }

        const void setHeight(int height) { m_Height = height; }
        const void setWidth(int width) { m_Width = width; }

        const bool isResizable() const { return m_Resizable; }

    protected:
        bool m_Resizable;
        bool m_ShowCustomeTitleBar = false;
        std::string m_Name;
        int m_Width = 0;
        int m_Height = 0;
    };

    class SceneLibrary {
    public:
        void add(Ref<Scene> scene);
        const Ref<Scene> getActiveScene() const { return m_ActiveScene; }
        void show(const std::string& name);

        inline size_t getSceneLibrarySize() const { return m_Scenes.size(); }
    private:
        Ref<Scene> m_ActiveScene;
        std::unordered_map<std::string, Ref<Scene>> m_Scenes;
    };

} // namespace vica
