/* 
 * File:   gfl_Rect.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 16:06
 */

#ifndef _GFL_RECT_H_
#define	_GFL_RECT_H_
#pragma once

namespace gfl {
namespace core {

// =============================================================================
// このファイルに含まれるクラス群
// =============================================================================
template<typename T> struct RectStr;
template<typename T> class Rect;

/**
 *  ４つの要素を持つRECT構造体
 */

#define GFL_RECT_X x
#define GFL_RECT_Y y
#define GFL_RECT_W w
#define GFL_RECT_H h

template<typename T>
struct RectStr {
	// -------------------------------------------------------------------------
	// Element
	// -------------------------------------------------------------------------
protected:

	union {
		T _Array[RXYWH];

		struct {
			T GFL_RECT_X;
			T GFL_RECT_Y;
			T GFL_RECT_W;
			T GFL_RECT_H;
		};
	};
};

/**
 *  ４つの要素を持つレクトクラス
 */

template<typename T>
class Rect : public RectStr<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Rect( void ) {
	}

	/**
	 *   Ｘ，Ｙ，Ｗ, Ｈ成分を設定するコンスタラクター
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  w 設定値
	 * @param  h 設定値
	 * @return 無し
	 */

	Rect( const T x, const T y, const T w, const T h ) {
		Set( x, y, w, h );
	}

	/**
	 *   Tuple3 を設定するコンスタラクター
	 * @param  t Tuple3クラス
	 * @return 無し
	 */

	Rect( const Rect& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T GetX( void ) const {
		return GFL_TP GFL_RECT_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T GetY( void ) const {
		return GFL_TP GFL_RECT_Y;
	}

	/**
	 *   Ｗ成分を取得
	 * @param  無し
	 * @return Ｗ成分
	 */

	inline T GetW( void ) const {
		return GFL_TP GFL_RECT_W;
	}

	/**
	 *   Ｈ成分を取得
	 * @param  無し
	 * @return Ｈ成分
	 */

	inline T GetH( void ) const {
		return GFL_TP GFL_RECT_H;
	}

	/**
	 *   Ｘ成分を取得
	 * @param  無し
	 * @return Ｘ成分
	 */

	inline T& GetRefX( void ) {
		return GFL_TP GFL_RECT_X;
	}

	/**
	 *   Ｙ成分を取得
	 * @param  無し
	 * @return Ｙ成分
	 */

	inline T& GetRefY( void ) {
		return GFL_TP GFL_RECT_Y;
	}

	/**
	 *   Ｗ成分を取得
	 * @param  無し
	 * @return Ｗ成分
	 */

	inline T& GetRefW( void ) {
		return GFL_TP GFL_RECT_W;
	}

	/**
	 *   Ｈ成分を取得
	 * @param  無し
	 * @return Ｈ成分
	 */

	inline T& GetRefH( void ) {
		return GFL_TP GFL_RECT_H;
	}

	/**
	 *   指定された成分を取得
	 * @param  i 添え字 RX(0) 〜 RH(3)
	 * @return 指定された成分
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( RX <= i && i <= RH, "Error !! oprator [%d] must be RX or more than RX or RH or less than RH.\n", i );
		return GFL_TP _Array[i];
	}

	/**
	 *   変数のポインターを取得
	 * @param  無し
	 * @return 変数のポインター
	 */

	inline T* Get( void ) {
		return GFL_TP _Array;
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   Ｘ成分を設定
	 * @param  x 設定値
	 * @return 無し
	 */

	inline void SetX( const T x ) {
		GFL_TP GFL_RECT_X = x;
	}

	/**
	 *   Ｙ成分を設定
	 * @param  y 設定値
	 * @return 無し
	 */

	inline void SetY( const T y ) {
		GFL_TP GFL_RECT_Y = y;
	}

	/**
	 *   Ｗ成分を設定
	 * @param  w 設定値
	 * @return 無し
	 */

	inline void SetW( const T w ) {
		GFL_TP GFL_RECT_W = w;
	}

	/**
	 *   Ｈ成分を設定
	 * @param  h 設定値
	 * @return 無し
	 */

	inline void SetH( const T h ) {
		GFL_TP GFL_RECT_H = h;
	}

	/**
	 *   指定した成分を設定
	 * @param  i 添え字 RX(0) 〜 RH(3)
	 * @param  val 設定値
	 * @return 無し
	 * @remark 有効範囲外の添え字だとアサートする。
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( Math::RX <= i && i <= Math::RH, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		if( i <= RH ){
			Get( i ) = val;
		}
	}

	/**
	 *   Ｘ，Ｙ，Ｗ, Ｈ成分を設定
	 * @param  x 設定値
	 * @param  y 設定値
	 * @param  w 設定値
	 * @param  h 設定値
	 * @return 無し
	 */

	inline void Set( const T x, const T y, const T w, const T h ) {
		GFL_TP GFL_RECT_X = x;
		GFL_TP GFL_RECT_Y = y;
		GFL_TP GFL_RECT_W = w;
		GFL_TP GFL_RECT_H = h;
	}

	/**
	 *   Rect を設定
	 * @param  t 設定したい Rect の参照
	 * @return 無し
	 */

	inline void Set( const Rect& t ) {
		GFL_TP GFL_RECT_X = t.GFL_RECT_X;
		GFL_TP GFL_RECT_Y = t.GFL_RECT_Y;
		GFL_TP GFL_RECT_W = t.GFL_RECT_W;
		GFL_TP GFL_RECT_H = t.GFL_RECT_H;
	}

	/**
	 *   各成分に０を設定
	 * @param  無し
	 * @return 無し
	 */

	inline void SetZero( void ) {
		GFL_TP GFL_RECT_X = GFL_TP GFL_RECT_Y = GFL_TP GFL_RECT_W = GFL_TP GFL_RECT_H = 0.0f;
	}

	/**
	 *   左上の座標を設定
	 * @param  x X座標
	 * @param  y Y座標
	 * @return 無し
	 */

	inline void SetPosition( const T x, const T y ) {
		GFL_TP GFL_RECT_X = x;
		GFL_TP GFL_RECT_Y = y;
	}

	/**
	 *   サイズを設定
	 * @param  xsize Xサイズ
	 * @param  ysize Yサイズ
	 * @return 無し
	 */

	inline void SetSize( const T xsize, const T ysize ) {
		GFL_TP GFL_RECT_W = xsize;
		GFL_TP GFL_RECT_H = ysize;
	}

	// -------------------------------------------------------------------------
	// Check
	// -------------------------------------------------------------------------
	/**
	 *   与えた座標が Rect 内にあるか？
	 * @param  x X座標
	 * @param  y Y座標
	 * @return 真　座標が Rect 内　偽　座標が Rect 外
	 */

	//		inline bool IsRange( const T x, const T y ) const {
	//			return (GFL_TP GFL_RECT_X < x && GFL_TP GFL_RECT_Y < y && x < (GFL_TP GFL_RECT_X + GFL_TP GFL_RECT_W) && y < (GFL_TP GFL_RECT_Y + GFL_TP GFL_RECT_H) );
	//		}

};

}
}

#endif	/* _GFL_RECT_H_ */
