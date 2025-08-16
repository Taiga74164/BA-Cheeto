#pragma once
#include "core/rendering/backend/renderer_backend.h"
#include "memory/hook_manager.h"

class DX12Backend : public IRendererBackend
{
public:
    DX12Backend();
    ~DX12Backend() override;

    bool initialize() override;
    void shutdown() override;
    bool isInitialized() const override { return m_initialized; }
    const char* getName() const override { return "DirectX 12"; }
    void onResize(int width, int height) override;
    int onInput(UINT msg, WPARAM wParam, LPARAM lParam) override;

    // Hook types
    using Present_t = HRESULT(WINAPI*)(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags);
    using ResizeBuffers_t = HRESULT(WINAPI*)(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    using ExecuteCommandLists_t = void (WINAPI*)(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);

    static HRESULT WINAPI hookedPresent(IDXGISwapChain* swapChain, UINT SyncInterval, UINT Flags);
    static HRESULT WINAPI hookedResizeBuffers(IDXGISwapChain* swapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);
    static void WINAPI hookedExecuteCommandLists(ID3D12CommandQueue* queue, UINT NumCommandLists, ID3D12CommandList* const* ppCommandLists);

private:
    bool setupHooks();
    bool getDeviceAndSwapchain(IDXGISwapChain* swapChain);
    void setupWindowHook();
    static void* getVTableFunction(void* instance, int index);

    bool createRenderTargets();
    void cleanupRenderTargets();

    bool createDeviceObjects();
    void cleanupDeviceObjects();

    bool initializeImGui() override;
    void shutdownImGui() override;
    void beginFrame() override;
    void endFrame() override;
    void renderImGui() override;

    struct FrameContext
    {
        ID3D12CommandAllocator* allocator = nullptr;
        UINT64 fenceValue = 0;
        ID3D12Resource* renderTarget = nullptr;
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    };

    void waitForFence(UINT64 value);

    static DX12Backend* s_instance;
    static Present_t m_originalPresent;
    static ResizeBuffers_t m_originalResizeBuffers;
    static ExecuteCommandLists_t m_originalExecuteCommandLists;

    bool m_initialized = false;
    bool m_imguiInitialized = false;

    // Window and hooks
    HWND m_window = nullptr;
    static WNDPROC m_originalWndProc;
    static LRESULT CALLBACK hookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // D3D12 objects
    ID3D12Device* m_device = nullptr;
    ID3D12CommandQueue* m_commandQueue = nullptr; // captured from ExecuteCommandLists
    IDXGISwapChain3* m_swapChain = nullptr;

    ID3D12DescriptorHeap* m_rtvHeap = nullptr;
    ID3D12DescriptorHeap* m_srvHeap = nullptr; // for ImGui
    UINT m_rtvDescriptorSize = 0;

    ID3D12GraphicsCommandList* m_commandList = nullptr;
    ID3D12Fence* m_fence = nullptr;
    HANDLE m_fenceEvent = nullptr;
    UINT64 m_fenceLastSignaled = 0;

    UINT m_frameCount = 0;
    UINT m_frameIndex = 0;
    std::vector<FrameContext> m_frames;
};
