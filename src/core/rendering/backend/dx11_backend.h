#pragma once
#include <d3d11.h>
#include <dxgi.h>

#include "renderer_backend.h"

class DX11Backend : public IRendererBackend
{
public:
    DX11Backend();
    ~DX11Backend() override;

    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override { return m_initialized; }
    const char* getName() const override { return "DirectX 11"; }
    void onResize(int width, int height) override;
    int onInput(UINT msg, WPARAM wParam, LPARAM lParam) override;

    // 简单的字体切换功能
    void switchFont();

protected:
    void renderImGui() override;
    void beginFrame() override;
    void endFrame() override;
    bool initializeImGui() override;
    void shutdownImGui() override;

private:
    // Hook functions
    static HRESULT WINAPI hookedPresent(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    static HRESULT WINAPI hookedResizeBuffers(IDXGISwapChain* swapChain, UINT bufferCount,
                                              UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

    // Original function pointers
    using Present_t = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT);
    using ResizeBuffers_t = HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);

    static Present_t m_originalPresent;
    static ResizeBuffers_t m_originalResizeBuffers;

    // Instance management
    static DX11Backend* s_instance;

    // State
    bool m_initialized;
    bool m_imguiInitialized;
    ID3D11Device* m_device;
    ID3D11DeviceContext* m_context;
    IDXGISwapChain* m_swapChain;
    ID3D11RenderTargetView* m_renderTargetView;
    HWND m_window;

    // 字体相关
    ImFont* m_englishFont;
    ImFont* m_chineseFont;

    // Window procedure hook
    static WNDPROC m_originalWndProc;
    static LRESULT CALLBACK hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Helper functions
    bool setupHooks();
    void setupWindowHook();
    void* getVTableFunction(void* instance, int index);
    bool createRenderTarget();
    void cleanupRenderTarget();
    bool getDeviceAndContext();
};
