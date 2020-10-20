#ifndef GFLIB2_GRX_COLOR_H_INCLUDED
#define GFLIB2_GRX_COLOR_H_INCLUDED

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Vector3.h>

namespace gfl2 { namespace gfx {
	
	//! @brief カラー
	class Color : public math::Vector4{
	public:
		inline Color(){}
		inline Color( const Vector4& v ) : Vector4( v ) {}
		inline Color( const float r, const float g, const float b, const float a = 1.0f ) : Vector4( r, g, b, a ){}
		
		//! @brief Ｒ値取得
		inline float GetR( void ) const { return math::Vector4::GetX(); }
		//! @brief Ｇ値取得
		inline float GetG( void ) const { return math::Vector4::GetY(); }
		//! @brief Ｂ値取得
		inline float GetB( void ) const { return math::Vector4::GetZ(); }
		//! @brief Ａ値取得
		inline float GetA( void ) const { return math::Vector4::GetW(); }
		//! @brief Ｒ値変更
		inline Color& SetR( float r ){ math::Vector4::SetX( r ); return (*this); }
		//! @brief Ｇ値変更
		inline Color& SetG( float g ){ math::Vector4::SetY( g ); return (*this); }
		//! @brief Ｂ値変更
		inline Color& SetB( float b ){ math::Vector4::SetZ( b ); return (*this); }
		//! @brief Ａ値変更
		inline Color& SetA( float a ){ math::Vector4::SetW( a ); return (*this); }
		
		//! @brief ＨＳＶ変換
		inline math::Vector4 CalcHSV( void ) const{
			return math::Vector4( CalcHue(), CalcSat(), CalcVal() );
		}
		//! @brief 色相(Ｈ)取得
		float CalcHue( void ) const{
			const float cmax = MaxElem3();
			if( cmax <= 0.0f ){
				return 0.0f;
			}
			
			const float rng = cmax - MinElem3();
			if( rng == 0.0f ){
				return 0.0f;	//  計算不可
			}
			const float r = GetR();
			const float g = GetG();
			const float b = GetB();
			const float rc = (cmax - r) / rng;
			const float gc = (cmax - g) / rng;
			const float bc = (cmax - b) / rng;
			
			float h;
			if( r == cmax ){
				h = (bc - gc);
			}
			else if( g == cmax ){
				h = (2.0f + rc - bc);
			}
			else/*if( b == cmax )*/{
				h = (4.0f + gc - rc);
			}
			h /= 6.0f;
			if( h < 0.0f ){
				h += 1.0f;
			}
			
			return h;
		}
		//! @brief 彩度(Ｓ)取得
		inline float CalcSat( void ) const{
			const float cmax = MaxElem3();
			return (cmax <= 0.0f)? 0.0f: float((cmax - MinElem3()) / cmax);
		}
		//! @brief 明度(Ｖ)取得
		inline float CalcVal( void ) const{
			return MaxElem3();
		}
		//! @brief 色相(Ｈ)変更
		inline Color& SetHue( float h ){
			(*this) = GetHSV( h, CalcSat(), CalcVal() );
			return (*this);
		}
		//! @brief 彩度(Ｓ)変更
		inline Color& SetSat( float s ){
			(*this) = GetHSV( CalcHue(), s, CalcVal() );
			return (*this);
		}
		//! @brief 明度(Ｖ)変更
		inline Color& SetVal( float v ){
			(*this) = GetHSV( CalcHue(), CalcSat(), v );
			return (*this);
		}
		
		//! @brief ＲＧＢカラー合成(int)
		inline static Color GetXRGB( int r, int g, int b ){
			return Vector4( static_cast< float >( r ), static_cast< float >( g ), static_cast< float >( b ), 255.0f ) / 255.0f;
		}
		//! @brief ＲＧＢカラー合成(float)
		inline static Color GetXRGBF( float r, float g, float b ){
			return Color( r, g, b, 1.0f );
		}
		//! @brief ＲＧＢＡカラー合成(int)
		inline static Color GetRGBA( int r, int g, int b, int a ){
			return Vector4( static_cast< float >( r ), static_cast< float >( g ), static_cast< float >( b ), static_cast< float >( a ) ) / 255.0f;
		}
		//! @brief ＲＧＢＡカラー合成(float)
		inline static Color GetRGBAF( float r, float g, float b, float a ){
			return Color( r, g, b, a );
		}
		
		//! @brief ＨＳＶカラー合成
		static Color GetHSV( float h, float s, float v ){
			if( s > 0.0f ){
				float n;
				const float h2 = modff( h, &n ) * 6.0f;
				const float f = modff( h2, &n );
				const float p = (1.0f - s) * v;
				const float q = (1.0f - (s * f)) * v;
				const float t = (1.0f - (s * (1.0f - f))) * v;
				switch( static_cast< int >( h2 ) ){
				case 0: return Color( v, t, p );
				case 1: return Color( q, v, p );
				case 2: return Color( p, v, t );
				case 3: return Color( p, q, v );
				case 4: return Color( t, p, v );
				case 5: return Color( v, p, q );
				}
			}
			return Color( v, v, v );
		}
		
#define DECL_COLOR( R, G, B, NAME )	\
		inline static Color Get##NAME() { return GetXRGB( (R), (G), (B) ); }
#include "ColorDef.inl"
	};
	
}}

#endif
