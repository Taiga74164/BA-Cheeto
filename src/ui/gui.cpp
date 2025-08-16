#include "pch.h"
#include "gui.h"

#include "user/cheat/feature_manager.h"
#include "user/main.h"
#include "utils/config_manager.h"
#include "utils/hotkey_manager.h"
#include <atomic>
#include <filesystem>
#include <shellapi.h>
std::atomic_bool GUI::s_softUninjectRequested{ false };

void GUI::requestSoftUninject() { s_softUninjectRequested.store(true); }
bool GUI::isSoftUninjectRequested() { return s_softUninjectRequested.load(); }
void GUI::clearSoftUninjectRequest() { s_softUninjectRequested.store(false); }

extern HMODULE g_hModule;

GUI::GUI()
{
    setupImGuiStyle();
}

GUI& GUI::getInstance()
{
    static GUI instance;
    return instance;
}

void GUI::render()
{
    if (!m_visible)
    {
        return;
    }

    // Render main menu bar
    renderMainMenuBar();

    // Render example window if enabled
    if (m_showExample)
    {
        renderExampleWindow();
    }

    // Defer shutdown until after UI finished for this frame
    if (isSoftUninjectRequested())
    {
        // Hide GUI to prevent any further drawing
        m_visible = false;
        clearSoftUninjectRequest();
        // Trigger shutdown on a detached thread a bit later to be safely outside Present
        std::thread([](){
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            Main::shutdown();
        }).detach();
    }
}

void GUI::showExampleWindow()
{
    m_showExample = true;
}

void GUI::renderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Windows"))
        {
            ImGui::MenuItem("Example Window", nullptr, &m_showExample);
            ImGui::EndMenu();
        }

    if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::BeginMenu("Theme"))
            {
                if (ImGui::MenuItem("Default"))
                {
                    ImGui::StyleColorsDark();
                    setupImGuiStyle();
                }
                if (ImGui::MenuItem("Light"))
                {
                    ImGui::StyleColorsLight();
                }
                if (ImGui::MenuItem("Classic"))
                {
                    ImGui::StyleColorsClassic();
                }
                ImGui::EndMenu();
            }

            ImGui::Separator();
            // Profiles
            if (ImGui::BeginMenu("Profiles"))
            {
                auto profiles = ConfigManager::getInstance().listProfiles();
                for (auto& p : profiles)
                {
                    bool sel = (p == ConfigManager::getInstance().getProfile());
                    if (ImGui::MenuItem(p.c_str(), nullptr, sel))
                    {
                        ConfigManager::getInstance().setProfile(p);
                        cheat::FeatureManager::getInstance().reloadConfig();
                    }
                    // Context menu for rename/delete
                    if (ImGui::BeginPopupContextItem((std::string("profile_ctx_") + p).c_str()))
                    {
                        static char renameBuf[64] = {0};
                        ImGui::InputTextWithHint("##rename", "new name", renameBuf, sizeof(renameBuf));
                        if (ImGui::Button("Rename") && renameBuf[0] != '\0')
                        {
                            auto base = std::filesystem::path(ConfigManager::getInstance().getConfigFilePath()).parent_path();
                            std::string oldFile = (p == "default") ? (base / "config.json").string()
                                                                      : (base / (std::string("config.") + p + ".json")).string();
                            std::string newFile = (std::string(renameBuf) == "default") ? (base / "config.json").string()
                                                                                         : (base / (std::string("config.") + renameBuf + ".json")).string();
                            std::error_code ec;
                            std::filesystem::rename(oldFile, newFile, ec);
                            if (!ec)
                            {
                                if (p == ConfigManager::getInstance().getProfile())
                                {
                                    ConfigManager::getInstance().setProfile(renameBuf);
                                    cheat::FeatureManager::getInstance().reloadConfig();
                                }
                            }
                            renameBuf[0] = '\0';
                            ImGui::CloseCurrentPopup();
                        }
                        if (p == "default")
                        {
                            ImGui::BeginDisabled();
                            ImGui::Button("Delete");
                            ImGui::EndDisabled();
                        }
                        else if (ImGui::Button("Delete"))
                        {
                            auto base = std::filesystem::path(ConfigManager::getInstance().getConfigFilePath()).parent_path();
                            auto file = (p == "default") ? (base / "config.json").string()
                                                          : (base / (std::string("config.") + p + ".json")).string();
                            std::error_code ec; std::filesystem::remove(file, ec);
                            if (p == ConfigManager::getInstance().getProfile())
                            {
                                ConfigManager::getInstance().setProfile("default");
                                cheat::FeatureManager::getInstance().reloadConfig();
                            }
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                }
                ImGui::Separator();
                static char newProfile[64] = {0};
                ImGui::InputTextWithHint("##newprofile", "new profile name", newProfile, sizeof(newProfile));
                ImGui::SameLine();
                if (ImGui::Button("Create") && newProfile[0] != '\0')
                {
                    if (ConfigManager::getInstance().createProfile(newProfile))
                    {
                        ConfigManager::getInstance().setProfile(newProfile);
                    }
                    newProfile[0] = '\0';
                }
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save now"))
                (void)ConfigManager::getInstance().saveNow();

            if (ImGui::MenuItem("Open config folder"))
            {
                auto path = std::filesystem::path(ConfigManager::getInstance().getConfigFilePath()).parent_path();
                ShellExecuteA(nullptr, "open", path.string().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
            }

            if (ImGui::MenuItem("Reset all to defaults"))
            {
                ConfigManager::getInstance().resetAll();
                ConfigManager::getInstance().save();
                cheat::FeatureManager::getInstance().reloadConfig();
            }

            // Hotkeys submenu removed; per-feature hotkeys are shown inline under each cheat.
            if (ImGui::MenuItem("Uninject (soft)", "", false, true))
            {
                // Request a deferred shutdown outside of the current ImGui frame/present
                GUI::requestSoftUninject();
            }

            ImGui::EndMenu();
        }

        // Performance info
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();

        float fps = ImGui::GetIO().Framerate;
        ImGui::Text("FPS: %.1f", fps);

        // Color code FPS
        if (fps >= 60.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "o");
        }
        else if (fps >= 30.0f)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.2f, 1.0f), "o");
        }
        else
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.2f, 0.2f, 1.0f), "o");
        }

    // Controls hint
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
    ImGui::TextDisabled("F2: Toggle GUI");

    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    ImGui::TextDisabled("Profile: %s", ConfigManager::getInstance().getProfile().c_str());

        ImGui::EndMainMenuBar();
    }
}

void GUI::renderExampleWindow()
{
    ImGui::Begin("Blue Archive Cheat - Taiga74164", nullptr, ImGuiWindowFlags_None);

    cheat::FeatureManager::getInstance().draw();

    ImGui::End();
}

void GUI::setupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowMinSize = ImVec2(600, 360);
    
    // Modern rounded corners
    style.WindowRounding = 12.0f;
    style.FrameRounding = 10.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.PopupRounding = 10.0f;

    // Spacious layout
    style.WindowPadding = ImVec2(18, 18);
    style.FramePadding = ImVec2(14, 8);
    style.ItemSpacing = ImVec2(14, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    style.IndentSpacing = 28.0f;
    style.ScrollbarSize = 16.0f;
    style.GrabMinSize = 12.0f;

    // Borders
    style.WindowBorderSize = 1.5f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    ImVec4* colors = style.Colors;

    // Text
    colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.58f, 0.62f, 1.00f);

    // Windows
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.12f, 0.16f, 0.92f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.14f, 0.18f, 0.92f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.15f, 0.20f, 0.98f);

    // Borders
    colors[ImGuiCol_Border] = ImVec4(0.22f, 0.25f, 0.32f, 0.60f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frames
    colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.18f, 0.24f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.25f, 0.32f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.29f, 0.38f, 1.00f);

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.15f, 0.20f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);

    // Menu bar
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.13f, 0.15f, 0.20f, 1.00f);

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.12f, 0.16f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.22f, 0.25f, 0.32f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.28f, 0.32f, 0.42f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.34f, 0.38f, 0.52f, 1.00f);

    // Vibrant accent
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.24f, 0.38f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.32f, 0.38f, 0.62f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);

    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.22f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.38f, 0.62f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.32f, 0.38f, 0.62f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);

    // Separators
    colors[ImGuiCol_Separator] = ImVec4(0.22f, 0.25f, 0.32f, 0.60f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.32f, 0.38f, 0.62f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);

    // Resize grips
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.32f, 0.38f, 0.62f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.32f, 0.38f, 0.62f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.38f, 0.44f, 0.72f, 0.95f);

    // Plots
    colors[ImGuiCol_PlotLines] = ImVec4(0.62f, 0.62f, 0.82f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.98f, 0.43f, 0.85f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.98f, 0.60f, 0.85f, 1.00f);

    // Navigation
    colors[ImGuiCol_NavHighlight] = ImVec4(0.38f, 0.44f, 0.72f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    // Modal overlay
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

