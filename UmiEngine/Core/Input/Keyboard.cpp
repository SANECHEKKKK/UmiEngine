module;
#include <iostream>
#include <conio.h>
#include <memory>
#include <windows.h>
module Keyboard;

using namespace Umi;

static_assert(sizeof(Keyboard_State) == 256 / 8, "キーボード状態構造体のサイズ不一致");

Keyboard::Keyboard()
{
    Reset();
}


void Keyboard::keyDown(int key)
{
    if (key < 0 || key > 0xfe) { return; }

    unsigned int* p = (unsigned int*)&gState;
    unsigned int bf = 1u << (key & 0x1f);
    p[(key >> 5)] |= bf;
}


void Keyboard::keyUp(int key)
{
    if (key < 0 || key > 0xfe) { return; }

    unsigned int* p = (unsigned int*)&gState;
    unsigned int bf = 1u << (key & 0x1f);
    p[(key >> 5)] &= ~bf;
}

void Keyboard::Update(void)
{
    gOldState = gState;
}


bool Keyboard::IsKeyDown(Keyboard_Keys key, const Keyboard_State* pState)
{
    if (key <= 0xfe)
    {
        unsigned int* p = (unsigned int*)pState;
        unsigned int bf = 1u << (key & 0x1f);
        return (p[(key >> 5)] & bf) != 0;
    }
    return false;
}


bool Keyboard::IsKeyUp(Keyboard_Keys key, const Keyboard_State* pState)
{
    if (key <= 0xfe)
    {
        unsigned int* p = (unsigned int*)pState;
        unsigned int bf = 1u << (key & 0x1f);
        return (p[(key >> 5)] & bf) == 0;
    }
    return false;
}


bool Keyboard::IsKeyTrigger(Keyboard_Keys key, const Keyboard_State* pState, const Keyboard_State* pOldState)
{
    if (key <= 0xfe)
    {
        unsigned int* p = (unsigned int*)pState;
        unsigned int* op = (unsigned int*)pOldState;
        unsigned int bf = 1u << (key & 0x1f);
        return ((p[(key >> 5)] & bf) != 0) && ((op[(key >> 5)] & bf) == 0);
    }
    return false;
}


bool Keyboard::IsKeyDown(Keyboard_Keys key)
{
    return IsKeyDown(key, &gState);
}


bool Keyboard::IsKeyUp(Keyboard_Keys key)
{
    return Keyboard::IsKeyUp(key, &gState);
}

bool Keyboard::IsKeyTrigger(Keyboard_Keys key)
{
    return IsKeyTrigger(key, &gState, &gOldState);
}

//キーボードの現在の状態を取得する
Keyboard_State* Keyboard::GetState()
{
    return &gState;
}


void Keyboard::Reset(void)
{
    ZeroMemory(&gState, sizeof(Keyboard_State));
}


//キーボード制御のためのウィンドウメッセージプロシージャフック関数
void Keyboard::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    Update();
    bool down = false;

    switch (message)
    {
    case WM_ACTIVATEAPP:
        Reset();
        return;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        down = true;
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:
        break;

    default:
        return;
    }

    int vk = (int)wParam;
    switch (vk)
    {
    case VK_SHIFT:
        vk = (int)MapVirtualKey(((unsigned int)lParam & 0x00ff0000) >> 16u, MAPVK_VSC_TO_VK_EX);
        if (!down)
        {
            //左シフトと右シフトの両方が同時に押された場合にクリアされるようにするための回避策
            keyUp(VK_LSHIFT);
            keyUp(VK_RSHIFT);
        }
        break;

    case VK_CONTROL:
        vk = ((UINT)lParam & 0x01000000) ? VK_RCONTROL : VK_LCONTROL;
        break;

    case VK_MENU:
        vk = ((UINT)lParam & 0x01000000) ? VK_RMENU : VK_LMENU;
        break;
    }

    if (down)
    {
        keyDown(vk);
    }
    else
    {
        keyUp(vk);
    }
}