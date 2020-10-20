//======================================================================
/**
* @file FieldCameraAreaUtility.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリア関連のUtil関数定義
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// gfl2
#include <math/include/gfl2_EulerRotation.h>
#include <math/include/gfl2_Quaternion.h>

//poke_3d
#include <model/include/gfl2_BaseCamera.h>

// Field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );


#if PM_DEBUG
struct TEXT_INFO 
{
  const c16* pText;
  s32        value;
};

static const TEXT_INFO sc_DebugEasingLabelTable[] = 
{
  L"LINEAR", gfl2::math::Easing::LINEAR,
  L"IN_SINE", gfl2::math::Easing::IN_SINE,
  L"OUT_SINE", gfl2::math::Easing::OUT_SINE,
  L"INOUT_SINE", gfl2::math::Easing::INOUT_SINE,

  L"IN_QUAD", gfl2::math::Easing::IN_QUAD,
  L"OUT_QUAD", gfl2::math::Easing::OUT_QUAD,
  L"INOUT_QUAD", gfl2::math::Easing::INOUT_QUAD,

  L"IN_CUBIC", gfl2::math::Easing::IN_CUBIC,
  L"OUT_CUBIC", gfl2::math::Easing::OUT_CUBIC,
  L"INOUT_CUBIC", gfl2::math::Easing::INOUT_CUBIC,

  L"IN_QUART", gfl2::math::Easing::IN_QUART,
  L"OUT_QUART", gfl2::math::Easing::OUT_QUART,
  L"INOUT_QUART", gfl2::math::Easing::INOUT_QUART,
  L"IN_QUINT", gfl2::math::Easing::IN_QUINT,
  L"OUT_QUINT", gfl2::math::Easing::OUT_QUINT,
  L"INOUT_QUINT", gfl2::math::Easing::INOUT_QUINT,

  L"IN_EXPO", gfl2::math::Easing::IN_EXPO,
  L"OUT_EXPO", gfl2::math::Easing::OUT_EXPO,
  L"INOUT_EXPO", gfl2::math::Easing::INOUT_EXPO,

  L"IN_CIRC", gfl2::math::Easing::IN_CIRC,
  L"OUT_CIRC", gfl2::math::Easing::OUT_CIRC,
  L"INOUT_CIRC", gfl2::math::Easing::INOUT_CIRC,

  L"IN_BACK", gfl2::math::Easing::IN_BACK,
  L"OUT_BACK", gfl2::math::Easing::OUT_BACK,
  L"INOUT_BACK", gfl2::math::Easing::INOUT_BACK,

  L"IN_ELASTIC", gfl2::math::Easing::IN_ELASTIC,
  L"OUT_ELASTIC", gfl2::math::Easing::OUT_ELASTIC,
  L"INOUT_ELASTIC", gfl2::math::Easing::INOUT_ELASTIC,

  L"IN_BOUNCE", gfl2::math::Easing::IN_BOUNCE,
  L"OUT_BOUNCE", gfl2::math::Easing::OUT_BOUNCE,
  L"INOUT_BOUNCE", gfl2::math::Easing::INOUT_BOUNCE,



};
static const c16* ERROR_TEXT = L"ERR";
#endif




/**
* @brief カメラパラメーターの補間
* @note  ターゲットオブジェク制御座標からの相対座標で補間する
*
* @param  pOut        結果格納先( r1、r2と別のインスタンスを指定 )
* @param  r1          0.0地点のパラメータ
* @param  r2          1.0地点のパラメータ
* @param  t           現在位置( 0.0f ～ 1.0f )
*
* @return 無し
*/
void Utility::Lerp( MAIN_PARAM* pOut, const MAIN_PARAM& r1, const MAIN_PARAM& r2, f32 t )
{
  // 線形補間
  pOut->fovY = gfl2::math::Lerp( r1.fovY, r2.fovY, t );
  pOut->distance = gfl2::math::Lerp( r1.distance, r2.distance, t );
  pOut->offset = r1.offset;
  pOut->offset.Lerp( r2.offset, t );

  // 回転パラメータは球面補間
  gfl2::math::EulerRotation rot1;
  gfl2::math::EulerRotation rot2;

  gfl2::math::Quaternion q1;
  gfl2::math::Quaternion q2;

  rot1.Set( r1.rotation, gfl2::math::EulerRotation::RotationOrderZYX );
  rot2.Set( r2.rotation, gfl2::math::EulerRotation::RotationOrderZYX );

  q1 = rot1.ToQuaternion();
  q2 = rot2.ToQuaternion();

  q1.Slerp( q2, t );
  q1.QuaternionToRadian( &pOut->rotation.x, &pOut->rotation.y, &pOut->rotation.z );
}

void Utility::ConvertQuatToEuler( const gfl2::math::Quaternion& rSrc, gfl2::math::Vector3* pOut )
{
  // クォータニオンからYXZ順の回転情報を復元
  // クォータニオンの回転順は問わない、たとえばLookAtを使っている場合はYXZ順になっていない
  // @note 計算誤差が出てしまう、極力フィールド使用型を保持する事

  gfl2::math::Vector3 nowRot( 0.0f, 0.0f, 0.0f );
  {
    gfl2::math::Matrix34 rotMat;
    rSrc.QuaternionToMatrix( rotMat );
    
    // Y回転を求める、XZ平面で傾きを求める
    gfl2::math::Vector3 front = rotMat * gfl2::math::Vector3( 0.0f, 0.0f, 1.0f );
    gfl2::math::Vector3 frontWork = gfl2::math::Vector3( front.x, 0.0f, front.z ).Normalize();
    nowRot.y = atan2( frontWork.x, frontWork.z );
   
    // X回転を求める、Y回転を打ち消しYZ平面で傾きを求める
    rotMat = gfl2::math::Matrix34::GetRotationY( - nowRot.y ) * rotMat;
    front = rotMat * gfl2::math::Vector3( 0.0f, 0.0f, 1.0f );
    nowRot.x = atan2( front.y * -1.0f, front.z );

    // Z回転を求める、Y回転とX回転を打ち消しYX平面で傾きを求める
    rotMat = gfl2::math::Matrix34::GetRotationX( - nowRot.x ) * rotMat;
    gfl2::math::Vector3 up = rotMat * gfl2::math::Vector3( 0.0f, 1.0f, 0.0f );
    nowRot.z = atan2( up.x * -1.0f, up.y );
  }

  *pOut = nowRot;
}


/**
 * @brief イベントスクリプト用のカメラパラメータの取得
 * @note  アニメーション再生中はTargetPositionが最後に指定された座標になっている
 */
void Utility::GetEventScriptCameraParameter( const poke_3d::model::BaseCamera* cpCamera, gfl2::math::Vector3 * pCameraTargetPos, gfl2::math::Vector3* pCameraPos, gfl2::math::Vector3* pCameraRotate, f32* pCameraDistance )
{
  *pCameraTargetPos = cpCamera->GetTargetPosition();
  *pCameraPos = cpCamera->GetPosition();
  *pCameraDistance = ( *pCameraPos - *pCameraTargetPos ).Length();
  ConvertQuatToEuler( cpCamera->GetRotationQuat(), pCameraRotate );
}

#if PM_DEBUG
// EaseFuncの値から表示テキストを取得
const c16* Utility::GetEasingFuncLable( gfl2::math::Easing::EaseFunc func )
{
  const TEXT_INFO* pInfo = &sc_DebugEasingLabelTable[ 0 ];
  for( u32 i = 0; i < GFL_NELEMS( sc_DebugEasingLabelTable ); i++, pInfo++ )
  {
    if( pInfo->value == func )
    {
      return pInfo->pText;
    }
  }
  return ERROR_TEXT;
}
#endif

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );