#include <system/include/InputDevice/gfl2_Pad.h>

namespace gfl2 { namespace system { namespace InputDevice {

//============ 明示的実体化 =================
//静的メンバは、やはりcppに書きたい。この方がしっくりくる。
#if defined(GF_PLATFORM_WIN32)

template<>
PadBase<WinDX11::WinDX11Pad>* PadBase<WinDX11::WinDX11Pad>::s_Gp = NULL;

#elif defined(GF_PLATFORM_CTR)

template<>
PadBase<CTR::CTRPad>* PadBase<CTR::CTRPad>::s_Gp = NULL;

#endif



}}}