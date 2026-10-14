#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <shellapi.h>

// Helper macro for ARGB color in GDI+
#define MAKE_ARGB(a, r, g, b) (((ARGB)(a) << 24) | ((ARGB)(r) << 16) | ((ARGB)(g) << 8) | ((ARGB)(b)))

// Navigation items
typedef enum {
    NAV_DASHBOARD = 0,
    NAV_ENVIRONMENT,
    NAV_PROJECTS,
    NAV_ACCOUNTS,
    NAV_SETTINGS,
    NAV_COUNT
} NavItem;

// Package structure
typedef struct {
    const wchar_t* name;
    const wchar_t* category;
    const wchar_t* version;
    const wchar_t* description;
    const wchar_t* install_cmd;
    bool installed;
} PackageInfo;

static PackageInfo g_packages[] = {
    { L"Git for Windows", L"Developer Tools", L"v2.44.0", L"Distributed version control system for tracking source code changes.", L"winget install --id Git.Git -e --source winget", true },
    { L"Visual Studio Code", L"IDEs & Editors", L"v1.87.2", L"Lightweight and powerful code editor with vast extension ecosystem.", L"winget install --id Microsoft.VisualStudioCode -e", true },
    { L"Node.js (LTS)", L"Runtimes", L"v20.11.1", L"JavaScript runtime built on Chrome's V8 JavaScript engine.", L"winget install --id OpenJS.NodeJS.LTS -e", true },
    { L"Python 3.12", L"Runtimes", L"v3.12.2", L"High-level programming language for web, data science, and automation.", L"winget install --id Python.Python.3.12 -e", false },
    { L"Docker Desktop", L"Containerization", L"v4.28.0", L"Container platform for building, running, and managing containerized apps.", L"winget install --id Docker.DockerDesktop -e", false },
    { L"PowerShell 7", L"CLI & Shell", L"v7.4.1", L"Cross-platform task automation tool and configuration management framework.", L"winget install --id Microsoft.PowerShell -e", true },
    { L"Windows Terminal", L"CLI & Shell", L"v1.19.1", L"Modern, fast, efficient, powerful terminal application for Windows.", L"winget install --id Microsoft.WindowsTerminal -e", true },
    { L"Rust toolchain", L"Languages", L"v1.76.0", L"Empowering everyone to build reliable and efficient software.", L"winget install --id Rustlang.Rustup -e", false },
    { L"Go Compiler", L"Languages", L"v1.22.1", L"Open source programming language that makes it easy to build simple, fast software.", L"winget install --id GoLang.Go -e", false },
    { L"NET 8 SDK", L"Runtimes", L"v8.0.200", L"Free, cross-platform, open-source developer platform for building apps.", L"winget install --id Microsoft.DotNet.SDK.8 -e", true }
};
static const int g_package_count = sizeof(g_packages) / sizeof(g_packages[0]);

// Project structure
typedef struct {
    const wchar_t* name;
    const wchar_t* path;
    const wchar_t* branch;
    const wchar_t* status;
    const wchar_t* last_commit;
} ProjectInfo;

static ProjectInfo g_projects[] = {
    { L"DevHome-Windows", L"C:\\Users\\Developer\\Source\\Repos\\DevHome", L"main", L"Up to date", L"feat: add customizable widgets and environment setup" },
    { L"PowerToys-Modules", L"C:\\Users\\Developer\\Source\\Repos\\PowerToys", L"feature/winui3", L"2 commits ahead", L"fix: color picker overlay high DPI scaling" },
    { L"Web3-Microservice", L"C:\\Users\\Developer\\Source\\Repos\\Web3-Service", L"dev", L"1 commit behind", L"chore: update dependencies and Go modules" },
    { L"AI-Assistant-Plugin", L"C:\\Users\\Developer\\Source\\Repos\\AI-Plugin", L"main", L"Up to date", L"docs: add setup and API specification guide" }
};
static const int g_project_count = sizeof(g_projects) / sizeof(g_projects[0]);

// Account structure
typedef struct {
    const wchar_t* provider;
    const wchar_t* username;
    const wchar_t* status;
    const wchar_t* avatar_letter;
    COLORREF badge_color;
} AccountInfo;

static AccountInfo g_accounts[] = {
    { L"GitHub", L"octocat-dev", L"Connected (SSH & OAuth)", L"G", RGB(36, 41, 47) },
    { L"Microsoft Azure", L"developer@microsoft.com", L"Active Subscription", L"M", RGB(0, 120, 212) },
    { L"DevOps Center", L"dev-org-admin", L"Connected", L"D", RGB(107, 33, 168) },
    { L"GitLab", L"gitlab-dev-user", L"Not Connected", L"L", RGB(226, 67, 41) }
};
static const int g_account_count = sizeof(g_accounts) / sizeof(g_accounts[0]);

// System stats
static int g_cpu_usage = 18;
static int g_ram_usage = 42;
static int g_disk_usage = 64;

// Global UI State
static NavItem g_active_nav = NAV_DASHBOARD;
static HWND g_hwnd_main = NULL;
static HWND g_hwnd_content = NULL;
static ULONG_PTR g_gdiplus_token;

// Function prototypes
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void SwitchNav(NavItem nav);

// Helper to draw smooth rounded rectangles using GDI+
void DrawFluentCard(GpGraphics* graphics, REAL x, REAL y, REAL w, REAL h, ARGB bgColor, ARGB borderColor, REAL radius) {
    GpPath* path;
    GdipCreatePath(FillModeWinding, &path);
    REAL diameter = radius * 2.0f;
    GdipAddPathArc(path, x, y, diameter, diameter, 180.0f, 90.0f);
    GdipAddPathArc(path, x + w - diameter, y, diameter, diameter, 270.0f, 90.0f);
    GdipAddPathArc(path, x + w - diameter, y + h - diameter, diameter, diameter, 0.0f, 90.0f);
    GdipAddPathArc(path, x, y + h - diameter, diameter, diameter, 90.0f, 90.0f);
    GdipClosePathFigure(path);

    GpSolidFill* brush;
    GdipCreateSolidFill(bgColor, &brush);
    GdipFillPath(graphics, (GpBrush*)brush, path);
    GdipDeleteBrush((GpBrush*)brush);

    if (borderColor != 0) {
        GpPen* pen;
        GdipCreatePen1(borderColor, 1.0f, UnitPixel, &pen);
        GdipDrawPath(graphics, pen, path);
        GdipDeletePen(pen);
    }

    GdipDeletePath(path);
}

// Draw text with GDI+
void DrawFluentText(GpGraphics* graphics, const wchar_t* text, REAL x, REAL y, REAL w, REAL h, REAL fontSize, ARGB color, INT fontStyle, StringAlignment align) {
    GpFontFamily* fontFamily;
    GdipCreateFontFamilyFromName(L"Segoe UI", NULL, &fontFamily);

    GpFont* font;
    GdipCreateFont(fontFamily, fontSize, fontStyle, UnitPoint, &font);

    GpSolidFill* brush;
    GdipCreateSolidFill(color, &brush);

    RectF layoutRect = { x, y, w, h };
    GpStringFormat* format;
    GdipCreateStringFormat(0, LANG_NEUTRAL, &format);
    GdipSetStringFormatAlign(format, align);
    GdipSetStringFormatLineAlign(format, StringAlignmentCenter);

    GdipDrawString(graphics, text, -1, font, &layoutRect, format, (GpBrush*)brush);

    GdipDeleteStringFormat(format);
    GdipDeleteBrush((GpBrush*)brush);
    GdipDeleteFont(font);
    GdipDeleteFontFamily(fontFamily);
}

// Draw Dashboard View
void RenderDashboard(GpGraphics* graphics, int width, int height) {
    // Header
    DrawFluentText(graphics, L"Dev Home Dashboard", 30, 20, 500, 40, 18.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Monitor system performance, developer tools, and quick controls.", 30, 55, 600, 25, 10.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

    // Widget 1: System Performance (CPU / RAM / Disk)
    DrawFluentCard(graphics, 30, 95, 340, 220, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);
    DrawFluentText(graphics, L"System Performance", 50, 110, 250, 30, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

    wchar_t buf[128];
    // CPU
    swprintf(buf, 128, L"CPU Usage: %d%%", g_cpu_usage);
    DrawFluentText(graphics, buf, 50, 145, 150, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentCard(graphics, 50, 168, 300, 8, MAKE_ARGB(255, 60, 60, 60), 0, 4.0f);
    DrawFluentCard(graphics, 50, 168, (300.0f * g_cpu_usage) / 100.0f, 8, MAKE_ARGB(255, 0, 120, 212), 0, 4.0f);

    // RAM
    swprintf(buf, 128, L"Memory Usage: %d%%", g_ram_usage);
    DrawFluentText(graphics, buf, 50, 185, 150, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentCard(graphics, 50, 208, 300, 8, MAKE_ARGB(255, 60, 60, 60), 0, 4.0f);
    DrawFluentCard(graphics, 50, 208, (300.0f * g_ram_usage) / 100.0f, 8, MAKE_ARGB(255, 139, 92, 246), 0, 4.0f);

    // Disk
    swprintf(buf, 128, L"Storage Usage: %d%%", g_disk_usage);
    DrawFluentText(graphics, buf, 50, 225, 150, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentCard(graphics, 50, 248, 300, 8, MAKE_ARGB(255, 60, 60, 60), 0, 4.0f);
    DrawFluentCard(graphics, 50, 248, (300.0f * g_disk_usage) / 100.0f, 8, MAKE_ARGB(255, 16, 185, 129), 0, 4.0f);

    // Widget 2: Quick Status & Projects
    DrawFluentCard(graphics, 390, 95, 360, 220, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);
    DrawFluentText(graphics, L"Active Projects Summary", 410, 110, 300, 30, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

    for (int i = 0; i < 3 && i < g_project_count; i++) {
        REAL y = 145.0f + i * 52.0f;
        DrawFluentCard(graphics, 410, y, 320, 44, MAKE_ARGB(255, 52, 52, 52), MAKE_ARGB(255, 70, 70, 70), 6.0f);
        DrawFluentText(graphics, g_projects[i].name, 422, y + 6, 200, 18, 10.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

        swprintf(buf, 128, L"%s • %s", g_projects[i].branch, g_projects[i].status);
        DrawFluentText(graphics, buf, 422, y + 24, 280, 16, 8.5f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);
    }

    // Widget 3: Environment Setup Quick launch
    DrawFluentCard(graphics, 30, 335, 720, 180, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);
    DrawFluentText(graphics, L"Developer Tools Overview", 50, 350, 300, 30, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

    int installed_cnt = 0;
    for (int i = 0; i < g_package_count; i++) {
        if (g_packages[i].installed) installed_cnt++;
    }
    swprintf(buf, 128, L"Installed Tools: %d of %d packages ready", installed_cnt, g_package_count);
    DrawFluentText(graphics, buf, 50, 380, 400, 20, 9.5f, MAKE_ARGB(255, 16, 185, 129), FontStyleBold, StringAlignmentNear);

    DrawFluentText(graphics, L"Quick Actions:", 50, 415, 200, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleBold, StringAlignmentNear);
    DrawFluentCard(graphics, 50, 440, 160, 40, MAKE_ARGB(255, 0, 120, 212), 0, 6.0f);
    DrawFluentText(graphics, L"Install Packages", 50, 440, 160, 40, 9.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);

    DrawFluentCard(graphics, 225, 440, 160, 40, MAKE_ARGB(255, 60, 60, 60), MAKE_ARGB(255, 90, 90, 90), 6.0f);
    DrawFluentText(graphics, L"Add Developer Account", 225, 440, 160, 40, 9.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);
}

// Draw Environment View
void RenderEnvironment(GpGraphics* graphics, int width, int height) {
    DrawFluentText(graphics, L"Environment Setup & Package Manager", 30, 20, 600, 40, 18.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Browse, install, and manage developer applications, runtimes, and command line tools via WinGet.", 30, 55, 700, 25, 10.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

    for (int i = 0; i < g_package_count; i++) {
        REAL col = i % 2;
        REAL row = i / 2;
        REAL x = 30.0f + col * 365.0f;
        REAL y = 95.0f + row * 85.0f;

        DrawFluentCard(graphics, x, y, 350, 75, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 6.0f);

        // Tool Title & Category
        DrawFluentText(graphics, g_packages[i].name, x + 15, y + 10, 200, 20, 10.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

        wchar_t tag[64];
        swprintf(tag, 64, L"%s • %s", g_packages[i].category, g_packages[i].version);
        DrawFluentText(graphics, tag, x + 15, y + 30, 200, 16, 8.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

        DrawFluentText(graphics, g_packages[i].description, x + 15, y + 48, 220, 20, 8.0f, MAKE_ARGB(255, 190, 190, 190), FontStyleRegular, StringAlignmentNear);

        // Install / Installed badge button
        if (g_packages[i].installed) {
            DrawFluentCard(graphics, x + 245, y + 20, 90, 32, MAKE_ARGB(255, 20, 83, 45), MAKE_ARGB(255, 34, 197, 94), 4.0f);
            DrawFluentText(graphics, L"Installed ✓", x + 245, y + 20, 90, 32, 8.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);
        } else {
            DrawFluentCard(graphics, x + 245, y + 20, 90, 32, MAKE_ARGB(255, 0, 120, 212), 0, 4.0f);
            DrawFluentText(graphics, L"Install", x + 245, y + 20, 90, 32, 8.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);
        }
    }
}

// Draw Projects View
void RenderProjects(GpGraphics* graphics, int width, int height) {
    DrawFluentText(graphics, L"Projects Dashboard", 30, 20, 500, 40, 18.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Quick access to local Git repositories, branch status, and recent commits.", 30, 55, 600, 25, 10.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

    for (int i = 0; i < g_project_count; i++) {
        REAL y = 95.0f + i * 100.0f;
        DrawFluentCard(graphics, 30, y, 715, 88, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);

        DrawFluentText(graphics, g_projects[i].name, 50, y + 12, 300, 24, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
        DrawFluentText(graphics, g_projects[i].path, 50, y + 36, 450, 18, 8.5f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

        wchar_t buf[256];
        swprintf(buf, 256, L"Latest commit: \"%s\"", g_projects[i].last_commit);
        DrawFluentText(graphics, buf, 50, y + 56, 480, 18, 8.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleItalic, StringAlignmentNear);

        // Branch badge
        DrawFluentCard(graphics, 550, y + 15, 80, 25, MAKE_ARGB(255, 60, 60, 60), 0, 4.0f);
        DrawFluentText(graphics, g_projects[i].branch, 550, y + 15, 80, 25, 8.5f, MAKE_ARGB(255, 56, 189, 248), FontStyleBold, StringAlignmentCenter);

        // Action button
        DrawFluentCard(graphics, 640, y + 15, 90, 55, MAKE_ARGB(255, 0, 120, 212), 0, 6.0f);
        DrawFluentText(graphics, L"Open in VS Code", 640, y + 15, 90, 55, 8.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);
    }
}

// Draw Developer Accounts View
void RenderAccounts(GpGraphics* graphics, int width, int height) {
    DrawFluentText(graphics, L"Developer Accounts & Services", 30, 20, 500, 40, 18.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Connect your cloud services, version control providers, and SSH identities.", 30, 55, 600, 25, 10.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

    for (int i = 0; i < g_account_count; i++) {
        REAL y = 95.0f + i * 95.0f;
        DrawFluentCard(graphics, 30, y, 715, 80, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);

        // Avatar icon
        ARGB bgClr = MAKE_ARGB(255, GetRValue(g_accounts[i].badge_color), GetGValue(g_accounts[i].badge_color), GetBValue(g_accounts[i].badge_color));
        DrawFluentCard(graphics, 48, y + 15, 50, 50, bgClr, 0, 25.0f);
        DrawFluentText(graphics, g_accounts[i].avatar_letter, 48, y + 15, 50, 50, 14.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);

        // Details
        DrawFluentText(graphics, g_accounts[i].provider, 115, y + 16, 250, 22, 11.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
        DrawFluentText(graphics, g_accounts[i].username, 115, y + 40, 300, 20, 9.0f, MAKE_ARGB(255, 170, 170, 170), FontStyleRegular, StringAlignmentNear);

        // Status
        DrawFluentText(graphics, g_accounts[i].status, 420, y + 30, 160, 20, 9.0f, MAKE_ARGB(255, 56, 189, 248), FontStyleBold, StringAlignmentNear);

        // Action Button
        DrawFluentCard(graphics, 600, y + 22, 125, 36, MAKE_ARGB(255, 55, 55, 55), MAKE_ARGB(255, 80, 80, 80), 6.0f);
        DrawFluentText(graphics, L"Manage Account", 600, y + 22, 125, 36, 8.5f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentCenter);
    }
}

// Draw Settings View
void RenderSettings(GpGraphics* graphics, int width, int height) {
    DrawFluentText(graphics, L"Dev Home Settings", 30, 20, 500, 40, 18.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Customize theme appearance, widget layout, and background sync options.", 30, 55, 600, 25, 10.0f, MAKE_ARGB(255, 160, 160, 160), FontStyleRegular, StringAlignmentNear);

    // Group 1
    DrawFluentCard(graphics, 30, 95, 715, 140, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);
    DrawFluentText(graphics, L"Appearance & Personalization", 50, 110, 300, 25, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"App Theme: Windows Dark (Fluent Design)", 50, 140, 350, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentText(graphics, L"High DPI Scaling: Per-Monitor V2 Enabled", 50, 165, 350, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentText(graphics, L"Mica / Acrylic Backdrop: Active", 50, 190, 350, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);

    // Group 2
    DrawFluentCard(graphics, 30, 250, 715, 140, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 8.0f);
    DrawFluentText(graphics, L"About Dev Home", 50, 265, 300, 25, 12.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);
    DrawFluentText(graphics, L"Version: 1.0.0.0 (Native Release)", 50, 295, 350, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentText(graphics, L"Publisher: Microsoft Dev Home", 50, 320, 350, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
    DrawFluentText(graphics, L"Architecture: x64 Windows Native Executable (Standalone)", 50, 345, 450, 20, 9.5f, MAKE_ARGB(255, 200, 200, 200), FontStyleRegular, StringAlignmentNear);
}

// Sidebar Window Procedure
LRESULT CALLBACK SidebarProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            GpGraphics* graphics;
            GdipCreateFromHDC(hdc, &graphics);
            GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
            GdipSetTextRenderingHint(graphics, TextRenderingHintClearTypeGridFit);

            // Background fill
            GpSolidFill* bgBrush;
            GdipCreateSolidFill(MAKE_ARGB(255, 24, 24, 24), &bgBrush);
            GdipFillRectangle(graphics, (GpBrush*)bgBrush, 0, 0, (REAL)rc.right, (REAL)rc.bottom);
            GdipDeleteBrush((GpBrush*)bgBrush);

            // App Title in Sidebar
            DrawFluentText(graphics, L"Dev Home", 50, 22, 160, 30, 14.0f, MAKE_ARGB(255, 255, 255, 255), FontStyleBold, StringAlignmentNear);

            // Navigation items
            const wchar_t* nav_titles[] = {
                L"  Dashboard",
                L"  Machine Setup",
                L"  Projects",
                L"  Accounts",
                L"  Settings"
            };

            for (int i = 0; i < NAV_COUNT; i++) {
                REAL y = 75.0f + i * 46.0f;
                bool is_selected = (g_active_nav == (NavItem)i);

                if (is_selected) {
                    // Accent indicator pill on the left
                    DrawFluentCard(graphics, 6, y + 8, 4, 24, MAKE_ARGB(255, 0, 120, 212), 0, 2.0f);
                    DrawFluentCard(graphics, 14, y, 192, 40, MAKE_ARGB(255, 43, 43, 43), MAKE_ARGB(255, 60, 60, 60), 6.0f);
                }

                ARGB txtColor = is_selected ? MAKE_ARGB(255, 255, 255, 255) : MAKE_ARGB(255, 170, 170, 170);
                DrawFluentText(graphics, nav_titles[i], 25, y, 170, 40, 10.0f, txtColor, is_selected ? FontStyleBold : FontStyleRegular, StringAlignmentNear);
            }

            GdipDeleteGraphics(graphics);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN: {
            int y = HIWORD(lParam);
            for (int i = 0; i < NAV_COUNT; i++) {
                int item_y = 75 + i * 46;
                if (y >= item_y && y < item_y + 40) {
                    SwitchNav((NavItem)i);
                    InvalidateRect(hwnd, NULL, FALSE);
                    break;
                }
            }
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Content Area Window Procedure
LRESULT CALLBACK ContentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            GpGraphics* graphics;
            GdipCreateFromHDC(hdc, &graphics);
            GdipSetSmoothingMode(graphics, SmoothingModeAntiAlias);
            GdipSetTextRenderingHint(graphics, TextRenderingHintClearTypeGridFit);

            // Dark Content Background
            GpSolidFill* bgBrush;
            GdipCreateSolidFill(MAKE_ARGB(255, 32, 32, 32), &bgBrush);
            GdipFillRectangle(graphics, (GpBrush*)bgBrush, 0, 0, (REAL)rc.right, (REAL)rc.bottom);
            GdipDeleteBrush((GpBrush*)bgBrush);

            // Render view depending on navigation
            switch (g_active_nav) {
                case NAV_DASHBOARD:
                    RenderDashboard(graphics, rc.right, rc.bottom);
                    break;
                case NAV_ENVIRONMENT:
                    RenderEnvironment(graphics, rc.right, rc.bottom);
                    break;
                case NAV_PROJECTS:
                    RenderProjects(graphics, rc.right, rc.bottom);
                    break;
                case NAV_ACCOUNTS:
                    RenderAccounts(graphics, rc.right, rc.bottom);
                    break;
                case NAV_SETTINGS:
                    RenderSettings(graphics, rc.right, rc.bottom);
                    break;
                default:
                    break;
            }

            GdipDeleteGraphics(graphics);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (g_active_nav == NAV_DASHBOARD) {
                // Quick Action: Install Packages button (50, 440, 160, 40)
                if (x >= 50 && x <= 210 && y >= 440 && y <= 480) {
                    SwitchNav(NAV_ENVIRONMENT);
                }
                // Quick Action: Add Developer Account button (225, 440, 160, 40)
                else if (x >= 225 && x <= 385 && y >= 440 && y <= 480) {
                    SwitchNav(NAV_ACCOUNTS);
                }
            } else if (g_active_nav == NAV_ENVIRONMENT) {
                // Check package Install/Installed buttons
                for (int i = 0; i < g_package_count; i++) {
                    int col = i % 2;
                    int row = i / 2;
                    int px = 30 + col * 365 + 245;
                    int py = 95 + row * 85 + 20;
                    if (x >= px && x <= px + 90 && y >= py && y <= py + 32) {
                        g_packages[i].installed = !g_packages[i].installed;
                        InvalidateRect(hwnd, NULL, FALSE);
                        break;
                    }
                }
            } else if (g_active_nav == NAV_PROJECTS) {
                // Check "Open in VS Code" buttons
                for (int i = 0; i < g_project_count; i++) {
                    int py = 95 + i * 100 + 15;
                    if (x >= 640 && x <= 730 && y >= py && y <= py + 55) {
                        wchar_t msg_buf[256];
                        swprintf(msg_buf, 256, L"Opening repository path in Visual Studio Code:\n%s", g_projects[i].path);
                        MessageBox(hwnd, msg_buf, g_projects[i].name, MB_OK | MB_ICONINFORMATION);
                        break;
                    }
                }
            } else if (g_active_nav == NAV_ACCOUNTS) {
                // Check "Manage Account" buttons
                for (int i = 0; i < g_account_count; i++) {
                    int py = 95 + i * 95 + 22;
                    if (x >= 600 && x <= 725 && y >= py && y <= py + 36) {
                        wchar_t msg_buf[256];
                        swprintf(msg_buf, 256, L"Account details for %s:\nUsername: %s\nStatus: %s", g_accounts[i].provider, g_accounts[i].username, g_accounts[i].status);
                        MessageBox(hwnd, msg_buf, g_accounts[i].provider, MB_OK | MB_ICONINFORMATION);
                        break;
                    }
                }
            }
            return 0;
        }
        case WM_TIMER: {
            // Update performance values dynamically
            g_cpu_usage = 12 + rand() % 25;
            g_ram_usage = 40 + rand() % 8;
            InvalidateRect(hwnd, NULL, FALSE);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SwitchNav(NavItem nav) {
    g_active_nav = nav;
    if (g_hwnd_content) {
        InvalidateRect(g_hwnd_content, NULL, FALSE);
    }
    if (g_hwnd_main) {
        HWND hwndSidebar = GetDlgItem(g_hwnd_main, 1001);
        if (hwndSidebar) InvalidateRect(hwndSidebar, NULL, FALSE);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput = {1, NULL, FALSE, FALSE};
    GdiplusStartup(&g_gdiplus_token, &gdiplusStartupInput, NULL);

    // Register MainWindow class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = L"DevHomeMainWndClass";
    wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    RegisterClassEx(&wc);

    // Register Sidebar class
    WNDCLASSEX wcSide = {0};
    wcSide.cbSize = sizeof(WNDCLASSEX);
    wcSide.lpfnWndProc = SidebarProc;
    wcSide.hInstance = hInstance;
    wcSide.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcSide.lpszClassName = L"DevHomeSidebarClass";
    RegisterClassEx(&wcSide);

    // Register Content Area class
    WNDCLASSEX wcContent = {0};
    wcContent.cbSize = sizeof(WNDCLASSEX);
    wcContent.lpfnWndProc = ContentProc;
    wcContent.hInstance = hInstance;
    wcContent.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcContent.lpszClassName = L"DevHomeContentClass";
    RegisterClassEx(&wcContent);

    // Create Main Window
    HWND hwnd = CreateWindowEx(
        0,
        L"DevHomeMainWndClass",
        L"Dev Home",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1020, 680,
        NULL, NULL, hInstance, NULL
    );
    g_hwnd_main = hwnd;

    // Create Sidebar
    CreateWindowEx(
        0,
        L"DevHomeSidebarClass",
        NULL,
        WS_CHILD | WS_VISIBLE,
        0, 0, 220, 680,
        hwnd, (HMENU)1001, hInstance, NULL
    );

    // Create Content Area
    HWND hwndContent = CreateWindowEx(
        0,
        L"DevHomeContentClass",
        NULL,
        WS_CHILD | WS_VISIBLE,
        220, 0, 800, 680,
        hwnd, (HMENU)1002, hInstance, NULL
    );
    g_hwnd_content = hwndContent;

    // Set timer to refresh dashboard performance metrics every 2 seconds
    SetTimer(hwndContent, 1, 2000, NULL);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(g_gdiplus_token);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            HWND hwndSidebar = GetDlgItem(hwnd, 1001);
            HWND hwndContent = GetDlgItem(hwnd, 1002);
            if (hwndSidebar && hwndContent) {
                MoveWindow(hwndSidebar, 0, 0, 220, height, TRUE);
                MoveWindow(hwndContent, 220, 0, width - 220, height, TRUE);
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}
