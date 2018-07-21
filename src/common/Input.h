#ifndef _INPUT_H_
#define _INPUT_H_

#include <windows.h>

class Input
{
    bool keys[256];
    bool active;

public:
    Input()
    {
        ZeroMemory(keys, sizeof(keys));
        active = true;
    }

    bool IsPressed(uint32_t key)
    {
        bool state = keys[key];
        keys[key] = 0;
        return state;
    }

    bool IsDown(uint32_t key)
    {
        return keys[key];
    }

    void OnKeyDown(uint32_t key)
    {
        keys[key] = true;
    }

    void OnKeyUp(uint32_t key)
    {
        keys[key] = false;
    }

    void OnFocus()
    {
        active = true;
    }

    void OnFocusLost()
    {
        ZeroMemory(keys, sizeof(keys));
        active = false;
    }
};

#endif
