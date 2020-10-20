/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenuCameraGroup.cpp
* @author ikuta_junya
* @data   2015.03.08
*/

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <math/include/gfl2_Easing.h>

// niji
#include "System/include/HeapDefine.h"
#include "GameSys/include/GameManager.h"
#include "Sound/include/Sound.h"

// Field
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/include/Event/FieldDebugEventMapCapture.h"

#include "Field/FieldRo/include/ActionCommand/FieldActionCommandStepJump.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeRideRoba2.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenuCameraGroup.h"
#include "Field/FieldRo/include/Event/FieldEventCameraSystemTest.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugOrthoView.h"
#include "Field/FieldRo/include/Event/FieldEventDebugCameraAdjustPosition.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeArea.h"
#include "Field/FieldRo/include/Camera/Area/SubController/FieldCameraAreaSubControllerTypeNecrozma.h"

// @note フィールド上のデバック定義は可能な限りここに集める
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"

#include "FieldScript/include/FieldScriptSystem.h"

#if defined(GF_PLATFORM_WIN32)
#include "Field/FieldRo/include/Debug/FieldDebugNameManglingManager.h"
#endif

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );


// デバッグメニューで項目を選択中に実行される関数
GFL_NAMESPACE_BEGIN( Item );

// Cameraグループ
GFL_NAMESPACE_BEGIN( Camera );


// カメラ位置調整用イベント起動
static void CallCameraAdjustPositionEvent( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 強制STOP解除
  Debug::DebugTypes::SetSwitchValue( Debug::DebugTypes::Switch::CAMERA_STOP, 0 );
  
  Field::Event::EventDebugCameraAdjustPosition* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Field::Event::EventDebugCameraAdjustPosition >( pGameManager->GetGameEventManager() );
  
  Field::Event::EventDebugCameraAdjustPosition::InitializeDescription initDesc;
  initDesc.m_pHeap = NULL;
  initDesc.dummy = 0;
  pEvent->Initialize( initDesc );

  // フィールドオブジェクトをイベント起動状態にする @todo 共通処理化
  {
    FieldScript::FieldScriptSystem* pScriptSystem = pGameManager->GetFieldScriptSystem();
    if( pScriptSystem->IsRunning() == false ){
      pGameManager->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
    }
  }

  // メニューを閉じる
  gfl2::debug::DebugWin_CloseWindow();
}

// 通常カメラに戻す
static void ResetNormalCamera( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 強制STOP解除
  Debug::DebugTypes::SetSwitchValue( Debug::DebugTypes::Switch::CAMERA_STOP, 0 );

  // 正射影カメラコントローラ破棄
  Field::Camera::CameraManager* pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();
  pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::DEBUG )->ChangeController( NULL );
  pCameraManager->ClearOverrideViewCamera();

  // コントローラーリセット
  Field::Camera::Controller::ControllerTypeArea* pAreaController = pCameraManager->GetAreaController();

  // JFでは無い
  if( pAreaController )
  {
    pAreaController->DebugModeReset();
  }

}

// 小数部切捨て
static void RoundDown( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  gfl2::math::Vector3* pValue = static_cast<gfl2::math::Vector3*>( pUserWork );
  pValue->x = static_cast<s32>( pValue->x );
  pValue->y = static_cast<s32>( pValue->y );
  pValue->z = static_cast<s32>( pValue->z );
}

static void SetZero_f32( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  f32* pValue = static_cast<f32*>( pUserWork );
  *pValue = 0.0f;
}

static void SetControlPointToCameraPositon( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Field::Debug::Event::EventMapCapture::SETTING* pSetting = Field::Debug::Event::EventMapCapture::GetSettingPtr();
  pSetting->control_position = pSetting->camera_position;
}



// 動作テスト用イベント起動
#if ENABLE_DEBUG_CAMERA_SYSTEM_TEST
static void CallCameraTestEvent( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Event::EventCameraSystemTest* pEventCameraSystemTest = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Event::EventCameraSystemTest >( pGameManager->GetGameEventManager() );
  
  Event::EventCameraSystemTest::InitializeDescription initDesc;
  initDesc.m_pHeap = NULL;
  initDesc.dummy = 0;
  pEventCameraSystemTest->Initialize( initDesc );

  // メニューを閉じる
  gfl2::debug::DebugWin_CloseWindow();
}
#endif

// 正射影カメラに変更
static void ChangeOthroCamera( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 正射影カメラコントローラ作成
  Field::Camera::CameraManager* pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();
  gfl2::heap::HeapBase* pHeap =  gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  Field::Camera::Controller::ControllerTypeDebugOrthoView* pController = GFL_NEW( pHeap ) Field::Camera::Controller::ControllerTypeDebugOrthoView(
    pCameraManager->GetTarget()
    );

  // 表示カメラのみを差し替え
  pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::DEBUG )->ChangeController( pController );
  pCameraManager->OverrideViewCameraByUnitNo( Field::Camera::UnitNo::DEBUG );
}

// 正射影カメラ終了
static void EndOthroCamera( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // 正射影カメラコントローラ破棄
  Field::Camera::CameraManager* pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();
  pCameraManager->GetCameraByUnitNo( Field::Camera::UnitNo::DEBUG )->ChangeController( NULL );
  pCameraManager->ClearOverrideViewCamera();
}

wchar_t* DispSupportDebugMode( void* userWork, void* pValue, gfl2::debug::DebugWinItem *item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32 value = *(s32*)pValue;

  wchar_t* table[] =
  {
    L"OFF",
    L"MIN_FIX",
    L"MAX_FIX",
    L"ERROR",
  };

  if( value >= GFL_NELEMS( table ) )
  {
    value = GFL_NELEMS( table ) - 1;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), table[ value ] );
  return workStr;
}


// フォーカスカメラチェック、距離に陸上ライド最大値を設定
static void SetFocusCameraDistance_GroundRideMax( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  DebugTypes::s_FocusCameraCheck.distance = 750 + 400;
}

// フォーカスカメラチェック、距離に海ライド最大値を設定
static void SetFocusCameraDistance_NaminoriRideMax( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  DebugTypes::s_FocusCameraCheck.distance = 850 + 400;
}

// フォーカスカメラチェック、距離に徒歩最大値を設定
static void SetFocusCameraDistance_NoRideMax( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  DebugTypes::s_FocusCameraCheck.distance = 550 + 400;
}

// マップ撮影イベント起動
static void CallMapCaptureEvent( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  // イベント中は起動不可
  if( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameEventManager()->IsExists() )
  {
    // 撮影イベントが起動している場合は終了リクエストを出す
    if( Field::Debug::Event::EventMapCapture::GetSettingPtr()->is_event_call )
    {
      Field::Debug::Event::EventMapCapture::GetSettingPtr()->is_end_req = true;
    }
    return;
  }

  // 強制STOP解除
  Debug::DebugTypes::SetSwitchValue( Debug::DebugTypes::Switch::CAMERA_STOP, 0 );

  Field::Debug::Event::EventMapCapture* pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, Field::Debug::Event::EventMapCapture >( pGameManager->GetGameEventManager() );

  Field::Debug::Event::EventMapCapture::InitializeDescription initDesc;
  initDesc.dummy = 0;
  pEvent->Initialize( initDesc );

  // フィールドオブジェクトをイベント起動状態にする
  {
    FieldScript::FieldScriptSystem* pScriptSystem = pGameManager->GetFieldScriptSystem();
    if( pScriptSystem->IsRunning() == false ){
      pGameManager->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
    }
  }

  // メニューを閉じる
  gfl2::debug::DebugWin_CloseWindow();

}


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Item );

GFL_NAMESPACE_BEGIN( GroupFunc );


// カメラデバッグ操作開始
static void CameraDebugStartSetUp( void *userWork, gfl2::debug::DebugWinItem *item )
{
  // キャラをとめる
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // フィールドオブジェクトをイベント起動状態にする
  {
    FieldScript::FieldScriptSystem* pScriptSystem = pGameManager->GetFieldScriptSystem();
    if( pScriptSystem->IsRunning() == false )
    {
      pGameManager->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
    }
  }
  // 強制STOP解除
  Debug::DebugTypes::SetSwitchValue( Debug::DebugTypes::Switch::CAMERA_STOP, 0 );

  Field::Camera::CameraManager* pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();
  Field::Camera::Controller::ControllerTypeArea* pAreaController = pCameraManager->GetAreaController();

  // JFでは無い
  if( pAreaController )
  {
    pAreaController->DebugModeStart();
  }
}

// カメラデバッグ操作終了
static void CameraDebugEndSetUp( void *userWork, gfl2::debug::DebugWinItem *item )
{
  // キャラを動かす
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  // フィールドオブジェクトをイベント無効状態にする
  {
    FieldScript::FieldScriptSystem* pScriptSystem = pGameManager->GetFieldScriptSystem();
    if( pScriptSystem->IsRunning() == false )
    {
      pGameManager->GetFieldmap()->GetMoveModelManager()->PullEventRequestAll();
    }
  }

  Field::Camera::CameraManager* pCameraManager = pGameManager->GetFieldmap()->GetCameraManager();
  Field::Camera::Controller::ControllerTypeArea* pAreaController = pCameraManager->GetAreaController();

  // JFでは無い
  if( pAreaController )
  {
    pAreaController->DebugModeEnd();
  }
}


/**
 *  @brief  Group ctor
 */
static void CreateCameraDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CameraDebugStartSetUp( userWork, item );
}

/**
 *  @brief  Group dtor
 */
static void DestroyCameraDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CameraDebugEndSetUp( userWork, item );
}

/**
 *  @brief  WindowOpen
 */
static void WindowOpenCameraDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CameraDebugStartSetUp( userWork, item );
}

/**
 *  @brief  WindowClose
 */
static void WindowCloseCameraDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  CameraDebugEndSetUp( userWork, item );
}

GFL_NAMESPACE_END( GroupFunc );

/**
* @brief コンストラクタ
* フィールドデバッグメニューの構築を実行します
*
* @param  pHeap         メニュー構築に使用するヒープ
* @param  pParentGroup  CameraGroupを追加するグループ
*
* @return 無し
*/ 
DebugMenuCameraGroup::DebugMenuCameraGroup( gfl2::heap::HeapBase* pHeap, gfl2::debug::DebugWinGroup* pParentGroup, gfl2::str::MsgData* pMsgData ) :
  m_pHeap( NULL )
  ,m_pParentGroup( NULL )
  ,m_pGrop( NULL )
  ,m_pMsgData( NULL )
{ 
  m_pHeap = pHeap;
  m_pParentGroup = pParentGroup;
  m_pMsgData = pMsgData;


  // カメラに関するグループを追加
  {
    gfl2::debug::DebugWinGroup* pGropWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_camera_top, m_pParentGroup );
    m_pGrop = pGropWork;

    gfl2::debug::DebugWinGroup* pAreaDebugWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_01, pGropWork );
    {
      // コールバッグ関数登録
      pAreaDebugWork->SetCreateDestroyGroupFunc( NULL, GroupFunc::CreateCameraDebugGroupFunc, GroupFunc::DestroyCameraDebugGroupFunc );
      pAreaDebugWork->SetWindowOpenCloseGroupFunc( NULL, GroupFunc::WindowOpenCameraDebugGroupFunc, GroupFunc::WindowCloseCameraDebugGroupFunc );

      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_02, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_03, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_04, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_05, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_06, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_07, NULL, NULL, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pAreaDebugWork, m_pHeap, m_pMsgData, msg_dmenu_debug_target_camera_08, NULL, NULL, NULL );

      // 選択項目が動かない様にする
      gfl2::util::List<gfl2::debug::DebugWinItem*>::Iterator it = pAreaDebugWork->GetItemList()->Begin();
      gfl2::util::List<gfl2::debug::DebugWinItem*>::Iterator end = pAreaDebugWork->GetItemList()->End();
      for( ; it != end; it++)
      {
        (*it)->SetIsSelectSkip( true );
      }
    }

    // 通常カメラに戻す
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork, 
      m_pHeap, 
      m_pMsgData, msg_dmenu_camera_00, 
      NULL,
      Item::Camera::ResetNormalCamera,
      NULL
      );

    // カメラ位置調整イベント起動
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork, 
      m_pHeap, 
      m_pMsgData, msg_dmenu_camera_event_dbg, 
      NULL, 
      Item::Camera::CallCameraAdjustPositionEvent,
      NULL
      );

    // カメラチェック
    gfl2::debug::DebugWinGroup* pCameraCheckGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"カメラチェック", pGropWork );
    if( pCameraCheckGroup )
    {
      // カメラ追従目標表示制御
      gfl2::debug::DebugWin_AddItemValueInt( pCameraCheckGroup, m_pHeap, L"方向予測MAX固定", Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::CAMERA_CHECK_FUTURE_MAX ), 0, 1, 1 );
      gfl2::debug::DebugWin_AddItemValueInt( pCameraCheckGroup, m_pHeap, L"ムーランド固定", Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::CAMERA_CHECK_FORCE_MUURAND ), 0, 1, 1 );
      gfl2::debug::DebugWin_AddItemValueInt( pCameraCheckGroup, m_pHeap, L"タッチパネル方向操作", Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::CAMERA_CHECK_DIR_FIX ), 0, 1, 1 );

      gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
      m_pMsgData->GetString( msg_dmenu_camera_support_dbgmode, *workStrBuf );
      gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pCameraCheckGroup, pHeap, workStrBuf->GetPtr(), NULL, Item::Camera::DispSupportDebugMode, &Debug::DebugTypes::s_CameraSupportDebugMode, 0, Debug::DebugTypes::CameraSupportDebugMode::MAX - 1, 1 );

    }

    // カメラチェック(フォーカス)
    gfl2::debug::DebugWinGroup* pCameraFocusCheckGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"カメラチェック(TRフォーカス)", pGropWork );
    {
      gfl2::debug::DebugWin_AddItemValueInt( pCameraFocusCheckGroup, m_pHeap, L"チェック有効化", &DebugTypes::s_FocusCameraCheck.camera_check_on, 0, 1, 1 );
      gfl2::debug::DebugWin_AddItemValueFloat( pCameraFocusCheckGroup, m_pHeap, L"チェック距離", &DebugTypes::s_FocusCameraCheck.distance, 1.0f, 3000.0f, 1.0f );
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pCameraFocusCheckGroup, 
        m_pHeap, 
        L"-SET 陸上ライド最大値",
        NULL, 
        Item::Camera::SetFocusCameraDistance_GroundRideMax,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pCameraFocusCheckGroup, 
        m_pHeap, 
        L"-SET 海ライド最大値",
        NULL, 
        Item::Camera::SetFocusCameraDistance_NaminoriRideMax,
        NULL
        );

      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pCameraFocusCheckGroup, 
        m_pHeap, 
        L"-SET 徒歩最大値",
        NULL, 
        Item::Camera::SetFocusCameraDistance_NoRideMax,
        NULL
        );

      gfl2::debug::DebugWin_AddItemValueFloat( pCameraFocusCheckGroup, m_pHeap, L"チェック距離(下駄)", &DebugTypes::s_FocusCameraCheck.geta, 0.0f, 2000.0f, 1.0f );
    }


    // カメラエリア設定表示制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_01, Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::DRAW_CAMERA_AREA_DATA ), 0, 1, 1 );

    // カメラエリアコリジョン表示制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_02, Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::VISIBLE_CAMERA_AREA_COLLISION ), 0, 1, 1 );
    
    // カメラコリジョン表示制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_03, Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::VISIBLE_CAMERA_EYE_COLLISION ), 0, 1, 1 );
    
    // カメラ強制STOP制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_04, Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::CAMERA_STOP ), 0, 1, 1 );
    
    // カメラ追従目標表示制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_05, Debug::DebugTypes::GetSwitchValuePtr( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET ), 0, 1, 1 );

    // カメラシェイク制御
    gfl2::debug::DebugWin_AddItemValueInt( pGropWork, m_pHeap, L"カメラシェイク無効化", &Field::Camera::Controller::ControllerTypeArea::GetDebugSettingPtr()->invalidShake, 0, 1, 1 );
#if 0
    // Near Far操作
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_near, &Debug::DebugTypes::s_CameraNear, 1.0f, 100000.0f , 1.0f  );
    gfl2::debug::DebugWin_AddItemValueFloat( pGropWork, m_pHeap, m_pMsgData, msg_dmenu_camera_far, &Debug::DebugTypes::s_CameraFar, 1.0f, 100000.0f , 1.0f  );
#endif

    Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pGropWork, m_pHeap, L"カメラエリア遷移イージング関数", &Field::Camera::Controller::ControllerTypeArea::GetSettingPtr()->areaIpoEaseFunc );

    // 正射影カメラに切り替え
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork, 
      m_pHeap, 
      m_pMsgData, msg_dmenu_camera_change_othro,
      NULL, 
      Item::Camera::ChangeOthroCamera,
      NULL
      );

    // 正射影カメラ、終了
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork, 
      m_pHeap, 
      m_pMsgData, msg_dmenu_camera_end_othro,
      NULL, 
      Item::Camera::EndOthroCamera,
      NULL
      );

#if ENABLE_DEBUG_CAMERA_SYSTEM_TEST
    // カメラ開発テストイベント起動
    gfl2::debug::DebugWin_AddItemUpdateFunc( 
      pGropWork, 
      m_pHeap, 
      m_pMsgData, msg_dmenu_camera_call_event,
      NULL, 
      Item::Camera::CallCameraTestEvent,
      NULL
      );
#endif
    // @todo 3月ROM仮対応、デバッグメニューグループの総数がリストサイズをオーバーしてしまうため、不要なグループを無効化
#if 0
    // ライドイベント関連
    gfl2::debug::DebugWinGroup* pRideOnGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"RideOnEvent", pGropWork );
    if( pRideOnGroup )
    {
      gfl2::debug::DebugWinGroup* pRandToSeaGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"RandToSea", pRideOnGroup );
      if( pRandToSeaGroup )
      {
        gfl2::debug::DebugWin_AddItemValueU32( pRandToSeaGroup, m_pHeap, L"移動開始フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.startFrame, 0, 1000, 1 );
        gfl2::debug::DebugWin_AddItemValueU32( pRandToSeaGroup, m_pHeap, L"移動フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.moveFrame, 1, 1000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pRandToSeaGroup, m_pHeap, L"移動イージング関数", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.moveEaseFunc );
        
        gfl2::debug::DebugWin_AddItemValueU32( pRandToSeaGroup, m_pHeap, L"フィールド補間開始フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.toFieldStartFrame, 0, 1000, 1 );
        gfl2::debug::DebugWin_AddItemValueU32( pRandToSeaGroup, m_pHeap, L"フィールド補間フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.toFieldIpoFrame, 0, 1000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pRandToSeaGroup, m_pHeap, L"フィールド補間イージング関数", &Field::EventPokemonRideTool::s_CameraMoveSettingRandToSea.toFieldIpoEaseFunc );
      }
      gfl2::debug::DebugWinGroup* pSeaToRandGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"SeaToRand", pRideOnGroup );
      if( pSeaToRandGroup )
      {
        gfl2::debug::DebugWin_AddItemValueU32( pSeaToRandGroup, m_pHeap, L"移動開始フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingSeaToRand.startFrame, 0, 1000, 1 );
        gfl2::debug::DebugWin_AddItemValueU32( pSeaToRandGroup, m_pHeap, L"移動フレーム", &Field::EventPokemonRideTool::s_CameraMoveSettingSeaToRand.moveFrame, 1, 1000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pSeaToRandGroup, m_pHeap, L"イージング関数", &Field::EventPokemonRideTool::s_CameraMoveSettingSeaToRand.moveEaseFunc );
      }
    }

    // 段差ジャンプ関連
    gfl2::debug::DebugWinGroup* pStepJump = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"段差ジャンプ設定", pGropWork );
    if( pStepJump )
    {
      // ジャンプ中のカメラ遅延設定
      gfl2::debug::DebugWinGroup* pStepJumpSettingGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"ジャンプ中のカメラ遅延設定[自機]", pStepJump );
      if( pStepJumpSettingGroup )
      {
        Field::MoveModel::ActionCommandStepJump::CAMERA_DELAY* pSetting = Field::MoveModel::ActionCommandStepJump::GetCameraDelaySettingPtr();
        gfl2::debug::DebugWin_AddItemValueU32( pStepJumpSettingGroup, m_pHeap, L"移動開始待ちフレーム", &pSetting->waitFrame, 0, 1000, 1 );
        gfl2::debug::DebugWin_AddItemValueU32( pStepJumpSettingGroup, m_pHeap, L"移動実行フレーム", &pSetting->moveFrame, 1, 1000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pStepJumpSettingGroup, m_pHeap, L"移動時のイージング", &pSetting->moveEaseFunc );
      }
      pStepJumpSettingGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"ジャンプ中のカメラ遅延設定[ライド]", pStepJump );
      if( pStepJumpSettingGroup )
      {
        Field::MoveModel::ActionCommandStepJump::CAMERA_DELAY* pSetting = Field::MoveModel::ActionCommandStepJump::GetCameraDelaySettingRidePtr();
        gfl2::debug::DebugWin_AddItemValueU32( pStepJumpSettingGroup, m_pHeap, L"移動開始待ちフレーム", &pSetting->waitFrame, 0, 1000, 1 );
        gfl2::debug::DebugWin_AddItemValueU32( pStepJumpSettingGroup, m_pHeap, L"移動実行フレーム", &pSetting->moveFrame, 1, 1000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pStepJumpSettingGroup, m_pHeap, L"移動時のイージング", &pSetting->moveEaseFunc );
      }
      
      // 着地時のカメラ揺れの設定、ライドだけ
      pStepJumpSettingGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"着地時のカメラ揺れ[軽量級]", pStepJump );
      if( pStepJumpSettingGroup )
      {
        Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_SETTING* pSetting = Field::MoveModel::ActionCommandStepJump::GetCameraShakeSettingPtr( Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_TYPE_LIGHT );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]質量", &pSetting->horizon.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]バネ定数", &pSetting->horizon.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]初速度", &pSetting->horizon.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]減衰率", &pSetting->horizon.damping, 0.1f, 1000.0f, 0.01f );

        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]質量", &pSetting->vertical.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]バネ定数", &pSetting->vertical.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]初速度", &pSetting->vertical.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]減衰率", &pSetting->vertical.damping, 0.1f, 1000.0f, 0.01f );
      }
      pStepJumpSettingGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"着地時のカメラ揺れ[中量級]", pStepJump );
      if( pStepJumpSettingGroup )
      {
        Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_SETTING* pSetting = Field::MoveModel::ActionCommandStepJump::GetCameraShakeSettingPtr( Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_TYPE_MIDDLE );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]質量", &pSetting->horizon.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]バネ定数", &pSetting->horizon.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]初速度", &pSetting->horizon.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]減衰率", &pSetting->horizon.damping, 0.1f, 1000.0f, 0.01f );

        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]質量", &pSetting->vertical.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]バネ定数", &pSetting->vertical.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]初速度", &pSetting->vertical.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]減衰率", &pSetting->vertical.damping, 0.1f, 1000.0f, 0.01f );
      }
      pStepJumpSettingGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"着地時のカメラ揺れ[重量級]", pStepJump );
      if( pStepJumpSettingGroup )
      {
        Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_SETTING* pSetting = Field::MoveModel::ActionCommandStepJump::GetCameraShakeSettingPtr( Field::MoveModel::ActionCommandStepJump::CAMERA_SHAKE_TYPE_HEAVY );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]質量", &pSetting->horizon.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]バネ定数", &pSetting->horizon.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]初速度", &pSetting->horizon.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[横]減衰率", &pSetting->horizon.damping, 0.1f, 1000.0f, 0.01f );

        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]質量", &pSetting->vertical.mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]バネ定数", &pSetting->vertical.k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]初速度", &pSetting->vertical.firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pStepJumpSettingGroup, m_pHeap, L"[縦]減衰率", &pSetting->vertical.damping, 0.1f, 1000.0f, 0.01f );
      }

    } // 段差ジャンプ関連

    // 地面馬
    gfl2::debug::DebugWinGroup* pRoba2 = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"地面馬振動", pGropWork );
    if( pRoba2 )
    {
      Field::MoveModel::FieldMoveCodeRideRoba2::CAMERA_SHAKE_SETTING* pSetting = Field::MoveModel::FieldMoveCodeRideRoba2::GetCameraShakeSettingPtr();
      gfl2::debug::DebugWin_AddItemValueInt( pRoba2, m_pHeap, L"[走り]有効化", &pSetting->isUseRun, 0, 1, 1 ); 
      gfl2::debug::DebugWin_AddItemValueInt( pRoba2, m_pHeap, L"[歩き]有効化", &pSetting->isUseWalk, 0, 1, 1 ); 

      // 走り
      {
        Field::MoveModel::FieldMoveCodeRideRoba2::CAMERA_SHAKE_SETTING_CORE* pSettingCore = &pSetting->runShake;
        gfl2::debug::DebugWinGroup* pWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"走り", pRoba2 );
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[右前足]有効化", &pSettingCore->isRightHand, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[左前足]有効化", &pSettingCore->isLeftHand, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[右後足]有効化", &pSettingCore->isRightFoot, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[左後足]有効化", &pSettingCore->isLeftFoot, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"質量", &pSettingCore->mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"バネ定数", &pSettingCore->k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"初速度", &pSettingCore->firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"減衰率", &pSettingCore->damping, 0.1f, 1000.0f, 0.01f );
      }
      // 歩き
      {
        Field::MoveModel::FieldMoveCodeRideRoba2::CAMERA_SHAKE_SETTING_CORE* pSettingCore = &pSetting->walkShake;
        gfl2::debug::DebugWinGroup* pWork = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"歩き", pRoba2 );
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[右前足]有効化", &pSettingCore->isRightHand, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[左前足]有効化", &pSettingCore->isLeftHand, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[右後足]有効化", &pSettingCore->isRightFoot, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueInt( pWork, m_pHeap, L"[左後足]有効化", &pSettingCore->isLeftFoot, 0, 1, 1 ); 
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"質量", &pSettingCore->mass, 0.1f, 1000.0f, 0.01f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"バネ定数", &pSettingCore->k, 1.0f, 1000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"初速度", &pSettingCore->firstVelocity, 0.1f, 1000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pWork, m_pHeap, L"減衰率", &pSettingCore->damping, 0.1f, 1000.0f, 0.01f );
      }
    }
#endif


    // @fix CONNMCat[582]:攻略本用マップ撮影機能、デバッグメニュー操作を追加
    gfl2::debug::DebugWinGroup* pMapCaptureGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"マップ撮影関連機能", pGropWork );
    if( pMapCaptureGroup )
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pMapCaptureGroup, 
        m_pHeap, 
        L"マップ撮影モード起動/終了",
        NULL, 
        Item::Camera::CallMapCaptureEvent,
        NULL
        );

      Field::Debug::Event::EventMapCapture::SETTING* pSetting = Field::Debug::Event::EventMapCapture::GetSettingPtr();
      gfl2::debug::DebugWin_AddItemValueFloat( pMapCaptureGroup, m_pHeap, L"十字キー操作移動量(カメラ)", &pSetting->step_move_value, 0.1f, 10000.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pMapCaptureGroup, m_pHeap, L"十字キー操作移動量(地形制御)", &pSetting->step_move_value_for_control, 0.1f, 10000.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pMapCaptureGroup, m_pHeap, L"正投影幅(縦)", &pSetting->ortho_draw_range, 0.1f, 50000.0f, 0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pMapCaptureGroup, m_pHeap, L"アニメーション停止フレーム", &pSetting->animation_stop_frame, 0.0f, 50000.0f, 1.0f );

      gfl2::debug::DebugWinGroup* pCameraPositonGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"カメラポジション調整", pMapCaptureGroup );
      if( pCameraPositonGroup )
      {
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraPositonGroup, m_pHeap, L"x", &pSetting->camera_position.x, -100000.0f, 100000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraPositonGroup, m_pHeap, L"y", &pSetting->camera_position.y, -100000.0f, 100000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraPositonGroup, m_pHeap, L"z", &pSetting->camera_position.z, -100000.0f, 100000.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemUpdateFunc( 
          pCameraPositonGroup, 
          m_pHeap, 
          L"小数切捨て",
          &pSetting->camera_position,
          Item::Camera::RoundDown,
          NULL
          );
      }

      gfl2::debug::DebugWinGroup* pCameraRotationGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"カメラ回転調整", pMapCaptureGroup );
      if( pCameraRotationGroup )
      {
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraRotationGroup, m_pHeap, L"x", &pSetting->camera_rot_deg.x, -360.0f, 360.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraRotationGroup, m_pHeap, L"y", &pSetting->camera_rot_deg.y, -360.0f, 360.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemValueFloat( pCameraRotationGroup, m_pHeap, L"z", &pSetting->camera_rot_deg.z, -360.0f, 360.0f, 0.1f );
        gfl2::debug::DebugWin_AddItemUpdateFunc( 
          pCameraRotationGroup, 
          m_pHeap, 
          L"小数切捨て",
          &pSetting->camera_rot_deg,
          Item::Camera::RoundDown,
          NULL
          );

        gfl2::debug::DebugWin_AddItemUpdateFunc( 
          pCameraRotationGroup, 
          m_pHeap, 
          L"Y回転無効化",
          &pSetting->camera_rot_deg.y,
          Item::Camera::SetZero_f32,
          NULL
          );
      }
      gfl2::debug::DebugWinGroup* pControlPositionGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, L"地形制御ポジション調整", pMapCaptureGroup );
      if( pControlPositionGroup )
      {
        gfl2::debug::DebugWin_AddItemValueFloat( pControlPositionGroup, m_pHeap, L"x", &pSetting->control_position.x, -100000.0f, 100000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemValueFloat( pControlPositionGroup, m_pHeap, L"z", &pSetting->control_position.z, -100000.0f, 100000.0f, 1.0f );
        gfl2::debug::DebugWin_AddItemUpdateFunc( 
          pControlPositionGroup, 
          m_pHeap, 
          L"小数切捨て",
          &pSetting->control_position,
          Item::Camera::RoundDown,
          NULL
          );

        gfl2::debug::DebugWin_AddItemUpdateFunc( 
          pControlPositionGroup, 
          m_pHeap, 
          L"カメラ直下に移動",
          NULL,
          Item::Camera::SetControlPointToCameraPositon,
          NULL
          );
      }
      
      gfl2::debug::DebugWin_AddItemValueInt( pMapCaptureGroup, m_pHeap, L"地形Middleモデル表示", Debug::DebugTypes::GetSwitchValuePtr( DebugTypes::Switch::TERRAIN_LOD ), 0, 1, 1 ); 
      gfl2::debug::DebugWin_AddItemValueInt( pMapCaptureGroup, m_pHeap, L"正射影表示", &pSetting->is_ortho, 0, 1, 1 ); 
    }

    gfl2::debug::DebugWinGroup* pNecrozmaGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"ネクロズママップ関連機能", pGropWork);
    if (pNecrozmaGroup)
    {
      Camera::Area::SubController::SubControllerTypeNecrozma::SETTING* pSetting = Camera::Area::SubController::SubControllerTypeNecrozma::GetSettingPtr();
     
      Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pNecrozmaGroup, m_pHeap, L"補間イージング関数", &pSetting->easingFunc );
      gfl2::debug::DebugWin_AddItemValueInt( pNecrozmaGroup, m_pHeap, L"エリア遷移補間フレーム", &pSetting->interpolationFrame, 0, 10000, 1 );

      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]距離", &pSetting->start.param.distance, 1.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]回転(X)", &pSetting->start.param.rotation.x, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]回転(Y)", &pSetting->start.param.rotation.y, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]回転(Z)", &pSetting->start.param.rotation.z, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]Offset(x)", &pSetting->start.param.offset.x, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]Offset(y)", &pSetting->start.param.offset.y, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]Offset(z)", &pSetting->start.param.offset.z, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[Start]画角(Y)", &pSetting->start.param.fovY, 0.1f, 180.0f, 0.1f ); 
      
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]距離", &pSetting->end.param.distance, 1.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]回転(X)", &pSetting->end.param.rotation.x, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]回転(Y)", &pSetting->end.param.rotation.y, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]回転(Z)", &pSetting->end.param.rotation.z, -360.0f, 360.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]Offset(x)", &pSetting->end.param.offset.x, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]Offset(y)", &pSetting->end.param.offset.y, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]Offset(z)", &pSetting->end.param.offset.z, -10000.0f, 10000.0f, 0.1f ); 
      gfl2::debug::DebugWin_AddItemValueFloat( pNecrozmaGroup, m_pHeap, L"[End]画角(Y)", &pSetting->end.param.fovY, 0.1f, 180.0f, 0.1f ); 

    }
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
DebugMenuCameraGroup::~DebugMenuCameraGroup( void )
{
  // 登録したグループは親が開放してくれるが、念のため
  if( m_pGrop )
  {
    gfl2::debug::DebugWin_RemoveGroup( m_pGrop );
    m_pGrop = NULL;
  }
}

#if defined(GF_PLATFORM_WIN32)
void AddEaseFuncHash( Field::Debug::NameMangling::NameManglingS32Node* pNode )
{
  pNode->AddHash( "LINEAR", gfl2::math::Easing::LINEAR );

  pNode->AddHash( "IN_SINE", gfl2::math::Easing::IN_SINE );
  pNode->AddHash( "OUT_SINE", gfl2::math::Easing::OUT_SINE );
  pNode->AddHash( "INOUT_SINE", gfl2::math::Easing::INOUT_SINE );

  pNode->AddHash( "IN_QUAD", gfl2::math::Easing::IN_QUAD );
  pNode->AddHash( "OUT_QUAD", gfl2::math::Easing::OUT_QUAD );
  pNode->AddHash( "INOUT_QUAD", gfl2::math::Easing::INOUT_QUAD );

  pNode->AddHash( "IN_CUBIC", gfl2::math::Easing::IN_CUBIC );
  pNode->AddHash( "OUT_CUBIC", gfl2::math::Easing::OUT_CUBIC );
  pNode->AddHash( "INOUT_CUBIC", gfl2::math::Easing::INOUT_CUBIC );

  pNode->AddHash( "IN_QUART", gfl2::math::Easing::IN_QUART );
  pNode->AddHash( "OUT_QUART", gfl2::math::Easing::OUT_QUART );
  pNode->AddHash( "INOUT_QUART", gfl2::math::Easing::INOUT_QUART );

  pNode->AddHash( "IN_QUINT", gfl2::math::Easing::IN_QUINT );
  pNode->AddHash( "OUT_QUINT", gfl2::math::Easing::OUT_QUINT );
  pNode->AddHash( "INOUT_QUINT", gfl2::math::Easing::INOUT_QUINT );

  pNode->AddHash( "IN_EXPO", gfl2::math::Easing::IN_EXPO );
  pNode->AddHash( "OUT_EXPO", gfl2::math::Easing::OUT_EXPO );
  pNode->AddHash( "INOUT_EXPO", gfl2::math::Easing::INOUT_EXPO );

  pNode->AddHash( "IN_CIRC", gfl2::math::Easing::IN_CIRC );
  pNode->AddHash( "OUT_CIRC", gfl2::math::Easing::OUT_CIRC );
  pNode->AddHash( "INOUT_CIRC", gfl2::math::Easing::INOUT_CIRC );

  pNode->AddHash( "IN_BACK", gfl2::math::Easing::IN_BACK );
  pNode->AddHash( "OUT_BACK", gfl2::math::Easing::OUT_BACK );
  pNode->AddHash( "INOUT_BACK", gfl2::math::Easing::INOUT_BACK );

  pNode->AddHash( "IN_ELASTIC", gfl2::math::Easing::IN_ELASTIC );
  pNode->AddHash( "OUT_ELASTIC", gfl2::math::Easing::OUT_ELASTIC );
  pNode->AddHash( "INOUT_ELASTIC", gfl2::math::Easing::INOUT_ELASTIC );

  pNode->AddHash( "IN_BOUNCE", gfl2::math::Easing::IN_BOUNCE );
  pNode->AddHash( "OUT_BOUNCE", gfl2::math::Easing::OUT_BOUNCE );
  pNode->AddHash( "INOUT_BOUNCE", gfl2::math::Easing::INOUT_BOUNCE );
}
#endif // #if defined(GF_PLATFORM_WIN32)


GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#endif // #if PM_DEBUG