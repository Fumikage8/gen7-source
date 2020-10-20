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
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
template<typename T> struct RectStr;
template<typename T> class Rect;

/**
 *  �S�̗v�f������RECT�\����
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
 *  �S�̗v�f�������N�g�N���X
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
	 *   �w�C�x�C�v, �g������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  w �ݒ�l
	 * @param  h �ݒ�l
	 * @return ����
	 */

	Rect( const T x, const T y, const T w, const T h ) {
		Set( x, y, w, h );
	}

	/**
	 *   Tuple3 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple3�N���X
	 * @return ����
	 */

	Rect( const Rect& t ) {
		Set( t );
	}

	// -------------------------------------------------------------------------
	// Get
	// -------------------------------------------------------------------------

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T GetX( void ) const {
		return GFL_TP GFL_RECT_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T GetY( void ) const {
		return GFL_TP GFL_RECT_Y;
	}

	/**
	 *   �v�������擾
	 * @param  ����
	 * @return �v����
	 */

	inline T GetW( void ) const {
		return GFL_TP GFL_RECT_W;
	}

	/**
	 *   �g�������擾
	 * @param  ����
	 * @return �g����
	 */

	inline T GetH( void ) const {
		return GFL_TP GFL_RECT_H;
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T& GetRefX( void ) {
		return GFL_TP GFL_RECT_X;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T& GetRefY( void ) {
		return GFL_TP GFL_RECT_Y;
	}

	/**
	 *   �v�������擾
	 * @param  ����
	 * @return �v����
	 */

	inline T& GetRefW( void ) {
		return GFL_TP GFL_RECT_W;
	}

	/**
	 *   �g�������擾
	 * @param  ����
	 * @return �g����
	 */

	inline T& GetRefH( void ) {
		return GFL_TP GFL_RECT_H;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� RX(0) �` RH(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( RX <= i && i <= RH, "Error !! oprator [%d] must be RX or more than RX or RH or less than RH.\n", i );
		return GFL_TP _Array[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline T* Get( void ) {
		return GFL_TP _Array;
	}

	// -------------------------------------------------------------------------
	// Set
	// -------------------------------------------------------------------------

	/**
	 *   �w������ݒ�
	 * @param  x �ݒ�l
	 * @return ����
	 */

	inline void SetX( const T x ) {
		GFL_TP GFL_RECT_X = x;
	}

	/**
	 *   �x������ݒ�
	 * @param  y �ݒ�l
	 * @return ����
	 */

	inline void SetY( const T y ) {
		GFL_TP GFL_RECT_Y = y;
	}

	/**
	 *   �v������ݒ�
	 * @param  w �ݒ�l
	 * @return ����
	 */

	inline void SetW( const T w ) {
		GFL_TP GFL_RECT_W = w;
	}

	/**
	 *   �g������ݒ�
	 * @param  h �ݒ�l
	 * @return ����
	 */

	inline void SetH( const T h ) {
		GFL_TP GFL_RECT_H = h;
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� RX(0) �` RH(3)
	 * @param  val �ݒ�l
	 * @return ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( Math::RX <= i && i <= Math::RH, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		if( i <= RH ){
			Get( i ) = val;
		}
	}

	/**
	 *   �w�C�x�C�v, �g������ݒ�
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  w �ݒ�l
	 * @param  h �ݒ�l
	 * @return ����
	 */

	inline void Set( const T x, const T y, const T w, const T h ) {
		GFL_TP GFL_RECT_X = x;
		GFL_TP GFL_RECT_Y = y;
		GFL_TP GFL_RECT_W = w;
		GFL_TP GFL_RECT_H = h;
	}

	/**
	 *   Rect ��ݒ�
	 * @param  t �ݒ肵���� Rect �̎Q��
	 * @return ����
	 */

	inline void Set( const Rect& t ) {
		GFL_TP GFL_RECT_X = t.GFL_RECT_X;
		GFL_TP GFL_RECT_Y = t.GFL_RECT_Y;
		GFL_TP GFL_RECT_W = t.GFL_RECT_W;
		GFL_TP GFL_RECT_H = t.GFL_RECT_H;
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		GFL_TP GFL_RECT_X = GFL_TP GFL_RECT_Y = GFL_TP GFL_RECT_W = GFL_TP GFL_RECT_H = 0.0f;
	}

	/**
	 *   ����̍��W��ݒ�
	 * @param  x X���W
	 * @param  y Y���W
	 * @return ����
	 */

	inline void SetPosition( const T x, const T y ) {
		GFL_TP GFL_RECT_X = x;
		GFL_TP GFL_RECT_Y = y;
	}

	/**
	 *   �T�C�Y��ݒ�
	 * @param  xsize X�T�C�Y
	 * @param  ysize Y�T�C�Y
	 * @return ����
	 */

	inline void SetSize( const T xsize, const T ysize ) {
		GFL_TP GFL_RECT_W = xsize;
		GFL_TP GFL_RECT_H = ysize;
	}

	// -------------------------------------------------------------------------
	// Check
	// -------------------------------------------------------------------------
	/**
	 *   �^�������W�� Rect ���ɂ��邩�H
	 * @param  x X���W
	 * @param  y Y���W
	 * @return �^�@���W�� Rect ���@�U�@���W�� Rect �O
	 */

	//		inline bool IsRange( const T x, const T y ) const {
	//			return (GFL_TP GFL_RECT_X < x && GFL_TP GFL_RECT_Y < y && x < (GFL_TP GFL_RECT_X + GFL_TP GFL_RECT_W) && y < (GFL_TP GFL_RECT_Y + GFL_TP GFL_RECT_H) );
	//		}

};

}
}

#endif	/* _GFL_RECT_H_ */
