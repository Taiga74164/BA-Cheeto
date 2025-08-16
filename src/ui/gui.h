#pragma once

class GUI
{
public:
    static GUI& getInstance();

    // Main render function called by backends
    void render();

    // Show/hide the GUI
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }

    // Demo functions
    void showExampleWindow();

    // Soft-uninject flow control
    static void requestSoftUninject();
    static bool isSoftUninjectRequested();
    static void clearSoftUninjectRequest();

private:
    GUI();

    GUI(const GUI&) = delete;
    GUI& operator=(const GUI&) = delete;

    // Visibility flags
    bool m_visible = true;
    bool m_showExample = true;

    // Rendering methods
    void renderMainMenuBar();
    void renderExampleWindow();

    // Utility
    void setupImGuiStyle();

    // Shared flag to coordinate safe shutdown outside the render callback
    static std::atomic_bool s_softUninjectRequested;
};
