#include <gfx/include/gfl2_GFGL.h>

namespace gfl2 { namespace gfx {

//============ 明示的実体化 =================
//静的メンバは、やはりcppに書きたい。この方がしっくりくる。
#if defined(GF_PLATFORM_WIN32)

#if defined(_WIN64)
	template<>
	GFGLBase<windx11::WinDX11GL>* GFGLBase<windx11::WinDX11GL>::s_Gp = NULL;
#else
	template<>
	GFGLBase<winOpenGL::WinOpenGLGL>* GFGLBase<winOpenGL::WinOpenGLGL>::s_Gp = NULL;
#endif

#elif defined(NN_PLATFORM_CTR)
	template<>
	GFGLBase<ctr::CTRGL>* GFGLBase<ctr::CTRGL>::s_Gp = NULL;
#endif


}}
