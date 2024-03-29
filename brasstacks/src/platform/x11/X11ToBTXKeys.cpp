#ifdef BTX_UNUSED

#ifdef BTX_LINUX

#include "brasstacks/platform/x11/X11ToBTXKeys.hpp"

namespace btx {

X11ToBTXKeys::X11ToBTXKeys() :
    _map {
        { static_cast<uint32_t>( 8), BTX_KB_BACKSPACE },
        { static_cast<uint32_t>( 9), BTX_KB_TAB },
        { static_cast<uint32_t>(13), BTX_KB_ENTER },
        { static_cast<uint32_t>(19), BTX_KB_PAUSE },
        { static_cast<uint32_t>(20), BTX_KB_CAPS_LOCK },
        { static_cast<uint32_t>(27), BTX_KB_ESCAPE },
        { static_cast<uint32_t>(32), BTX_KB_SPACE },
        { static_cast<uint32_t>(33), BTX_KB_PAGE_UP },
        { static_cast<uint32_t>(34), BTX_KB_PAGE_DOWN },
        { static_cast<uint32_t>(35), BTX_KB_END },
        { static_cast<uint32_t>(36), BTX_KB_HOME },
        { static_cast<uint32_t>(37), BTX_KB_LEFT },
        { static_cast<uint32_t>(38), BTX_KB_UP },
        { static_cast<uint32_t>(39), BTX_KB_RIGHT },
        { static_cast<uint32_t>(40), BTX_KB_DOWN },
        { static_cast<uint32_t>(44), BTX_KB_PRINT_SCREEN },
        { static_cast<uint32_t>(45), BTX_KB_INSERT },
        { static_cast<uint32_t>(46), BTX_KB_DELETE },
        { static_cast<uint32_t>(48), BTX_KB_0 },
        { static_cast<uint32_t>(49), BTX_KB_1 },
        { static_cast<uint32_t>(50), BTX_KB_2 },
        { static_cast<uint32_t>(51), BTX_KB_3 },
        { static_cast<uint32_t>(52), BTX_KB_4 },
        { static_cast<uint32_t>(53), BTX_KB_5 },
        { static_cast<uint32_t>(54), BTX_KB_6 },
        { static_cast<uint32_t>(55), BTX_KB_7 },
        { static_cast<uint32_t>(56), BTX_KB_8 },
        { static_cast<uint32_t>(57), BTX_KB_9 },

        { static_cast<uint32_t>(BTX_KB_A), BTX_KB_A },
        { static_cast<uint32_t>(BTX_KB_B), BTX_KB_B },
        { static_cast<uint32_t>(BTX_KB_C), BTX_KB_C },
        { static_cast<uint32_t>(BTX_KB_D), BTX_KB_D },
        { static_cast<uint32_t>(BTX_KB_E), BTX_KB_E },
        { static_cast<uint32_t>(BTX_KB_F), BTX_KB_F },
        { static_cast<uint32_t>(BTX_KB_G), BTX_KB_G },
        { static_cast<uint32_t>(BTX_KB_H), BTX_KB_H },
        { static_cast<uint32_t>(BTX_KB_I), BTX_KB_I },
        { static_cast<uint32_t>(BTX_KB_J), BTX_KB_J },
        { static_cast<uint32_t>(BTX_KB_K), BTX_KB_K },
        { static_cast<uint32_t>(BTX_KB_L), BTX_KB_L },
        { static_cast<uint32_t>(BTX_KB_M), BTX_KB_M },
        { static_cast<uint32_t>(BTX_KB_N), BTX_KB_N },
        { static_cast<uint32_t>(BTX_KB_O), BTX_KB_O },
        { static_cast<uint32_t>(BTX_KB_P), BTX_KB_P },
        { static_cast<uint32_t>(BTX_KB_Q), BTX_KB_Q },
        { static_cast<uint32_t>(BTX_KB_R), BTX_KB_R },
        { static_cast<uint32_t>(BTX_KB_S), BTX_KB_S },
        { static_cast<uint32_t>(BTX_KB_T), BTX_KB_T },
        { static_cast<uint32_t>(BTX_KB_U), BTX_KB_U },
        { static_cast<uint32_t>(BTX_KB_V), BTX_KB_V },
        { static_cast<uint32_t>(BTX_KB_W), BTX_KB_W },
        { static_cast<uint32_t>(BTX_KB_X), BTX_KB_X },
        { static_cast<uint32_t>(BTX_KB_Y), BTX_KB_Y },
        { static_cast<uint32_t>(BTX_KB_Z), BTX_KB_Z },

        { static_cast<uint32_t>( 93), BTX_KB_MENU },
        { static_cast<uint32_t>( 96), BTX_KB_NP_0 },
        { static_cast<uint32_t>( 97), BTX_KB_NP_1 },
        { static_cast<uint32_t>( 98), BTX_KB_NP_2 },
        { static_cast<uint32_t>( 99), BTX_KB_NP_3 },
        { static_cast<uint32_t>(100), BTX_KB_NP_4 },
        { static_cast<uint32_t>(101), BTX_KB_NP_5 },
        { static_cast<uint32_t>(102), BTX_KB_NP_6 },
        { static_cast<uint32_t>(103), BTX_KB_NP_7 },
        { static_cast<uint32_t>(104), BTX_KB_NP_8 },
        { static_cast<uint32_t>(105), BTX_KB_NP_9 },
        { static_cast<uint32_t>(106), BTX_KB_NP_MULTIPLY },
        { static_cast<uint32_t>(107), BTX_KB_NP_ADD },
        { static_cast<uint32_t>(109), BTX_KB_NP_SUBTRACT },
        { static_cast<uint32_t>(110), BTX_KB_NP_DECIMAL },
        { static_cast<uint32_t>(111), BTX_KB_NP_DIVIDE },
        { static_cast<uint32_t>(112), BTX_KB_F1 },
        { static_cast<uint32_t>(113), BTX_KB_F2 },
        { static_cast<uint32_t>(114), BTX_KB_F3 },
        { static_cast<uint32_t>(115), BTX_KB_F4 },
        { static_cast<uint32_t>(116), BTX_KB_F5 },
        { static_cast<uint32_t>(117), BTX_KB_F6 },
        { static_cast<uint32_t>(118), BTX_KB_F7 },
        { static_cast<uint32_t>(119), BTX_KB_F8 },
        { static_cast<uint32_t>(120), BTX_KB_F9 },
        { static_cast<uint32_t>(121), BTX_KB_F10 },
        { static_cast<uint32_t>(122), BTX_KB_F11 },
        { static_cast<uint32_t>(123), BTX_KB_F12 },
        { static_cast<uint32_t>(144), BTX_KB_NUM_LOCK },
        { static_cast<uint32_t>(145), BTX_KB_SCROLL_LOCK },
        { static_cast<uint32_t>(160), BTX_KB_LEFT_SHIFT },
        { static_cast<uint32_t>(161), BTX_KB_RIGHT_SHIFT },
        { static_cast<uint32_t>(186), BTX_KB_SEMICOLON },
        { static_cast<uint32_t>(187), BTX_KB_EQUAL },
        { static_cast<uint32_t>(188), BTX_KB_COMMA },
        { static_cast<uint32_t>(189), BTX_KB_MINUS },
        { static_cast<uint32_t>(190), BTX_KB_PERIOD },
        { static_cast<uint32_t>(191), BTX_KB_FRONT_SLASH },
        { static_cast<uint32_t>(192), BTX_KB_GRAVE },
        { static_cast<uint32_t>(219), BTX_KB_LEFT_BRACKET },
        { static_cast<uint32_t>(221), BTX_KB_RIGHT_BRACKET },
        { static_cast<uint32_t>(220), BTX_KB_BACK_SLASH },
        { static_cast<uint32_t>(222), BTX_KB_APOSTROPHE },
        { static_cast<uint32_t>(335), BTX_KB_NP_ENTER },

        { static_cast<uint32_t>(BTX_KB_LEFT_CTRL),   BTX_KB_LEFT_CTRL },
        { static_cast<uint32_t>(BTX_KB_RIGHT_CTRL),  BTX_KB_RIGHT_CTRL },
        { static_cast<uint32_t>(BTX_KB_LEFT_SUPER),  BTX_KB_LEFT_SUPER },
        { static_cast<uint32_t>(BTX_KB_RIGHT_SUPER), BTX_KB_RIGHT_SUPER },
        { static_cast<uint32_t>(BTX_KB_LEFT_ALT),    BTX_KB_LEFT_ALT },
        { static_cast<uint32_t>(BTX_KB_RIGHT_ALT),   BTX_KB_RIGHT_ALT },
    }
{ }

} // namespace btx

#endif // BTX_LINUX

#endif // BTX_UNUSED