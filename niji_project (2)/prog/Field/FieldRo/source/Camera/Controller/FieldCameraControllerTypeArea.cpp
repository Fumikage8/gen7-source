/**
* @brief  フィールドカメラの操作、カメラエリア用
* @file   FieldCameraControllerTypeArea.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSys/include/GameManager.h"

// camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaDataManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaCollisionManager.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaScrollStopManager.h"

// SubController
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeFollow.h"
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeFollowParamIpo.h"
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeHold.h"
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeNecrozma.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <util/include/gfl2_DrawUtilText.h>
#include "Field/FieldRo/include/Fieldmap.h"

// Debug
#include <debug/include/gfl2_DebugWinSystem.h>

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

#if PM_DEBUG
ControllerTypeArea::DEBUG_SETTING ControllerTypeArea::m_DebugSetting =
{
  false // b32 invalidShake;     // カメラシェイク無効化
};
#endif // #if PM_DEBUG


void ConvertArrayToVector3( const f32* pSrc, gfl2::math::Vector3* pDist )
{
  pDist->x = pSrc[0];
  pDist->y = pSrc[1];
  pDist->z = pSrc[2];
}

void ConvertBinToParam( const Area::PointData& rSrc, Area::MAIN_PARAM* pDist )
{
  pDist->distance = rSrc.distance;
  pDist->fovY = gfl2::math::ConvDegToRad( rSrc.fovY );
  ConvertArrayToVector3( &rSrc.offset[0], &pDist->offset );
  ConvertArrayToVector3( &rSrc.rotation[0], &pDist->rotation );
  Area::Utility::ConvertDegToRad( &pDist->rotation );
}

void ConvertBinToParam( const Area::BIN_SUPPORT_MOVE_SETTING_DATA& rSrc, Area::SUPPORT_MOVE_SETTING_DATA* pDist )
{
  pDist->waitFrame = rSrc.waitFrame;
  pDist->moveFrame = rSrc.moveFrame;
  pDist->easeFunc = static_cast<gfl2::math::Easing::EaseFunc>( rSrc.easeFunc );
}

void ConvertBinToParam( const Area::BIN_SUPPORT_DATA& rSrc, Area::CAMERA_AREA_SUPPORT_SETTING_DATA* pDist )
{
  pDist->directionData = rSrc.directionData;
  pDist->maxValue = rSrc.maxValue;
  pDist->type = rSrc.type;

  ConvertBinToParam( rSrc.inData, &pDist->inData );
  ConvertBinToParam( rSrc.outData, &pDist->outData );
}

void ConvertCameraParamToMainParam( const Target::TargetBase* pTarget, const poke_3d::model::BaseCamera* pCamera, Area::MAIN_PARAM* pDist )
{
  gfl2::math::Vector3 targetPositon;
  pTarget->GetCameraTargetPosition( &targetPositon );
  gfl2::math::Vector3 cameraTargetPositon = pCamera->GetTargetPosition();
  gfl2::math::Vector3 cameraPositon = pCamera->GetPosition();

  pDist->offset = cameraTargetPositon - targetPositon;
  pDist->fovY = pCamera->GetFovy();
  pDist->distance = ( cameraPositon - cameraTargetPositon ).Length();

  // Z軸回転無視
  Area::Utility::ConvertQuatToEuler( pCamera->GetRotationQuat(), &pDist->rotation );
}

#if PM_DEBUG
static const c16* DEBUG_TEXT_USER_SET = L"UserSet";
static const c16* DEBUG_TEXT_DEFAULT = L"Default";

const c16* GetSettingText( s32 value )
{
  if( value )
  {
    return DEBUG_TEXT_DEFAULT;
  }
  else
  {
    return DEBUG_TEXT_USER_SET;
  }
}

gfl2::math::Vector2 DrawPointParam( gfl2::util::DrawUtilText* pDrawText, const gfl2::math::Vector2& rPos, f32 lineDistance, const c16* pHeaderName, const Area::PointData& rPointData )
{
  gfl2::math::Vector2 pos = rPos;

#if defined(GF_PLATFORM_WIN32)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawTextW( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawText( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#endif


  CAMERA_DRAW_TEXT( L"%ls", pHeaderName );
  CAMERA_DRAW_TEXT( L"-Setting[%ls]", GetSettingText( rPointData.defaultFlag ) );
  CAMERA_DRAW_TEXT( L"-オフセット[%f,%f,%f]", rPointData.offset[0], rPointData.offset[1], rPointData.offset[2] );
  CAMERA_DRAW_TEXT( L"-軸回転[%f,%f,%f]", rPointData.rotation[0], rPointData.rotation[1], rPointData.rotation[2] );
  CAMERA_DRAW_TEXT( L"-画角[%f]", rPointData.fovY );
  CAMERA_DRAW_TEXT( L"-距離[%f]", rPointData.distance );

  return pos;
}

void DrawCameraArea( gfl2::util::DrawUtilText* pDrawText, const Area::AreaData& rData )
{
  gfl2::math::Vector2 start( 5.0f, 5.0f );
  gfl2::math::Vector2 pos( 0.0f, 0.0f );
  pos = start;
  f32 lineDistance = 12.0f;
  pDrawText->SetTextScale( 0.75f, 0.75f );
  pDrawText->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );

#if defined(GF_PLATFORM_WIN32)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawTextW( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawText( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#endif

  // ゾーンデフォルト設定、置き換えデータの場合は表記を追加する
  const c16* pDefaultText = L"[Default]";
  const c16* pReplaceText = L"[Replace]";
  const c16* pExType = L"";
  if( rData.isDefault )
  {
    pExType = pDefaultText;
  }
  else if( rData.isReplace )
  {
    pExType = pReplaceText;
  }

  switch( rData.commonData.type )
  {
  case Area::DataType::FOLLOW:
    {
      CAMERA_DRAW_TEXT( L"TYPE[Static]%ls", pExType );
      CAMERA_DRAW_TEXT( L"補間フレーム[%d]", rData.commonData.interpolationFrame );
      CAMERA_DRAW_TEXT( L"プライオリティ[%d]", rData.commonData.priority );
      CAMERA_DRAW_TEXT( L"" );
      pos = DrawPointParam( pDrawText, pos, lineDistance, L"PointParam", rData.follow.pointData );
      break;
    }
  case Area::DataType::FOLLOW_TRANS:
    {
      CAMERA_DRAW_TEXT( L"TYPE[%ls]%ls", Area::Utility::GetEasingFuncLable( static_cast< gfl2::math::Easing::EaseFunc > ( rData.followTrans.easeFunc ) ), pExType );
      CAMERA_DRAW_TEXT( L"補間フレーム[%d]", rData.commonData.interpolationFrame );
      CAMERA_DRAW_TEXT( L"プライオリティ[%d]", rData.commonData.priority );
      CAMERA_DRAW_TEXT( L"" );
      pos = DrawPointParam( pDrawText, pos, lineDistance, L"PointAParam", rData.followTrans.pointDataA );
      pos = DrawPointParam( pDrawText, pos, lineDistance, L"PointBParam", rData.followTrans.pointDataB );
      break;
    }
  case Area::DataType::HOLD:
    {
      CAMERA_DRAW_TEXT( L"TYPE[Hold]%ls", pExType );
      CAMERA_DRAW_TEXT( L"補間フレーム[%d]", rData.commonData.interpolationFrame );
      CAMERA_DRAW_TEXT( L"プライオリティ[%d]", rData.commonData.priority );
      CAMERA_DRAW_TEXT( L"" );
      CAMERA_DRAW_TEXT( L"HoldParam" );

      if( rData.hold.playerFlag )
      {
        CAMERA_DRAW_TEXT( L"-Player[true]" );
      }
      else
      {
        CAMERA_DRAW_TEXT( L"-Player[false]" );
      }
      CAMERA_DRAW_TEXT( L"-オフセット[%f,%f,%f]", rData.hold.offset[0], rData.hold.offset[1], rData.hold.offset[2] );
      CAMERA_DRAW_TEXT( L"-注視点[%f,%f,%f]", rData.hold.target[0], rData.hold.target[1], rData.hold.target[2] );
      CAMERA_DRAW_TEXT( L"-カメラ位置[%f,%f,%f]", rData.hold.position[0], rData.hold.position[1], rData.hold.position[2] );
      CAMERA_DRAW_TEXT( L"-画角[%f]", rData.hold.fovY );
      CAMERA_DRAW_TEXT( L"-バンク角[%f]", rData.hold.bank );
      break;
    }
  }

}
#endif

ControllerTypeArea::SETTING ControllerTypeArea::m_Setting =
{
   gfl2::math::Easing::OUT_SINE // gfl2::math::Easing::EaseFunc ::areaIpoEaseFunc;   // カメラエリア変更の遷移で使用するイージング関数
};


/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
ControllerTypeArea::ControllerTypeArea( const InitializeDescription& rInitDesc ) : 
  // 内部制御
  m_NowAreaDataNo( Area::AreaDataNo::INVALID )
  ,m_OldAreaDataNo( Area::AreaDataNo::INVALID )
  ,m_NowRatio( 0.0f )
  ,m_PrevAreaCheckPosition( -9999.0f, -9999.0f, -9999.0f )
  ,m_pNowAreaData( NULL )
  ,m_PrevTargetObjectPosition( 0.0f, 0.0f, 0.0f )
  ,m_PrevCameraTargetPosition( 0.0f, 0.0f, 0.0f )
  ,m_PrevCameraPosition( 0.0f, 0.0f, 0.0f )

#if PM_DEBUG
  ,m_DebugSupportSettingOverRideFlag( false )
  ,m_IsDebugController( false )
  ,m_IsDebugOpen( false )
#endif // #if PM_DEBUG

  // 内部オブジェクト
  ,m_pSubController( NULL )
  ,m_pLocalHeapForSubController( NULL )
  ,m_pSupport( NULL )
  ,m_pFocus( NULL )

  // 外部オブジェクト
  ,m_pBaseCamera( NULL )
  ,m_pFieldMap( rInitDesc.m_pFieldMap )
  ,m_pAreaDataManager( rInitDesc.m_pAreaDataManager )
  ,m_pAreaCollisionManager( rInitDesc.m_pAreaCollisionManager )
  ,m_pScrollStopManager( rInitDesc.m_pScrollStopManager )
  ,m_pTarget( rInitDesc.m_pTarget )
  ,m_pCallBack( NULL )
{
  {
    Area::Focus::InitializeDescription initDesc;
    initDesc.m_pTarget = m_pTarget;
    initDesc.m_pPlacementDataManager = rInitDesc.m_pPlacementDataManager;
    m_pFocus = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::Focus( initDesc );
  }
  {
    Area::Support::InitializeDescription initDesc;
    initDesc.m_pTarget = m_pTarget;
    initDesc.m_pFocus = m_pFocus;
    initDesc.m_pMoveModelPlayer = m_pFieldMap->GetPlayerCharacter();
    m_pSupport = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::Support( initDesc );
  }

  size_t workSize = gfl2::heap::Manager::CalcCreateHeapSize( 1024, gfl2::heap::HEAP_TYPE_EXP );
  m_pLocalHeapForSubController = GFL_CREATE_LOCAL_HEAP_NAME( rInitDesc.m_pSystemHeap, workSize, gfl2::heap::HEAP_TYPE_EXP, false, "FieldCameraSubController" );
  m_pNowAreaData = GFL_NEW( rInitDesc.m_pSystemHeap ) Area::AreaData;

  m_OutSideInterpolation.Clear();
  m_InSideInterpolation.Clear();
  m_horizonSpring.Init();
  m_verticalSpring.Init();
}

/**
* @brief デストラクタ
*
* @return 無し
*/
ControllerTypeArea::~ControllerTypeArea( void )
{
  GFL_SAFE_DELETE( m_pSubController );
  GFL_SAFE_DELETE( m_pNowAreaData );

  if( m_pLocalHeapForSubController )
  {
    GFL_DELETE_HEAP( m_pLocalHeapForSubController );
    m_pLocalHeapForSubController = NULL;
  }
  
  GFL_SAFE_DELETE( m_pFocus );
  GFL_SAFE_DELETE( m_pSupport );
}


/**
* @brief 初期化処理
* @note  ControllerがCameraUnitの設定された直後に実行される
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::SetUp( poke_3d::model::BaseCamera* pCamera )
{
  m_pBaseCamera = pCamera;
}

/**
* @brief 更新処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::Update( poke_3d::model::BaseCamera* pCamera )
{
  // @note 下記更新の反映はSubコントローラー内部で行っている

  // バネ更新
  m_horizonSpring.Update();
  m_verticalSpring.Update();

  // 内部補間更新
  m_InSideInterpolation.Update();

  // 外部補間更新
  m_OutSideInterpolation.Update();

  // 所属するカメラエリアを更新し、SubControllerの切り替えを行う処理
#if PM_DEBUG
  if( m_IsDebugController == false )
#endif // #if PM_DEBUG
  {
    UpdateCameraArea();
  }

#if PM_DEBUG
  // デバッグ調整機能、カメラサポート設定の強制上書き
  if( Debug::DebugTypes::s_CameraSupportEnable )
  {
    m_DebugSupportSettingOverRideFlag = true;
    // m_pSupport->Change( Debug::DebugTypes::s_CameraSupport );
  }
  else if( m_DebugSupportSettingOverRideFlag )
  {
    // デバッグ判定をリセット
    m_DebugSupportSettingOverRideFlag = false;
    ChangeSubController( *m_pNowAreaData, true );
  }
#endif // #if PM_DEBUG

  // サポート挙動更新
  m_pSupport->Update();

  // フォーカス挙動更新
  // m_pFocus->Update( pCamera );

#if PM_DEBUG

  // デバッグメニュー操作Open中の処理
  if( m_IsDebugOpen )
  {
    // デバッグウィンドウが閉じたらデバッグカメラ操作をOFFにする。　デバッグメニュー側の制御で閉じた瞬間のキャッチが出来なかったのでこうしている
    m_pSubController->SetDebugControlFlag( gfl2::debug::DebugWin_IsOpen() );
  }

  // サポートデバッグ監視
  if( Debug::DebugTypes::s_CameraSupportDebugMode == Debug::DebugTypes::CameraSupportDebugMode::MAX_FIX )
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
    gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  }
#endif // #if PM_DEBUG

  // エリアデータに合わせたカメラ更新処理
  if( m_pSubController )
  {
    // @note SubControllerが返すのは最新の望ましい位置のみにし、バネの反映とカメラへの最終適応をControllerTypeArea::Update内部で行うようにする
    m_pSubController->Update( pCamera );

#if PM_DEBUG
    
    // カメラ情報表示(テキスト)
    if( m_pSubController->GetDebugControlFlag() )
    {
      m_pSubController->UpdateDebugDraw( pCamera, m_pAreaDataManager );
    }
    else
    {
      // カメラエリア設定を表示
      if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::DRAW_CAMERA_AREA_DATA ) )
      {
        DrawCameraArea( Field::Debug::DebugTypes::s_pDrawUtilTextUpper, *m_pNowAreaData );
      }
    }

    // カメラ情報出力
    if( m_pSubController->IsPutsInfoFile() )
    {
      m_pSubController->PutsInfoFile( pCamera, m_pAreaDataManager );
    }

#endif // #if PM_DEBUG
  }

  // @fix GFNMcat[1736][1547]スクロールストップとTRフォーカスカメラ同時に発生するとアングルがおかしくなる、TRフォーカス中にカメラエリアの設定が切り替わるとカクつく場合がある、対処。フォーカスに渡す座標にスクロールストップ結果や補間が含まれていた事が原因。各モジュールの更新順を根本的に見直した
  gfl2::math::Vector3 fixTargetObjectPosition;
  m_pTarget->GetCameraTargetPosition( &fixTargetObjectPosition );
  gfl2::math::Vector3 fixCameraPosition( pCamera->GetPosition() );
  gfl2::math::Vector3 fixTargetPosition( pCamera->GetTargetPosition() );
  
  if (m_pCallBack)
  {
    m_pCallBack->InSideInterpolationPrev( this, &fixCameraPosition, &fixTargetPosition );
  }

  // 内部補間
  if( m_InSideInterpolation.IsValid() )
  {
    m_InSideInterpolation.Get(
      fixTargetObjectPosition,
      fixCameraPosition,
      fixTargetPosition,
      &fixCameraPosition,
      &fixTargetPosition
      );
  }

  // カメラエリア変化時の補間に仕様する更新結果を記憶
  m_pTarget->GetCameraTargetPosition( &m_PrevTargetObjectPosition );
  m_PrevCameraTargetPosition = fixTargetPosition;
  m_PrevCameraPosition = fixCameraPosition;

  // スクロールストップ処理
  gfl2::math::Vector3 workTargetPosition( fixTargetPosition );
  if( m_pScrollStopManager->Execute( fixTargetPosition, &fixTargetPosition ) )
  {
    fixCameraPosition += ( fixTargetPosition - workTargetPosition );
    m_pSupport->SetScrollStopFlag( true );
  }
  else
  {
    m_pSupport->SetScrollStopFlag( false );
  }

  // 外部補間
  if( m_OutSideInterpolation.IsValid() )
  {
    m_OutSideInterpolation.Get(
      fixTargetObjectPosition,
      fixCameraPosition,
      fixTargetPosition,
      &fixCameraPosition,
      &fixTargetPosition
      );
  }

  // 振動
#if PM_DEBUG
  if( !m_DebugSetting.invalidShake )
#endif // #if PM_DEBUG
  {
    // @note ワールド座標系でX軸を揺らしている為カメラ角によってい揺れ方が変わるが、仕様。
    fixCameraPosition.x += m_horizonSpring.GetPostion();
    fixTargetPosition.x += m_horizonSpring.GetPostion();
    fixCameraPosition.y += m_verticalSpring.GetPostion();
    fixTargetPosition.y += m_verticalSpring.GetPostion();
  }

  // カメラ実体に設定
  {
    pCamera->SetupCameraLookAt(
      fixCameraPosition,
      fixTargetPosition,
      gfl2::math::Vector3( 0.0f, 1.0f, 0.0f ) 
      );
  }

  // 更新結果をゲームデータに保存
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::Camera::SaveData saveData;
    
    // @fix cov_ctr:[11304]:操作していないメンバ変数が未初期化で上書きされていた。操作しないメンバ変数は現在の値が適用されるように修正
    pGameManager->GetGameData()->GetFieldCameraData( &saveData );

    saveData.areaId = pGameManager->GetGameData()->GetFieldArea()->GetAreaID();
    saveData.areaDataNo = m_NowAreaDataNo;
    saveData.oldAreaDataNo = m_OldAreaDataNo;

    pGameManager->GetGameData()->SetFieldCameraData( saveData );
  }
}

/**
* @brief 再開処理
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::Resume( poke_3d::model::BaseCamera* pCamera )
{
  // 各モジュールを強制リセット
  m_pSupport->Reset();
  m_pFocus->Reset();
  m_horizonSpring.Init();
  m_verticalSpring.Init();
  m_OutSideInterpolation.Clear();
  m_InSideInterpolation.Clear();

  // 最新のカメラエリア設定に更新
  UpdateCameraArea( true );
}

/**
* @brief 補間を打ち切り、最新の状態に更新する
*
* @param  pCamera 操作するカメラの実体
*
* @return 無し
*/
void ControllerTypeArea::Fix( poke_3d::model::BaseCamera* pCamera )
{
  // 各モジュールを強制リセット
  m_pSupport->Reset();
  m_pFocus->Reset();
  m_horizonSpring.Init();
  m_verticalSpring.Init();
  m_OutSideInterpolation.Clear();
  m_InSideInterpolation.Clear();

  // 最新のカメラエリア設定に更新
  UpdateCameraArea( true );

  // 通常更新
  Update( pCamera );
}

/**
* @brief フィールド操作開始時に実行する処理
*
* @return 無し
*/
void ControllerTypeArea::Ready( void )
{
#if PM_DEBUG
  m_pFocus->ResistDebugMenu();
  m_pSupport->ResistDebugMenu();
#endif
}

/**
* @brief  補間指定(外部指定)
*
* @param  rTargetPosition カメラターゲット座標(始点)
* @param  rCameraPosition カメラ座標(始点)
* @param  frame           補間にかけるフレーム
*
* @return 成否(コントローラーの初期化が終わっていない場合は失敗する)
*/
b32 ControllerTypeArea::SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc )
{
  // ターゲットオブジェクトのローカル座標系で記憶
  gfl2::math::Vector3 targetPostion;
  if( m_pTarget == NULL || m_pTarget->GetCameraTargetPosition( &targetPostion ) == false )
  {
    return false;
  }

  m_OutSideInterpolation.SetUp( targetPostion, rCameraPosition, rTargetPosition, frame );
  m_OutSideInterpolation.SetEaseFunc( easeFunc );
  return true;
}

/**
* @brief カメラサポート挙動の制御アクションの実行
*
* @param  value     アクション種別
*
* @return 成否
*/
b32 ControllerTypeArea::SetSupportAction( SupportAction::Value value )
{
  if( m_pSupport )
  {
    return m_pSupport->Action( value );
  }
  else
  {
    return false;
  }
}

#if PM_DEBUG
void ControllerTypeArea::DebugModeStart( void )
{
  if( m_IsDebugController == false )
  {
    // フォーカス＆サポートを強制リセット
    m_pSupport->Reset();
    m_pFocus->Reset();
    m_InSideInterpolation.Clear();
    m_pSubController->Update( m_pBaseCamera );

    // 最新の状態を元にStaticカメラを生成
    Area::SubController::SubControllerTypeFollow::DATA data;
    ConvertCameraParamToMainParam( m_pTarget, m_pBaseCamera,  &data.param );

    GFL_SAFE_DELETE( m_pSubController );
    m_pSubController = GFL_NEW( m_pLocalHeapForSubController ) Area::SubController::SubControllerTypeFollow(
      data,
      m_pTarget,
      m_pSupport,
      m_pFocus,
      m_NowAreaDataNo
      );

    m_IsDebugController = true;
  }

  m_pSubController->SetDebugControlFlag( true );
  m_IsDebugOpen = true;
}

void ControllerTypeArea::DebugModeEnd( b32 resetFlag )
{
  m_IsDebugOpen = false;

  if( resetFlag )
  {
    DebugModeReset();
  }
  else
  {
    m_pSubController->SetDebugControlFlag( false );
  }
}
void ControllerTypeArea::DebugModeReset( void )
{
  m_IsDebugOpen = false;
  m_IsDebugController = false;
  UpdateCameraArea( true );
}
#endif

//
// private:
//

/**
* @brief 所属するカメラエリアを更新し、SubControllerの切り替えを行う処理
* @note  カメラエリア制御座標が前回から変わっていない場合はカメラエリアの更新はスキップされます
*
* @param force true カメラエリア制御座標が前回から変わっていなくてもカメラエリアの更新を行う
*
* @return 無し
*/
void ControllerTypeArea::UpdateCameraArea( b32 force )
{
  // コリジョン判定は座標が替わった時のみ行う
  // データがヒットしなかった場合はエリア単位で指定されているデフォルト設定を使用する
  u32 newAreaDataId = Area::AreaDataNo::INVALID;
  u32 oldAreaDataId = m_NowAreaDataNo;

  // 始めてなら補間できない
  b32 firstFlag( false );
  if( m_NowAreaDataNo == Area::AreaDataNo::INVALID )
  {
    firstFlag = true;
  }

  // コリジョンによるエリアデータ更新、座標が更新されたときのみ行う
  gfl2::math::Vector3 cameraControlPosition;
  if( m_pTarget->GetCameraControlPosition( &cameraControlPosition ) )
  {
    if( ( cameraControlPosition != m_PrevAreaCheckPosition || force ) && m_pAreaCollisionManager )
    {
      Area::AreaCollisionManager::HitResult hitResult;
      m_pAreaCollisionManager->Check( cameraControlPosition, *m_pAreaDataManager, &hitResult );

      if( hitResult.m_IsHit )
      {
        newAreaDataId = hitResult.m_AreaDataNo;
        m_NowRatio = hitResult.m_Ratio;
      }
      else
      {
        // HITしなかったらDEFAULTに切り替える
        newAreaDataId = Area::AreaDataNo::DEFAULT;
      }

      // コリジョンに使用した座標を記憶
      m_PrevAreaCheckPosition = cameraControlPosition;

    } // コリジョンによるエリアデータ更新、座標が更新されたときのみ行う
  }
  else
  {
    // 座標の取得に失敗した場合は現在の値を継続(ライドチェンジなどで発生)
    if( m_NowAreaDataNo == Area::AreaDataNo::INVALID )
    {
      newAreaDataId = Area::AreaDataNo::DEFAULT;
    }
    else
    {
      newAreaDataId = m_NowAreaDataNo;
    }
  }

  // 適用データが無効のままになる場合は、デフォルトに切り替える
  if( newAreaDataId == Area::AreaDataNo::INVALID && m_NowAreaDataNo == Area::AreaDataNo::INVALID )
  {
    newAreaDataId = Area::AreaDataNo::DEFAULT;
  }

  // 初回更新の場合は、移動元をゲームデータから復帰する
  if( firstFlag )
  {
    u32 work;
    if( this->RecoveryCameraArea( newAreaDataId, &work ) )
    {
      oldAreaDataId = work;
    }
  }

  // 適用データを切り替える処理
  if( newAreaDataId != Area::AreaDataNo::INVALID && ( m_NowAreaDataNo != newAreaDataId || force ) )
  {
    if( newAreaDataId == Area::AreaDataNo::DEFAULT )
    {
      if( m_pAreaDataManager->GetDefaultDataByZoneId( m_pFieldMap->GetZoneID(), m_pNowAreaData ) == false )
      {
        // 取得に失敗した際はプログラムで指定されている値を使用する
        // GFL_ASSERT( 0 );
        m_pAreaDataManager->GetDefaultData( m_pNowAreaData );
      }
    }
    else
    {
      if( m_pAreaDataManager->GetData( m_pNowAreaData, m_pFieldMap->GetZoneID(), newAreaDataId, oldAreaDataId ) == false )
      {
        // 取得に失敗した際はプログラムで指定されている値を使用する
        // GFL_ASSERT( 0 );
        m_pAreaDataManager->GetDefaultData( m_pNowAreaData );
      }
    }

    // @fix GFNMcat[5465]:前のエリアの情報が復帰時に破棄される、対処。上書き結果が反映されている変数を設定。
    m_OldAreaDataNo = oldAreaDataId;
    m_NowAreaDataNo = newAreaDataId;
    ChangeSubController( *m_pNowAreaData, firstFlag | force );
  }

}

/**
* @brief ゲームデータから直前のカメラNoを復帰する
*
* @param nowAreaDataNo  復帰地点のエリアデータNo
* @param pOldAreaDataNo 直前のカメラNo格納先
*
* @return true 復帰した false 復帰条件を満たしておらず、復帰しなかった
*/
b32 ControllerTypeArea::RecoveryCameraArea( u32 nowAreaDataNo, u32* pOldAreaDataNo )
{
  *pOldAreaDataNo =  Area::AreaDataNo::INVALID;

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Field::Camera::SaveData saveData;
  pGameManager->GetGameData()->GetFieldCameraData( &saveData );

  // 保存されていない
  if( saveData.areaId == Field::AREAID_ERROR )
  {
    return false;
  }

  // 違うエリアID
  if( saveData.areaId != pGameManager->GetGameData()->GetFieldArea()->GetAreaID() )
  {
    return false;
  }

  // データNoが不正
  if( saveData.areaDataNo == Area::AreaDataNo::INVALID )
  {
    return false;
  }

  // デフォルトでは例外カメラを使用しない運用 ＆ ゾーンまたぎで違うデータの可能性あり
  if( saveData.areaDataNo == Area::AreaDataNo::DEFAULT )
  {
    return false;
  }

  // 復帰地点が違う場合は復帰しない
  if( nowAreaDataNo != saveData.areaDataNo )
  {
    return false;
  }

  *pOldAreaDataNo = saveData.oldAreaDataNo;
  return true;
}

void ControllerTypeArea::ChangeSubController( const Area::AreaData& rData, b32 force )
{
  GFL_SAFE_DELETE( m_pSubController );

  u32 interpolationFrame = rData.commonData.interpolationFrame;
  if( force )
  {
    interpolationFrame = 0;
  }

  // 補間設定
  if( interpolationFrame == 0 )
  {
    m_InSideInterpolation.Clear();
  }
  else
  {
    m_InSideInterpolation.SetUp(
       m_PrevTargetObjectPosition,
       m_PrevCameraPosition,
       m_PrevCameraTargetPosition,
       interpolationFrame
      );

    m_InSideInterpolation.SetEaseFunc( m_Setting.areaIpoEaseFunc );
  }


  switch( rData.commonData.type )
  {
  case Area::Type::FOLLOW:
    {
      Area::SubController::SubControllerTypeFollow::DATA data;
      ConvertBinToParam( rData.follow.pointData, &data.param );

      m_pSubController = GFL_NEW( m_pLocalHeapForSubController ) Area::SubController::SubControllerTypeFollow(
        data,
        m_pTarget,
        m_pSupport,
        m_pFocus,
        m_NowAreaDataNo
        );

      // サポート挙動
      Area::CAMERA_AREA_SUPPORT_SETTING_DATA supportData;
      ConvertBinToParam( rData.follow.supportData, &supportData );
      m_pSupport->Change( supportData, force );

      break;
    }

  case Area::Type::FOLLOW_IPO:
    {
      Area::SubController::SubControllerTypeFollowParamIpo::DATA data;
      ConvertBinToParam( rData.followTrans.pointDataA, &data.paramA );
      ConvertBinToParam( rData.followTrans.pointDataB, &data.paramB);
      data.paramEaseFunc = static_cast< gfl2::math::Easing::EaseFunc >( rData.followTrans.easeFunc );

      m_pSubController = GFL_NEW( m_pLocalHeapForSubController ) Area::SubController::SubControllerTypeFollowParamIpo(
        data,
        &m_NowRatio,
        m_pTarget,
        m_pSupport,
        m_pFocus,
        m_NowAreaDataNo
        );

      // サポート挙動
      Area::CAMERA_AREA_SUPPORT_SETTING_DATA supportData;
      ConvertBinToParam( rData.followTrans.supportData, &supportData );
      m_pSupport->Change( supportData, force );
      
      break;
    }

  case Area::Type::HOLD:
    {
      Area::SubController::SubControllerTypeHold::DATA data;
      data.isTargetFocus = rData.hold.playerFlag;
      data.offset = gfl2::math::Vector3( rData.hold.offset[0], rData.hold.offset[1], rData.hold.offset[2] );
      data.position = gfl2::math::Vector3( rData.hold.position[0], rData.hold.position[1], rData.hold.position[2] );
      data.target = gfl2::math::Vector3( rData.hold.target[0], rData.hold.target[1], rData.hold.target[2] );
      data.bank = gfl2::math::ConvDegToRad( rData.hold.bank );
      data.fovY= gfl2::math::ConvDegToRad( rData.hold.fovY );

      m_pSubController = GFL_NEW( m_pLocalHeapForSubController ) Area::SubController::SubControllerTypeHold(
        data,
        m_pTarget,
        m_NowAreaDataNo
        );

      break;
    }

  case Area::Type::NECROZMA:
    {
      m_pSubController = GFL_NEW( m_pLocalHeapForSubController ) Area::SubController::SubControllerTypeNecrozma(
        m_pTarget,
        &m_InSideInterpolation
        );
      break;
    }

  default:
    {
      // 知らない型、どうしようもない
      GFL_ASSERT_STOP( 0 );
    }
  } // switch( rData.dataType )

  if (m_pCallBack)
  {
    m_pCallBack->AreaChangeAfter( this, m_NowAreaDataNo );
  }
}



GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );