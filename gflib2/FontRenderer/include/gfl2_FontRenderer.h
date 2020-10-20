#ifndef GFLIB2_FONTRENDERER_FONTRENDERER_H_INCLUDED
#define GFLIB2_FONTRENDERER_FONTRENDERER_H_INCLUDED

#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
//#include <math/include/gfl2_MathUtil.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <FontString/include/gfl2_FontString.h>
#include <FontRenderer/include/gfl2_FontRenderer.h>

using namespace gfl2;
using namespace gfl2::gfx;
using namespace gfl2::fontstring;

namespace gfl2 { namespace fontrenderer {
class FontRenderer
{
  struct VertexFormat
  {
	  math::Vector4				pos;
	  math::Vector4				uv;
	  math::Vector4				color;
	  VertexFormat()
	  {
		  pos = math::Vector4::GetZero();
		  uv = math::Vector4::GetZero();
		  color = math::Vector4::GetZero();
	  }
	  VertexFormat( math::Vector4 pos_, math::Vector4 uv_, math::Vector4 color_ )
	  {
		  pos = pos_;
		  uv = uv_;
		  color = color_;
	  }
  };

public:
	static void Create(s16 screenWidth, s16 screenHeight);
	static void Destroy();
	static b32 Update();
	static u16 Draw(const c16* wstr, f32 x, f32 y, f32 w, f32 h);
	static u16 DrawEx( f32 x, f32 y, f32 w, f32 h, const c16* wstr, ... );
	static void Reset();
	static b32 Begin();
	static void End();
	static void SetScreenSize(s16 screenWidth, s16 screenHeight);
  static void SetDefaultFontColor(const math::Vector4& color);

private:
	class Impl;
	static Impl* s_pImpl;
	// 標準のコンストラクタ, デストラクタ, コピーコンストラクタ, 代入禁止
	FontRenderer();
	~FontRenderer();
	FontRenderer(const FontRenderer&);
	void operator=(const FontRenderer&);
};

}}


#define GFL_FONT_PRINT( x, y, w, h, ...) gfl2::fontrenderer::FontRenderer::DrawEx( x, y, w, h, __VA_ARGS__ )


#endif