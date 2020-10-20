#ifndef GFLIB2_GRX_COLORU8_H_INCLUDED
#define GFLIB2_GRX_COLORU8_H_INCLUDED

#include <gfx/include/gfl2_Color.h>

namespace gfl2 { namespace gfx {
	
	//! @brief カラー
	class ColorU8 {
	public:
		inline ColorU8() : r(0), g(0), b(0), a(0){}

		inline ColorU8( const Color& color )
    {
      r = static_cast<u8>( color.GetR() * 255.0f );
      g = static_cast<u8>( color.GetG() * 255.0f );
      b = static_cast<u8>( color.GetB() * 255.0f );
      a = static_cast<u8>( color.GetA() * 255.0f );
    }

    inline ColorU8( const u8 _r, const u8 _g, const u8 _b, const u8 _a = 1.0f ) : r(_r), g(_g), b(_b), a(_a){}

    inline ColorU8&  operator =( const Color& rhs )
    {
      r = static_cast<u8>( rhs.GetR() * 255.0f );
      g = static_cast<u8>( rhs.GetG() * 255.0f );
      b = static_cast<u8>( rhs.GetB() * 255.0f );
      a = static_cast<u8>( rhs.GetA() * 255.0f );

      return *this;
    }

    inline Color GetColor() const
    {
      return Color ( static_cast<f32>(r) / 255.0f,
                     static_cast<f32>(g) / 255.0f,
                     static_cast<f32>(b) / 255.0f,
                     static_cast<f32>(a) / 255.0f );
    }
		
    //! @brief ＲＧＢカラー合成(u8)
	  inline static ColorU8 GetXRGB( u8 r, u8 g, u8 b )
    {
		  return ColorU8( r, g, b );
	  }

#define DECL_COLOR( R, G, B, NAME )	\
		inline static ColorU8 Get##NAME() { return GetXRGB( (R), (G), (B) ); }
#include "ColorDef.inl"

    u8        r;
    u8        g;
    u8        b;
    u8        a;

	};
	
}}

#endif
