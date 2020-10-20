#include <system/include/InputDevice/gfl2_Mouse.h>

namespace gfl2 { namespace system { namespace InputDevice {

//============ 明示的実体化 =================
//静的メンバは、やはりcppに書きたい。この方がしっくりくる。
#if defined(GF_PLATFORM_WIN32)

template<>
MouseBase<WinDX11::WinDX11Mouse>* MouseBase<WinDX11::WinDX11Mouse>::s_Gp = NULL;

#endif


}}}