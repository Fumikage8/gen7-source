#ifndef __GFL_MTX_H__
#define __GFL_MTX_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_Mtx.h
 *	@brief  �s�񉉎Z    �֐���`
 *	@author	tomoya takahashi
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn/math.h>

#include <math/gfl_MathType.h>

namespace gfl {
namespace math {


//-----------------------------------------------------------------------------
/**
 *        MTX22
 */
//-----------------------------------------------------------------------------
inline MTX22* MTX22Identity( MTX22* pOut) { return nn::math::MTX22Identity( pOut); }
inline bool MTX22IsIdentity( const MTX22* p) { return nn::math::MTX22IsIdentity( p); }
inline MTX22* MTX22Copy( MTX22* pOut, const MTX22* p) { return nn::math::MTX22Copy( pOut,p); }
inline MTX22* MTX22Zero( MTX22* pOut) { return nn::math::MTX22Zero( pOut); }
inline MTX22* MTX23ToMTX22( MTX22* pOut, const MTX23* pM) { return nn::math::MTX23ToMTX22( pOut,pM); }
inline MTX22* MTX22MAdd( MTX22* pOut, f32 t, const MTX22* p1, const MTX22* p2) { return nn::math::MTX22MAdd( pOut,t,p1,p2); }


//-----------------------------------------------------------------------------
/**
 *        MTX23
 */
//-----------------------------------------------------------------------------
inline MTX23* MTX23Copy( MTX23* pOut, const MTX23* p) { return nn::math::MTX23Copy( pOut,p); }
inline MTX23* MTX23Zero( MTX23* pOut) { return nn::math::MTX23Zero( pOut); }
inline MTX23* MTX23Identity( MTX23* pOut) { return nn::math::MTX23Identity( pOut); }
inline bool MTX23IsIdentity( const MTX23* p) { return nn::math::MTX23IsIdentity( p); }
inline MTX23* MTX23Add( MTX23* pOut, const MTX23* p1, const MTX23* p2) { return nn::math::MTX23Add( pOut,p1,p2); }
inline MTX23* MTX23Sub( MTX23* pOut, const MTX23* p1, const MTX23* p2) { return nn::math::MTX23Sub( pOut,p1,p2); }
inline MTX23* MTX23Mult( MTX23* pOut, const MTX23* p, f32 f) { return nn::math::MTX23Mult( pOut,p,f); }
inline MTX23* MTX23Mult( MTX23* pOut, const MTX23* p1, const MTX23* p2) { return nn::math::MTX23Mult( pOut,p1,p2); }
inline MTX23* MTX23Scale( MTX23* pOut, const MTX23* pM, const VEC2* pS) { return nn::math::MTX23Scale( pOut,pM,pS); }
// SDK4.0.2 MTX23Translate�̎d�l�ύX : �]���̏��� > MTX23MultTranslate
inline MTX23* MTX23Translate( MTX23* pOut, const MTX23* pM, const VEC2* pT) { return nn::math::MTX23MultTranslate( pOut,pM,pT); }
inline MTX23* MTX23RotFIdx( MTX23* pOut, f32 fIdx) { return nn::math::MTX23RotFIdx( pOut,fIdx); }
inline MTX23* MTX23RotCenterFIdx( MTX23* pOut, const VEC2* pCenter, f32 fIdx) { return nn::math::MTX23RotCenterFIdx( pOut,pCenter,fIdx); }


inline MTX23* MTX23MAdd( MTX23* pOut, f32 t, const MTX23* p1, const MTX23* p2) { return nn::math::MTX23MAdd( pOut,t,p1,p2); }
inline MTX23* MTX22ToMTX23( MTX23* pOut, const MTX22* pM) { return nn::math::MTX22ToMTX23( pOut,pM); }


//-----------------------------------------------------------------------------
/**
 *        MTX33
 */
//-----------------------------------------------------------------------------
inline bool MTX33IsIdentity( const MTX33* p) { return nn::math::MTX33IsIdentity( p); }
inline MTX33* MTX33Copy( MTX33* pOut, const MTX33* p) { return nn::math::MTX33Copy( pOut,p); }
inline MTX33* MTX33Zero( MTX33* pOut) { return nn::math::MTX33Zero( pOut); }
inline MTX33* MTX33Identity( MTX33* pOut) { return nn::math::MTX33Identity( pOut); }
inline MTX33* MTX33MAdd( MTX33* pOut, f32 t, const MTX33* p1, const MTX33* p2) { return nn::math::MTX33MAdd( pOut,t,p1,p2); }
inline u32 MTX34InvTranspose( MTX33* pOut, const MTX34* p) { return nn::math::MTX34InvTranspose( pOut,p); }
inline MTX34* MTX33ToMTX34( MTX34* pOut, const MTX33* pM) { return nn::math::MTX33ToMTX34( pOut,pM); }
inline VEC3* VEC3Transform( VEC3* pOut, const MTX33* pM, const VEC3* pV) { return reinterpret_cast<VEC3*>(nn::math::VEC3Transform( pOut,pM,pV)); }

// SDK��MTX33Mult���g�p����ƃe���v���[�g�̑��d��`�ŃR���p�C�����ʂ�Ȃ��̂Ŏ��삵�܂����B
// �������̂��� pOut �ɂ� p1 �܂��� p2 �Ɠ����|�C���^��ݒ肵�Ă͂����܂���B
inline void MTX33MultLimit(MTX33* pOut, const MTX33* p1, const MTX33* p2);
// inline void MTX33Mult(MTX33* pOut, const MTX33* p1, const MTX33* p2) { nn::math::MTX33Mult(pOut, p1, p2); }

// �X�P�[�� or �e����]�}�g���N�X�ɓ��̓}�g���N�X����Z���܂��B
// �|�C���^�̐�����MTX33MultLimit()�Ɠ����ł��B
inline void MTX33MultLimitScale  (MTX33* dest, const MTX33* src, const VEC3* scale);
inline void MTX33MultLimitScale  (MTX33* dest, const MTX33* src, const float scale) { VEC3 v(scale, scale, scale); MTX33MultLimitScale(dest, src, &v); }
inline void MTX33MultLimitRotXRad(MTX33* dest, const MTX33* src, const float rad);
inline void MTX33MultLimitRotYRad(MTX33* dest, const MTX33* src, const float rad);
inline void MTX33MultLimitRotZRad(MTX33* dest, const MTX33* src, const float rad);

inline void MTX33Transpose(MTX33* dest, const MTX33* src);

/// ���K���s�}�g���N�X�̋t�s����擾����B�Ƃ肠�����X�P�[�����Ȃ���]�݂̂̃}�g���N�X�Ƃ��������ł悢�B�������x�͊i�i�ɂ�����̕��������B
inline void MTX33InverseOrtho(MTX33* dest, const MTX33* src) { MTX33Transpose(dest, src); }
inline int  MTX33Inverse     (MTX33* dest, const MTX33* src); ///< �f�o�b�O���Ă��Ȃ��̂ŕs�������Ƃ��͒r�c�܂ŕ񍐂��Ă��������B

inline void MTX33Scale(MTX33* dest, const VEC3* scale);
inline void MTX33Scale(MTX33* dest, const float scale) { VEC3 v(scale, scale, scale); MTX33Scale(dest, &v); }
inline void MTX33RotXRad(MTX33* dest, const float rad);
inline void MTX33RotYRad(MTX33* dest, const float rad);
inline void MTX33RotZRad(MTX33* dest, const float rad);
inline void MTX33RotAxisRad(MTX33* dest, const VEC3* axis, const float rad); ///< �C�ӎ���]�}�g���N�X���擾����B��]���͐��K���K�{�B

inline void MTX33GetAxisX(VEC3* axis, const MTX33& mat) { axis->x = mat.m[0][0], axis->y = mat.m[1][0], axis->z = mat.m[2][0]; }
inline void MTX33GetAxisY(VEC3* axis, const MTX33& mat) { axis->x = mat.m[0][1], axis->y = mat.m[1][1], axis->z = mat.m[2][1]; }
inline void MTX33GetAxisZ(VEC3* axis, const MTX33& mat) { axis->x = mat.m[0][2], axis->y = mat.m[1][2], axis->z = mat.m[2][2]; }
inline void MTX33SetAxis(MTX33* dest, const VEC3& axisX, const VEC3& axisY, const VEC3& axisZ);

/// �Q�̃}�g���N�X�Ԃ̋��ʐ��`��Ԓl���擾����B�N�H�[�^�j�I���ɕϊ����Čv�Z����̂ŏ����͏d���B
inline void MTX33Slerp(MTX33* dest, const MTX33& src1, const MTX33& src2, const float ratio);

/// �}�g���N�X�𐳋K������B��Ƃ��Čv�Z�덷�ɂ�萳�K������ꂽ���Ɏg�p����B�d���̂Ő��x�����߂Ȃ����͎g�p���Ȃ����ƁB
inline void MTX33Normalize(MTX33* dest, const MTX33& src);

extern const MTX33 MTX33_IDENTITY;

//-----------------------------------------------------------------------------
/**
 *        MTX34
 */
//-----------------------------------------------------------------------------
inline MTX34* MTX34Zero( MTX34* pOut) { return nn::math::MTX34Zero( pOut); }
inline bool MTX34IsIdentity( const MTX34* p) { return nn::math::MTX34IsIdentity( p); }
inline MTX34* MTX34Identity( MTX34* pOut) { return nn::math::MTX34Identity( pOut); }
inline MTX34* MTX34Copy( MTX34* pOut, const MTX34* p) { return nn::math::MTX34Copy( pOut,p); }

inline MTX34* MTX34Add( MTX34* pOut, const MTX34* p1, const MTX34* p2) { return nn::math::MTX34Add( pOut,p1,p2); }
inline MTX34* MTX34Sub( MTX34* pOut, const MTX34* p1, const MTX34* p2) { return nn::math::MTX34Sub( pOut,p1,p2); }
inline MTX34* MTX34Mult( MTX34* pOut, const MTX34* p, f32 f) { return nn::math::MTX34Mult( pOut,p,f); }
inline MTX34* MTX34Mult( MTX34* pOut, const MTX34* p1, const MTX34* p2) { return nn::math::MTX34Mult( pOut,p1,p2); }
inline MTX34* MTX34MAdd( MTX34* pOut, f32 t, const MTX34* p1, const MTX34* p2) { return nn::math::MTX34MAdd( pOut,t,p1,p2); }


inline MTX34* MTX34MultArray( MTX34* pOut, const MTX34* p1, const MTX34* pSrc, s32 count) { return nn::math::MTX34MultArray( pOut,p1,pSrc,count); }

inline void MTX34InverseOrtho(MTX34* dest, const MTX34* src);
inline u32  MTX34Inverse     (MTX34* pOut, const MTX34* p) { return nn::math::MTX34Inverse( pOut,p); }

inline MTX34* MTX34Transpose( MTX34* pOut, const MTX34* p) { return nn::math::MTX34Transpose( pOut,p); }
inline u32 MTX34InvTranspose( MTX34* pOut, const MTX34* p) { return nn::math::MTX34InvTranspose( pOut,p); }

inline MTX34* MTX34LookAt( MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamUp, const VEC3* pTarget) { return nn::math::MTX34LookAt( pOut,pCamPos,pCamUp,pTarget); }
inline MTX34* MTX34LookAt( MTX34* pOut, const VEC3* pCamPos, f32 twistDeg, const VEC3* pTarget) { return nn::math::MTX34LookAt( pOut,pCamPos,twistDeg,pTarget); }
inline MTX34* MTX34CameraRotate( MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamRotateDeg) { return nn::math::MTX34CameraRotate( pOut,pCamPos,pCamRotateDeg); }

inline MTX34* MTX34TextureProjectionFrustum( MTX34* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 scaleS, f32 scaleT, f32 translateS, f32 translateT) { return nn::math::MTX34TextureProjectionFrustum( pOut,l,r,b,t,n,scaleS,scaleT,translateS,translateT); }
inline MTX34* MTX34TextureProjectionPerspective( MTX34* pOut, f32 fovy, f32 aspect, f32 scaleS, f32 scaleT, f32 translateS, f32 translateT) { return nn::math::MTX34TextureProjectionPerspective( pOut,fovy,aspect,scaleS,scaleT,translateS,translateT); }
inline MTX34* MTX34TextureProjectionOrtho( MTX34* pOut, f32 l, f32 r, f32 b, f32 t, f32 scaleS, f32 scaleT, f32 translateS, f32 translateT) { return nn::math::MTX34TextureProjectionOrtho( pOut,l,r,b,t,scaleS,scaleT,translateS,translateT); }

inline MTX34* MTX34RotXYZFIdx( MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ) { return nn::math::MTX34RotXYZFIdx( pOut,fIdxX,fIdxY,fIdxZ); }
inline MTX34* MTX34RotXYZTranslateFIdx( MTX34* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ, const VEC3* pT) { return nn::math::MTX34RotXYZTranslateFIdx( pOut,fIdxX,fIdxY,fIdxZ,pT); }
inline MTX34* MTX34RotAxisFIdx( MTX34* pOut, const VEC3* pAxis, f32 fIdx) { return nn::math::MTX34RotAxisFIdx( pOut,pAxis,fIdx); }


inline MTX34* MTX34RotXYZRad( MTX34* pOut, f32 radX, f32 radY, f32 radZ) { return nn::math::MTX34RotXYZRad( pOut,radX,radY,radZ); }
inline MTX34* MTX34RotAxisRad( MTX34* pOut, const VEC3* pAxis, f32 rad) { return nn::math::MTX34RotAxisRad( pOut,pAxis,rad); }


inline MTX34* MTX34Scale( MTX34* pOut, const VEC3* pS) { return nn::math::MTX34Scale( pOut,pS); }
inline MTX34* MTX34MultScale( MTX34* pOut, const MTX34* pM, const VEC3* pS) { return nn::math::MTX34MultScale( pOut,pM,pS); }
inline MTX34* MTX34MultScale( MTX34* pOut, const VEC3* pS, const MTX34* pM) { return nn::math::MTX34MultScale( pOut,pS,pM); }

inline MTX34* MTX34Translate( MTX34* pOut, const VEC3* pT) { return nn::math::MTX34Translate( pOut,pT); }
inline MTX34* MTX34MultTranslate( MTX34* pOut, const MTX34* pM, const VEC3* pT) { return nn::math::MTX34MultTranslate( pOut,pM,pT); }
inline MTX34* MTX34MultTranslate( MTX34* pOut, const VEC3* pT, const MTX34* pM) { return nn::math::MTX34MultTranslate( pOut,pT,pM); }
inline MTX34* QUATToMTX34( MTX34* pOut, const QUAT* p) { return nn::math::QUATToMTX34( pOut,p); }

inline VEC3* VEC3Transform( VEC3* pOut, const MTX34* __restrict pM, const VEC3* __restrict pV) { return reinterpret_cast<VEC3*>(nn::math::VEC3Transform( pOut,pM,pV)); }

inline void MTX34GetAxisX    (VEC3* axis, const MTX34& mat) { axis->x = mat.m[0][0], axis->y = mat.m[1][0], axis->z = mat.m[2][0]; }
inline void MTX34GetAxisY    (VEC3* axis, const MTX34& mat) { axis->x = mat.m[0][1], axis->y = mat.m[1][1], axis->z = mat.m[2][1]; }
inline void MTX34GetAxisZ    (VEC3* axis, const MTX34& mat) { axis->x = mat.m[0][2], axis->y = mat.m[1][2], axis->z = mat.m[2][2]; }
inline void MTX34GetTranslate(VEC3* posi, const MTX34& mat) { posi->x = mat.m[0][3], posi->y = mat.m[1][3], posi->z = mat.m[2][3]; }

inline void MTX34SetTranslate(MTX34* mat, const VEC3& posi) { mat->m[0][3] = posi.x, mat->m[1][3] = posi.y, mat->m[2][3] = posi.z; }
inline void MTX34SetAxisTranslate(MTX34* dest, const VEC3& axisX, const VEC3& axisY, const VEC3& axisZ, const VEC3& posi);
inline void MTX34SetMTX33(MTX34* dest, const MTX33& sr);
inline void MTX34SetMTX33VEC3(MTX34* dest, const MTX33& sr, const VEC3& posi);

/// �Q�̃}�g���N�X�Ԃ̋��ʐ��`��Ԓl���擾����B�N�H�[�^�j�I���ɕϊ����Čv�Z����̂ŏ����͏d���B
inline void MTX34Slerp(MTX34* dest, const MTX34& src1, const MTX34& src2, const float ratio);

extern const MTX34 MTX34_IDENTITY;

//-----------------------------------------------------------------------------
/**
 *        MTX43
 */
//-----------------------------------------------------------------------------
inline MTX43* MTX43Zero( MTX43* pOut) { return nn::math::MTX43Zero( pOut); }
inline MTX43* MTX43Identity( MTX43* pOut) { return nn::math::MTX43Identity( pOut); }
inline bool MTX43IsIdentity( const MTX43* p) { return nn::math::MTX43IsIdentity( p); }
inline MTX43* MTX43Copy( MTX43* pOut, const MTX43* p) { return nn::math::MTX43Copy( pOut,p); }
inline MTX43* MTX43Add( MTX43* pOut, const MTX43* p1, const MTX43* p2) { return nn::math::MTX43Add( pOut,p1,p2); }
inline MTX43* MTX43Sub( MTX43* pOut, const MTX43* p1, const MTX43* p2) { return nn::math::MTX43Sub( pOut,p1,p2); }
inline MTX43* MTX43Mult( MTX43* pOut, const MTX43* p1, const MTX43* p2) { return nn::math::MTX43Mult( pOut,p1,p2); }
inline MTX43* MTX43Mult( MTX43* pOut, const MTX43* p, f32 f) { return nn::math::MTX43Mult( pOut,p,f); }





//-----------------------------------------------------------------------------
/**
 *        MTX44
 */
//-----------------------------------------------------------------------------

enum PivotDirection
{
    PIVOT_NONE,             //!< ��]�����������Ȃ��܂���B
    PIVOT_UPSIDE_TO_TOP,    //!< �J�������������ʏ�����������悤�ɉ�]���܂��B
    PIVOT_UPSIDE_TO_RIGHT,  //!< �J�������������ʉE�����������悤�ɉ�]���܂��B
    PIVOT_UPSIDE_TO_BOTTOM, //!< �J�������������ʉ������������悤�ɉ�]���܂��B
    PIVOT_UPSIDE_TO_LEFT,   //!< �J�������������ʍ������������悤�ɉ�]���܂��B
    PIVOT_NUM
};

inline MTX44* MTX44Zero( MTX44* pOut) { return nn::math::MTX44Zero( pOut); }
inline bool MTX44IsIdentity( const MTX44* p) { return nn::math::MTX44IsIdentity( p); }
inline MTX44* MTX44Identity( MTX44* pOut) { return nn::math::MTX44Identity( pOut); }
inline MTX44* MTX44Copy( MTX44* pOut, const MTX44* p) { return nn::math::MTX44Copy( pOut,p); }

inline MTX44* MTX44Add( MTX44* pOut, const MTX44* p1, const MTX44* p2) { return nn::math::MTX44Add( pOut,p1,p2); }
inline MTX44* MTX44Sub( MTX44* pOut, const MTX44* p1, const MTX44* p2) { return nn::math::MTX44Sub( pOut,p1,p2); }
inline MTX44* MTX44Mult( MTX44* pOut, const MTX44* p, f32 f) { return nn::math::MTX44Mult( pOut,p,f); }
inline MTX44* MTX44Mult( MTX44* pOut, const MTX44* p1, const MTX44* p2) { return nn::math::MTX44Mult( pOut,p1,p2); }

inline MTX44* MTX44Frustum( MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) { return nn::math::MTX44Frustum( pOut,l,r,b,t,n,f); }
inline MTX44* MTX44Ortho( MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f) { return nn::math::MTX44Ortho( pOut,l,r,b,t,n,f); }
inline MTX44* MTX44PerspectiveRad( MTX44* pOut, f32 fovy, f32 aspect, f32 n, f32 f) { return nn::math::MTX44PerspectiveRad( pOut,fovy,aspect,n,f); }
inline MTX44* MTX44Transpose( MTX44* pOut, const MTX44 *pSrc) { return nn::math::MTX44Transpose( pOut,*pSrc); }
inline MTX44* MTX44MultArray( MTX44* pOut, const MTX44* p1, const MTX44* pSrc, s32 count) { return nn::math::MTX44MultArray( pOut,p1,pSrc,count); }
inline u32 MTX44Inverse( MTX44* pOut, const MTX44* p) { return nn::math::MTX44Inverse( pOut,p); }

inline MTX44* MTX44RotXYZFIdx( MTX44* pOut, f32 fIdxX, f32 fIdxY, f32 fIdxZ) { return nn::math::MTX44RotXYZFIdx( pOut,fIdxX,fIdxY,fIdxZ); }
inline MTX44* MTX44RotAxisFIdx( MTX44* pOut, const VEC3* pAxis, f32 fIdx) { return nn::math::MTX44RotAxisFIdx( pOut,pAxis,fIdx); }

inline MTX44* MTX44RotXYZRad( MTX44* pOut, f32 radX, f32 radY, f32 radZ) { return nn::math::MTX44RotXYZRad( pOut,radX,radY,radZ); }
inline MTX44* MTX44RotAxisRad( MTX44* pOut, const VEC3* pAxis, f32 rad) { return nn::math::MTX44RotAxisRad( pOut,pAxis,rad); }

inline MTX44* MTX44Scale( MTX44* pOut, const VEC3* pS) { return nn::math::MTX44Scale( pOut,pS); }
inline MTX44* MTX44MultScale( MTX44* pOut, const MTX44* pM, const VEC3* pS) { return nn::math::MTX44MultScale( pOut,pM,pS); }
inline MTX44* MTX44MultScale( MTX44* pOut, const VEC3* pS, const MTX44* pM) { return nn::math::MTX44MultScale( pOut,pS,pM); }

inline MTX44* MTX44Translate( MTX44* pOut, const VEC3* pT) { return nn::math::MTX44Translate( pOut,pT); }
inline MTX44* MTX44MultTranslate( MTX44* pOut, const MTX44* pM, const VEC3* pT) { return nn::math::MTX44MultTranslate( pOut,pM,pT); }
inline MTX44* MTX44MultTranslate( MTX44* pOut, const VEC3* pT, const MTX44* pM) { return nn::math::MTX44MultTranslate( pOut,pT,pM); }

inline MTX44* MTX44FrustumPivot( MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, PivotDirection pivot) { return nn::math::MTX44FrustumPivot( pOut,l,r,b,t,n,f,static_cast<nn::math::PivotDirection>(pivot)); }
inline MTX44* MTX44OrthoPivot( MTX44* pOut, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, PivotDirection pivot) { return nn::math::MTX44OrthoPivot( pOut,l,r,b,t,n,f,static_cast<nn::math::PivotDirection>(pivot)); }
inline MTX44* MTX44PerspectivePivotRad( MTX44* pOut, f32 fovy, f32 aspect, f32 n, f32 f, PivotDirection pivot) { return nn::math::MTX44PerspectivePivotRad( pOut,fovy,aspect,n,f,static_cast<nn::math::PivotDirection>(pivot)); }



inline MTX44* MTX44Pivot(  MTX44* pOut, PivotDirection pivot ) { return nn::math::MTX44Pivot( pOut, static_cast<nn::math::PivotDirection>(pivot)); }

extern const MTX44 MTX44_IDENTITY;

//-----------------------------------------------------------------------------
/**
 *    MatrixUtility
 */
//-----------------------------------------------------------------------------

inline MTX34* MTX43Transpose( MTX34* pOut, const MTX43* p) { return nn::math::MTX43Transpose( pOut,p); }
inline MTX43* MTX34Transpose( MTX43* pOut, const MTX34* p) { return nn::math::MTX34Transpose( pOut,p); }

inline QUAT* MTX34ToQUAT( QUAT* pOut, const MTX34* pMtx) { return nn::math::MTX34ToQUAT( pOut,pMtx); }

//-----------------------------------------------------------------------------
/**
 *    inline implement
 */
//-----------------------------------------------------------------------------

inline void MTX33MultLimit(MTX33* pOut, const MTX33* p1, const MTX33* p2)
{
  GFL_ASSERT(pOut != p1 && pOut != p2);

  pOut->f._00 = p1->f._00 * p2->f._00 + p1->f._01 * p2->f._10 + p1->f._02 * p2->f._20;
  pOut->f._01 = p1->f._00 * p2->f._01 + p1->f._01 * p2->f._11 + p1->f._02 * p2->f._21;
  pOut->f._02 = p1->f._00 * p2->f._02 + p1->f._01 * p2->f._12 + p1->f._02 * p2->f._22;

  pOut->f._10 = p1->f._10 * p2->f._00 + p1->f._11 * p2->f._10 + p1->f._12 * p2->f._20;
  pOut->f._11 = p1->f._10 * p2->f._01 + p1->f._11 * p2->f._11 + p1->f._12 * p2->f._21;
  pOut->f._12 = p1->f._10 * p2->f._02 + p1->f._11 * p2->f._12 + p1->f._12 * p2->f._22;

  pOut->f._20 = p1->f._20 * p2->f._00 + p1->f._21 * p2->f._10 + p1->f._22 * p2->f._20;
  pOut->f._21 = p1->f._20 * p2->f._01 + p1->f._21 * p2->f._11 + p1->f._22 * p2->f._21;
  pOut->f._22 = p1->f._20 * p2->f._02 + p1->f._21 * p2->f._12 + p1->f._22 * p2->f._22;
}

inline void MTX33MultLimitScale(MTX33* dest, const MTX33* src, const VEC3* scale)
{
  const float m00 = scale->x, m11 = scale->y, m22 = scale->z;
  dest->f._00 = src->f._00 * m00;
  dest->f._01 = src->f._01 * m11;
  dest->f._02 = src->f._02 * m22;

  dest->f._10 = src->f._10 * m00;
  dest->f._11 = src->f._11 * m11;
  dest->f._12 = src->f._12 * m22;

  dest->f._20 = src->f._20 * m00;
  dest->f._21 = src->f._21 * m11;
  dest->f._22 = src->f._22 * m22;
}

inline void MTX33MultLimitRotXRad(MTX33* dest, const MTX33* src, const float rad)
{
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  const float m00 = 1.0f, m11 = cos, m12 = -sin, m21 = sin, m22 = cos;

  dest->f._00 = src->f._00;
  dest->f._01 = src->f._01 * m11 + src->f._02 * m21;
  dest->f._02 = src->f._01 * m12 + src->f._02 * m22;

  dest->f._10 = src->f._10;
  dest->f._11 = src->f._11 * m11 + src->f._12 * m21;
  dest->f._12 = src->f._11 * m12 + src->f._12 * m22;

  dest->f._20 = src->f._20;
  dest->f._21 = src->f._21 * m11 + src->f._22 * m21;
  dest->f._22 = src->f._21 * m12 + src->f._22 * m22;
}

inline void MTX33MultLimitRotYRad(MTX33* dest, const MTX33* src, const float rad)
{
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  const float m00 = cos, m02 = sin, m11 = 1.0f, m20 = -sin, m22 = cos;

  dest->f._00 = src->f._00 * m00 + src->f._02 * m20;
  dest->f._01 = src->f._01;
  dest->f._02 = src->f._00 * m02 + src->f._02 * m22;

  dest->f._10 = src->f._10 * m00 + src->f._12 * m20;
  dest->f._11 = src->f._11;
  dest->f._12 = src->f._10 * m02 + src->f._12 * m22;

  dest->f._20 = src->f._20 * m00 + src->f._22 * m20;
  dest->f._21 = src->f._21;
  dest->f._22 = src->f._20 * m02 + src->f._22 * m22;
}

inline void MTX33MultLimitRotZRad(MTX33* dest, const MTX33* src, const float rad)
{
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  const float m00 = cos, m01 = -sin, m10 = sin, m11 = cos, m22 = 1.0f;

  dest->f._00 = src->f._00 * m00 + src->f._01 * m10;
  dest->f._01 = src->f._00 * m01 + src->f._01 * m11;
  dest->f._02 = src->f._02;

  dest->f._10 = src->f._10 * m00 + src->f._11 * m10;
  dest->f._11 = src->f._10 * m01 + src->f._11 * m11;
  dest->f._12 = src->f._12;

  dest->f._20 = src->f._20 * m00 + src->f._21 * m10;
  dest->f._21 = src->f._20 * m01 + src->f._21 * m11;
  dest->f._22 = src->f._22;
}

inline void MTX33Transpose(MTX33* dest, const MTX33* src)
{
  dest->m[0][0] = src->m[0][0];
  dest->m[1][1] = src->m[1][1];
  dest->m[2][2] = src->m[2][2];

  // dest == src ���̑Ώ�
  const float m01 = src->m[0][1];
  const float m02 = src->m[0][2];
  const float m12 = src->m[1][2];

  dest->m[0][1] = src->m[1][0];
  dest->m[0][2] = src->m[2][0];
  dest->m[1][2] = src->m[2][1];

  dest->m[1][0] = m01;
  dest->m[2][0] = m02;
  dest->m[2][1] = m12;
}

inline int MTX33Inverse(MTX33* dest, const MTX33* src)
{
  float det = src->_00*src->_11*src->_22 + src->_01*src->_12*src->_20 + src->_02*src->_10*src->_21 -
              src->_20*src->_11*src->_02 - src->_10*src->_01*src->_22 - src->_00*src->_21*src->_12;

  if( det == 0.0f ) return 0;

  det = 1.0f / det;

  MTX33 temp;
  temp._00 =  (src->_11*src->_22 - src->_21*src->_12) * det;
  temp._01 = -(src->_01*src->_22 - src->_21*src->_02) * det;
  temp._02 =  (src->_01*src->_12 - src->_11*src->_02) * det;

  temp._10 = -(src->_10*src->_22 - src->_20*src->_12) * det;
  temp._11 =  (src->_00*src->_22 - src->_20*src->_02) * det;
  temp._12 = -(src->_00*src->_12 - src->_10*src->_02) * det;

  temp._20 =  (src->_10*src->_21 - src->_20*src->_11) * det;
  temp._21 = -(src->_00*src->_21 - src->_20*src->_01) * det;
  temp._22 =  (src->_00*src->_11 - src->_10*src->_01) * det;

  gfl::math::MTX33Copy(dest, &temp);
  return 1;
}

inline void MTX33Scale(MTX33* dest, const VEC3* scale)
{
  dest->m[0][0] = scale->x, dest->m[0][1] =     0.0f, dest->m[0][2] =     0.0f;
  dest->m[1][0] =     0.0f, dest->m[1][1] = scale->y, dest->m[1][2] =     0.0f;
  dest->m[2][0] =     0.0f, dest->m[2][1] =     0.0f, dest->m[2][2] = scale->z;
}

inline void MTX33RotXRad(MTX33* dest, const float rad)
{
  //  1  0  0
  //  0  c -s
  //  0  s  c
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  dest->m[0][0] = 1.0f, dest->m[0][1] = 0.0f, dest->m[0][2] = 0.0f;
  dest->m[1][0] = 0.0f, dest->m[1][1] =  cos, dest->m[1][2] = -sin;
  dest->m[2][0] = 0.0f, dest->m[2][1] =  sin, dest->m[2][2] =  cos;
}

inline void MTX33RotYRad(MTX33* dest, const float rad)
{
  //  c  0  s
  //  0  1  0
  // -s  0  c
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  dest->m[0][0] =  cos, dest->m[0][1] = 0.0f, dest->m[0][2] =  sin;
  dest->m[1][0] = 0.0f, dest->m[1][1] = 1.0f, dest->m[1][2] = 0.0f;
  dest->m[2][0] = -sin, dest->m[2][1] = 0.0f, dest->m[2][2] =  cos;
}

inline void MTX33RotZRad(MTX33* dest, const float rad)
{
  //  c -s  0
  //  s  c  0
  //  0  0  1
  float sin, cos;
  gfl::math::SinCosRad(&sin, &cos, rad);
  dest->m[0][0] =  cos, dest->m[0][1] = -sin, dest->m[0][2] = 0.0f;
  dest->m[1][0] =  sin, dest->m[1][1] =  cos, dest->m[1][2] = 0.0f;
  dest->m[2][0] = 0.0f, dest->m[2][1] = 0.0f, dest->m[2][2] = 1.0f;
}

inline void MTX33RotAxisRad(MTX33* dest, const VEC3* axis, const float rad)
{
  float s, c;
  gfl::math::SinCosRad(&s, &c, rad);
  const float t = 1.0f - c;

  const float x = axis->x;
  const float y = axis->y;
  const float z = axis->z;
  const float xSq = x*x;
  const float ySq = y*y;
  const float zSq = z*z;
  const float sx  = s*x;
  const float sy  = s*y;
  const float sz  = s*z;
  const float txy = t*x*y;
  const float txz = t*x*z;
  const float tyz = t*y*z;

  dest->m[0][0] = ( t*xSq ) + ( c  );
  dest->m[0][1] = ( txy   ) - ( sz );
  dest->m[0][2] = ( txz   ) + ( sy );

  dest->m[1][0] = ( txy   ) + ( sz );
  dest->m[1][1] = ( t*ySq ) + ( c  );
  dest->m[1][2] = ( tyz   ) - ( sx );

  dest->m[2][0] = ( txz   ) - ( sy );
  dest->m[2][1] = ( tyz   ) + ( sx );
  dest->m[2][2] = ( t*zSq ) + ( c  );
}

inline void MTX33SetAxis(MTX33* dest, const VEC3& axisX, const VEC3& axisY, const VEC3& axisZ)
{
  dest->m[0][0] = axisX.x;
  dest->m[0][1] = axisY.x;
  dest->m[0][2] = axisZ.x;

  dest->m[1][0] = axisX.y;
  dest->m[1][1] = axisY.y;
  dest->m[1][2] = axisZ.y;

  dest->m[2][0] = axisX.z;
  dest->m[2][1] = axisY.z;
  dest->m[2][2] = axisZ.z;
}

inline void MTX33Slerp(MTX33* dest, const MTX33& src1, const MTX33& src2, const float ratio)
{
  MTX34 temp1(src1);
  nn::math::QUAT quat1; nn::math::MTX34ToQUAT(&quat1, &temp1);

  MTX34 temp2(src2);
  nn::math::QUAT quat2; nn::math::MTX34ToQUAT(&quat2, &temp2);

  nn::math::QUAT quat3; QUATSlerp(&quat3, &quat1, &quat2, ratio);
  MTX34 temp3; nn::math::QUATToMTX34(&temp3, &quat3);
  nn::math::MTX34ToMTX33(dest, &temp3);
}

inline void MTX33Normalize(MTX33* dest, const MTX33& src)
{
  VEC3 axisX; MTX33GetAxisX(&axisX, src); VEC3Normalize(&axisX, &axisX);
  VEC3 axisY; MTX33GetAxisY(&axisY, src); VEC3Normalize(&axisY, &axisY);
  VEC3 axisZ; MTX33GetAxisZ(&axisZ, src); VEC3Normalize(&axisZ, &axisZ);
  MTX33SetAxis(dest, axisX, axisY, axisZ);
}

inline void MTX34InverseOrtho(MTX34* dest, const MTX34* src)
{
  MTX33 inMat33(*src);
  MTX33InverseOrtho(&inMat33, &inMat33);
  const VEC3 trans(-inMat33.m[0][0]*src->m[0][3] - inMat33.m[0][1]*src->m[1][3] - inMat33.m[0][2]*src->m[2][3], 
                   -inMat33.m[1][0]*src->m[0][3] - inMat33.m[1][1]*src->m[1][3] - inMat33.m[1][2]*src->m[2][3],
                   -inMat33.m[2][0]*src->m[0][3] - inMat33.m[2][1]*src->m[1][3] - inMat33.m[2][2]*src->m[2][3]);
  MTX34SetMTX33VEC3(dest, inMat33, trans);
}

inline void MTX34SetAxisTranslate(MTX34* dest, const VEC3& axisX, const VEC3& axisY, const VEC3& axisZ, const VEC3& posi)
{
  dest->m[0][0] = axisX.x, dest->m[0][1] = axisY.x, dest->m[0][2] = axisZ.x, dest->m[0][3] = posi.x;
  dest->m[1][0] = axisX.y, dest->m[1][1] = axisY.y, dest->m[1][2] = axisZ.y, dest->m[1][3] = posi.y;
  dest->m[2][0] = axisX.z, dest->m[2][1] = axisY.z, dest->m[2][2] = axisZ.z, dest->m[2][3] = posi.z;
}

inline void MTX34SetMTX33(MTX34* dest, const MTX33& sr)
{
  dest->m[0][0] = sr.m[0][0], dest->m[0][1] = sr.m[0][1], dest->m[0][2] = sr.m[0][2];
  dest->m[1][0] = sr.m[1][0], dest->m[1][1] = sr.m[1][1], dest->m[1][2] = sr.m[1][2];
  dest->m[2][0] = sr.m[2][0], dest->m[2][1] = sr.m[2][1], dest->m[2][2] = sr.m[2][2];
}
inline void MTX34SetMTX33VEC3(MTX34* dest, const MTX33& sr, const VEC3& posi)
{
  dest->m[0][0] = sr.m[0][0], dest->m[0][1] = sr.m[0][1], dest->m[0][2] = sr.m[0][2], dest->m[0][3] = posi.x;
  dest->m[1][0] = sr.m[1][0], dest->m[1][1] = sr.m[1][1], dest->m[1][2] = sr.m[1][2], dest->m[1][3] = posi.y;
  dest->m[2][0] = sr.m[2][0], dest->m[2][1] = sr.m[2][1], dest->m[2][2] = sr.m[2][2], dest->m[2][3] = posi.z;
}

inline void MTX34Slerp(MTX34* dest, const MTX34& src1, const MTX34& src2, const float ratio)
{
  MTX33 rot;   MTX33Slerp(&rot, MTX33(src1), MTX33(src2), ratio);
  VEC3  begin; MTX34GetTranslate(&begin, src1);
  VEC3  end;   MTX34GetTranslate(&end,   src2);
  VEC3  posi = end - begin;
  VEC3Scale(&posi, &posi,  ratio);
  VEC3Add  (&posi, &begin, &posi);
  MTX34SetMTX33VEC3(dest, rot, posi);
}

} /* end of namespace math */
} /* end of namespace gfl */

#endif // __GFL_MTX_H__
