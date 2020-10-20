/**
 * @file	gfl_Ellipse.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/10/31, 10:20
 */

#ifndef GFL_ELLIPSE_H
#define	GFL_ELLIPSE_H
#pragma once

namespace gfl {
namespace core {

/**
 * 楕円でクランプするクラス
 */
class ClampEllipse {
public:

	enum {
		CENTER_PLUS,
		CENTER_MINUS,
		CENTER_MAX
	};

	ClampEllipse( void ) {
	}

	/**
	 * コンストラクタ
	 * @param v　ベクトル２型　X軸半径　Y軸半径
	 */
	explicit ClampEllipse( const gfl::core::Vector2& v ) {
		SetFocus( v );
	}

	/**
	 * コンストラクタ
	 * @param x　X軸半径
	 * @param y　Y軸半径
	 */
	ClampEllipse( const f32 x, const f32 y ) {
		SetFocus( x, y );
	}

	/**
	 * 楕円の焦点を求める
	 * @param x　X軸半径
	 * @param y　Y軸半径
	 */
	void SetFocus( const f32 x, const f32 y ) {
		const f32 a = x*x;
		const f32 b = y*y;

		m_A = x, m_B = y, m_A2 = a, m_B2 = b;
		// 焦点を設定
		if( a < b ){ // Y軸の方が大きい場合は、Y軸上に焦点がある
			m_Center[CENTER_MINUS].GFL_VECTOR_X = m_Center[CENTER_PLUS].GFL_VECTOR_X = 0.0f;
			const f32 cy = gfl::core::Math::FSqrt( b - a );
			m_Center[CENTER_MINUS].GFL_VECTOR_Y = -cy;
			m_Center[CENTER_PLUS].GFL_VECTOR_Y = cy;
		} else { // X軸の方が大きい場合は、X軸上に焦点がある　等しい場合は円
			m_Center[CENTER_MINUS].GFL_VECTOR_Y = m_Center[CENTER_PLUS].GFL_VECTOR_Y = 0.0f;
			const f32 cx = gfl::core::Math::FSqrt( a - b );
			m_Center[CENTER_MINUS].GFL_VECTOR_X = -cx;
			m_Center[CENTER_PLUS].GFL_VECTOR_X = cx;
		}
		m_Distance = GetDistance( x, 0.0f ); // x 軸上の点を基準とする
	}

	void SetFocus( const gfl::core::Vector2& v ) {
		SetFocus( v.GFL_VECTOR_X, v.GFL_VECTOR_Y );
	}

	/**
	 * 楕円の外の場合は楕円軌道上に設定する
	 * @param v　入力ベクトル　内側の場合は変化なし
	 * @param over 制限をオーバーした値
	 */
	bool Clamp( gfl::core::Vector2* v, gfl::core::Vector2* over = 0 ) {
		if( m_A == 0.0f ){ // Y軸上のリミット　線分でのリミット
			f32 y = v->GFL_VECTOR_Y;
			f32 ly;
			s32 ret = false;
			if( 0.0f < y ){ // plus mode
				ly = m_B;
				if( ly < y ){
					y = ly;
					ret = true;
				}
			} else {
				ly = -m_B;
				if( y < ly ){
					y = ly;
					ret = true;
				}
			}
			if( over ){
				over->GFL_VECTOR_Y = 0.0f;
			}
			if(ret){
				if( over ){
					over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - ly;
				}
				v->GFL_VECTOR_Y = y;
			}
			v->GFL_VECTOR_X = 0.0f;
			if( over ){
				over->GFL_VECTOR_X = v->GFL_VECTOR_X;
			}
			return true; // ret じゃないよ
		} else if( m_B == 0.0f ){ // X軸上のリミット　線分でのリミット
			f32 x = v->GFL_VECTOR_X;
			f32 lx;
			s32 ret = false;
			if( 0.0f < x ){ // plus mode
				lx = m_A;
				if( lx < x ){
					x = lx;
					ret = true;
				}
			} else {
				lx = -m_A;
				if( x < lx ){
					x = lx;
					ret = true;
				}
			}
			if( over ){
				over->GFL_VECTOR_X = 0.0f;
			}
			if(ret){
				if( over ){
					over->GFL_VECTOR_X = v->GFL_VECTOR_X - lx;
				}
				v->GFL_VECTOR_X = x;
			}
			v->GFL_VECTOR_Y = 0.0f;
			if( over ){
				over->GFL_VECTOR_Y = v->GFL_VECTOR_Y;
			}
			return true; // ret じゃないよ
		} else if( GetDistance( ) < GetDistance( *v ) ){ // 楕円の外側にある　Distance2で比較　Sqrtしないので高速
			if( v->GFL_VECTOR_X == 0.0f ){ // xが０だと傾きが無限大になってしまうので
				const f32 y = (0.0f < v->GFL_VECTOR_Y) ? m_B : -m_B;
				if( over ){
					over->GFL_VECTOR_X = 0.0f;
					over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - y;
				}
				v->GFL_VECTOR_X = 0.0f;
				v->GFL_VECTOR_Y = y;
			} else {
				const f32 m = v->GFL_VECTOR_Y / v->GFL_VECTOR_X; // 直線の傾き
				f32 x = (m_A * m_B) / gfl::core::Math::FSqrt( m_B2 + m_A2 * m * m ); // 楕円の式に代入して整理
				if( v->GFL_VECTOR_X < 0.0f ){ // ｘの解は±なので
					x = -x;
				}
				const f32 y = m * x;
				if( over ){
					over->GFL_VECTOR_X = v->GFL_VECTOR_X - x;
					over->GFL_VECTOR_Y = v->GFL_VECTOR_Y - y;
				}
				v->GFL_VECTOR_X = x;
				v->GFL_VECTOR_Y = y; // 直線の式に代入
			}
			return true;
		}
		return false;
	}

	/**
	 * 楕円の外の場合は楕円軌道上に設定する
	 * @param x　制限をかけるX値
	 * @param y　制限をかけるY値
	 * @param x_over　制限をオーバーしたX値
	 * @param y_over　制限をオーバーしたY値
	 */
	void Clamp( f32* x, f32* y, f32* x_over = 0, f32* y_over = 0 ) {
		gfl::core::Vector2 pos( *x, *y );
		if( x_over && y_over ){
			gfl::core::Vector2 over;
			if( Clamp( &pos, &over ) ){
				*x = pos.GFL_VECTOR_X;
				*y = pos.GFL_VECTOR_Y;
				*x_over = over.GFL_VECTOR_X;
				*y_over = over.GFL_VECTOR_Y;
			}else{
				*x_over = *y_over = 0.0f;
			}
		} else {
			if( Clamp( &pos ) ){
				*x = pos.GFL_VECTOR_X;
				*y = pos.GFL_VECTOR_Y;
			}
		}
	}
private:
public:
	f32 GetDistance( const gfl::core::Vector2 & pos ) const {
		return pos.Distance( m_Center[CENTER_MINUS] ) + pos.Distance( m_Center[CENTER_PLUS] );
	}

	f32 GetDistance( const f32 x, const f32 y ) const {
		return GetDistance( gfl::core::Vector2( x, y ) );
	}

	f32 GetDistance( void ) const {
		return m_Distance;
	}

	gfl::core::Vector2 m_Center[CENTER_MAX];
	f32 m_A;
	f32 m_B;
	f32 m_A2;
	f32 m_B2;
	f32 m_Distance;
};

}
}

#endif	/* GFL_ELLIPSE_H */

