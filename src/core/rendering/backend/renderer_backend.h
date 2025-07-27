#pragma once

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class IRendererBackend
{
public:
    virtual ~IRendererBackend() = default;

    // Initialize the backend and set up hooks
    virtual bool initialize() = 0;

    // Shutdown and cleanup
    virtual void shutdown() = 0;

    // Check if the backend is initialized
    virtual bool isInitialized() const = 0;

    // Get the name of the backend
    virtual const char* getName() const = 0;

    // Called when the render target is resized
    virtual void onResize(int width, int height) = 0;

    // Handle input messages
    virtual int onInput(UINT msg, WPARAM wParam, LPARAM lParam) = 0; // TODO: It's probably the same for all backends

    // 字体切换功能
    virtual void switchFont() = 0;

protected:
    // Called when ImGui should be rendered
    virtual void renderImGui() = 0;

    // Called to begin a new ImGui frame
    virtual void beginFrame() = 0;

    // Called to end the current ImGui frame
    virtual void endFrame() = 0;

    // Initialize ImGui for the specific backend
    virtual bool initializeImGui() = 0;

    // Shutdown ImGui
    virtual void shutdownImGui() = 0;
};
