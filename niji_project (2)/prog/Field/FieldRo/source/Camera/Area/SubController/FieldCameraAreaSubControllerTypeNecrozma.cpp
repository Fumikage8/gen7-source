//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeNecrozma.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( ネクロズママップ用 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeNecrozma.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"

// debug
#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>
#endif // #if PM_DEBUG


#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "GameSys/include/GameManager.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
GFL_NAMESPACE_BEGIN( SubController );

static const gfl2::math::Vector3 sc_CenterPosition( 2100.0f, 0.0f, 2100.0f );     // 螺旋階段の中心(Y座標は無し)
SubControllerTypeNecrozma::SETTING SubControllerTypeNecrozma::m_Setting = 
{
  // @note デバッグメニューで操作するため、度で設定。カメラに適用する際にラディアンに変換している
  {  
    gfl2::math::Vector3( 0.0f, 133.33f, 0.0f ),                           //gfl2::math::Vector3  offset;
    gfl2::math::Vector3( 0.0f, 0.0f, -12.2f ),                             // gfl2::math::Vector3 rotation;
    35.0f,                                                                // f32                 fovY;
    1500.0f,                                                               // f32                distance;
    0.0f,
  },
  {  
    gfl2::math::Vector3( 0.0f, 133.33f, 0.0f ),                           //gfl2::math::Vector3  offset;
    gfl2::math::Vector3( 0.0f, 0.0f, -16.0f),                              // gfl2::math::Vector3 rotation;
    35.0f,                                                                // f32                 fovY;
    500.0f,                                                               // f32                distance;
    915.0f,
  },
  gfl2::math::Easing::LINEAR,
  15,                                                                     // interpolationFrame;
  {  
    gfl2::math::Vector3( 0.0f, 133.33f -42.1f, -5.3f ),                   //gfl2::math::Vector3  offset;
    gfl2::math::Vector3( -17.0f, -64.2f, 0.0f),                            // gfl2::math::Vector3 rotation;
    35.0f,                                                                // f32                 fovY;
    1511.5f,                                                              // f32                distance;
    100.0f,             // f32 effectiveHeight;    // この高さ以下なら有効
    -1.39161253,        // f32 startDirection;     // 開始角度
    -1.12143135f,       // f32 endDirection;       // 終了角度
  },
};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
SubControllerTypeNecrozma::SubControllerTypeNecrozma( const Target::TargetBase* pTarget, const Area::Interpolation* pIpo ) :
  m_pParamRatio( NULL )
#if PM_DEBUG
  ,m_IsDebugEffect( false )
  ,m_IsPrevDebugUpdate( false )
  ,m_IsPutsInfoFile( false )
#endif
{
  m_pTarget = pTarget;
  m_pIpo = pIpo;
#if 0
  m_Data = rData;
  m_DataNo = dataNo;
  m_pSupport = pSupport;
  m_pParamRatio = pParamRatio;
  m_pFocus = pFocus;
#endif

  GameSys::GameManager*     game_manager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  game_manager->GetFieldmap()->GetCameraUiDeviceTranslator()->SetMode( UiDeviceTranslator::Mode::DIRECT );
}

/**
* @brief デストラクタ
*
* @return 無し
*/
SubControllerTypeNecrozma::~SubControllerTypeNecrozma( void )
{
  GameSys::GameManager*     game_manager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  if (game_manager->GetFieldmap()->GetCameraUiDeviceTranslator())
  {
    game_manager->GetFieldmap()->GetCameraUiDeviceTranslator()->SetMode(UiDeviceTranslator::Mode::NORMAL);
  }
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void SubControllerTypeNecrozma::Update( poke_3d::model::BaseCamera* pCamera )
{
  // 接線ベクトルを求める
  gfl2::math::Vector3 baseDirection;
  gfl2::math::Vector3 cameraTargetRowPosition;
  m_pTarget->GetTargetRowPosition( &cameraTargetRowPosition );

  baseDirection = cameraTargetRowPosition - sc_CenterPosition;
  baseDirection.y = 0.0f;
  baseDirection = gfl2::math::Matrix34::GetRotationY( gfl2::math::ConvDegToRad( 90 ) ) * baseDirection;
  baseDirection.NormalizeRet();
  f32 direction = atan2( baseDirection.z, baseDirection.x ) * -1.0f - PI * 0.5f;
  f32 rawDirection = direction;

  // 高さから適用すべき設定を取得
  Area::MAIN_PARAM param; // ラジアン
  f32 height = cameraTargetRowPosition.y;
  GetDataByHeight( height, &param );

  // 向きを反映
  gfl2::math::Vector3 rotation = param.rotation;

  // エントランスと補間する領域は向きを加工する。Y回転のターンがおきないようにするため。
  if (height <= m_Setting.entrance.effectiveHeight )
  {
    if (rawDirection <= m_Setting.entrance.endDirection)
    {
      direction = m_Setting.entrance.endDirection;
    }
  }
  rotation.y += direction;

  // カメラに設定
  gfl2::math::Vector3 fixTargetPosition;
  gfl2::math::Vector3 fixCameraPosition;
  gfl2::math::Vector3 cameraOffset(0.0f, 0.0f, param.distance );
  gfl2::math::Matrix34 rotMat(gfl2::math::Matrix34::GetRotationZYX(rotation.z, rotation.y, rotation.x));
  cameraOffset = rotMat.TransformCoord(cameraOffset);
  fixTargetPosition = cameraTargetRowPosition + param.offset;
  fixCameraPosition = fixTargetPosition + cameraOffset;

  // エントランスと補間
  if (height <= m_Setting.entrance.effectiveHeight )
  {
    MAIN_PARAM paramC(m_Setting.entrance.param);
    Area::Utility::ConvertDegToRad(&paramC.rotation);
    paramC.fovY = gfl2::math::ConvDegToRad(paramC.fovY);
    
    gfl2::math::Vector3 workTargetPosition;
    gfl2::math::Vector3 workCameraPosition;
    gfl2::math::Vector3 cameraOffset(0.0f, 0.0f, paramC.distance);
    gfl2::math::Matrix34 rotMat(gfl2::math::Matrix34::GetRotationZYX(paramC.rotation.z, paramC.rotation.y, paramC.rotation.x));
    cameraOffset = rotMat.TransformCoord(cameraOffset);
    workTargetPosition = cameraTargetRowPosition + paramC.offset;
    workCameraPosition = workTargetPosition + cameraOffset;
    
    f32 t = (rawDirection - m_Setting.entrance.startDirection) / (m_Setting.entrance.endDirection - m_Setting.entrance.startDirection );
    t = gfl2::math::Clamp(t, 0.0f, 1.0f);

    workTargetPosition.Lerp( fixTargetPosition, t );
    workCameraPosition.Lerp( fixCameraPosition, t );

    fixTargetPosition = workTargetPosition; 
    fixCameraPosition = workCameraPosition;
  }

  f32 fovy = param.fovY;
  pCamera->SetFovy(fovy);
  pCamera->SetupCameraLookAt(
    fixCameraPosition,
    fixTargetPosition,
    gfl2::math::Vector3(0.0f, 1.0f, 0.0f)
    );
}

/**
 * @brief　高さから適用する設定を取得します
 *
 * @param  height　高さ
 * @param  pOut 結果を格納するバッファです
 *
 * @return 無し
*/
void SubControllerTypeNecrozma::GetDataByHeight(f32 height, MAIN_PARAM* pOut)
{
  if (height <= m_Setting.start.height)
  {
    *pOut = m_Setting.start.param;
     Area::Utility::ConvertDegToRad( &pOut->rotation );
     pOut->fovY = gfl2::math::ConvDegToRad( pOut->fovY );
    return;
  }

  if (height >= m_Setting.end.height)
  {
    *pOut = m_Setting.end.param;
     Area::Utility::ConvertDegToRad( &pOut->rotation );
     pOut->fovY = gfl2::math::ConvDegToRad( pOut->fovY );
    return;
  }

  f32 t = (height - m_Setting.start.height) / (m_Setting.end.height - m_Setting.start.height);
  t = gfl2::math::Easing::GetValue( m_Setting.easingFunc, t );

  MAIN_PARAM paramA( m_Setting.start.param );
  MAIN_PARAM paramB( m_Setting.end.param );

  Area::Utility::ConvertDegToRad( &paramA.rotation );
  Area::Utility::ConvertDegToRad( &paramB.rotation );
  paramA.fovY = gfl2::math::ConvDegToRad( paramA.fovY );
  paramB.fovY = gfl2::math::ConvDegToRad( paramB.fovY );

  Area::Utility::Lerp( pOut, paramA, paramB, t );
}


GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );