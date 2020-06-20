#ifndef _INPUT_H_
#define _INPUT_H_

#include <string.h>

#include "key_codes.h"

namespace sr
{

class Input
{
  public:
    struct Point
    {
        int x;
        int y;
    };

    Input()
    {
        memset(keys_, 0, sizeof(keys_));
        active_ = true;
    }

    bool IsPressed(uint32_t key)
    {
        bool state = keys_[key];
        keys_[key] = 0;
        return state;
    }

    bool IsHolding(uint32_t key) const
    {
        return keys_[key];
    }

    void OnKeyDown(uint32_t key)
    {
        keys_[key] = true;
    }

    void OnKeyUp(uint32_t key)
    {
        keys_[key] = false;
    }

    void OnFocus()
    {
        active_ = true;
    }

    void OnFocusLost()
    {
        memset(keys_, 0, sizeof(keys_));
        active_ = false;
    }

    void OnMouseButtonDown(uint32_t button, int x, int y)
    {
        keys_[button] = true;
        click_points_[button] = Point{x, y};
    }

    void OnMouseButtonUp(uint32_t button, int /* x */, int /* y */)
    {
        keys_[button] = false;
    }

    Point GetClickPoint(uint32_t button = KEY_LBUTTON) const
    {
        return click_points_[button];
    }

  private:
    static const size_t MAX_MOUSE_BUTTON_KEYCODE = static_cast<size_t>(KEY_XBUTTON2);

    bool keys_[256];
    bool active_;

    Point click_points_[MAX_MOUSE_BUTTON_KEYCODE + 1];
};

} // namespace sr

#endif
