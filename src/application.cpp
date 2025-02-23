#include "application.h"

#include <print>
#include <unordered_set>

#include "timestep.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

namespace vica {
    Application* Application::s_Instance = nullptr;

    Application::Application(const char* title, int width, int height, ApplicationFlag flags)
        : m_LastFrameTime(0.0f) {
        m_ApplicationSpecs.name = title;
        m_ApplicationSpecs.width = width;
        m_ApplicationSpecs.height = height;
        m_ApplicationSpecs.applicationFlag = flags;
        s_Instance = this;
        init();
    }

    Application::~Application() {
        close();
    }

    void Application::init() {
        if (!glfwInit())
            std::print("glfw is not initialized.");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        if (m_ApplicationSpecs.isInCategory(ApplicationFlag_CustomTitleBar))
            glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

        m_Window = glfwCreateWindow(m_ApplicationSpecs.width, m_ApplicationSpecs.height, m_ApplicationSpecs.name, nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::print("glad not initialized.");

        glfwSwapInterval(1);
        initCallbacks();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

        ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
        ImGui_ImplOpenGL3_Init("#version 460");

        loadImages();
    }

    Ref<Image> Application::getImage(const std::string& name) {
        if (m_Images.find(name) != m_Images.end())
            return m_Images[name];
        return nullptr;
    }

    void Application::run() {


        while (!glfwWindowShouldClose(m_Window) && m_Running) {

            glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            float time = (float)glfwGetTime();
            Timestep timestep = time - m_LastFrameTime;
            m_LastFrameTime = time;

            glfwPollEvents();

            while (!Application::Get().m_EventQueue.empty()) {
                auto e = Application::Get().m_EventQueue.front();
                Application::Get().m_EventQueue.pop();
                onEvent(e);
            }

            ImGuiIO& io = ImGui::GetIO(); (void)io;
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            auto windowFlags = ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBringToFrontOnFocus;

            ImGui::SetNextWindowPos({ 0,0 });
            ImGui::SetNextWindowSize(io.DisplaySize);
            ImGui::Begin("Main Window", nullptr, windowFlags);

            if (m_Scenes.getSceneLibrarySize())
                getScenes().getActiveScene()->onUpdate(timestep);

            ImGui::End();

            ImGui::EndFrame();
            ImGui::Render();
            glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(glfwGetCurrentContext());
        }
    }

    void Application::close() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    // Events
    bool onWindowClose(WindowCloseEvent& e) {
        return true;
    }

    bool onWindowResize(WindowResizeEvent& e) {
        auto& app = Application::Get();
        app.getSpecs().width = e.getWidth();
        app.getSpecs().height = e.getHeight();
        return false;
    }

    bool onKeyPressed(KeyPressedEvent& e) {
        return true;
    }

    bool onKeyReleased(KeyReleasedEvent& e) {
        return true;
    }

    bool onKeyTyped(KeyTypedEvent& e) {
        return true;
    }

    bool onMouseMoved(MouseMovedEvent& e) {
        return true;
    }

    bool onMouseScrolled(MouseScrolledEvent& e) {
        return true;
    }

    bool onMouseButtonPressed(MouseButtonPressedEvent& e) {
        return true;
    }

    bool onMouseButtonReleased(MouseButtonReleasedEvent& e) {
        return true;
    }

    void Application::onEvent(Ref<Event> e) {
        EventDispatcher dispatcher(*e);

        if (e->isInCategory(EventCategoryMouseButton)) {
            dispatcher.dispatch<MouseButtonPressedEvent>(onMouseButtonPressed);
            dispatcher.dispatch<MouseButtonReleasedEvent>(onMouseButtonReleased);
        }
        else if (e->isInCategory(EventCategoryMouse)) {
            dispatcher.dispatch<MouseMovedEvent>(onMouseMoved);
            dispatcher.dispatch<MouseScrolledEvent>(onMouseScrolled);
        }
        if (e->isInCategory(EventCategoryInput)) {
            dispatcher.dispatch<KeyTypedEvent>(onKeyTyped);
        }

        if (e->isInCategory(EventCategoryApplication)) {
            dispatcher.dispatch<WindowCloseEvent>(onWindowClose);
            dispatcher.dispatch<WindowResizeEvent>(onWindowResize);
        }


        if (e->isInCategory(EventCategoryKeyboard)) {
            dispatcher.dispatch<KeyPressedEvent>(onKeyPressed);
            dispatcher.dispatch<KeyReleasedEvent>(onKeyReleased);
        }
    }

    void Application::loadImages() {
        std::filesystem::path imageDir("res");

        static const std::unordered_set<std::string> stb_image_extensions = {
            ".jpg", ".jpeg", ".png", ".bmp", ".gif", ".tga",
            ".psd", ".hdr", ".pic", ".ppm", ".pgm"
        };

        if (!std::filesystem::exists(imageDir)) {
            std::error_code ec;
            if (!std::filesystem::create_directories(imageDir, ec))
                throw std::runtime_error{ "Failed to create directory: " + imageDir.string() + ": " + ec.message() };
        }

        for (const auto& entry : std::filesystem::recursive_directory_iterator(imageDir))
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                if (stb_image_extensions.contains(ext))
                    m_Images[entry.path().filename().string()] = CreateRef<Image>(entry.path());
            }
    }

    void Application::initCallbacks() {
        glfwSetErrorCallback([](int error, const char* description) {
            fprintf(stderr, "GLFW Error %d: %s\n", error, description);
            });

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
            auto& app = Application::Get();
            if (app.m_ApplicationSpecs.width != width || app.m_ApplicationSpecs.height != height)
                app.m_EventQueue.push(CreateRef<WindowResizeEvent>(width, height));
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) {
            Application::Get().m_EventQueue.push(CreateRef<WindowCloseEvent>());
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scanCode, int action, int modes) {
            switch (action) {
            case GLFW_PRESS:    Application::Get().m_EventQueue.push(CreateRef<KeyPressedEvent>((KeyCode)key, false)); break;
            case GLFW_RELEASE:  Application::Get().m_EventQueue.push(CreateRef<KeyReleasedEvent>((KeyCode)key)); break;
            case GLFW_REPEAT:   Application::Get().m_EventQueue.push(CreateRef<KeyPressedEvent>((KeyCode)key, true)); break;
            }
            });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, uint32_t keycode) {
            Application::Get().m_EventQueue.push(CreateRef<KeyTypedEvent>((KeyCode)keycode));
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int modes) {
            switch (action) {
            case GLFW_PRESS:    Application::Get().m_EventQueue.push(CreateRef<MouseButtonPressedEvent>((MouseButton)button)); break;
            case GLFW_RELEASE:  Application::Get().m_EventQueue.push(CreateRef<MouseButtonReleasedEvent>((MouseButton)button)); break;
            }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset) {
            Application::Get().m_EventQueue.push(CreateRef<MouseScrolledEvent>((float)xOffset, (float)yOffset));
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos) {
            Application::Get().m_EventQueue.push(CreateRef<MouseMovedEvent>(xPos, yPos));
            });
    }

} // namespace vica