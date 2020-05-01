#include "convert_key.h"

#include <map>

#include <X11/keysym.h>

namespace sr
{

const std::map<int, KeyCodes> Table = {
    {XK_BackSpace, KEY_BACKSPACE},
    {XK_Tab, KEY_TAB},

    {XK_Clear, KEY_CLEAR},
    {XK_Return, KEY_ENTER},

    {XK_Shift_L, KEY_SHIFT},
    {XK_Control_L, KEY_CONTROL},
    {XK_Alt_L, KEY_ALT},
    {XK_Pause, KEY_PAUSE},
    {XK_Caps_Lock, KEY_CAPSLOCK},

    {XK_Escape, KEY_ESCAPE},
    {XK_space, KEY_SPACE},
    {XK_Page_Up, KEY_PAGEUP},
    {XK_Page_Down, KEY_PAGEDOWN},
    {XK_End, KEY_END},
    {XK_Home, KEY_HOME},
    {XK_Left, KEY_LEFT},
    {XK_Up, KEY_UP},
    {XK_Right, KEY_RIGHT},
    {XK_Down, KEY_DOWN},
    {XK_Select, KEY_SELECT},
    {XK_Print, KEY_PRINT},
    {XK_Execute, KEY_EXECUTE},
    {XK_Sys_Req, KEY_PRINTSCREEN},
    {XK_Insert, KEY_INSERT},
    {XK_Delete, KEY_DELETE},
    {XK_Help, KEY_HELP},
    {XK_0, KEY_0},
    {XK_1, KEY_1},
    {XK_2, KEY_2},
    {XK_3, KEY_3},
    {XK_4, KEY_4},
    {XK_5, KEY_5},
    {XK_6, KEY_6},
    {XK_7, KEY_7},
    {XK_8, KEY_8},
    {XK_9, KEY_9},

    {XK_a, KEY_A},
    {XK_b, KEY_B},
    {XK_c, KEY_C},
    {XK_d, KEY_D},
    {XK_e, KEY_E},
    {XK_f, KEY_F},
    {XK_g, KEY_G},
    {XK_h, KEY_H},
    {XK_i, KEY_I},
    {XK_j, KEY_J},
    {XK_k, KEY_K},
    {XK_l, KEY_L},
    {XK_m, KEY_M},
    {XK_n, KEY_N},
    {XK_o, KEY_O},
    {XK_p, KEY_P},
    {XK_q, KEY_Q},
    {XK_r, KEY_R},
    {XK_s, KEY_S},
    {XK_t, KEY_T},
    {XK_u, KEY_U},
    {XK_v, KEY_V},
    {XK_w, KEY_W},
    {XK_x, KEY_X},
    {XK_y, KEY_Y},
    {XK_z, KEY_Z},
    {XK_Super_L, KEY_LEFTWIN},
    {XK_Super_R, KEY_RIGHTWIN},

    {XK_KP_0, KEY_NUMPAD0},
    {XK_KP_1, KEY_NUMPAD1},
    {XK_KP_2, KEY_NUMPAD2},
    {XK_KP_3, KEY_NUMPAD3},
    {XK_KP_4, KEY_NUMPAD4},
    {XK_KP_5, KEY_NUMPAD5},
    {XK_KP_6, KEY_NUMPAD6},
    {XK_KP_7, KEY_NUMPAD7},
    {XK_KP_8, KEY_NUMPAD8},
    {XK_KP_9, KEY_NUMPAD9},

    {XK_multiply, KEY_MULTIPLY},
    {XK_KP_Add, KEY_ADD},
    {XK_KP_Separator, KEY_SEPARATOR},
    {XK_KP_Subtract, KEY_SUBTRACT},
    {XK_KP_Decimal, KEY_DECIMAL},
    {XK_KP_Divide, KEY_DIVIDE},
    {XK_F1, KEY_F1},
    {XK_F2, KEY_F2},
    {XK_F3, KEY_F3},
    {XK_F4, KEY_F4},
    {XK_F5, KEY_F5},
    {XK_F6, KEY_F6},
    {XK_F7, KEY_F7},
    {XK_F8, KEY_F8},
    {XK_F9, KEY_F9},
    {XK_F10, KEY_F10},
    {XK_F11, KEY_F11},
    {XK_F12, KEY_F12},
    {XK_F13, KEY_F13},
    {XK_F14, KEY_F14},
    {XK_F15, KEY_F15},
    {XK_F16, KEY_F16},
    {XK_F17, KEY_F17},
    {XK_F18, KEY_F18},
    {XK_F19, KEY_F19},
    {XK_F20, KEY_F20},
    {XK_F21, KEY_F21},
    {XK_F22, KEY_F22},
    {XK_F23, KEY_F23},
    {XK_F24, KEY_F24},

    {XK_Scroll_Lock, KEY_SCROLLOCK},

    // {XK_Shift_L, KEY_LSHIFT},
    {XK_Shift_R, KEY_RSHIFT},
    // {XK_Control_L, KEY_LCONTROL},
    {XK_Control_R, KEY_RCONTROL},
    {XK_Menu, KEY_RMENU},
};

KeyCodes ConvertKey(int key)
{
    const auto it = Table.find(key);
    if (it != Table.end())
    {
        return it->second;
    }

    return KEY_UNKNOWN;
}

} // namespace sr
