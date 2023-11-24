#include "main.h"

#include <Windows.h>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <winbase.h>
#include <tchar.h>
#include <WinInet.h>
#include <thread>
#include <chrono>



#pragma comment(lib,"Wininet.lib")

#pragma comment(lib, "winmm.lib")



using namespace std;

int Berat = 0.;
int Tinggi = 0;
float Bmi = 0.0f;

namespace Variables {
    int ActiveTab = 1;
}

void hitungBMI()
{
    float tinggiMeter = Tinggi / 100.0f;

    if (tinggiMeter > 0.0f && Berat > 0.0f)
    {
        Bmi = Berat / (tinggiMeter * tinggiMeter);
    }
}

const char* kategoriBMI()
{
    if (Bmi < 18.5)
    {
        return "Berat Badan Kamu Kurang";
    }
    else if (Bmi >= 18.5 && Bmi <= 24.9)
    {
        return "Berat Badan Kamu Normal";
    }
    else if (Bmi >= 25 && Bmi <= 29.9)
    {
        return "Kamu kelebihan berat badan";
    }
    else
    {
        return "Kamu sedang mengalami obesitas";
    }
}

void Overlay()
{
    float ScreenX = GetSystemMetrics(SM_CXSCREEN);
    float ScreenY = GetSystemMetrics(SM_CYSCREEN);

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(ScreenX, ScreenY));
    ImGui::Begin("#overlay", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMouseInputs);
    auto draw = ImGui::GetBackgroundDrawList();
}

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    //main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, wc.hInstance, NULL);



    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }


    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);


    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();


    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);



    DWORD window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    auto screenwidth = float(screen_rect.right - WINDOW_WIDTH) / 2.f;
    auto screenheight = float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f;


    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }





        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            ImU32 titleBgColor = IM_COL32(255, 0, 0, 255);

            float x = 100.0f;
            float y = 100.0f;

            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(400, 250));
            ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::SetNextWindowBgAlpha(1.0f);
            
            ImGui::Begin("                     Body Mass Index", &loader_active, window_flags);
            {     

                ImGui::InputInt("Tinggi Badan (cm)", &Tinggi);
                ImGui::InputInt("Berat Badan (kg)", &Berat);
                //ISI WINDOW NTAH TOMBOL ATAUPUN TERSERAH
                if (ImGui::Button("Hitung Bmi", ImVec2(100, 50)))
                {
                    hitungBMI();
                }

                ImGui::Text("Bmi kamu adalah: %.2f", Bmi);
                ImGui::Text("Kategori Bmi: %s", kategoriBMI());
            }

            ImGui::End();
            ImGui::PopStyleColor();
        }
        
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }


        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);


        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
