/* 
 * File:   gfl_Tuple.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 13:18
 */

#ifndef GFL_TUPLE_H
#define	GFL_TUPLE_H
#pragma once

//#if defined(GFL_PLATFORM_3DS) // 3DS �ɂ͂Ȃ��c
//#include <arm_neon.h>
//#endif

namespace gfl {
namespace core {

// =============================================================================
// ���̃t�@�C���Ɋ܂܂��N���X�Q
// =============================================================================
template<typename T> struct TupleStr2;
template<typename T> class Tuple2;
class Tuple2f;

template<typename T> struct TupleStr3;
template<typename T> class Tuple3;
class Tuple3f;

template<typename T> struct TupleStr4;
template<typename T> class Tuple4;
class Tuple4f;

#ifdef GFL_PLATFORM_WII
typedef Tuple3f Tuple;
#else
/**
 *  �f�t�H���g�̃^�v���^
 */
typedef Tuple4f Tuple;
#endif

/**
 *  �Q�̗v�f�����^�v���\����
 */

template<typename T>
struct TupleStr2 {

	union {
		T _Array[TXY];

		struct {
			T _x;
			T _y;
		};
	};
};

/**
 *  �Q�̗v�f�����^�v���e���v���[�g�N���X
 */

template<typename T>
class Tuple2 : public TupleStr2<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @return ����
	 */

	Tuple2( const T x, const T y ) {
		Set( x, y );
	}

	/**
	 *   Tuple2 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple2�N���X
	 * @return ����
	 */

	template<typename S>
	Tuple2( const S& t ) {
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
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T& GetRef( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
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
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   �x������ݒ�
	 * @param  y �ݒ�l
	 * @return ����
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) �` TY(2)
	 * @param  val �ݒ�l
	 * @return ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   �w�C�x������ݒ�
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @return ����
	 */

	inline void Set( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   Tuple2 ��ݒ�
	 * @param  t �ݒ肵���� Tuple �̎Q��
	 * @return ����
	 */

	template<typename S>
	inline void Set( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = static_cast<T> (0);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S>
	inline void Add( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S, typename R>
	inline void Add( const S& t, const R& s ) {
		SetX( t._x + s._x );
		SetY( t._y + s._y );
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @return ����
	 */

	inline void Add( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @return ����
	 */

	template<typename S>
	inline void Add( const S& t, const T x, const T y ) {
		SetX( t._x + x );
		SetY( t._y + y );
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S>
	inline void Sub( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S, typename R>
	inline void Sub( const S& t, const R& s ) {
		SetX( t._x - s._x );
		SetY( t._y - s._y );
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @return ����
	 */

	inline void Sub( const T x, const T y ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @return ����
	 */

	template<typename S>
	inline void Sub( const S& t, const T x, const T y ) {
		SetX( t._x - x );
		SetY( t._y - y );
	}

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �Ɗe��������Z����
	 */

	inline void Mul( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	template<typename S>
	inline void Mul( const T f, const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	template<typename S>
	inline void Mul( const S& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	template<typename S>
	inline void Mul( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x *= t._x;
		GFL_TEMPLATE_PARAMETER _y *= t._y;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	template<typename S, typename R>
	inline void Mul( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x * s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y * s._y;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline f32 Magnitude2( void ) const {
		return (f32) (GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y);
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   �傫���̋t��
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Magnitude( ) );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */
	inline f32 Distance2( const Tuple2<T>& t ) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r.Magnitude2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline f32 Distance( const S& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  ����
	 * @return ����
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Neg( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	template<typename S>
	inline T Dot( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y;
	}

	// -------------------------------------------------------------------------
	// Cross �񎟌��ɊO�ς͂���܂���
	// -------------------------------------------------------------------------

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  s �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	template<typename S, typename R>
	inline void Lerp( const S& t, const R& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER _x = t._x * rate_1 + s._x * rate;
		GFL_TEMPLATE_PARAMETER _y = t._y * rate_1 + s._y * rate;
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	template<typename S>
	inline void Lerp( const S& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ []
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TY, "Error !! oprator [] must be TX or more than TX or TY or less than TY.\n" );
		return Get( i );
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� Tuple2<T>
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple2<T> & operator=(const S& t) {
		Set( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple2<T> & operator+=(const S& t) {
		Add( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple2<T> & operator-=(const S& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple2<T> & operator*=(const T f) {
		Mul( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple2<T> & operator*=(const S& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	template<typename S>
	inline bool operator==(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	template<typename S>
	inline bool operator!=(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y;
	}

	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator+(const S& t) const {
		Tuple2<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator-(const S& t) const {
		Tuple2<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  f �ώZ�������l
	 * @return �ώZ���ꂽ�N���X Tuple2<T>
	 */

	inline Tuple2<T> operator*(const T f) const {
		Tuple2<T> r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X Tuple2<T>
	 */

	template<typename S>
	inline Tuple2<T> operator*(const S& t) const {
		Tuple2<T> r;
		r.Mul( *this, t );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y );
	}
};

/**
 *  �Q�̗v�f�����^�v���N���X
 */

class Tuple2f : public Tuple2<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple2f( void ) {
	}

	/**
	 *   �w�C�x������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @return ����
	 */

	Tuple2f( const f32 x, const f32 y ) : Tuple2<f32>::Tuple2( x, y ) {
	}

	/**
	 *   Tuple2f ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple2f�N���X
	 * @return ����
	 */

	template<typename S>
	Tuple2f( const S& t ) : Tuple2<f32>::Tuple2( t ) {
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   �t���@�w�C�x�����̋t����ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Recip( void ) {
		SetX( Math::FRecip( _x ) );
		SetY( Math::FRecip( _y ) );
	}

	/**
	 *   �t���@�s�̂w�C�x�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Recip( const S& t ) {
		SetX( Math::FRecip( t._x ) );
		SetY( Math::FRecip( t._y ) );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark �e������ f �ŏ��Z����
	 */

	inline void Div( const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark t �̊e������ f �ŏ��Z���� �����I�ɂ͋t���̊|���Z
	 */

	template<typename S>
	inline void Div( const S& t, const f32 f ) {
		f32 f_1 = Math::FRecip( f );
		SetX( f_1 * t._x );
		SetY( f_1 * t._y );
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  ����
	 * @return ����
	 */

	inline void Normalize( void ) {
		Mul( MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Normalize( const S& t ) {
		Mul( t, MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  ����
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( void ) {
		f32 ret = Magnitude( );
		Mul( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ���̑傫��
	 */

	template<typename S>
	inline f32 NormalizeRet( const S& t ) {
		f32 ret = Magnitude( );
		Mul( t, Math::FRecip( ret ) );
		return ret;
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   �e���������Ă��邩�H
	 * @param  t �ΏۃN���X
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	template<typename S>
	inline bool IsSimilar( const S& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::FAbs( t._x - _x ) <= epsilon && Math::FAbs( t._y - _y ) <= epsilon;
	}

	// -------------------------------------------------------------------------
	// Absolute
	// -------------------------------------------------------------------------

	/**
	 *   �e�����̐�Βl��ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Abs( void ) {
		SetX( Math::FAbs( _x ) );
		SetY( Math::FAbs( _y ) );
	}

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ���̓N���X
	 * @return ����
	 */

	template<typename S>
	inline void Abs( const S& t ) {
		SetX( Math::FAbs( t._x ) );
		SetY( Math::FAbs( t._y ) );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple2<f32>::operator=;
	using Tuple2<f32>::operator+;
	using Tuple2<f32>::operator+=;
	using Tuple2<f32>::operator-;
	using Tuple2<f32>::operator-=;
	using Tuple2<f32>::operator*;
	using Tuple2<f32>::operator*=;

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple2f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Tuple2f
	 */

	inline Tuple2f operator/(const f32 f) const {
		Tuple2f r;
		r.Div( *this, f );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %f %f\n", str, _x, _y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %f %f\n", str, i, _x, _y );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %f %f\n", str, i, j, _x, _y );
	}
};

/**
 *  �R�̗v�f�����^�v���\����
 */

template<typename T>
struct TupleStr3 {
	// -------------------------------------------------------------------------
	// Element
	// -------------------------------------------------------------------------

	union {
		T _Array[TXYZ];

		struct {
			T mX;
			T mY;
			T mZ;
		};

		struct {
			T _x;
			T _y;
			T _z;
		};
	};
};

/**
 *  �R�̗v�f�����^�v���e���v���[�g�N���X
 */

template<typename T>
class Tuple3 : public TupleStr3<T> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple3( void ) {
	}

	/**
	 *   �w�C�x�C�y������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @return ����
	 */

	Tuple3( const T x, const T y, const T z ) {
		Set( x, y, z );
	}

	/**
	 *   Tuple3 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple3�N���X
	 * @return ����
	 */

	Tuple3( const T* t ) {
		Set( t );
	}

	/**
	 *   Tuple3 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple3�N���X
	 * @return ����
	 */

	Tuple3( T* t ) {
		Set( t );
	}

	/**
	 *   Tuple3 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple3�N���X
	 * @return ����
	 */

	template<typename S>
	Tuple3( const S& t ) {
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
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �y�������擾
	 * @param  ����
	 * @return �y����
	 */

	inline T GetZ( void ) const {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Z�������擾
	 * @param  ����
	 * @return Z����
	 */

	inline T& GetRefZ( void ) {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T& GetRef( const s32 i ) {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
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
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   �x������ݒ�
	 * @param  y �ݒ�l
	 * @return ����
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   �y������ݒ�
	 * @param  z �ݒ�l
	 * @return ����
	 */

	inline void SetZ( const T z ) {
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   �v������ݒ肷��ӂ������_�~�[�֐�
	 * @param  T �ݒ�l
	 * @return ����
	 */

	inline void SetW( const T ) {
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @param  val �ݒ�l
	 * @return ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   �w�C�x�C�y������ݒ�
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @return ����
	 */

	inline void Set( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   �w�C�x�C�y������ݒ�@�v�̓_�~�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @param  w �_�~�[�l
	 * @return ����
	 */

	inline void Set( const T x, const T y, const T z, const T w ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_AVOID_WARNING( w );
	}

	/**
	 *   Tuple2 ��ݒ�
	 * @param  t �ݒ肵���� Tuple2 �̎Q��
	 * @param  z �y�����̒l �w�肵�Ȃ��ꍇ�� 0 ������
	 * @return ����
	 */

	inline void Set( const Tuple2<T>& t, const T z = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   Tuple3 ��ݒ�
	 * @param  t �ݒ肵���� Tuple3 �̎Q��
	 * @return ����
	 */

	inline void Set( const f32* val ) {
		GFL_TEMPLATE_PARAMETER _x = val[TX];
		GFL_TEMPLATE_PARAMETER _y = val[TY];
		GFL_TEMPLATE_PARAMETER _z = val[TZ];
	}

	/**
	 *   Tuple3 ��ݒ�
	 * @param  t �ݒ肵���� Tuple3 �̎Q��
	 * @return ����
	 */

	inline void Set( f32* val ) {
		GFL_TEMPLATE_PARAMETER _x = val[TX];
		GFL_TEMPLATE_PARAMETER _y = val[TY];
		GFL_TEMPLATE_PARAMETER _z = val[TZ];
	}

	/**
	 *   Tuple3 ��ݒ�
	 * @param  t �ݒ肵���� Tuple3 �̎Q��
	 * @return ����
	 */

	template<typename S>
	inline void Set( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 * @remark Tuple3 �̏ꍇ�� SetZero �Ɠ���
	 */

	inline void SetUnit( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y = GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S>
	inline void Add( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S, typename R>
	inline void Add( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y + s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z + s._z;
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @return ����
	 */

	inline void Add( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
		GFL_TEMPLATE_PARAMETER _z += z;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @return ����
	 */

	template<typename S>
	inline void Add( const S& t, const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + x;
		GFL_TEMPLATE_PARAMETER _y = t._y + y;
		GFL_TEMPLATE_PARAMETER _z = t._z + z;
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S>
	inline void Sub( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
		GFL_TEMPLATE_PARAMETER _z -= t._z;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	template<typename S, typename R>
	inline void Sub( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y - s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z - s._z;
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @return ����
	 */

	inline void Sub( const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
		GFL_TEMPLATE_PARAMETER _z -= z;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @return ����
	 */

	template<typename S>
	inline void Sub( const S& t, const T x, const T y, const T z ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - x;
		GFL_TEMPLATE_PARAMETER _y = t._y - y;
		GFL_TEMPLATE_PARAMETER _z = t._z - z;
	}

	// -------------------------------------------------------------------------
	// Mul
	// -------------------------------------------------------------------------

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �Ɗe��������Z����
	 */

	inline void Mul( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
		GFL_TEMPLATE_PARAMETER _z *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	template<typename S>
	inline void Mul( const T f, const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	template<typename S>
	inline void Mul( const S& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	template<typename S>
	inline void Mul( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x *= t._x;
		GFL_TEMPLATE_PARAMETER _y *= t._y;
		GFL_TEMPLATE_PARAMETER _z *= t._z;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  s ��Z�������N���X
	 * @return ����
	 * @remark �e�������m����Z����
	 */

	template<typename S, typename R>
	inline void Mul( const S& t, const R& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x * s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y * s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z * s._z;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   �傫���̓�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline T MagnitudeXYZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �傫�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 MagnitudeXYZ( void ) const {
		return Math::FSqrt( MagnitudeXYZ2( ) );
	}

	/**
	 *   �傫���̋t�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeXYZRecip( void ) const {
		return Math::FRecip( MagnitudeXYZ( ) );
	}

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline T Magnitude2( void ) const {
		return MagnitudeXYZ2( );
	}

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline T MagnitudeXZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	inline T MagnitudeXY2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   �傫���̋t��
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeRecip( void ) const {
		const f32 len = Magnitude( );
		return (len != 0.0f) ? Math::FRecip( len ) : 0.0f;
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	template<typename S>
	inline T Distance2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.Magnitude2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline f32 Distance( const S& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	template<typename S>
	inline T DistanceXZ2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXZ2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline f32 DistanceXZ( const S& t ) const {
		return Math::FSqrt( DistanceXZ2( t ) );
	}

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	template<typename S>
	inline T DistanceXY2( const S& t ) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXY2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline f32 DistanceXY( const S& t ) const {
		return Math::FSqrt( DistanceXY2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  ����
	 * @return ����
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
		GFL_TEMPLATE_PARAMETER _z = -GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Neg( const S& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
		GFL_TEMPLATE_PARAMETER _z = -t._z;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	template<typename S>
	inline T Dot( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   ����XZ
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	template<typename S>
	inline T DotXZ( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	// -------------------------------------------------------------------------
	// Cross
	// -------------------------------------------------------------------------

	/**
	 *   �O��
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline void Cross( const S& s ) {
		T x = GFL_TEMPLATE_PARAMETER _x;
		T y = GFL_TEMPLATE_PARAMETER _y;
		T z = GFL_TEMPLATE_PARAMETER _z;

		GFL_TEMPLATE_PARAMETER _x = y * s._z - z * s._y;
		GFL_TEMPLATE_PARAMETER _y = z * s._x - x * s._z;
		GFL_TEMPLATE_PARAMETER _z = x * s._y - y * s._x;
	}

	/**
	 *   �O��
	 * @param  t �O�ς����߂����N���X
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	//				 * @remark t, s �� this �ƈႤ�N���X�łȂ��ƃA�T�[�g���܂�
	 */

	template<typename S, typename R>
	inline void Cross( const S& t, const R& s ) {
		GFL_MATH_ASSERT( this != &t && this != &s, "Error !! Cross class Should be different.\n" );
		GFL_TEMPLATE_PARAMETER _x = t._y * s._z - t._z * s._y;
		GFL_TEMPLATE_PARAMETER _y = t._z * s._x - t._x * s._z;
		GFL_TEMPLATE_PARAMETER _z = t._x * s._y - t._y * s._x;
	}

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  s �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	template<typename S, typename R>
	inline void Lerp( const S& t, const R& s, const f32 rate ) {
		const f32 rate_1 = 1.0f - rate;
		GFL_TEMPLATE_PARAMETER _x = t._x * rate_1 + s._x * rate;
		GFL_TEMPLATE_PARAMETER _y = t._y * rate_1 + s._y * rate;
		GFL_TEMPLATE_PARAMETER _z = t._z * rate_1 + s._z * rate;
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l (0.0f-1.0f)�͈̔͂ɂȂ��ƃA�T�[�g���܂��B
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	template<typename S>
	inline void Lerp( const S& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ []
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [] must be TX or more than TX or TZ or less than TZ.\n" );
		return Get( i );
	}

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ��������� Tuple
	 * @return this �̎Q�� �y������ 0 ������
	 */

	inline Tuple3<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple3<T> & operator=(const Tuple3<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple3<T> & operator=(const Tuple4<T>& t) {
		Set( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple3<T> & operator+=(const S& t) {
		Add( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple3<T> & operator-=(const S& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple3<T> & operator*=(const T f) {
		Mul( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple3<T> & operator*=(const S& t) {
		Mul( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ^=
	 * @param  t �O�ς����߂����N���X
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple3<T> & operator^=(const S& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	template<typename S>
	inline bool operator==(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y && GFL_TEMPLATE_PARAMETER _z == t._z;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	template<typename S>
	inline bool operator!=(const S& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y || GFL_TEMPLATE_PARAMETER _z != t._z;
	}

	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator+(const S& t) const {
		Tuple3<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator-(const S& t) const {
		Tuple3<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  f �ώZ�������l
	 * @return �ώZ���ꂽ�N���X Tuple3<T>
	 */

	inline Tuple3<T> operator*(const T f) const {
		Tuple3<T> r;
		r.Mul( *this, f );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  t �ώZ�������N���X
	 * @return �ώZ���ꂽ�N���X Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator*(const S& t) const {
		Tuple3<T> r;
		r.Mul( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ ^
	 * @param  t �O�ς����߂����N���X
	 * @return �O�ς��ݒ肳�ꂽ�N���X Tuple3<T>
	 */

	template<typename S>
	inline Tuple3<T> operator^(const S& t) const {
		Tuple3<T> r;
		r.Cross( *this, t );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s %d %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] %d %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] %d %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z );
	}
};

/**
 *  �R�̗v�f�����^�v���N���X
 */

class Tuple3f : public Tuple3<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple3f( void ) {
	}

	/**
	 *   �w�C�x�C�y������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @return ����
	 */

	Tuple3f( const f32 x, const f32 y, const f32 z ) : Tuple3<f32>::Tuple3( x, y, z ) {
	}

	/**
	 *   Tuple3f ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple3f�N���X
	 * @return ����
	 */

	template<typename S>
	Tuple3f( const S& t ) : Tuple3<f32>::Tuple3( t ) {
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   �t���@�w�C�x�C�y�����̋t����ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Recip( void ) {
		_x = Math::FRecip( _x );
		_y = Math::FRecip( _y );
		_z = Math::FRecip( _z );
	}

	/**
	 *   �t���@�s�̂w�C�x�C�y�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Recip( const S& t ) {
		_x = Math::FRecip( t._x );
		_y = Math::FRecip( t._y );
		_z = Math::FRecip( t._z );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark �e������ f �ŏ��Z����
	 */

	inline void Div( const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark t �̊e������ f �ŏ��Z����
	 */

	template<typename S>
	inline void Div( const S& t, const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  ����
	 * @return ����
	 */

	inline void Normalize( void ) {
		Mul( MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Normalize( const S& t ) {
		Mul( t, t.MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  ����
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( void ) {
		const f32 ret = Magnitude( );
		Mul( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ���̑傫��
	 */

	template<typename S>
	inline f32 NormalizeRet( const S& t ) {
		const f32 ret = t.Magnitude( );
		Mul( t, Math::FRecip( ret ) );
		return ret;
	}

	void RotateY( const f32 s, const f32 c ) {
		const f32 x = _x;
		const f32 z = _z;
		_x = gfl::core::Math::GetX_RotateY_SinCosXZ( s, c, x, z );
		_z = gfl::core::Math::GetZ_RotateY_SinCosXZ( s, c, x, z );
	}

	void RotateY( const f32 ang ) {
		f32 s, c;
		gfl::core::Math::FSinCos( &s, &c, ang );
		RotateY( s, c );
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   �e���������Ă��邩�H
	 * @param  t �ΏۃN���X
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	template<typename T>
	inline bool IsSimilar( const T& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( t._x, _x, epsilon ) &&
			Math::IsSimilar( t._y, _y, epsilon ) &&
			Math::IsSimilar( t._z, _z, epsilon );
	}

	inline bool IsSimilar( const f32 x, const f32 y, const f32 z, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( x, _x, epsilon ) &&
			Math::IsSimilar( y, _y, epsilon ) &&
			Math::IsSimilar( z, _z, epsilon );
	}

	inline bool IsAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( 0.0f, _x, epsilon ) &&
			Math::IsSimilar( 0.0f, _y, epsilon ) &&
			Math::IsSimilar( 0.0f, _z, epsilon );
	}

	inline bool IsNotAlmostZero( const f32 epsilon = Math::EPSILON ) const {
		return !Math::IsSimilar( 0.0f, _x, epsilon ) ||
			!Math::IsSimilar( 0.0f, _y, epsilon ) ||
			!Math::IsSimilar( 0.0f, _z, epsilon );
	}

	inline bool IsFrontDegree90( void ) {
		return gfl::core::Math::IsFrontDegree90( _x, _z );
	}

	// -------------------------------------------------------------------------
	// Absolute
	// -------------------------------------------------------------------------

	/**
	 *   �e�����̐�Βl��ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Abs( void ) {
		_x = Math::FAbs( _x );
		_y = Math::FAbs( _y );
		_z = Math::FAbs( _z );
	}

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ��Βl�����������N���X
	 * @return ����
	 */

	template<typename S>
	inline void Abs( const S& t ) {
		_x = Math::FAbs( t._x );
		_y = Math::FAbs( t._y );
		_z = Math::FAbs( t._z );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple3<f32>::operator=;
	using Tuple3<f32>::operator+;
	using Tuple3<f32>::operator+=;
	using Tuple3<f32>::operator-;
	using Tuple3<f32>::operator-=;
	using Tuple3<f32>::operator*;
	using Tuple3<f32>::operator*=;

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple3f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Tuple3f
	 */

	inline Tuple3f operator/(const f32 f) {
		Tuple3f r;
		r.Div( *this, f );
		return r;
	}

#ifdef GFL_PLATFORM_3DS

	void FromVEC3( const gfl::math::VEC3& src ) {
		Set( src.x, src.y, src.z );
	}

	void FromVEC4( const gfl::math::VEC4& src ) {
		Set( src.x, src.y, src.z );
	}

	void ToVEC3( gfl::math::VEC3* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}

	void ToVEC4( gfl::math::VEC4* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}
#endif
	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  ����
	 * @return ����
	 */

	inline void PrintConsole( void ) const {
		Debug::PrintConsole( " %ff, %ff, %ff\n", _x, _y, _z );
	}

	inline void PrintConsoleDegree( void ) const {
		Debug::PrintConsole( " %ff, %ff, %ff\n", gfl::core::Math::RadianToDegree( _x ), gfl::core::Math::RadianToDegree( _y ),
							 gfl::core::Math::RadianToDegree( _z ) );
	}

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s", str );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str ) const {
		Debug::PrintConsole( "%s", str );
		PrintConsoleDegree( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d]", str, i );
		PrintConsoleDegree( );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsole( );
	}

	inline void PrintConsoleDegree( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d]", str, i, j );
		PrintConsoleDegree( );
	}

	static const TupleStr3<f32> sZero;

	static const TupleStr3<f32>& GetZero( void ) {
		return sZero;
	}

};

/**
 *  �S�̗v�f�����^�v���\���́@�z��ɑ�����ď��������������Ȃ�
 */
#if defined(GFL_PLATFORM_WINDOWS)
#include <nmmintrin.h>   // MMX-SSE4.2���߃Z�b�g���g�p����ꍇ�C���N���[�h //
#include <smmintrin.h>   // MMX-SSE4.1���߃Z�b�g���g�p����ꍇ�C���N���[�h //
#include <intrin.h>      // MMX-SSE3���߃Z�b�g���g�p����ꍇ�C���N���[�h   //
#include <emmintrin.h>   // MMX-SSE2���߃Z�b�g���g�p����ꍇ�C���N���[�h   //
#include <xmmintrin.h>   // MMX-SSE���߃Z�b�g���g�p����ꍇ�C���N���[�h    //
#include <mmintrin.h>    // MMX���߃Z�b�g���g�p����ꍇ�C���N���[�h        //
#endif

template<typename T>
struct TupleStr4 {
	// -------------------------------------------------------------------------
	// Element
	// -------------------------------------------------------------------------

	union {
		//				f32 GFL_ATTR_ALIGN(_Array[TXYZW],16); /**< �x�N�^�[�̊e���� */
		T _Array[TXYZW]; /**< �x�N�^�[�̊e���� */

			struct {
			T mX;
			T mY;
			T mZ;
			T mW;
		};

		struct {
			T _x;
			T _y;
			T _z;
			T _w;
		};
#if defined(GFL_PLATFORM_3DS)
		//		float32x4_t mTuple4;
#elif defined(GFL_PLATFORM_WII)

		struct {
			u64 mXY;
			u64 mZW;
		};
#elif defined(GFL_PLATFORM_XENON)
		__vector4 mTuple4;
#elif defined(GFL_PLATFORM_PS3)
		vec_float4 mTuple4;
#elif defined(GFL_PLATFORM_WINDOWS)
		__m128 mTuple4;
#endif
	};
};

/**
 *  �S�̗v�f�����^�v���e���v���[�g�N���X
 */

template<typename T>
class Tuple4 : public TupleStr4<T> {
public:

	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple4( void ) {
	}

	/**
	 *   �w�C�x�C�y�C�v������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @param  w �ݒ�l �w�肵�Ȃ��ꍇ�� 0 ������
	 * @return ����
	 */

	Tuple4( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		Set( x, y, z, w );
	}

	/**
	 *   Tuple4 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple4�N���X
	 * @return ����
	 */

	template<typename S>
	Tuple4( const S& t ) {
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
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T GetY( void ) const {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   �y�������擾
	 * @param  ����
	 * @return �y����
	 */

	inline T GetZ( void ) const {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �v�������擾
	 * @param  ����
	 * @return �v����
	 */

	inline T GetW( void ) const {
		return GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TW(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T Get( const s32 i ) const {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   �w�������擾
	 * @param  ����
	 * @return �w����
	 */

	inline T& GetRefX( void ) {
		return GFL_TEMPLATE_PARAMETER _x;
	}

	/**
	 *   �x�������擾
	 * @param  ����
	 * @return �x����
	 */

	inline T& GetRefY( void ) {
		return GFL_TEMPLATE_PARAMETER _y;
	}

	/**
	 *   Z�������擾
	 * @param  ����
	 * @return Z����
	 */

	inline T& GetRefZ( void ) {
		return GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   W�������擾
	 * @param  ����
	 * @return W����
	 */

	inline T& GetRefW( void ) {
		return GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   �w�肳�ꂽ�������擾
	 * @param  i �Y���� TX(0) �` TZ(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T& GetRef( const s32 i ) {
		GFL_MATH_ASSERT( TX <= i && i <= TZ, "Error !! oprator [%d] must be TX or more than TX or TZ or less than TZ.\n", i );
		return GFL_TEMPLATE_PARAMETER _Array[i];
	}

	/**
	 *   �ϐ��̃|�C���^�[���擾
	 * @param  ����
	 * @return �ϐ��̃|�C���^�[
	 */

	inline T* Get( void ) {
		return GFL_TEMPLATE_PARAMETER _Array;
	}

	inline const T* Get( void ) const {
		return GFL_TEMPLATE_PARAMETER _Array;
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
		GFL_TEMPLATE_PARAMETER _x = x;
	}

	/**
	 *   �x������ݒ�
	 * @param  y �ݒ�l
	 * @return ����
	 */

	inline void SetY( const T y ) {
		GFL_TEMPLATE_PARAMETER _y = y;
	}

	/**
	 *   �y������ݒ�
	 * @param  z �ݒ�l
	 * @return ����
	 */

	inline void SetZ( const T z ) {
		GFL_TEMPLATE_PARAMETER _z = z;
	}

	/**
	 *   �v������ݒ�
	 * @param  w �ݒ�l
	 * @return ����
	 */

	inline void SetW( const T w ) {
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   �w�肵��������ݒ�
	 * @param  i �Y���� TX(0) �` TW(3)
	 * @param  val �ݒ�l
	 * @return ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline void SetElem( const s32 i, const T val ) {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [%d] must be TX or more than TX or TW or less than TW.\n", i );
		GFL_TEMPLATE_PARAMETER _Array[i] = val;
	}

	/**
	 *   �w�C�x�C�y�C�v������ݒ�
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @param  w �ݒ�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	inline void Set( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = x;
		GFL_TEMPLATE_PARAMETER _y = y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   ���R�Q�z���ݒ�
	 * @param  a �z��̎Q��
	 * @return ����
	 */

	inline void Set( const T& a ) {
		GFL_TEMPLATE_PARAMETER _x = (&a)[TX];
		GFL_TEMPLATE_PARAMETER _y = (&a)[TY];
		GFL_TEMPLATE_PARAMETER _z = (&a)[TZ];
		GFL_TEMPLATE_PARAMETER _w = (&a)[TW];
	}

	/**
	 *   Tuple2 ��ݒ�
	 * @param  t �ݒ肵���� Tuple2 �̎Q��
	 * @param  z �y�����̒l �w�肵�Ȃ��ꍇ�� 0 ������
	 * @param  w �v�����̒l �w�肵�Ȃ��ꍇ�� 0 ������
	 * @return ����
	 */

	inline void Set( const Tuple2<T>& t, const T z = static_cast<T> (0), const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   Tuple3 ��ݒ�
	 * @param  t �ݒ肵���� Tuple3 �̎Q��
	 * @param  w �v�����̒l �w�肵�Ȃ��ꍇ�� 0 ������
	 * @return ����
	 */

	inline void Set( const Tuple3<T>& t, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
		GFL_TEMPLATE_PARAMETER _w = w;
	}

	/**
	 *   Tuple4 ��ݒ�
	 * @param  t �ݒ肵���� Tuple4 �̎Q��
	 * @return ����
	 */

	template<typename S>
	inline void Set( const S& t ) {
#if 0 // def GFL_PLATFORM_WINDOWS
		mTuple4 = _mm_load_ps( (T*) & t );
#else
		GFL_TEMPLATE_PARAMETER _x = t._x;
		GFL_TEMPLATE_PARAMETER _y = t._y;
		GFL_TEMPLATE_PARAMETER _z = t._z;
		GFL_TEMPLATE_PARAMETER _w = t._w;
#endif
	}

	/**
	 *   �e�����ɂO��ݒ�
	 * @param  ����
	 * @return ����
	 */

	inline void SetZero( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y =
			GFL_TEMPLATE_PARAMETER _z = GFL_TEMPLATE_PARAMETER _w = static_cast<T> (0);
	}

	/**
	 *   �w�C�x�C�y�����ɂO��ݒ肵�v������ 1 ��ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void SetUnit( void ) {
		GFL_TEMPLATE_PARAMETER _x = GFL_TEMPLATE_PARAMETER _y =
			GFL_TEMPLATE_PARAMETER _z = static_cast<T> (0);
		GFL_TEMPLATE_PARAMETER _w = static_cast<T> (1);
	}

	// -------------------------------------------------------------------------
	// Add
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const Tuple4<T>& t ) {
#if 0 // def GFL_PLATFORM_3DS
		mTuple4 = vaddq_f32( mTuple4, t.mTuple4 );
#else
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
		GFL_TEMPLATE_PARAMETER _w += t._w;
#endif
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const Tuple3<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x += t._x;
		GFL_TEMPLATE_PARAMETER _y += t._y;
		GFL_TEMPLATE_PARAMETER _z += t._z;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Add( const Tuple4<T>& t, const Tuple4<T>& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y + s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z + s._z;
		GFL_TEMPLATE_PARAMETER _w = t._w + s._w;
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @param  w ���Z�������v�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	inline void Add( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x += x;
		GFL_TEMPLATE_PARAMETER _y += y;
		GFL_TEMPLATE_PARAMETER _z += z;
		GFL_TEMPLATE_PARAMETER _w += w;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @param  w ���Z�������v�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	inline void Add( const Tuple4<T>& t, const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x + x;
		GFL_TEMPLATE_PARAMETER _y = t._y + y;
		GFL_TEMPLATE_PARAMETER _z = t._z + z;
		GFL_TEMPLATE_PARAMETER _w = t._w + w;
	}

	// -------------------------------------------------------------------------
	// Sub
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x -= t._x;
		GFL_TEMPLATE_PARAMETER _y -= t._y;
		GFL_TEMPLATE_PARAMETER _z -= t._z;
		GFL_TEMPLATE_PARAMETER _w -= t._w;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X�̎Q��
	 * @param  s ���Z�������N���X�̎Q��
	 * @return ����
	 * @remark �e�������m�����Z����
	 */

	inline void Sub( const Tuple4<T>& t, const Tuple4<T>& s ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - s._x;
		GFL_TEMPLATE_PARAMETER _y = t._y - s._y;
		GFL_TEMPLATE_PARAMETER _z = t._z - s._z;
		GFL_TEMPLATE_PARAMETER _w = t._w - s._w;
	}

	/**
	 *   ���Z
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @param  w ���Z�������v�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	inline void Sub( const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x -= x;
		GFL_TEMPLATE_PARAMETER _y -= y;
		GFL_TEMPLATE_PARAMETER _z -= z;
		GFL_TEMPLATE_PARAMETER _w -= w;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  x ���Z�������w�l
	 * @param  y ���Z�������x�l
	 * @param  z ���Z�������y�l
	 * @param  w ���Z�������v�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	inline void Sub( const Tuple4<T>& t, const T x, const T y, const T z, const T w = static_cast<T> (0) ) {
		GFL_TEMPLATE_PARAMETER _x = t._x - x;
		GFL_TEMPLATE_PARAMETER _y = t._y - y;
		GFL_TEMPLATE_PARAMETER _z = t._z - z;
		GFL_TEMPLATE_PARAMETER _w = t._w - w;
	}

	// -------------------------------------------------------------------------
	// Scale
	// -------------------------------------------------------------------------

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �Ɗe��������Z����B�l�����ɂ��Ă��Ȃ��̂͌p���ŉB�������̂�����邽�߁B
	 */

	inline void Scale( const T f ) {
		GFL_TEMPLATE_PARAMETER _x *= f;
		GFL_TEMPLATE_PARAMETER _y *= f;
		GFL_TEMPLATE_PARAMETER _z *= f;
		GFL_TEMPLATE_PARAMETER _w *= f;
	}

	/**
	 *   ��Z
	 * @param  f ��Z�������l
	 * @param  t ��Z�������N���X
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Scale( const T f, const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
		GFL_TEMPLATE_PARAMETER _w = f * t._w;
	}

	/**
	 *   ��Z
	 * @param  t ��Z�������N���X
	 * @param  f ��Z�������l
	 * @return ����
	 * @remark f �� t �̊e��������Z����
	 */

	inline void Scale( const Tuple4<T>& t, const T f ) {
		GFL_TEMPLATE_PARAMETER _x = f * t._x;
		GFL_TEMPLATE_PARAMETER _y = f * t._y;
		GFL_TEMPLATE_PARAMETER _z = f * t._z;
		GFL_TEMPLATE_PARAMETER _w = f * t._w;
	}

	// -------------------------------------------------------------------------
	// Magnitude
	// -------------------------------------------------------------------------

	/**
	 *   �傫���̓�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline T MagnitudeXYZ2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z;
	}

	/**
	 *   �傫�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 MagnitudeXYZ( void ) const {
		return Math::FSqrt( MagnitudeXYZ2( ) );
	}

	/**
	 *   �傫���̓��̋t�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫���̓��̋t��
	 */

	inline f32 MagnitudeXYZ2Recip( void ) const {
		return Math::FRecip( MagnitudeXYZ2( ) );
	}

	/**
	 *   �傫���̋t�� �w�x�y�݂̂łv�͌v�Z���Ȃ�
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeXYZRecip( void ) const {
		return Math::FRecip( MagnitudeXYZ( ) );
	}

	/**
	 *   �傫���̓��
	 * @param  ����
	 * @return �傫���̓��
	 */

	inline T Magnitude2( void ) const {
		return GFL_TEMPLATE_PARAMETER _x * GFL_TEMPLATE_PARAMETER _x + GFL_TEMPLATE_PARAMETER _y * GFL_TEMPLATE_PARAMETER _y
			+ GFL_TEMPLATE_PARAMETER _z * GFL_TEMPLATE_PARAMETER _z + GFL_TEMPLATE_PARAMETER _w * GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   �傫��
	 * @param  ����
	 * @return �傫��
	 */

	inline f32 Magnitude( void ) const {
		return Math::FSqrt( Magnitude2( ) );
	}

	/**
	 *   �傫���̓��̋t��
	 * @param  ����
	 * @return �傫���̓��̋t��
	 */

	inline f32 Magnitude2Recip( void ) const {
		return Math::FRecip( Magnitude2( ) );
	}

	/**
	 *   �傫���̋t��
	 * @param  ����
	 * @return �傫���̋t��
	 */

	inline f32 MagnitudeRecip( void ) const {
		return Math::FRecip( Magnitude( ) );
	}

	// -------------------------------------------------------------------------
	// Distance
	// -------------------------------------------------------------------------

	/**
	 *   �����̓��
	 * @param  t ���������߂����N���X
	 * @return �����̓��
	 */

	template <typename S>
	inline T Distance2( const S& t ) const {
		Tuple4<T> r;
		r.Sub( *this, t );
		return r.MagnitudeXYZ2( );
	}

	/**
	 *   ����
	 * @param  t ���������߂����N���X
	 * @return ����
	 */

	inline f32 Distance( const Tuple4f& t ) const {
		return Math::FSqrt( Distance2( t ) );
	}

	// -------------------------------------------------------------------------
	// Neg
	// -------------------------------------------------------------------------

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  ����
	 * @return ����
	 */

	inline void Neg( void ) {
		GFL_TEMPLATE_PARAMETER _x = -GFL_TEMPLATE_PARAMETER _x;
		GFL_TEMPLATE_PARAMETER _y = -GFL_TEMPLATE_PARAMETER _y;
		GFL_TEMPLATE_PARAMETER _z = -GFL_TEMPLATE_PARAMETER _z;
		GFL_TEMPLATE_PARAMETER _w = -GFL_TEMPLATE_PARAMETER _w;
	}

	/**
	 *   �l�Q�[�g�@�e�����̕����𔽓]������
	 * @param  t �l�Q�[�g�������N���X
	 * @return ����
	 */

	inline void Neg( const Tuple4<T>& t ) {
		GFL_TEMPLATE_PARAMETER _x = -t._x;
		GFL_TEMPLATE_PARAMETER _y = -t._y;
		GFL_TEMPLATE_PARAMETER _z = -t._z;
		GFL_TEMPLATE_PARAMETER _w = -t._w;
	}

	// -------------------------------------------------------------------------
	// Dot
	// -------------------------------------------------------------------------

	/**
	 *   �w�x�y�����̓���
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline T DotXYZ( const Tuple4<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   �w�y�����̓���
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	template<typename S>
	inline T DotXZ( const S& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _z * t._z;
	}

	/**
	 *   ����
	 * @param  t ���ς����߂����N���X
	 * @return ���ϒl
	 */

	inline T Dot( const Tuple4<T>& t ) const {
		return GFL_TEMPLATE_PARAMETER _x * t._x + GFL_TEMPLATE_PARAMETER _y * t._y + GFL_TEMPLATE_PARAMETER _z * t._z + GFL_TEMPLATE_PARAMETER _w * t._w;
	}

	// -------------------------------------------------------------------------
	// Cross
	// -------------------------------------------------------------------------

	/**
	 *   �O��
	 * @param  t �O�ς����߂����N���X
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	 */

	template<typename S, typename R>
	inline void Cross( const S& t, const R& s ) {
		Set(
			 t._y * s._z - t._z * s._y,
			 t._z * s._x - t._x * s._z,
			 t._x * s._y - t._y * s._x );
	}

	/**
	 *   �O��
	 * @param  s �O�ς����߂����N���X
	 * @return ����
	 */

	template<typename S>
	inline void Cross( const S& s ) {
		Cross( *this, s );
	}

	// -------------------------------------------------------------------------
	// Lerp
	// -------------------------------------------------------------------------

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  s �⊮�������N���X
	 * @param  rate �����l
	 * @return ����
	 * @remark this = t*rate + s*(1.0f-rate)
	 */

	inline void Lerp( const Tuple4<T>& t, const Tuple4<T>& s, const f32 rate ) {
		f32 rate_1 = 1.0f - rate;
		Set( t._x * rate_1 + s._x * rate,
			 t._y * rate_1 + s._y * rate,
			 t._z * rate_1 + s._z * rate,
			 t._w * rate_1 + s._w * rate );
	}

	/**
	 *   �����⊮
	 * @param  t �⊮�������N���X
	 * @param  rate �����l
	 * @return ����
	 * @remark this = this*rate + t*(1.0f-rate)
	 */

	inline void Lerp( const Tuple4<T>& t, const f32 rate ) {
		Lerp( *this, t, rate );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------

	/**
	 *   �I�y���[�^�[ []
	 * @param  i �Y���� TX(0) �` TW(3)
	 * @return �w�肳�ꂽ����
	 * @remark �L���͈͊O�̓Y�������ƃA�T�[�g����B
	 */

	inline T operator [](s32 i) const {
		GFL_MATH_ASSERT( TX <= i && i <= TW, "Error !! oprator [] must be TX or more than TX or TW or less than TW.\n" );
		return Get( i );
	}
#if 0 // ���ꂶ�Ⴗ�܂Ȃ������c

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ���������Tuple
	 * @return this �̎Q��
	 */

	template<typename S>
		inline Tuple4<T> & operator=(const S& t) {
		Set( t );
		return *this;
	}
#else

	/**
	 *   �I�y���[�^�[ =
	 * @param  t ���������Tuple
	 * @return this �̎Q��
	 */

	inline Tuple4<T> & operator=(const Tuple2<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple4<T> & operator=(const Tuple3<T>& t) {
		Set( t );
		return *this;
	}

	inline Tuple4<T> & operator=(const Tuple4<T>& t) {
		Set( t );
		return *this;
	}
#endif

	/**
	 *   �I�y���[�^�[ +
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple4<T>
	 */

	inline Tuple4<T> operator+(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Add( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ +=
	 * @param  t ���Z�������N���X
	 * @return this �̎Q��
	 */

	inline Tuple4<T> & operator+=(const Tuple4<T>& t) {
		Add( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ -
	 * @param  t ���Z�������N���X
	 * @return ���Z���ꂽ�N���X Tuple4<T>
	 */

	inline Tuple4<T> operator-(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Sub( *this, t );
		return r;
	}

	/**
	 *   �I�y���[�^�[ -=
	 * @param  t ��Z�������N���X
	 * @return this �̎Q��
	 */

	inline Tuple4<T> & operator-=(const Tuple4<T>& t) {
		Sub( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ *
	 * @param  f �ώZ�������l
	 * @return �ώZ���ꂽ�N���X Vector4
	 */

	inline Tuple4<T> operator*(const T f) const {
		Tuple4<T> r;
		r.Scale( *this, f );
		return r;
	}

	/**
	 *   �I�y���[�^�[ *=
	 * @param  f ��Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple4<T> & operator*=(const T f) {
		Scale( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ^=
	 * @param  t �O�ς����߂����N���X
	 * @return this �̎Q��
	 */

	inline Tuple4<T> & operator^=(const Tuple4<T>& t) {
		Cross( t );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ ==
	 * @param  t ��r�������N���X
	 * @return �e��������v�����Ƃ��^�A���Ȃ��Ƃ��ɋU
	 */

	inline bool operator==(const Tuple4<T>& t) const {
		return GFL_TEMPLATE_PARAMETER _x == t._x && GFL_TEMPLATE_PARAMETER _y == t._y && GFL_TEMPLATE_PARAMETER _z == t._z && GFL_TEMPLATE_PARAMETER _w == t._w;
	}

	/**
	 *   �I�y���[�^�[ !=
	 * @param  t ��r�������N���X
	 * @return �e�����̑S�Ă������Ƃ��ɋU�A����ȊO�͋U
	 */

	inline bool operator!=(const Tuple4<T>& t) const {
		return GFL_TEMPLATE_PARAMETER _x != t._x || GFL_TEMPLATE_PARAMETER _y != t._y || GFL_TEMPLATE_PARAMETER _z != t._z || GFL_TEMPLATE_PARAMETER _w != t._w;
	}

	/**
	 *   �I�y���[�^�[ ^
	 * @param  t �O�ς����߂����N���X
	 * @return �O�ς��ݒ肳�ꂽ�N���X Tuple4<T>
	 */

	inline Tuple4<T> operator^(const Tuple4<T>& t) const {
		Tuple4<T> r;
		r.Cross( *this, t );
		return r;
	}

#ifdef GFL_PLATFORM_3DS

	void FromVEC3( const gfl::math::VEC3& src ) {
		Set( src.x, src.y, src.z );
	}

	void FromVEC4( const gfl::math::VEC4& src ) {
		Set( src.x, src.y, src.z, src.w );
	}

	void ToVEC3( gfl::math::VEC3* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
	}

	void ToVEC4( gfl::math::VEC4* dst ) const {
		dst->x = GetX( );
		dst->y = GetY( );
		dst->z = GetZ( );
		dst->w = GetW( );
	}
#endif
	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s = %d %d %d %d\n", str, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = %d %d %d %d\n", str, i, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = %d %d %d %d\n", str, i, j, GFL_TEMPLATE_PARAMETER _x, GFL_TEMPLATE_PARAMETER _y, GFL_TEMPLATE_PARAMETER _z, GFL_TEMPLATE_PARAMETER _w );
	}
};

/**
 *  �S�̗v�f�����^�v���N���X�@�u�����^�� �p�����^�̋��ʃC���^�[�t�F�C�X�N���X
 */

class Tuple4f : public Tuple4<f32> {
public:
	// -------------------------------------------------------------------------
	// Constructer
	// -------------------------------------------------------------------------

	Tuple4f( void ) {
	}

	/**
	 *   �w�C�x�C�y�C�v������ݒ肷��R���X�^���N�^�[
	 * @param  x �ݒ�l
	 * @param  y �ݒ�l
	 * @param  z �ݒ�l
	 * @param  w �ݒ�l �w�肵�Ȃ��ꍇ�� 0.0f ������
	 * @return ����
	 */

	Tuple4f( const f32 x, const f32 y, const f32 z, const f32 w = 0.0f ) : Tuple4<f32>::Tuple4( x, y, z, w ) {
	}

	/**
	 *   Tuple4 ��ݒ肷��R���X�^���N�^�[
	 * @param  t Tuple4�N���X
	 * @return ����
	 */
	//		Tuple4f(const Tuple4<T>& t) : Tuple4<f32>::Tuple4(t) {}

	template<typename S>
	Tuple4f( const S& t ) : Tuple4<f32>::Tuple4( t ) {
	}

	static const TupleStr4<f32>& GetZero( void ) {
		return sZero;
	}

	static const TupleStr4<f32>& GetX1( void ) {
		return sX1;
	}

	static const TupleStr4<f32>& GetY1( void ) {
		return sY1;
	}

	static const TupleStr4<f32>& GetZ1( void ) {
		return sZ1;
	}

	static const TupleStr4<f32>& GetW1( void ) {
		return sW1;
	}

	// -------------------------------------------------------------------------
	// Recip
	// -------------------------------------------------------------------------

	/**
	 *   �t���@�w�C�x�C�y�����̋t����ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void RecipXYZ( void ) {
		_x = Math::FRecip( _x );
		_y = Math::FRecip( _y );
		_z = Math::FRecip( _z );
	}

	/**
	 *   �t���@�s�̂w�C�x�C�y�����̋t����ݒ肷��
	 * @param  t �t���ɂ������N���X
	 * @return ����
	 */

	template<typename S>
	inline void RecipXYZ( const S& t ) {
		_x = Math::FRecip( t._x );
		_y = Math::FRecip( t._y );
		_z = Math::FRecip( t._z );
	}

	// -------------------------------------------------------------------------
	// Div
	// -------------------------------------------------------------------------

	/**
	 *   ���Z
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark �e������ f �ŏ��Z����
	 */

	inline void Div( const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
		_w *= f_1;
	}

	/**
	 *   ���Z
	 * @param  t ���Z�������N���X
	 * @param  f ���Z�������l
	 * @return ����
	 * @remark t �̊e������ f �ŏ��Z����
	 */

	inline void Div( const Tuple4f& t, const f32 f ) {
		const f32 f_1 = Math::FRecip( f );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
		_w = f_1 * t._w;
	}

	/**
	 *   �v�����łw�C�x�C�y���������Z
	 * @param  ����
	 * @return ����
	 */

	inline void DivByW( void ) {
		const f32 f_1 = Math::FRecip( _w );
		_x *= f_1;
		_y *= f_1;
		_z *= f_1;
		_w = 1.0f;
	}

	/**
	 *   �v�����łw�C�x�C�y���������Z
	 * @param  t ���Z�������N���X
	 * @return ����
	 */

	template<typename S>
	inline void DivByW( const S& t ) {
		const f32 f_1 = Math::FRecip( _w );
		_x = f_1 * t._x;
		_y = f_1 * t._y;
		_z = f_1 * t._z;
		_w = 1.0f;
	}

	// -------------------------------------------------------------------------
	// Normalize
	// -------------------------------------------------------------------------

	/**
	 *   ���K��
	 * @param  ����
	 * @return ����
	 */

	inline void Normalize( void ) {
		Scale( MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ����
	 */

	inline void Normalize( const Tuple4f& t ) {
		Scale( t, MagnitudeRecip( ) );
	}

	/**
	 *   ���K��
	 * @param  ����
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( void ) {
		const f32 ret = Magnitude( );
		Scale( Math::FRecip( ret ) );
		return ret;
	}

	/**
	 *   ���K��
	 * @param  t ���K���������N���X
	 * @return ���̑傫��
	 */

	inline f32 NormalizeRet( const Tuple4f& t ) {
		const f32 ret = t.Magnitude( );
		Scale( t, Math::FRecip( ret ) );
		return ret;
	}

	// -------------------------------------------------------------------------
	// IsSimilar
	// -------------------------------------------------------------------------

	/**
	 *   �덷�͈͂�^���Ĕ�r
	 * @param  t �ΏۃN���X
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �e�����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	inline bool IsSimilar( const Tuple4f& t, const f32 epsilon = Math::EPSILON ) const {
		return Math::IsSimilar( t._x, _x, epsilon ) &&
			Math::IsSimilar( t._y, _y, epsilon ) &&
			Math::IsSimilar( t._z, _z, epsilon ) &&
			Math::IsSimilar( t._w, _w, epsilon );
	}

	// -------------------------------------------------------------------------
	// Abs
	// -------------------------------------------------------------------------

	/**
	 *   t �̊e�����̐�Βl��ݒ肷��
	 * @param  t ��Βl�����������N���X
	 * @return ����
	 */

	inline void Abs( const Tuple4f& t ) {
		Set( Math::FAbs( t._x ),
			 Math::FAbs( t._y ),
			 Math::FAbs( t._z ),
			 Math::FAbs( t._w ) );
	}

	/**
	 *   �e�����̐�Βl��ݒ肷��
	 * @param  ����
	 * @return ����
	 */

	inline void Abs( void ) {
		Abs( *this );
	}

	// -------------------------------------------------------------------------
	// Operator
	// -------------------------------------------------------------------------
	using Tuple4<f32>::operator=;
	using Tuple4<f32>::operator+;
	using Tuple4<f32>::operator+=;
	using Tuple4<f32>::operator-;
	using Tuple4<f32>::operator-=;
	using Tuple4<f32>::operator*;
	using Tuple4<f32>::operator*=;

	/**
	 *   �I�y���[�^�[ /=
	 * @param  f ���Z�������l
	 * @return this �̎Q��
	 */

	inline Tuple4f & operator/=(const f32 f) {
		Div( f );
		return *this;
	}

	/**
	 *   �I�y���[�^�[ /
	 * @param  f ���Z�������l
	 * @return ���Z���ꂽ�N���X Tuple4f
	 */

	inline Tuple4f operator/(const f32 f) {
		Tuple4f r;
		r.Div( *this, f );
		return r;
	}

	// -------------------------------------------------------------------------
	// Print
	// -------------------------------------------------------------------------

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str ) const {
		Debug::PrintConsole( "%s = %ff, %ff, %ff, %ff\n", str, _x, _y, _z, _w );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i ) const {
		Debug::PrintConsole( "%s[%d] = %ff, %ff, %ff, %ff\n", str, i, _x, _y, _z, _w );
	}

	/**
	 *   �R���\�[���Ɋe�������o��
	 * @param  str ������
	 * @param  i �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @param  j �l�@�Y�����Ȃǂ�\���������Ƃ�
	 * @return ����
	 */

	inline void PrintConsole( const c8 * const str, const s32 i, const s32 j ) const {
		Debug::PrintConsole( "%s[%d][%d] = %ff, %ff, %ff, %ff\n", str, i, j, _x, _y, _z, _w );
	}

	// -------------------------------------------------------------------------
	// static const parameters
	// -------------------------------------------------------------------------
	static const TupleStr4<f32> sZero;
	static const TupleStr4<f32> sX1;
	static const TupleStr4<f32> sY1;
	static const TupleStr4<f32> sZ1;
	static const TupleStr4<f32> sW1;

};

}
}

#endif	/* GFL_TUPLE_H */

