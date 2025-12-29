#pragma once
#include <imgui.h>

namespace ChatThemes {

    static inline void GitHubLight() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 6.f;
    s.FrameRounding  = 4.f;
    s.ScrollbarRounding = 10.f;

    ImVec4* c = s.Colors;

    // Base Colors
    c[ImGuiCol_Text]          = {0.11f, 0.11f, 0.11f, 1.f}; // Near-black text
    c[ImGuiCol_TextDisabled]  = {0.60f, 0.60f, 0.60f, 1.f};

    c[ImGuiCol_WindowBg]      = {0.98f, 0.98f, 0.98f, 1.f}; // Very light grey/off-white background
    c[ImGuiCol_ChildBg]       = {1.00f, 1.00f, 1.00f, 1.f}; // Pure white child background
    c[ImGuiCol_PopupBg]       = {1.00f, 1.00f, 1.00f, 0.94f};

    c[ImGuiCol_Border]        = {0.85f, 0.85f, 0.85f, 1.f}; // Light border

    // Frames & Widgets
    c[ImGuiCol_FrameBg]       = {0.94f, 0.94f, 0.94f, 1.f}; // Subtle background for input fields
    c[ImGuiCol_FrameBgHovered] = {0.88f, 0.88f, 0.88f, 1.f};
    c[ImGuiCol_FrameBgActive]  = {0.80f, 0.80f, 0.80f, 1.f};

    // Main Accent Color (Blue)
    c[ImGuiCol_CheckMark]     = {0.05f, 0.40f, 0.80f, 1.f};
    c[ImGuiCol_SliderGrab]    = {0.30f, 0.60f, 0.90f, 1.f};
    c[ImGuiCol_SliderGrabActive] = {0.05f, 0.40f, 0.80f, 1.f};
    
    // Buttons
    c[ImGuiCol_Button]        = {0.05f, 0.40f, 0.80f, 1.f}; // Primary blue button
    c[ImGuiCol_ButtonHovered] = {0.15f, 0.50f, 0.90f, 1.f};
    c[ImGuiCol_ButtonActive]  = {0.00f, 0.35f, 0.75f, 1.f};
    
    // Headers (e.g., CollapsingHeader, Tabs)
    c[ImGuiCol_Header]        = {0.90f, 0.90f, 0.90f, 1.f}; // Very light grey header
    c[ImGuiCol_HeaderHovered] = {0.85f, 0.85f, 0.85f, 1.f};
    c[ImGuiCol_HeaderActive]  = {0.80f, 0.80f, 0.80f, 1.f};

    // Scrollbar
    c[ImGuiCol_ScrollbarBg]   = {0.90f, 0.90f, 0.90f, 0.53f};
    c[ImGuiCol_ScrollbarGrab] = {0.70f, 0.70f, 0.70f, 1.f};
    c[ImGuiCol_ScrollbarGrabHovered] = {0.60f, 0.60f, 0.60f, 1.f};
    c[ImGuiCol_ScrollbarGrabActive]  = {0.50f, 0.50f, 0.50f, 1.f};
}

static inline void DarkBlue() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 7.f;
    s.FrameRounding  = 4.f;
    s.ScrollbarRounding = 10.f;

    ImVec4* c = s.Colors;

    c[ImGuiCol_Text]         = {0.88f, 0.92f, 1.00f, 1.f};
    c[ImGuiCol_TextDisabled] = {0.50f, 0.55f, 0.65f, 1.f};

    c[ImGuiCol_WindowBg]     = {0.06f, 0.08f, 0.12f, 1.f};
    c[ImGuiCol_ChildBg]      = {0.08f, 0.10f, 0.16f, 1.f};

    c[ImGuiCol_Border]       = {0.18f, 0.22f, 0.32f, 1.f};

    c[ImGuiCol_FrameBg]        = {0.12f, 0.15f, 0.22f, 1.f};
    c[ImGuiCol_FrameBgHovered] = {0.16f, 0.20f, 0.28f, 1.f};
    c[ImGuiCol_FrameBgActive]  = {0.20f, 0.25f, 0.35f, 1.f};

    c[ImGuiCol_Button]        = {0.15f, 0.30f, 0.55f, 1.f};
    c[ImGuiCol_ButtonHovered] = {0.20f, 0.38f, 0.67f, 1.f};
    c[ImGuiCol_ButtonActive]  = {0.10f, 0.22f, 0.47f, 1.f};

    c[ImGuiCol_Header]        = {0.12f, 0.14f, 0.22f, 1.f};
    c[ImGuiCol_HeaderHovered] = {0.16f, 0.17f, 0.26f, 1.f};
    c[ImGuiCol_HeaderActive]  = {0.20f, 0.20f, 0.30f, 1.f};
}

static inline void Dracula() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 6.f;
    s.FrameRounding  = 4.f;

    ImVec4* c = s.Colors;

    c[ImGuiCol_Text]           = {0.96f, 0.96f, 0.96f, 1.f};
    c[ImGuiCol_TextDisabled]   = {0.55f, 0.55f, 0.60f, 1.f};

    c[ImGuiCol_WindowBg]       = {0.12f, 0.13f, 0.20f, 1.f};
    c[ImGuiCol_ChildBg]        = {0.15f, 0.16f, 0.24f, 1.f};
    c[ImGuiCol_PopupBg]        = {0.10f, 0.12f, 0.19f, 1.f};

    c[ImGuiCol_Border]         = {0.23f, 0.25f, 0.33f, 1.f};

    c[ImGuiCol_FrameBg]        = {0.20f, 0.17f, 0.33f, 0.90f};
    c[ImGuiCol_FrameBgHovered] = {0.26f, 0.21f, 0.43f, 1.f};
    c[ImGuiCol_FrameBgActive]  = {0.32f, 0.24f, 0.51f, 1.f};

    c[ImGuiCol_Button]         = {0.25f, 0.20f, 0.40f, 1.f};
    c[ImGuiCol_ButtonHovered]  = {0.33f, 0.28f, 0.52f, 1.f};
    c[ImGuiCol_ButtonActive]   = {0.40f, 0.32f, 0.62f, 1.f};

    c[ImGuiCol_Header]         = {0.28f, 0.22f, 0.46f, 1.f};
    c[ImGuiCol_HeaderHovered]  = {0.36f, 0.28f, 0.58f, 1.f};
    c[ImGuiCol_HeaderActive]   = {0.42f, 0.33f, 0.66f, 1.f};
}

static inline void GruvboxDark() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 5.f;
    s.FrameRounding  = 3.f;
    s.ScrollbarRounding = 8.f;

    ImVec4* c = s.Colors;

    c[ImGuiCol_Text]           = {0.90f, 0.86f, 0.80f, 1.f};
    c[ImGuiCol_TextDisabled]   = {0.60f, 0.55f, 0.48f, 1.f};

    c[ImGuiCol_WindowBg]       = {0.15f, 0.12f, 0.10f, 1.f};
    c[ImGuiCol_ChildBg]        = {0.18f, 0.14f, 0.11f, 1.f};

    c[ImGuiCol_FrameBg]        = {0.26f, 0.20f, 0.18f, 1.f};
    c[ImGuiCol_FrameBgHovered] = {0.32f, 0.26f, 0.20f, 1.f};
    c[ImGuiCol_FrameBgActive]  = {0.36f, 0.28f, 0.22f, 1.f};

    c[ImGuiCol_Button]         = {0.35f, 0.25f, 0.12f, 1.f};
    c[ImGuiCol_ButtonHovered]  = {0.45f, 0.33f, 0.17f, 1.f};
    c[ImGuiCol_ButtonActive]   = {0.55f, 0.40f, 0.20f, 1.f};

    c[ImGuiCol_Header]         = {0.30f, 0.22f, 0.11f, 1.f};
    c[ImGuiCol_HeaderHovered]  = {0.42f, 0.32f, 0.15f, 1.f};
    c[ImGuiCol_HeaderActive]   = {0.50f, 0.37f, 0.18f, 1.f};
}

static inline void OneDark() {
    ImGuiStyle& s = ImGui::GetStyle();

    s.WindowRounding = 6.f;
    s.FrameRounding  = 4.f;
    s.ScrollbarRounding = 12.f;

    ImVec4* c = s.Colors;

    c[ImGuiCol_Text]           = {0.85f, 0.87f, 0.89f, 1.f};
    c[ImGuiCol_TextDisabled]   = {0.50f, 0.52f, 0.54f, 1.f};

    c[ImGuiCol_WindowBg]       = {0.11f, 0.12f, 0.14f, 1.f};
    c[ImGuiCol_ChildBg]        = {0.14f, 0.15f, 0.17f, 1.f};

    c[ImGuiCol_FrameBg]        = {0.17f, 0.18f, 0.20f, 1.f};
    c[ImGuiCol_FrameBgHovered] = {0.22f, 0.23f, 0.25f, 1.f};
    c[ImGuiCol_FrameBgActive]  = {0.26f, 0.27f, 0.30f, 1.f};

    c[ImGuiCol_Button]         = {0.20f, 0.33f, 0.52f, 1.f};
    c[ImGuiCol_ButtonHovered]  = {0.26f, 0.42f, 0.67f, 1.f};
    c[ImGuiCol_ButtonActive]   = {0.18f, 0.29f, 0.45f, 1.f};

    c[ImGuiCol_Header]         = {0.22f, 0.25f, 0.28f, 1.f};
    c[ImGuiCol_HeaderHovered]  = {0.28f, 0.32f, 0.36f, 1.f};
    c[ImGuiCol_HeaderActive]   = {0.34f, 0.38f, 0.42f, 1.f};
}

} // namespace ChatThemes
