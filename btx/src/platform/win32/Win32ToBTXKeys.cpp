#include "brasstacks/brasstacks.hpp"
#include "brasstacks/platform/win32/Win32ToBTXKeys.hpp"

namespace btx {

std::unordered_map<::USHORT, Keycode> const Win32ToBTXKeys::map {
    {  static_cast<::USHORT>( 8), BTX_KB_BACKSPACE },
    {  static_cast<::USHORT>( 9), BTX_KB_TAB },
    {  static_cast<::USHORT>(13), BTX_KB_ENTER },
    {  static_cast<::USHORT>(19), BTX_KB_PAUSE },
    {  static_cast<::USHORT>(20), BTX_KB_CAPS_LOCK },
    {  static_cast<::USHORT>(27), BTX_KB_ESCAPE },
    {  static_cast<::USHORT>(32), BTX_KB_SPACE },
    {  static_cast<::USHORT>(33), BTX_KB_PAGE_UP },
    {  static_cast<::USHORT>(34), BTX_KB_PAGE_DOWN },
    {  static_cast<::USHORT>(35), BTX_KB_END },
    {  static_cast<::USHORT>(36), BTX_KB_HOME },
    {  static_cast<::USHORT>(37), BTX_KB_LEFT },
    {  static_cast<::USHORT>(38), BTX_KB_UP },
    {  static_cast<::USHORT>(39), BTX_KB_RIGHT },
    {  static_cast<::USHORT>(40), BTX_KB_DOWN },
    {  static_cast<::USHORT>(44), BTX_KB_PRINT_SCREEN },
    {  static_cast<::USHORT>(45), BTX_KB_INSERT },
    {  static_cast<::USHORT>(46), BTX_KB_DELETE },
    {  static_cast<::USHORT>(48), BTX_KB_0 },
    {  static_cast<::USHORT>(49), BTX_KB_1 },
    {  static_cast<::USHORT>(50), BTX_KB_2 },
    {  static_cast<::USHORT>(51), BTX_KB_3 },
    {  static_cast<::USHORT>(52), BTX_KB_4 },
    {  static_cast<::USHORT>(53), BTX_KB_5 },
    {  static_cast<::USHORT>(54), BTX_KB_6 },
    {  static_cast<::USHORT>(55), BTX_KB_7 },
    {  static_cast<::USHORT>(56), BTX_KB_8 },
    {  static_cast<::USHORT>(57), BTX_KB_9 },

    {  static_cast<::USHORT>(BTX_KB_A), BTX_KB_A },
    {  static_cast<::USHORT>(BTX_KB_B), BTX_KB_B },
    {  static_cast<::USHORT>(BTX_KB_C), BTX_KB_C },
    {  static_cast<::USHORT>(BTX_KB_D), BTX_KB_D },
    {  static_cast<::USHORT>(BTX_KB_E), BTX_KB_E },
    {  static_cast<::USHORT>(BTX_KB_F), BTX_KB_F },
    {  static_cast<::USHORT>(BTX_KB_G), BTX_KB_G },
    {  static_cast<::USHORT>(BTX_KB_H), BTX_KB_H },
    {  static_cast<::USHORT>(BTX_KB_I), BTX_KB_I },
    {  static_cast<::USHORT>(BTX_KB_J), BTX_KB_J },
    {  static_cast<::USHORT>(BTX_KB_K), BTX_KB_K },
    {  static_cast<::USHORT>(BTX_KB_L), BTX_KB_L },
    {  static_cast<::USHORT>(BTX_KB_M), BTX_KB_M },
    {  static_cast<::USHORT>(BTX_KB_N), BTX_KB_N },
    {  static_cast<::USHORT>(BTX_KB_O), BTX_KB_O },
    {  static_cast<::USHORT>(BTX_KB_P), BTX_KB_P },
    {  static_cast<::USHORT>(BTX_KB_Q), BTX_KB_Q },
    {  static_cast<::USHORT>(BTX_KB_R), BTX_KB_R },
    {  static_cast<::USHORT>(BTX_KB_S), BTX_KB_S },
    {  static_cast<::USHORT>(BTX_KB_T), BTX_KB_T },
    {  static_cast<::USHORT>(BTX_KB_U), BTX_KB_U },
    {  static_cast<::USHORT>(BTX_KB_V), BTX_KB_V },
    {  static_cast<::USHORT>(BTX_KB_W), BTX_KB_W },
    {  static_cast<::USHORT>(BTX_KB_X), BTX_KB_X },
    {  static_cast<::USHORT>(BTX_KB_Y), BTX_KB_Y },
    {  static_cast<::USHORT>(BTX_KB_Z), BTX_KB_Z },

    { static_cast<::USHORT>( 93), BTX_KB_MENU },
    { static_cast<::USHORT>( 96), BTX_KB_NP_0 },
    { static_cast<::USHORT>( 97), BTX_KB_NP_1 },
    { static_cast<::USHORT>( 98), BTX_KB_NP_2 },
    { static_cast<::USHORT>( 99), BTX_KB_NP_3 },
    { static_cast<::USHORT>(100), BTX_KB_NP_4 },
    { static_cast<::USHORT>(101), BTX_KB_NP_5 },
    { static_cast<::USHORT>(102), BTX_KB_NP_6 },
    { static_cast<::USHORT>(103), BTX_KB_NP_7 },
    { static_cast<::USHORT>(104), BTX_KB_NP_8 },
    { static_cast<::USHORT>(105), BTX_KB_NP_9 },
    { static_cast<::USHORT>(106), BTX_KB_NP_MULTIPLY },
    { static_cast<::USHORT>(107), BTX_KB_NP_ADD },
    { static_cast<::USHORT>(109), BTX_KB_NP_SUBTRACT },
    { static_cast<::USHORT>(110), BTX_KB_NP_DECIMAL },
    { static_cast<::USHORT>(111), BTX_KB_NP_DIVIDE },
    { static_cast<::USHORT>(112), BTX_KB_F1 },
    { static_cast<::USHORT>(113), BTX_KB_F2 },
    { static_cast<::USHORT>(114), BTX_KB_F3 },
    { static_cast<::USHORT>(115), BTX_KB_F4 },
    { static_cast<::USHORT>(116), BTX_KB_F5 },
    { static_cast<::USHORT>(117), BTX_KB_F6 },
    { static_cast<::USHORT>(118), BTX_KB_F7 },
    { static_cast<::USHORT>(119), BTX_KB_F8 },
    { static_cast<::USHORT>(120), BTX_KB_F9 },
    { static_cast<::USHORT>(121), BTX_KB_F10 },
    { static_cast<::USHORT>(122), BTX_KB_F11 },
    { static_cast<::USHORT>(123), BTX_KB_F12 },
    { static_cast<::USHORT>(144), BTX_KB_NUM_LOCK },
    { static_cast<::USHORT>(145), BTX_KB_SCROLL_LOCK },
    { static_cast<::USHORT>(160), BTX_KB_LEFT_SHIFT },
    { static_cast<::USHORT>(161), BTX_KB_RIGHT_SHIFT },
    { static_cast<::USHORT>(186), BTX_KB_SEMICOLON },
    { static_cast<::USHORT>(187), BTX_KB_EQUAL },
    { static_cast<::USHORT>(188), BTX_KB_COMMA },
    { static_cast<::USHORT>(189), BTX_KB_MINUS },
    { static_cast<::USHORT>(190), BTX_KB_PERIOD },
    { static_cast<::USHORT>(191), BTX_KB_FRONT_SLASH },
    { static_cast<::USHORT>(192), BTX_KB_GRAVE },
    { static_cast<::USHORT>(219), BTX_KB_LEFT_BRACKET },
    { static_cast<::USHORT>(221), BTX_KB_RIGHT_BRACKET },
    { static_cast<::USHORT>(220), BTX_KB_BACK_SLASH },
    { static_cast<::USHORT>(222), BTX_KB_APOSTROPHE },
    { static_cast<::USHORT>(335), BTX_KB_NP_ENTER },

    { static_cast<::USHORT>(BTX_KB_LEFT_CTRL),   BTX_KB_LEFT_CTRL },
    { static_cast<::USHORT>(BTX_KB_RIGHT_CTRL),  BTX_KB_RIGHT_CTRL },
    { static_cast<::USHORT>(BTX_KB_LEFT_SUPER),  BTX_KB_LEFT_SUPER },
    { static_cast<::USHORT>(BTX_KB_RIGHT_SUPER), BTX_KB_RIGHT_SUPER },
    { static_cast<::USHORT>(BTX_KB_LEFT_ALT),    BTX_KB_LEFT_ALT },
    { static_cast<::USHORT>(BTX_KB_RIGHT_ALT),   BTX_KB_RIGHT_ALT },
};

} // namespace btx