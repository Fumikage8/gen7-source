//======================================================================
/**
* @file FieldCameraAreaSubControllerTypeFollow.cpp
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドカメラエリアの子操作( 単純追従型 )
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

// niji
#include "GameSys/include/GameManager.h"

// Field
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeFollow.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"

#if PM_DEBUG
// debug
#include <fs/include/gfl2_Fs.h>
#include <fs/include/gfl2_FsPcUtil.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif // #if PM_DEBUG

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
GFL_NAMESPACE_BEGIN( SubController );

static const f32 X_LIMIT( gfl2::math::ConvDegToRad( 85.0f ) );

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
SubControllerTypeFollow::SubControllerTypeFollow( 
  const DATA& rData, 
  const Target::TargetBase* pTarget,
  Area::Support*            pSupport,
  Area::Focus*              pFocus,
  s32 dataNo
#if PM_DEBUG
  ,b32 isDebugEffect
#endif
  )
#if PM_DEBUG
  :
  m_IsDebugEffect( isDebugEffect )
  ,m_IsPrevDebugUpdate( false )
  ,m_IsPutsInfoFile( false )
  ,m_pMsgData( NULL )
#endif
{
  m_Data = rData;
  m_DataNo = dataNo;
  m_pTarget = pTarget;
  m_pSupport = pSupport;
  m_pFocus = pFocus;

#if PM_DEBUG
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork ) m_pMsgData = devWork->GetMessageData();
#endif  //#if PM_DEBUG 
}

/**
* @brief デストラクタ
*
* @return 無し
*/
SubControllerTypeFollow::~SubControllerTypeFollow( void )
{

}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void SubControllerTypeFollow::Update( poke_3d::model::BaseCamera* pCamera )
{
  gfl2::math::Vector3 targetObjectPosition;
  gfl2::math::Vector3 fixTargetPosition;
  gfl2::math::Vector3 fixCameraPosition;
  m_pTarget->GetCameraTargetPosition( &targetObjectPosition );

  // デバッグ操作
#if PM_DEBUG
  m_IsPutsInfoFile = false;
  if( this->GetDebugControlFlag() )
  {
    UpdateDebug( pCamera );
  }
#endif

  // パラメータから最新の座標を計算
  f32 distance = m_Data.param.distance;
  gfl2::math::Vector3 rotation = m_Data.param.rotation;

  distance = m_Data.param.distance;
  
  // 距離サポートによる距離の加工
  if( m_pSupport->IsValid() )
  {
    distance = m_pSupport->ConvertDistance( distance );
  }

  // カメラに設定
  rotation = m_Data.param.rotation;
  gfl2::math::Vector3 cameraOffset( 0.0f, 0.0f, distance );
  gfl2::math::Matrix34 rotMat( gfl2::math::Matrix34::GetRotationZYX( rotation.z, rotation.y, rotation.x ) );
  cameraOffset = rotMat.TransformCoord( cameraOffset );
  fixTargetPosition = targetObjectPosition + m_Data.param.offset;
  fixCameraPosition = fixTargetPosition + cameraOffset;

  // フォーカス更新のため、一旦カメラオブジェクトに反映
  f32 fovy = m_Data.param.fovY;
  pCamera->SetFovy( fovy );
  pCamera->SetupCameraLookAt(
    fixCameraPosition,
    fixTargetPosition,
    gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
    );

  // フォーカス処理
  m_pFocus->Update( pCamera, m_Data.param.distance );
  m_pFocus->Execute( pCamera );
}

#if PM_DEBUG

/**
* @brief デバッグ処理(Updateで呼ばれる)
*
* @param  pCamera           操作するカメラの実体
* @param  pAreaDataManager  エリア設定データ
*
* @return 無し
*/
void SubControllerTypeFollow::UpdateDebugDraw( poke_3d::model::BaseCamera* pCamera, const Area::AreaDataManager* pAreaDataManager )
{
  if( !Debug::DebugTypes::s_pDrawUtilTextUpper )
  {
    return;
  }

  Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
  Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 0.75f, 0.75f );

  gfl2::math::Vector2 start( 5.0f, 5.0f );
  gfl2::math::Vector2 pos( 0.0f, 0.0f );
  pos = start;
  f32 lineDistance = 12.0f;

#define CAMERA_DRAW_TEXT( ... ) Debug::DebugTypes::s_pDrawUtilTextUpper->DrawText( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;

  // 実際に操作している時は文字色を変更します
  if( m_IsDebugEffect )
  {
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
  }
  CAMERA_DRAW_TEXT( L"TYPE[Static]" );
  CAMERA_DRAW_TEXT( L"補間フレーム[--]" );
  CAMERA_DRAW_TEXT( L"プライオリティ[--]" );
  CAMERA_DRAW_TEXT( L"" );
  CAMERA_DRAW_TEXT( L"PointParam" );
  CAMERA_DRAW_TEXT( L"-Setting[--]" );
  CAMERA_DRAW_TEXT( L"-オフセット[%f,%f,%f]", m_Data.param.offset.x, m_Data.param.offset.y, m_Data.param.offset.z );
  CAMERA_DRAW_TEXT( L"-軸回転[%f,%f,%f]", 
    gfl2::math::ConvRadToDeg( m_Data.param.rotation.x ),
    gfl2::math::ConvRadToDeg( m_Data.param.rotation.y ),
    gfl2::math::ConvRadToDeg( m_Data.param.rotation.z )
    );

  CAMERA_DRAW_TEXT( L"-画角[%f]", gfl2::math::ConvRadToDeg( m_Data.param.fovY ) );
  CAMERA_DRAW_TEXT( L"-距離[%f]", m_Data.param.distance );
}

/**
* @brief カメラ情報を出力
*
* @param  pCamera           操作するカメラの実体
* @param  pAreaDataManager  エリア設定データ
*
* @return 無し
*/
void SubControllerTypeFollow::PutsInfoFile( poke_3d::model::BaseCamera* pCamera, const Area::AreaDataManager* pAreaDataManager )
{
  bool enableFlag;
  gfl2::fs::Result result = gfl2::fs::PcUtil::IsPcAccessEnableFlag( &enableFlag );
  if( result.IsSuccess() == false || enableFlag == false )
  {
    return;
  }

  char buffer[2048];
  u32 start = 0;

#if defined(GF_PLATFORM_WIN32)
#define CAMERA_PUTS_TEXT( text, ... ) start += sprintf_s( &buffer[ start ], sizeof( buffer ) - start, text, __VA_ARGS__ )
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_PUTS_TEXT( ... ) start += sprintf( &buffer[ start ], __VA_ARGS__ )
#endif


  // Mayaインポート用情報出力
  start = 0;
  CAMERA_PUTS_TEXT( "<Root>\r\n");
  CAMERA_PUTS_TEXT( "  <Data>\r\n");
  CAMERA_PUTS_TEXT( "    <MainParam>\r\n");
  CAMERA_PUTS_TEXT( "      <Type>Static</Type>\r\n");
  CAMERA_PUTS_TEXT( "      <CompFrame>%d</CompFrame>\r\n", 15 ); // @note 運用ルール、15固定
  CAMERA_PUTS_TEXT( "      <PointAParam>\r\n");

  if( m_IsDebugEffect )
  {
    CAMERA_PUTS_TEXT( "        <Setting>UserSet</Setting>\r\n");
    CAMERA_PUTS_TEXT( "        <Ofs>%f %f %f</Ofs>\r\n", m_Data.param.offset.x, m_Data.param.offset.y, m_Data.param.offset.z );
    CAMERA_PUTS_TEXT( "        <Axis>%f %f %f</Axis>\r\n", 
      gfl2::math::ConvRadToDeg( m_Data.param.rotation.x ),
      gfl2::math::ConvRadToDeg( m_Data.param.rotation.y ),
      gfl2::math::ConvRadToDeg( m_Data.param.rotation.z ) );
    CAMERA_PUTS_TEXT( "        <Fov>%f</Fov>\r\n", gfl2::math::ConvRadToDeg( m_Data.param.fovY ) );
    CAMERA_PUTS_TEXT( "        <Distance>%f</Distance>\r\n", m_Data.param.distance );
  }
  else
  {
    CAMERA_PUTS_TEXT( "        <Setting>Default</Setting>\r\n");
  }

  CAMERA_PUTS_TEXT( "      </PointAParam>\r\n");
  CAMERA_PUTS_TEXT( "    </MainParam>\r\n");
  CAMERA_PUTS_TEXT( "  </Data>\r\n");
  CAMERA_PUTS_TEXT( "</Root>\r\n");
  
  buffer[ start ] = '\0';
  gfl2::fs::PcUtil::WriteFile( "R:/home/momiji/tool/FieldTools/Maya/Resource/camera_adjustment.txt", buffer, start );
}

#endif // #if PM_DEBUG




//
// private
//


#if PM_DEBUG

/**
* @brief デバッグ操作
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void SubControllerTypeFollow::UpdateDebug( poke_3d::model::BaseCamera* pCamera )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::VectorDevice* pVecDevice = pDeviceManager->GetStick(gfl2::ui::DeviceManager::STICK_STANDARD);
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  gfl2::math::Vector3 pos( 0.0f, 0.0f, 0.0f );
  gfl2::math::Vector3 pos_r( 0.0f, 0.0f, 0.0f );
//  bool pad_u = false;
//  bool pad_d = false;
//  bool pad_l = false;
//  bool pad_r = false;

  pos_r.x = pVecDevice->GetX();
  pos_r.y = pVecDevice->GetY();
//  pad_u = pButton->IsHold( gfl2::ui::BUTTON_UP );
//  pad_d = pButton->IsHold( gfl2::ui::BUTTON_DOWN );
//  pad_l = pButton->IsHold( gfl2::ui::BUTTON_LEFT );
//  pad_r = pButton->IsHold( gfl2::ui::BUTTON_RIGHT );

  f32 stickValueX = pVecDevice->GetX();
  f32 stickValueY = pVecDevice->GetY();

  b32 debugUpdate = false;

  // 単軸モード
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    if( gfl2::math::Abs( stickValueX ) > gfl2::math::Abs( stickValueY ) )
    {
      stickValueY = 0.0f;
    }
    else
    {
      stickValueX = 0.0f;
    }
  }

  // 画角
  {
    f32 moveValue( 0.1f );
    // 加速
    {
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        moveValue = 0.4f;
      }
    }

    if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )
    {
      m_Data.param.fovY -= gfl2::math::ConvDegToRad( moveValue );
    }
    else if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )
    {
      m_Data.param.fovY += gfl2::math::ConvDegToRad( moveValue );

    }

    m_Data.param.fovY = gfl2::math::Clamp( m_Data.param.fovY, gfl2::math::ConvDegToRad( 1.0f ), gfl2::math::ConvDegToRad( 180.0f ) );
  }

  // 上下左右平行移動
  if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {
    if( stickValueX != 0.0f || stickValueY != 0.0f )
    {
      f32 moveValue( 2.0f );
      gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );

      // 加速
      {
        if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
        {
          moveValue = 10.0f;
        }
      }

      // 入力値からローカル空間での移動量を算出
      {
        move.x = stickValueX * moveValue;
        move.y = stickValueY * moveValue;
      }

      // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
      {
        gfl2::math::Matrix34 rotMat;
        pCamera->GetRotationQuat().QuaternionToMatrix( rotMat );
        move = rotMat.TransformCoord( move );
        m_Data.param.offset += move;
      }

      debugUpdate = true;
    }
  }
  // 奥行左右平行移動
  else if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    f32 moveValue( 2.0f );
    gfl2::math::Vector3 offsetMove( 0.0f, 0.0f, 0.0f );
    f32 distanceMove( 0.0f );

    // 加速
    {
      if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
      {
        moveValue = 10.0f;
      }
    }

    // 入力値からローカル空間での移動量を算出
    {
      offsetMove.x = stickValueX * moveValue;
      distanceMove = stickValueY * moveValue * -1.0f;
    }

    // 距離
    {
      if( distanceMove != 0.0f )
      {
        m_Data.param.distance += distanceMove;
        m_Data.param.distance = gfl2::math::Clamp( m_Data.param.distance, 1.0f, 100000.0f );
        debugUpdate = true;
      }
    }

    // ローカル移動量をワールド移動量に変換し、現在座標に足しこむ
    {
      gfl2::math::Matrix34 rotMat;
      pCamera->GetRotationQuat().QuaternionToMatrix( rotMat );
      offsetMove = rotMat.TransformCoord( offsetMove );
      if( offsetMove != gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ) )
      {
        m_Data.param.offset += offsetMove;
        debugUpdate = true;
      }
    }
  }
  // 回転、距離
  else
  {
    b32 speedUp( false );
    if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
    {
      speedUp = true;
    }

    // 回転
    if( stickValueX != 0.0f || stickValueY != 0.0f )
    {
      f32 moveValue( 0.025f );
      gfl2::math::Vector3 move( 0.0f, 0.0f, 0.0f );
      if( speedUp )
      {
        moveValue = 0.05f;
      }

      move.y = stickValueX * moveValue * -1.0f;
      move.x = stickValueY * moveValue * -1.0f;

      m_Data.param.rotation += move;

      // X軸の回転には制限をつける
      m_Data.param.rotation.x = gfl2::math::Clamp( m_Data.param.rotation.x, X_LIMIT * -1.0f, X_LIMIT );
      debugUpdate = true;
    }

  } // 回転、距離


  if( debugUpdate )
  {
    m_IsDebugEffect = true;
  }
  else
  {
    // デバッグ操作が終了した瞬間に情報を出力する
    if( m_IsPrevDebugUpdate )
    {
      m_IsPutsInfoFile = true;
    }
  }

  m_IsPrevDebugUpdate = debugUpdate;
}

#endif // #if PM_DEBUG

GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );