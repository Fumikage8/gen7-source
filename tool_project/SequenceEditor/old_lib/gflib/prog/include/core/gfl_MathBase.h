/*
 * File:   gfl_BaseMath.h
 * Author: s-kimura
 *
 * Created on 2010/05/28, 11:29
 */

#ifndef GFL_BASEMATH_H
#define	GFL_BASEMATH_H

#ifdef GFL_PLATFORM_3DS
#include <nn/math.h>
#else
#include <math.h>
#endif
// �Z�p�A�T�[�g�́A�Ȃ�
#define GFL_MATH_ASSERT(...)

/**
 * �{���̓v���b�g�t�H�[���ʂ�����
 */
#define GFL_VECTOR_X x
#define GFL_VECTOR_Y y
#define GFL_VECTOR_Z z
#define GFL_VECTOR_W w

#define GFL_MTX_ARRAY1D a
#define GFL_MTX_ARRAY2D m
#define GFL_MTX_VECTOR  v
#define GFL_MTX_00  _00
#define GFL_MTX_01  _01
#define GFL_MTX_02  _02
#define GFL_MTX_03  _03
#define GFL_MTX_10  _10
#define GFL_MTX_11  _11
#define GFL_MTX_12  _12
#define GFL_MTX_13  _13
#define GFL_MTX_20  _20
#define GFL_MTX_21  _21
#define GFL_MTX_22  _22
#define GFL_MTX_23  _23
#define GFL_MTX_30  _30
#define GFL_MTX_31  _31
#define GFL_MTX_32  _32
#define GFL_MTX_33  _33

namespace gfl {
namespace core {

class Vector2;
class Vector3;
class Vector4;

#ifdef GFL_PLATFORM_3DS
typedef nn::math::VEC2_ VectorStruct2;
typedef nn::math::VEC3_ VectorStruct3;
typedef nn::math::VEC4_ VectorStruct4;
typedef nn::math::VEC2 VectorNative2;
typedef nn::math::VEC3 VectorNative3;
typedef nn::math::VEC4 VectorNative4;
#else
struct VectorStruct2;
struct VectorStruct3;
struct VectorStruct4;
class VectorNative2;
class VectorNative3;
class VectorNative4;
#endif

#if defined( GFL_PLATFORM_SONY )
typedef Vector4 Vector; /**< �f�t�H���g�x�N�g���^ */
typedef VectorNative4 VectorNative; /**< �f�t�H���g�l�C�e�B�u�x�N�g���^ */
#else
typedef Vector3 Vector; /**< �f�t�H���g�x�N�g���^ */
typedef VectorNative3 VectorNative; /**< �f�t�H���g�l�C�e�B�u�x�N�g���^ */
#endif

/**
 *  Tuple �z��̓Y����
 * @remark Math �̊O�ɏo���Ă���͕̂������Z�����邽��
 */

enum TupleArray {
	TX, /**< �w�̓Y���� */
	TY, /**< �x�̓Y���� */
	TZ, /**< �y�̓Y���� */
	TXY = TZ, /**< �w�x�̗v�f�� */
	TW, /**< �v�̓Y���� */
	TXYZ = TW, /**< �w�x�y�̗v�f�� */
	TXYZW /**< �w�x�y�v�̗v�f�� */
};

/**
 *  ���W�A�p�x�A�X�P�[��
 */

enum TypeArray {
	TRANS, /**< ���W�̓Y���� */
	ROT, /**< �p�x�̓Y���� */
	TRANS_ROT,
	SCALE = TRANS_ROT, /**< �X�P�[���̓Y���� */
	TRANS_ROT_SCALE /**< �R�̗v�f�� */
};

/**
 *  Rect �z��̓Y����
 */

enum RectArray {
	RX, /**< �w�̓Y���� */
	RY, /**< �x�̓Y���� */
	RW, /**< �v�̓Y���� */
	RH, /**< �g�̓Y���� */
	RXYWH /**< �w�x�v�g�̗v�f�� */
};

/**
 *  �Z�p�N���X
 */

class Math {
public:
	// ����
	static const u8 U8_MAX = 0xff; /**< @brief u8  �ōő�̒l */
	static const s8 S8_MAX = 0x7f; /**< @brief s8  �ōő�̒l */
	static const s8 S8_MIN = -S8_MAX - 1; /**< @brief s8  �ōŏ��̒l */
	static const u16 U16_MAX = 0xffff; /**< @brief u16 �ōő�̒l */
	static const s16 S16_MAX = 0x7fff; /**< @brief s16 �ōő�̒l */
	static const s16 S16_MIN = -S16_MAX - 1; /**< @brief s16 �ōŏ��̒l */
	static const u32 U32_MAX = 0xffffffff; /**< @brief u32 �ōő�̒l */
	static const s32 S32_MAX = 0x7fffffff; /**< @brief s32 �ōő�̒l */
	static const u64 U64_MAX = 0xffffffffffffffff; /**< @brief u64 �ōő�̒l */
	static const s64 S64_MAX = 0x7fffffffffffffff; /**< @brief s64 �ōő�̒l */
	static const s32 S32_MIN = -S32_MAX - 1; /**< @brief s32 �ōŏ��̒l */

#undef F32_MIN
#undef F32_MAX

	// ��������
	static const f32 F32_ZERO; /**< @brief f32 0.0f */
	static const f32 F32_MIN; /**< @brief f32 �ōŏ��̒l */
	static const f32 F32_MAX; /**< @brief f32 �ōő�̒l */
	static const f32 EPSILON; /**< @brief �قƂ�ǃ[���ɋ߂��l */

	// ���W�A��
	static const f32 ONE_PI; /**< @brief ���������_�̃p�C */
	static const f32 QUARTER_PI; /**< @brief ���������_�̃p�C/4 */
	static const f32 HALF_PI; /**< @brief ���������_�̃p�C/2 */
	static const f32 ONE_HALF_PI; /**< @brief ���������_�̃p�C*1.5f */
	static const f32 TWO_PI; /**< @brief ���������_�̂Q�p�C */

	// ���[�g
	static const f32 ROOT_2; /**< @brief ���[�g�Q */
	static const f32 ROOT_3; /**< @brief ���[�g�R */

	/**
	 *   �l�Ƀ��~�b�g��������
	 * @param  v ���~�b�g�����������l
	 * @param  v_min ����
	 * @param  v_max ���
	 * @return ����
	 */

	template<typename T, typename S>
	static inline void Clamp( T* v, const S v_min, const S v_max ) {
		if( v_max < *v ){
			*v = v_max;
		} else if( *v < v_min ){
			*v = v_min;
		}
	}

	/**
	 *   �}v_abs �Œl�Ƀ��~�b�g��������
	 * @param  v ���~�b�g�����������l
	 * @param  v_abs ���~�b�g�̐�Βl
	 * @return ����
	 * @remark Clamp(v,-v_abs,v_abs)
	 */

	template<typename T, typename S>
	static inline void ClampAbs( T* v, const S v_abs ) {
		Clamp( v, -v_abs, v_abs );
	}

	/**
	 *   �����_���̎��ݒ�
	 * @param  seed ��l
	 * @return ����
	 */

	static inline void RandomSeed( const s32 seed ) {
#ifdef GFL_PLATFORM_3DS
		GFL_AVOID_WARNING( seed );
#else
		srand( seed );
#endif
	}

	/**
	 *   �����_���̒l��Ԃ��i���������j
	 * @param  max �ő�l
	 * @return �O ���� max �܂Łimax �͊܂܂Ȃ��j�̃����_�����������l
	 */

	static inline f32 Random( const f32 max ) {
#ifdef GFL_PLATFORM_3DS
		return max;
#else
		return (static_cast<f32> (rand( )) * max) * (1.0f / (static_cast<f32> (RAND_MAX) + 1.0f));
#endif
	}

	/**
	 *   �����_���̒l��Ԃ��i���������j
	 * @param  min �ŏ��l
	 * @param  max �ő�l
	 * @return min ���� max �܂Łimax �͊܂܂Ȃ��j�̃����_�����������l
	 */

	static inline f32 Random( const f32 min, const f32 max ) {
		f32 df = max - min;
		if( 0.0f < df ){
			df *= Random( 1.0f );
		} else {
			df = 0.0f;
		}
		return min + df;
	}

	/**
	 *   �����_���̒l��Ԃ��i�����j
	 * @param  max �ő�l
	 * @return �O ���� max �܂Łimax �͊܂܂Ȃ��j�̃����_�������l
	 */

	template<typename T>
	static inline T Random( T max ) {
		return static_cast<T> (Random( static_cast<f32> (max) ));
	}

	/**
	 *   �����_���̒l��Ԃ��i�����j
	 * @param  ����
	 * @return �O ���� RAND_MAX �܂ł̃����_���l
	 */

	static inline int Random( void ) {
#ifdef GFL_PLATFORM_3DS
		return 0;
#else
		return rand( );
#endif
	}

	/**
	 *   ���������_��Βl
	 * @param  val �����l
	 * @return ��Βl
	 */

	static inline f32 FAbs( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FAbs( val );
#else
		return fabsf( val );
#endif
	}

	/**
	 *   ������Βl
	 * @param  val �����l
	 * @return ��Βl
	 */

	static inline s32 Abs( const s32 val ) {
		return ( 0 <= val) ? val : -val;
	}

	/**
	 *   ���������_������
	 * @param  val �����l
	 * @return ������
	 */

	static inline f32 FSqrt( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FSqrt( val );
#else
		return sqrtf( val );
#endif
	}

	/**
	 *   ���������_�t��
	 * @param  val �����l
	 * @return �t��
	 */

	static inline f32 FRecip( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FInv( val );
#else
		return 1.0f / val;
#endif
	}

	/**
	 * ���������̗]����擾����
	 * @param x�@���q
	 * @param y�@����
	 * @return �]��
	 */
	static inline f32 FMod( const f32 x, const f32 y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FMod( x, y );
#else
		return fmodf( x, y );
#endif
	}

	/**
	 * ���������̐������Ə������𕪗�
	 * @param x�@����
	 * @param y�@�������̃|�C���^
	 * @return ������
	 */
	static inline f32 FModf( const f32 x, f32* y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FModf( x, y );
#else
		return modff( x, y );
#endif
	}

	/**
	 * �؂�グ
	 * @param val�@����
	 * @return �؂�グ��ꂽ�l
	 */

	static inline f32 FCeil( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FCeil( val );
#else
		return ceilf( val );
#endif
	}

	/**
	 * �؂�̂�
	 * @param val�@����
	 * @return�@�؂�̂Ă�ꂽ�l
	 */
	static inline f32 FFloor( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::FFloor( val );
#else
		return floorf( val );
#endif
	}

	/**
	 *   �T�C��
	 * @param  val ���W�A��
	 * @return �T�C���l
	 */

	static inline f32 FSin( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::SinRad( val );
#else
		return sinf( val );
#endif
	}

	/**
	 *   �R�T�C��
	 * @param  val ���W�A��
	 * @return �R�T�C���l
	 */

	static inline f32 FCos( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::CosRad( val );
#else
		return cosf( val );
#endif
	}

	/**
	 *   �^���W�F���g
	 * @param  val ���W�A��
	 * @return �^���W�F���g�l
	 */

	static inline f32 FTan( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::TanRad( val );
#else
		return tanf( val );
#endif
	}

	/**
	 *   �T�C���R�T�C��
	 * @param  sin_val �T�C���l
	 * @param  cos_val �R�T�C���l
	 * @param  val ���W�A��
	 * @return ����
	 */

	static inline void FSinCos( f32* sin_val, f32* cos_val, const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		nn::math::SinCosRad( sin_val, cos_val, val );
#else
		*sin_val = FSin( val );
		*cos_val = FCos( val );
#endif
	}

	/**
	 *   �A�[�N�T�C��
	 * @param  val �T�C���l
	 * @return �A�[�N�T�C���l
	 */

	static inline f32 FASin( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AsinRad( val );
#else
		return asinf( val );
#endif
	}

	/**
	 *   �A�[�N�R�T�C��
	 * @param  val �R�T�C���l
	 * @return �A�[�N�R�T�C���l
	 */

	static inline f32 FACos( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AcosRad( val );
#else
		return acosf( val );
#endif
	}

	/**
	 *   �A�[�N�^���W�F���g
	 * @param  val �^���W�F���g�l
	 * @return �A�[�N�^���W�F���g�l
	 */

	static inline f32 FATan( const f32 val ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::AtanRad( val );
#else
		return atanf( val );
#endif
	}

	/**
	 *   �A�[�N�^���W�F���g�Q
	 * @param  x X
	 * @param  y Y
	 * @return �A�[�N�^���W�F���g�l
	 */

	static inline f32 FATan2( const f32 x, const f32 y ) {
#ifdef GFL_PLATFORM_3DS
		return nn::math::Atan2Rad( x, y );
#else
		return atan2f( x, y );
#endif
	}

	/**
	 * �T�C������R�T�C�������߂�
	 * @param sc�@�T�C��
	 * @return �R�T�C��
	 */
	static inline f32 SinToCos( const f32 sc ) {
		return FSqrt( 1.0f - sc * sc );
	}

	/**
	 * �R�T�C������T�C�������߂�
	 * @param sc�@�R�T�C��
	 * @return �T�C��
	 */
	static inline f32 CosToSin( const f32 sc ) {
		return SinToCos( sc );
	}

	/**
	 *   �T�C���R�T�C����^���Ăw�y���W����]���ꂽ�w���W�𓾂�
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @param  x �w���W
	 * @param  z �y���W
	 * @return ��]�����w���W
	 */

	static inline f32 GetX_RotateY_SinCosXZ( const f32 s, const f32 c, const f32 x, const f32 z ) {
		return c * x + s * z;
	}

	static inline f32 GetX_RotateY_SinCosX( const f32, const f32 c, const f32 x ) {
		return c * x;
	}

	static inline f32 GetX_RotateY_SinCosZ( const f32 s, const f32, const f32 z ) {
		return s * z;
	}

	/**
	 *   �T�C���R�T�C����^���Ăw�y���W����]���ꂽ�y���W�𓾂�
	 * @param  s �T�C���l
	 * @param  c �R�T�C���l
	 * @param  x �w���W
	 * @param  z �y���W
	 * @return ��]�����y���W
	 */

	static inline f32 GetZ_RotateY_SinCosXZ( const f32 s, const f32 c, const f32 x, const f32 z ) {
		return c * z - s * x;
	}

	static inline f32 GetZ_RotateY_SinCosX( const f32 s, const f32, const f32 x ) {
		return -s * x;
	}

	static inline f32 GetZ_RotateY_SinCosZ( const f32, const f32 c, const f32 z ) {
		return c * z;
	}

	/**
	 *   ���W���x��]���ꂽ���W�𓾂�
	 * @param  xpos �w���W
	 * @param  zpos �y���W
	 * @param  yang �x��]�l�i���W�A���j
	 * @return ����
	 */
	static void GetXZRotateY( f32* xpos, f32* zpos, const f32 yang );

	/**
	 *   ���W���x��]���ꂽ���W�𓾂�
	 * @param  xtag �o�͂w���W
	 * @param  ztag �o�͂y���W
	 * @param  xorg ���͂w���W
	 * @param  zorg ���͂y���W
	 * @param  yang �x��]�l�i���W�A���j
	 * @return ����
	 */
	static void GetXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg );
	static void AddXZRotateY( f32* xtag, f32* ztag, const f32 yang, const f32 xorg, const f32 zorg );
	static void AddXZRotateY( Vector* tag, const f32 yang, Vector& org );

	/**
	 *   ���W���x��]���ꂽ���W�𓾂�
	 * @param  xtag �o�͂w���W
	 * @param  ztag �o�͂y���W
	 * @param  xorg ���͂w���W
	 * @param  yang �x��]�l�i���W�A���j
	 * @return ����
	 */
	static void GetXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg );
	static void AddXZRotateY_X( f32* xtag, f32* ztag, const f32 yang, const f32 xorg );
	static void AddXZRotateY_X( Vector* tag, const f32 yang, const f32 xorg );

	/**
	 *   ���W���x��]���ꂽ���W�𓾂�
	 * @param  xtag �o�͂w���W
	 * @param  ztag �o�͂y���W
	 * @param  zorg ���͂y���W
	 * @param  yang �x��]�l�i���W�A���j
	 * @return ����
	 */
	static void GetXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg );
	static void AddXZRotateY_Z( f32* xtag, f32* ztag, const f32 yang, const f32 zorg );
	static void AddXZRotateY_Z( Vector* tag, const f32 yang, const f32 zorg );

	static void GetXZRotateY_Z1( f32* xtag, f32* ztag, const f32 yang ) {
		FSinCos( xtag, ztag, yang );
	}

	/**
	 * �O���X�O�x�ȓ��ɂ������W�����܂��Ă��邩
	 * @param  x X���W
	 * @param  z Z���W
	 * @return �^�@�O�@�U�@�㍶�E
	 */

	static bool IsFrontDegree90( const f32 x, const f32 z ) {
		if( 0.0f < z ){
			if( 0.0f < x ){
				return ( x < z);
			} else {
				return ( -x < z);
			}
		}
		return false;
	}

	/**
	 * ����X�O�x�ȓ��ɂ������W�����܂��Ă��邩
	 * @param  x X���W
	 * @param  z Z���W
	 * @return �^�@��@�U�@�O���E
	 */

	static bool IsBackDegree90( const f32 x, const f32 z ) {
		if( z < 0.0f ){
			if( x < 0.0f ){
				return ( z < x);
			} else {
				return ( z < -x);
			}
		}
		return false;
	}

	/**
	 * ���X�O�x�ȓ��ɂ������W�����܂��Ă��邩
	 * @param  x X���W
	 * @param  z Z���W
	 * @return �^�@���@�U�@�O��E
	 */
	static bool IsLeftDegree90( const f32 x, const f32 z ) {
		if( x < 0.0f ){
			if( 0.0f < z ){
				return ( z < x);
			} else {
				return ( -z < x);
			}
		}
		return false;
	}

	/**
	 * �E�X�O�x�ȓ��ɂ������W�����܂��Ă��邩
	 * @param  x X���W
	 * @param  z Z���W
	 * @return �^�@�E�@�U�@�O�㍶
	 */
	static bool IsRightDegree90( const f32 x, const f32 z ) {
		if( 0.0f < x ){
			if( 0.0f < z ){
				return ( z < x);
			} else {
				return ( -z < x);
			}
		}
		return false;
	}

	/**
	 *   �O��ǂ��炩�H
	 * @param  org ��_
	 * @param  org_yang ��_��Y�p�x
	 * @param  tag �ڕW�_
	 * @return �^�@�O�@�U�@��
	 */

	static bool IsPositionFront( const Vector& org, const f32 org_yang, const Vector& tag );
	/**
	 *   �O�㍶�E�̑O���H
	 * @param  org ��_
	 * @param  org_yang ��_��Y�p�x
	 * @param  tag �ڕW�_
	 * @return �^�@�O�@�U�@�O����Ȃ�
	 */

	static bool IsPositionFrontDegree90( const Vector& org, const f32 org_yang, const Vector& tag );

	/**
	 *   �قƂ�ǂO�̒l���O�ɂ���
	 * @param  val �l
	 * @return �O�ɋ߂��Ƃ��͂O���A�����łȂ��Ƃ��͂��̂܂܂̒l
	 */

	static inline f32 AlmostZeroToZero( f32 val ) {
		if( Math::FAbs( val ) <= EPSILON ){
			val = 0.0f;
		}
		return val;
	}

	/**
	 *   �p�x��x�����烉�W�A���ɕϊ�
	 * @param  x �x��
	 * @return ���W�A��
	 */

	static inline f32 DegreeToRadian( const f32 x ) {
		return x * (ONE_PI / 180.0f);
	}

	/**
	 *   �p�x�����W�A������x���ɕϊ�
	 * @param  x ���W�A��
	 * @return �x��
	 */

	static inline f32 RadianToDegree( const f32 x ) {
		return x * (180.0f / ONE_PI);
	}

	/**
	 *   ���W�A�����[�o�h�`�o�h�͈̔͂Ɏ��߂�
	 * @param  rad ���W�A��
	 * @return ���W�A��
	 */
	static f32 OptimizeRadian( f32 rad );

	/**
	 *   ���W�A�����Βl�̏������ق��ɂ���
	 * @param  vxp ���W�A��
	 * @param  vyp ���W�A��
	 * @param  vzp ���W�A��
	 * @return ���W�A��
	 */
	static void OptimizeRadian( f32* vxp, f32* vyp, f32* vzp );

	/**
	 *   ���W�A����ڕW�l�ɏ��X�ɋ߂Â���
	 * @param  cur ���ݒl
	 * @param  tag �ڕW�l
	 * @param  rate ����
	 * @return �␳�l
	 */

	static f32 AdjustRadianGradually( const f32 cur, const f32 tag, const f32 rate ) {
		return gfl::core::Math::AddRadian( cur, gfl::core::Math::AddRadian( tag, -cur ) * rate );
	}

	/**
	 *   ���W�A����ڕW�l�ɏ��X�ɋ߂Â���
	 * @param  cur ���ݒl
	 * @param  tag �ڕW�l
	 * @param  rate ����
	 * @param  lim �����̏���l
	 * @return �␳�l
	 */

	static f32 AdjustRadianGraduallyWithLimit( const f32 cur, const f32 tag, const f32 rate, const f32 lim ) {
		f32 diff = gfl::core::Math::AddRadian( tag, -cur );
		diff *= rate;
		ClampAbs( &diff, lim );
		return gfl::core::Math::AddRadian( cur, diff );
	}

	/**
	 *   ���W�A�����Βl�̏������ق��ɂ���
	 * @param  vxp ���W�A��
	 * @param  vyp ���W�A��
	 * @param  vzp ���W�A��
	 * @param  bx ���W�A��
	 * @param  by ���W�A��
	 * @param  bz ���W�A��
	 * @return ���W�A��
	 */
	static void SmoothRadian( f32* vxp, f32* vyp, f32* vzp, f32 bx, f32 by, f32 bz );

	/**
	 *   ���W�A���̉��Z
	 * @param  rad ���W�A��
	 * @param  add ���W�A��
	 * @return ���W�A��
	 * @remark ���ʂ��[�o�h�`�o�h�͈̔͂Ɏ��߂�B���͂��͈͓��łȂ��ƕۏ�ł��Ȃ��B
	 */
	static f32 AddRadian( f32 rad, f32 add );

	/**
	 *   ���W�A���� step �i�K�ɕϊ�
	 * @param  val_f ���W�A��
	 * @param  step �i�K
	 * @return �i�K
	 */
	static u32 RoundRadianStep( f32 val_f, const u32 step );

	/**
	 *   ���������_�l �� step �i�K�̋�؂肪�����l�ɂ���
	 * @param  val  ���������_�l
	 * @param  step ���������_�i�K
	 * @return ���������_�l
	 */
	static f32 RoundFloat( f32 val, const f32 step );

	/**
	 *   ���������_�l �� step �i�K�̋�؂肪�����l�ɂ���
	 * @param  val  ���������_�l
	 * @return ���������_�l
	 */

	static f32 RoundFloat( f32 val ) {
		return FFloor( val + 0.5f );
	}

	/**
	 *   ���������_�l �� �l�̌ܓ�����
	 * @param  val  ���������_�l
	 * @param  range �������l�@�f�t�H���g�� 0.5f
	 * @return ���������_�l
	 * @remark range �� 0.01f �Ƃ�������� 1.0001f -> 1.0f, 1.1f -> 1.1f �̂悤��
	 * ���������v�Z�덷���B�������ꍇ�Ɏg���ƕ֗�
	 */
	static f32 RoundOff( f32 val, const f32 range = 0.5f );

	/**
	 *   ���������Ă��邩�H
	 * @param  t1 ���������P
	 * @param  t2 ���������Q
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	static inline bool IsSimilar( const f32 t1, const f32 t2, const f32 epsilon = EPSILON ) {
		return Math::FAbs( t1 - t2 ) <= epsilon;
	}

	/**
	 *   �قƂ�ǂO��
	 * @param  t1 ���������P
	 * @param  t2 ���������Q
	 * @param  epsilon �덷�͈́@�w�肵�Ȃ��ꍇ�� EPSILON
	 * @return �����l�̌덷�� epsilon ���Ȃ�^�A����ȊO�͋U
	 */

	static inline bool IsAlmostZero( const f32 t1, const f32 epsilon = EPSILON ) {
		return Math::FAbs( t1 ) <= epsilon;
	}

	/**
	 * ���̘a��Ԃ�
	 * @param x�@����
	 * @param y�@����
	 * @param z�@����
	 * @return ���̘a
	 */
	static inline f32 Magnitude2( const f32 x, const f32 y, const f32 z ) {
		return ( x * x + y * y + z * z);
	}

	/**
	 * �傫����Ԃ�
	 * @param x�@����
	 * @param y�@����
	 * @param z�@����
	 * @return �@�傫��
	 */

	static inline f32 Magnitude( const f32 x, const f32 y, const f32 z ) {
		return Math::FSqrt( Magnitude2( x, y, z ) );
	}

	/**
	 *   �X���b�v�֐�
	 * @param  a �X���b�v����ϐ��܂��̓N���X
	 * @param  b �X���b�v����ϐ��܂��̓N���X
	 * @return ����
	 */

	template <class T>
	static void Swap( T& a, T& b ) {
		T tmp = a;
		a = b;
		b = tmp;
	}

	/**
	 *   �X���b�v�֐�
	 * @param  a �X���b�v����ϐ��܂��̓N���X �������]������ b �ɑ��
	 * @param  b �X���b�v����ϐ��܂��̓N���X b �͂��̂܂� a �ɑ��
	 * @return ����
	 */

	template <class T>
	static void SwapNeg( T& a, T& b ) {
		T tmp = -a;
		a = b;
		b = tmp;
	}

	/**
	 *   �X���b�v�֐�
	 * @param  a �X���b�v����ϐ��܂��̓N���X �������]������ b �ɑ��
	 * @param  b �X���b�v����ϐ��܂��̓N���X �������]������ a �ɑ��
	 * @return ����
	 */

	template <class T>
	static void SwapNegNeg( T& a, T& b ) {
		T tmp = -a;
		a = -b;
		b = tmp;
	}

	template<typename T>
	static T Greater( const T l, const T r ) {
		return (l < r) ? r : l;
	}

	template<typename T>
	static T Lesser( const T l, const T r ) {
		return (l > r) ? r : l;
	}
};

}
}

#endif	/* GFL_BASEMATH_H */
