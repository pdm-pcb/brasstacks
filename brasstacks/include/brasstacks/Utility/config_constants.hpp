// Win32 Control IDs
constexpr std::uint16_t IDC_GPU_LIST   = 1;
constexpr std::uint16_t IDC_RES_LIST   = 2;
constexpr std::uint16_t IDC_START      = 3;
constexpr std::uint16_t IDC_QUIT       = 4;
constexpr std::uint16_t IDC_VSYNC      = 5;
constexpr std::uint16_t IDC_FULLSCREEN = 6;
constexpr std::uint16_t IDC_API        = 7;

// Config window dimensions
constexpr std::uint16_t WINDOW_X      = 640;
constexpr std::uint16_t WINDOW_Y      = 240;
constexpr std::uint16_t WINDOW_PAD    = 10;
constexpr std::uint16_t LABEL_HEIGHT  = 25;
constexpr std::uint16_t LABEL_WIDTH   = (WINDOW_X / 2) -
                                   static_cast<std::uint32_t>(WINDOW_PAD * 1.5);
constexpr std::uint16_t BUTTON_HEIGHT = 25;
constexpr std::uint16_t BUTTON_WIDTH  = 80;
constexpr std::uint16_t LIST_WIDTH    = LABEL_WIDTH;
constexpr std::uint16_t LIST_HEIGHT   = WINDOW_Y - (WINDOW_PAD * 2) -
                                   LABEL_HEIGHT - BUTTON_HEIGHT;