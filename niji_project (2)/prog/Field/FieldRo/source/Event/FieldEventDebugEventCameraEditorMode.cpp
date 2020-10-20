//======================================================================
/**
* @file FieldEventDebugEventCameraEditorMode.cpp
* @brief キャラエディターの動作モード
* @author saita_kazuki
* @data 2015.10.22
*/
//======================================================================

#if PM_DEBUG

#include "Field/FieldRo/include/Event/FieldEventDebugEventCameraEditorMode.h"
#if defined(GF_ENABLE_DEBUG_EDITOR)
#include <locale.h> // Win32のみなので使う


// gfl2
#include <Debug/include/gfl2_DebugPrint.h>

// niji
#include "Sound/include/Sound.h"

// field
#include "Field/FieldRo/include/Fieldmap.h"

// field camera
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugAdjustPosition.h"
#include <model/include/gfl2_BaseCamera.h>


// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinLocal.h>

// field script
#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldRo/include/Window/FieldTalkWindow.h"

// conv_header
#include <niji_conv_header/message/debug/msg_debug_menu.h>


GFL_NAMESPACE_BEGIN( Field )

// enumと合わせる
static const wchar_t* EASING_FUNC_TABLE[] =
{
  L"LINEAR",
  
  L"IN_SINE",
  L"OUT_SINE",
  L"INOUT_SINE",
  
  L"IN_QUAD",
  L"OUT_QUAD",
  L"INOUT_QUAD",
  
  L"IN_CUBIC",
  L"OUT_CUBIC",
  L"INOUT_CUBIC",
  
  L"IN_QUART",
  L"OUT_QUART",
  L"INOUT_QUART",

  L"IN_QUINT",
  L"OUT_QUINT",
  L"INOUT_QUINT",

  L"IN_EXPO",
  L"OUT_EXPO",
  L"INOUT_EXPO",

  L"IN_CIRC",
  L"OUT_CIRC",
  L"INOUT_CIRC",

  L"IN_BACK",
  L"OUT_BACK",
  L"INOUT_BACK",

  L"IN_ELASTIC",
  L"OUT_ELASTIC",
  L"INOUT_ELASTIC",

  L"IN_BOUNCE",
  L"OUT_BOUNCE",
  L"INOUT_BOUNCE"
};

s32 DebugEventCameraEditorModeManager::m_IsDrawTextInfo( false );

//------------------------------------------------------------------------------
// DebugEventCameraEditorCommonWork
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugEventCameraEditorCommonWork::DebugEventCameraEditorCommonWork()
  : defaultPos( gfl2::math::Vector3::GetZero())
  , defaultTarget( gfl2::math::Vector3::GetZero())
  , firstFrameWork()
  , lastFrameWork()
  , easingType( gfl2::math::Easing::INOUT_SINE)
  , frame( 60)
  , isLoop( false)
  , scriptCameraControllerParam()
{
  gfl2::std::MemClear( isTalkWindowVisibleDefault, sizeof( isTalkWindowVisibleDefault));
}

/**
 * @brief デストラクタ
 */ 
DebugEventCameraEditorCommonWork::~DebugEventCameraEditorCommonWork()
{
}


//------------------------------------------------------------------------------
// DebugEventCameraEditorModeManager
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugEventCameraEditorModeManager::DebugEventCameraEditorModeManager()
  : m_pHeap( NULL)
  , m_pDebugGroup( NULL)
  , m_pMsgData( NULL)
  , m_pFieldmap( NULL)
  , m_state( STATE_NONE)
  , m_pCurrentMode( NULL)
  , m_commonWork()
{
}

/**
 * @brief デストラクタ
 */ 
DebugEventCameraEditorModeManager::~DebugEventCameraEditorModeManager()
{
}

/**
 * @brief 初期化
 * @param desc 初期化設定
 */ 
void DebugEventCameraEditorModeManager::Initialize( const InitializeDescription& desc)
{
  m_pHeap         = desc.pHeap;
  m_pMsgData      = desc.pMsgData;
  m_pFieldmap     = desc.pFieldmap;
  m_pScriptSystem = desc.pScriptSystem;

  // 動作モード生成
  m_pModeTable[ MODE_EDIT ] = GFL_NEW( m_pHeap) DebugEventCameraEditorModeEdit();
  m_pModeTable[ MODE_FRAMING ]  = GFL_NEW( m_pHeap) DebugEventCameraEditorModeFraming();

  // 動作モード初期化
  DebugEventCameraEditorModeBase::InitializeDescription initDesc;
  initDesc.pHeap        = m_pHeap;
  initDesc.pMsgData     = m_pMsgData;
  initDesc.pFieldmap    = m_pFieldmap;
  initDesc.pManager     = this;
  for( u32 i = 0; i < MODE_MAX; ++i)
  {
    m_pModeTable[i]->Initialize( initDesc);
  }

  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  Field::Camera::Controller::ControllerTypeEventScript* pController = reinterpret_cast<Field::Camera::Controller::ControllerTypeEventScript*>( pCameraUnit->GetControllerBase());

  // スクリプトカメラコントローラーのパラメータを覚えておく
  pController->GetTargetParamStart( &m_commonWork.scriptCameraControllerParam.moveTargetStart);
  pController->GetTargetParamEnd( &m_commonWork.scriptCameraControllerParam.moveTargetEnd);
  pController->GetCommonParameters(
    &m_commonWork.scriptCameraControllerParam.actionState,
    &m_commonWork.scriptCameraControllerParam.bank,
    &m_commonWork.scriptCameraControllerParam.moveType,
    &m_commonWork.scriptCameraControllerParam.moveEndFrame,
    &m_commonWork.scriptCameraControllerParam.moveNowFrame,
    &m_commonWork.scriptCameraControllerParam.easingFunc
    );

  // 実行状態クリア
  pController->SetCommonParameters(
    Camera::Controller::ControllerTypeEventScript::ActionState::NONE,
    m_commonWork.scriptCameraControllerParam.bank,
    m_commonWork.scriptCameraControllerParam.moveType,
    m_commonWork.scriptCameraControllerParam.moveEndFrame,
    m_commonWork.scriptCameraControllerParam.moveNowFrame,
    m_commonWork.scriptCameraControllerParam.easingFunc
    );

  // 現在のカメラ位置、ターゲット位置をデフォルトに設定
  const poke_3d::model::BaseCamera* pBaseCamera = pCameraUnit->GetBaseCamera();
  m_commonWork.defaultPos = pBaseCamera->GetPosition();
  m_commonWork.defaultTarget = pBaseCamera->GetTargetPosition();
  m_commonWork.defaultRotation = pBaseCamera->GetRotationQuat();

  // FFに移動前の情報を設定
  pController->SetMoveFirstFrame(); // 移動命令情報が無い場合は今の情報のまま
  m_commonWork.firstFrameWork.pos = pBaseCamera->GetPosition();
  m_commonWork.firstFrameWork.target = pBaseCamera->GetTargetPosition();
  m_commonWork.firstFrameWork.rotation = pBaseCamera->GetRotationQuat();
  m_commonWork.firstFrameWork.fovy = pBaseCamera->GetFovy();
  m_commonWork.firstFrameWork.bank = pController->GetNowBank();
  
  // LFに移動後の情報を設定
  pController->SetMoveEndFrame(); // 移動命令情報が無い場合は今の情報のまま
  m_commonWork.lastFrameWork.pos = pBaseCamera->GetPosition();
  m_commonWork.lastFrameWork.target = pBaseCamera->GetTargetPosition();
  m_commonWork.lastFrameWork.rotation = pBaseCamera->GetRotationQuat();
  m_commonWork.lastFrameWork.fovy = pBaseCamera->GetFovy();
  m_commonWork.lastFrameWork.bank = pController->GetNowBank();

  if( m_commonWork.scriptCameraControllerParam.moveEndFrame != 0 )
  {
    m_commonWork.easingType = m_commonWork.scriptCameraControllerParam.easingFunc;
    m_commonWork.frame = m_commonWork.scriptCameraControllerParam.moveEndFrame;
  }
  
  // 開始時は移動前の状態にする
  pController->SetMoveFirstFrame();
}

/**
 * @brief 破棄
 */ 
void DebugEventCameraEditorModeManager::Terminate()
{
  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  Field::Camera::Controller::ControllerTypeEventScript* pController = reinterpret_cast<Field::Camera::Controller::ControllerTypeEventScript*>( pCameraUnit->GetControllerBase());

  // カメラ位置、ターゲット位置をデフォルトに戻す
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい
  pBaseCamera->SetPosition( m_commonWork.defaultPos);
  pBaseCamera->SetTargetPosition( m_commonWork.defaultTarget);
  pBaseCamera->SetRotationQuat( m_commonWork.defaultRotation);

  // スクリプトカメラコントローラーのパラメータを戻す
  pController->SetTargetParamStart( m_commonWork.scriptCameraControllerParam.moveTargetStart);
  pController->SetTargetParamEnd( m_commonWork.scriptCameraControllerParam.moveTargetEnd);
  pController->SetCommonParameters(
    m_commonWork.scriptCameraControllerParam.actionState,
    m_commonWork.scriptCameraControllerParam.bank,
    m_commonWork.scriptCameraControllerParam.moveType,
    m_commonWork.scriptCameraControllerParam.moveEndFrame,
    m_commonWork.scriptCameraControllerParam.moveNowFrame,
    m_commonWork.scriptCameraControllerParam.easingFunc
    );

  for( u32 i = 0; i < MODE_MAX; ++i)
  {
    if( m_pModeTable[i])
    {
      m_pModeTable[i]->Terminate();
    }
    GFL_SAFE_DELETE( m_pModeTable[i]);
  }
}

/**
 * @brief 更新
 * @return 更新後の状態
 */ 
DebugEventCameraEditorModeManager::State DebugEventCameraEditorModeManager::Update( gfl2::util::DrawUtilText* pDrawText )
{
  DebugEventCameraEditorModeBase::State state;
  state = m_pCurrentMode->Update( this);
  m_pCurrentMode->DrawTextInfo( pDrawText );

  // モードが終了チェック
  if( state == DebugEventCameraEditorModeBase::STATE_END)
  {
    m_pCurrentMode->Clean();
    return STATE_END;
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugEventCameraEditorModeManager::Setup( Mode mode, gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  m_pCurrentMode = m_pModeTable[ mode ];
  m_pCurrentMode->Setup( m_pDebugGroup);
}

/**
 * @brief モード取得
 * @param mode モード
 * @return 指定したモード
 */ 
DebugEventCameraEditorModeBase* DebugEventCameraEditorModeManager::GetMode( Mode mode) const
{
  return m_pModeTable[ mode ];
}

/**
 * @brief 共通ワーク取得
 * @return 共通ワーク(書き換え可)
 */ 
DebugEventCameraEditorCommonWork* DebugEventCameraEditorModeManager::GetCommonWork()
{
  return &m_commonWork;
}

/**
 * @brief 会話ウインドウの表示状態を保存
 */ 
void DebugEventCameraEditorModeManager::SaveTalkWindowVisible()
{
  FieldScript::SystemSingletones* pSystemSingletones = m_pScriptSystem->GetSystemWork()->GetSingletones();
  for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
  {
    if( pSystemSingletones->m_pFieldTalkWindow[i])
    {
      m_commonWork.isTalkWindowVisibleDefault[i] = pSystemSingletones->m_pFieldTalkWindow[i]->IsVisible();
    }
    else
    {
      m_commonWork.isTalkWindowVisibleDefault[i] = false;
    }
  }
}

/**
 * @brief 会話ウインドウの表示状態変更
 */ 
void DebugEventCameraEditorModeManager::SetTalkWindowVisible( bool isVisible)
{
  FieldScript::SystemSingletones* pSystemSingletones = m_pScriptSystem->GetSystemWork()->GetSingletones();
  for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
  {
    if( pSystemSingletones->m_pFieldTalkWindow[i] && m_commonWork.isTalkWindowVisibleDefault[i])
    {
      if( pSystemSingletones->m_pFieldTalkWindow[i]->IsVisible() != isVisible)
      {
        pSystemSingletones->m_pFieldTalkWindow[i]->SetVisible( isVisible);
      }
    }
  }
}

/**
 * @brief 会話ウインドウの表示状態を保存した状態にする
 */ 
void DebugEventCameraEditorModeManager::ResetTalkWindowVisible()
{
  FieldScript::SystemSingletones* pSystemSingletones = m_pScriptSystem->GetSystemWork()->GetSingletones();
  for( u32 i = 0; i < FIELDTALKWINDOW_MAX; ++i)
  {
    if( pSystemSingletones->m_pFieldTalkWindow[i])
    {
      pSystemSingletones->m_pFieldTalkWindow[i]->SetVisible( m_commonWork.isTalkWindowVisibleDefault[i]);
    }
  }
}

//------------------------------------------------------------------------------
// DebugEventCameraEditorModeBase
//------------------------------------------------------------------------------

/**
 * @brief コンストラクタ
 */ 
DebugEventCameraEditorModeBase::DebugEventCameraEditorModeBase()
  : m_pHeap( NULL)
  , m_pDebugGroup( NULL)
  , m_pMsgData( NULL)
  , m_pFieldmap( NULL)
{
}

/**
 * @brief デストラクタ
 */ 
DebugEventCameraEditorModeBase::~DebugEventCameraEditorModeBase()
{
}

/**
 * @brief 初期化
 * @param desc 初期化設定
 */ 
void DebugEventCameraEditorModeBase::Initialize( const InitializeDescription& desc)
{
  m_pHeap         = desc.pHeap;
  m_pDebugGroup   = desc.pDebugGroup;
  m_pMsgData      = desc.pMsgData;
  m_pFieldmap     = desc.pFieldmap;
  m_pManager      = desc.pManager;

  this->initialize();
}


//------------------------------------------------------------------------------
// DebugEventCameraEditorModeEdit
//------------------------------------------------------------------------------

void UpdateFunc_EditEasingType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  s32* pValue = (s32*)userWork;

  // 最大値
  const s32 MAX = gfl2::math::Easing::INOUT_BOUNCE + 1;

  // 加算値調整
  s32 add = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add = 5;
  }

  if( pButton->IsRepeat( gfl2::ui::DIR_LEFT ) )
  {
    *pValue += -add;
    if( *pValue < 0 )
    {
      *pValue += MAX;
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::DIR_RIGHT ) )
  {
    *pValue += add;
    if( *pValue >= MAX )
    {
      *pValue -= MAX;
    }
  }
}

wchar_t* DrawFunc_EditEasingType( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32* pValue = (s32*)userWork;
  s32 value = *pValue;

  // 最大値
  const s32 MAX = gfl2::math::Easing::INOUT_BOUNCE + 1;

  if( value >= MAX || value < 0 )
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls [ ERROR(%d) ]", item->GetName(), value );
  }
  else
  {
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls [ %ls ]", item->GetName(), EASING_FUNC_TABLE[ value ] );
  }
  return workStr;

}

void UpdateFunc_EditPlay( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugEventCameraEditorModeEdit* pSelf = (DebugEventCameraEditorModeEdit*)userWork;
  DebugEventCameraEditorCommonWork* pCommonWork = pSelf->GetModeManager()->GetCommonWork();

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  Camera::Controller::ControllerTypeEventScript* pController = pSelf->GetEventScriptCameraController();
  if( pController->IsMoving())
  {
    return;
  }

  // 切り替え
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) || pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pCommonWork->isLoop = !pCommonWork->isLoop;
  }

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pSelf->PlayCameraMotion();
  }
}

wchar_t* DrawFunc_EditPlay( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugEventCameraEditorModeEdit* pSelf = (DebugEventCameraEditorModeEdit*)userWork;
  DebugEventCameraEditorCommonWork* pCommonWork = pSelf->GetModeManager()->GetCommonWork();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls [ %s ]", item->GetName(), pCommonWork->isLoop ? L"LOOP" : L"ONCE" );

  return workStr;
}

void UpdateFunc_EditStop( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugEventCameraEditorModeEdit* pSelf = (DebugEventCameraEditorModeEdit*)userWork;

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  // 停止処理
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pSelf->StopCameraMotion();
  }
}

void UpdateFunc_EditPrintFirstFrame( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugEventCameraEditorModeEdit* pSelf = (DebugEventCameraEditorModeEdit*)userWork;

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  // ログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pSelf->PrintFirstFrame();
  }
}

void UpdateFunc_EditPrintMotion( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugEventCameraEditorModeEdit* pSelf = (DebugEventCameraEditorModeEdit*)userWork;

  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  // ログ出力
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pSelf->PrintMotion();
  }
}

/**
 * @brief コンストラクタ
 */ 
DebugEventCameraEditorModeEdit::DebugEventCameraEditorModeEdit()
  : m_pCameraControllerEventScript( NULL)
  , m_isMoving( false)
{
}

/**
 * @brief デストラクタ
 */ 
DebugEventCameraEditorModeEdit::~DebugEventCameraEditorModeEdit()
{
}

/**
 * @brief 破棄
 */ 
void DebugEventCameraEditorModeEdit::Terminate()
{
}

/**
 * @brief 更新
 * @return 更新後のモードの状態
 */ 
DebugEventCameraEditorModeBase::State DebugEventCameraEditorModeEdit::Update( DebugEventCameraEditorModeManager* pManager)
{
  gfl2::ui::Button* pButton = gfl2::debug::DebugWin_GetUiButton();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen() )
  {
    return STATE_END;
  }

  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  // カメラモーション終了時の制御
  if( m_pCameraControllerEventScript->IsMoving() == false && m_isMoving)
  {
    m_isMoving = false;
    if( pCommonWork->isLoop)
    {
      this->PlayCameraMotion();
    }
  }

  // 再生ショートカット
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
    if( m_pCameraControllerEventScript->IsMoving() == false)
    {
      this->PlayCameraMotion();
    }
  }

  // モーション出力ショートカット
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    this->PrintMotion();
  }

  // 会話ウインドウ表示ON/OFF
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    m_pManager->SetTalkWindowVisible( false);
  }
  else
  {
    m_pManager->SetTalkWindowVisible( true);
  }

  // L + A でカメラ基準回転を行うので、Aが押されている際は無反応
  if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) && !pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {
    // 構図アシストON/OFF
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION ) )
      {
        FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION, 0);
      }
      else
      {
        FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION, 1);
      }
    }
    // カメラターゲット座標表示
    else
    {
      if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST) )
      {
        FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST, 0);
      }
      else
      {
        FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST, 1);
      }
    }
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugEventCameraEditorModeEdit::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  // デバッグ項目登録
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_easing, reinterpret_cast<void*>( &pCommonWork->easingType), UpdateFunc_EditEasingType, DrawFunc_EditEasingType );
  gfl2::debug::DebugWin_AddItemValueU32( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_frame_num, &pCommonWork->frame, 1, 30*30, 1);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_play, reinterpret_cast<void*>( this), UpdateFunc_EditPlay, DrawFunc_EditPlay );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_stop, reinterpret_cast<void*>( this), UpdateFunc_EditStop, NULL );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_print_first_frame, reinterpret_cast<void*>( this), UpdateFunc_EditPrintFirstFrame, NULL );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_print_motion, reinterpret_cast<void*>( this), UpdateFunc_EditPrintMotion, NULL );

  // メンバの初期化
  m_pCameraControllerEventScript = NULL;
  m_isMoving = false;

  // スクリプトカメラコントローラー取得
  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  m_pCameraControllerEventScript = reinterpret_cast<Field::Camera::Controller::ControllerTypeEventScript*>( pCameraUnit->GetControllerBase());

  // 現在のカメラ位置、ターゲット位置をファーストフレームにする
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);

  // カメラ更新許可
  pCameraUnit->SetUpdateFlag( true);

  // 会話ウインドウの元の表示状態を保持しておく
  m_pManager->SaveTalkWindowVisible();
}

/**
 * @brief クリーン
 */ 
void DebugEventCameraEditorModeEdit::Clean()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);

  // 会話ウインドウの元の表示状態を元に戻す
  m_pManager->ResetTalkWindowVisible();

  // カメラ更新禁止
  pCameraUnit->SetUpdateFlag( false);

  // カメラ位置、ターゲット位置をファーストフレームに戻す
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);

  // メンバの後始末
  m_pCameraControllerEventScript = NULL;
  m_isMoving = false;

  // デバッグ項目削除
  m_pDebugGroup->RemoveAllItem();
}

/**
 * @brief ログ出力
 */
void DebugEventCameraEditorModeEdit::PrintFirstFrame()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  gfl2::math::Vector3 printPos = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.pos);
  gfl2::math::Vector3 printTarget = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.target);

  GFL_PRINT( "EvCameraStart();\n");
  GFL_PRINT( "EvCameraMoveLookAt( 1, EASE_LINEAR, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f );\n",
    printPos.x, printPos.y, printPos.z,
    printTarget.x, printTarget.y, printTarget.z,
    gfl2::math::ConvRadToDeg( pCommonWork->firstFrameWork.bank), 
    gfl2::math::ConvRadToDeg( pCommonWork->firstFrameWork.fovy)
    );
  GFL_PRINT( "EvCameraMoveWait();\n");

}

/**
 * @brief ログ出力
 */
void DebugEventCameraEditorModeEdit::PrintMotion()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  gfl2::math::Vector3 printFirstPos = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.pos);
  gfl2::math::Vector3 printFirstTarget = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.target);
  gfl2::math::Vector3 printLastPos = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->lastFrameWork.pos);
  gfl2::math::Vector3 printLastTarget = m_pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->lastFrameWork.target);

  GFL_PRINT( "EvCameraStart();\n");
  GFL_PRINT( "EvCameraMoveLookAt( 1, EASE_LINEAR, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f );\n",
    printFirstPos.x, printFirstPos.y, printFirstPos.z,
    printFirstTarget.x, printFirstTarget.y, printFirstTarget.z,
    gfl2::math::ConvRadToDeg( pCommonWork->firstFrameWork.bank), 
    gfl2::math::ConvRadToDeg( pCommonWork->firstFrameWork.fovy)
    );
  GFL_PRINT( "EvCameraMoveWait();\n");

  GFL_PRINT( "EvCameraMoveLookAt( %d, EASE_%ls, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f, %.1f );\n",
    pCommonWork->frame,
    EASING_FUNC_TABLE[ pCommonWork->easingType ],
    printLastPos.x, printLastPos.y, printLastPos.z,
    printLastTarget.x, printLastTarget.y, printLastTarget.z,
    gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.bank), 
    gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.fovy)
    );
  GFL_PRINT( "EvCameraMoveWait();\n");
}

/**
 * @brief カメラモーション再生
 */
void DebugEventCameraEditorModeEdit::PlayCameraMotion()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  m_isMoving = true;

  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい

  // スクリプトカメラコントローラーのパラメーターを覚えておく
  m_pCameraControllerEventScript->GetCommonParameters(
    &m_ScriptCameraControllerParam_PlayBefore.actionState,
    &m_ScriptCameraControllerParam_PlayBefore.bank,
    &m_ScriptCameraControllerParam_PlayBefore.moveType,
    &m_ScriptCameraControllerParam_PlayBefore.moveEndFrame,
    &m_ScriptCameraControllerParam_PlayBefore.moveNowFrame,
    &m_ScriptCameraControllerParam_PlayBefore.easingFunc
    );

  // ファーストフレームの状態にする
  m_pCameraControllerEventScript->SetCommonParameters(
    Camera::Controller::ControllerTypeEventScript::ActionState::NONE,
    pCommonWork->firstFrameWork.bank,
    0,
    pCommonWork->frame,
    0.0f,
    static_cast<gfl2::math::Easing::EaseFunc>( pCommonWork->easingType)
    );

  // 開始位置にファーストフレームをセット
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);

  // ラストフレームを目標にMove開始
  m_pCameraControllerEventScript->Move(
    pCommonWork->lastFrameWork.pos,
    pCommonWork->lastFrameWork.target,
    pCommonWork->frame,
    static_cast<gfl2::math::Easing::EaseFunc>( pCommonWork->easingType),
    gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.bank),
    gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.fovy)
    );
}

/**
 * @brief カメラモーション停止
 */
void DebugEventCameraEditorModeEdit::StopCameraMotion()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  m_isMoving = false;

  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい

  // ファーストフレームの位置に戻す
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);

  // 再生前の状態に戻す
  m_pCameraControllerEventScript->SetCommonParameters(
    Camera::Controller::ControllerTypeEventScript::ActionState::NONE,
    m_ScriptCameraControllerParam_PlayBefore.bank,
    m_ScriptCameraControllerParam_PlayBefore.moveType,
    m_ScriptCameraControllerParam_PlayBefore.moveEndFrame,
    m_ScriptCameraControllerParam_PlayBefore.moveNowFrame,
    m_ScriptCameraControllerParam_PlayBefore.easingFunc
    );
}

/**
 * @brief 初期化
 */ 
void DebugEventCameraEditorModeEdit::initialize()
{
}


//------------------------------------------------------------------------------
// DebugEventCameraEditorModeFraming
//------------------------------------------------------------------------------

wchar_t* DrawFunc_FramingName( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s", item->GetName() );
  return workStr;
}

wchar_t* DrawFunc_FramingVector3( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::math::Vector3* pVec3 = (gfl2::math::Vector3*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f, %.1f, %.1f ]", item->GetName(), pVec3->x, pVec3->y, pVec3->z );
  return workStr;
}

wchar_t* DrawFunc_FramingFirstFramePosition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugEventCameraEditorModeFraming* pSelf = (DebugEventCameraEditorModeFraming*)userWork;
  DebugEventCameraEditorModeManager* pManager = pSelf->GetModeManager();
  DebugEventCameraEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  gfl2::math::Vector3 printPos = pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.pos);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f, %.1f, %.1f ]", item->GetName(), printPos.x, printPos.y, printPos.z );
  return workStr;
}

wchar_t* DrawFunc_FramingFirstFrameTarget( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugEventCameraEditorModeFraming* pSelf = (DebugEventCameraEditorModeFraming*)userWork;
  DebugEventCameraEditorModeManager* pManager = pSelf->GetModeManager();
  DebugEventCameraEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  gfl2::math::Vector3 printPos = pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->firstFrameWork.target);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f, %.1f, %.1f ]", item->GetName(), printPos.x, printPos.y, printPos.z );
  return workStr;
}

wchar_t* DrawFunc_FramingLastFramePosition( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugEventCameraEditorModeFraming* pSelf = (DebugEventCameraEditorModeFraming*)userWork;
  DebugEventCameraEditorModeManager* pManager = pSelf->GetModeManager();
  DebugEventCameraEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  gfl2::math::Vector3 printPos = pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->lastFrameWork.pos);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f, %.1f, %.1f ]", item->GetName(), printPos.x, printPos.y, printPos.z );
  return workStr;
}

wchar_t* DrawFunc_FramingLastFrameTarget( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  DebugEventCameraEditorModeFraming* pSelf = (DebugEventCameraEditorModeFraming*)userWork;
  DebugEventCameraEditorModeManager* pManager = pSelf->GetModeManager();
  DebugEventCameraEditorCommonWork* pCommonWork = pManager->GetCommonWork();

  gfl2::math::Vector3 printPos = pManager->GetScriptSystem()->CalcEventPositionOffset( pCommonWork->lastFrameWork.target);

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f, %.1f, %.1f ]", item->GetName(), printPos.x, printPos.y, printPos.z );
  return workStr;
}

wchar_t* DrawFunc_FramingU32( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  u32* pU32 = (u32*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %d ]", item->GetName(), *pU32 );
  return workStr;
}

wchar_t* DrawFunc_FramingF32( void* userWork, gfl2::debug::DebugWinItem* item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  f32* pFloat = (f32*)userWork;
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%s [ %.1f ]", item->GetName(), *pFloat );
  return workStr;
}

wchar_t* DrawFunc_FramingZoomBank( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugEventCameraEditorCommonWork::FrameWork* pWork = (DebugEventCameraEditorCommonWork::FrameWork*)userWork;

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"・Fov [ %.1f ]    Bank[ %.1f ]", gfl2::math::ConvRadToDeg( pWork->fovy), gfl2::math::ConvRadToDeg( pWork->bank));
  return workStr;
}

/**
 * @brief コンストラクタ
 */ 
DebugEventCameraEditorModeFraming::DebugEventCameraEditorModeFraming()
  : m_pCameraControllerAdjustPosition( NULL)
  , m_pCameraControllerEventScript( NULL)
  , m_isMoving( false)
{
}

/**
 * @brief デストラクタ
 */ 
DebugEventCameraEditorModeFraming::~DebugEventCameraEditorModeFraming()
{
}

/**
 * @brief 破棄
 */ 
void DebugEventCameraEditorModeFraming::Terminate()
{
}

/**
 * @brief 更新
 * @return 更新後のモードの状態
 */ 
DebugEventCameraEditorModeBase::State DebugEventCameraEditorModeFraming::Update( DebugEventCameraEditorModeManager* pManager)
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) && gfl2::debug::DebugWin_IsOpen())
  {
    return STATE_END;
  }

  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい

  // カメラ移動中は操作禁止
  if( m_pCameraControllerEventScript->IsMoving())
  {
    return STATE_CONTINUE;
  }
  else
  {
    if( m_isMoving)
    {
      pCameraUnit->SwapController( m_pCameraControllerAdjustPosition);

#if DEBUG_ONLY_FOR_saita_kazuki
      m_pCameraControllerAdjustPosition->DumpParameter();
#endif // DEBUG_ONLY_FOR_saita_kazuki

      m_isMoving = false;
    }
  }

  // 再生
  if( pButton->IsTrigger( gfl2::ui::BUTTON_START ) )
  {
#if DEBUG_ONLY_FOR_saita_kazuki
    m_pCameraControllerAdjustPosition->DumpParameter();
#endif // DEBUG_ONLY_FOR_saita_kazuki

    pCameraUnit->SwapController( m_pCameraControllerEventScript);

    m_pCameraControllerEventScript->SetCommonParameters(
      Camera::Controller::ControllerTypeEventScript::ActionState::NONE,
      pCommonWork->firstFrameWork.bank,
      0,
      pCommonWork->frame,
      0.0f,
      static_cast<gfl2::math::Easing::EaseFunc>( pCommonWork->easingType)
      );

    pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
    pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
    pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);
    pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);

    m_pCameraControllerEventScript->Move(
      pCommonWork->lastFrameWork.pos,
      pCommonWork->lastFrameWork.target,
      pCommonWork->frame,
      static_cast<gfl2::math::Easing::EaseFunc>( pCommonWork->easingType),
      gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.bank),
      gfl2::math::ConvRadToDeg( pCommonWork->lastFrameWork.fovy)
      );

    m_isMoving = true;
  }

  {
    if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
    {
      // ファーストフレーム適用
      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) )
      {
        pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
        pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
        pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);
        pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);
      
        m_pCameraControllerAdjustPosition->SetUp( pBaseCamera );
      }

      // ラストフレーム適用
      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL ) )
      {
        pBaseCamera->SetPosition( pCommonWork->lastFrameWork.pos);
        pBaseCamera->SetTargetPosition( pCommonWork->lastFrameWork.target);
        pBaseCamera->SetRotationQuat( pCommonWork->lastFrameWork.rotation);
        pBaseCamera->SetFovy( pCommonWork->lastFrameWork.fovy);
        
        m_pCameraControllerAdjustPosition->SetUp( pBaseCamera );
      }
    }
    else
    {
      // ファーストフレーム登録
      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZR ) )
      {
        pCommonWork->firstFrameWork.pos = pBaseCamera->GetPosition();
        pCommonWork->firstFrameWork.target = pBaseCamera->GetTargetPosition();
        pCommonWork->firstFrameWork.rotation = pBaseCamera->GetRotationQuat();

        pCommonWork->firstFrameWork.fovy = pBaseCamera->GetFovy();
        pCommonWork->firstFrameWork.bank = m_pCameraControllerAdjustPosition->GetBank();
      }

      // ラストフレーム登録
      if( pButton->IsTrigger( gfl2::ui::BUTTON_ZL ) )
      {
        pCommonWork->lastFrameWork.pos = pBaseCamera->GetPosition();
        pCommonWork->lastFrameWork.target = pBaseCamera->GetTargetPosition();
        pCommonWork->lastFrameWork.rotation = pBaseCamera->GetRotationQuat();

        pCommonWork->lastFrameWork.fovy = pBaseCamera->GetFovy();
        pCommonWork->lastFrameWork.bank = m_pCameraControllerAdjustPosition->GetBank();
      }
    }
  }

  {
    // 会話ウインドウ表示ON/OFF
    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      m_pManager->SetTalkWindowVisible( false);
    }
    else
    {
      m_pManager->SetTalkWindowVisible( true);
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
    {
      // 構図アシストON/OFF
      if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
      {
        if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION ) )
        {
          FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION, 0);
        }
        else
        {
          FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION, 1);
        }
      }
      // カメラターゲット座標表示
      else
      {
        if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST) )
        {
          FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST, 0);
        }
        else
        {
          FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::VISIBLE_CAMERA_COMPOSITION_ASSIST, 1);
        }
      }
    }
  }

  return STATE_CONTINUE;
}

/**
 * @brief セットアップ
 */ 
void DebugEventCameraEditorModeFraming::Setup( gfl2::debug::DebugWinGroup* pDebugGroup)
{
  m_pDebugGroup = pDebugGroup;

  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  // デバッグ項目登録
  // gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, m_pMsgData, msg_dmenu_event_camera_editor_info, reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"[START]カメラ移動を再生", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"[RT]今のカメラをFFに保存　[LT]今のカメラをLFに保存", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"[X+RT]今のカメラにFFを適用[X+LT]今のカメラにLFを適用", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true );
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"★FirstFrame (FF)", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Body ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingFirstFramePosition )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Int  ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingFirstFrameTarget )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Zoom Bank", reinterpret_cast<void*>( &pCommonWork->firstFrameWork), NULL, DrawFunc_FramingZoomBank )->SetIsSelectSkip( true);
  // gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"     ↓     ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true);
  // gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"     ↓     ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"★LastFrame (LF)", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingName )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Body ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingLastFramePosition )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Int  ", reinterpret_cast<void*>( this), NULL, DrawFunc_FramingLastFrameTarget )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"・Zoom Bank", reinterpret_cast<void*>( &pCommonWork->lastFrameWork), NULL, DrawFunc_FramingZoomBank )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemUpdateFunc( m_pDebugGroup, m_pHeap, L"★EasingType", reinterpret_cast<void*>( &pCommonWork->easingType), UpdateFunc_EditEasingType, DrawFunc_EditEasingType )->SetIsSelectSkip( true);
  gfl2::debug::DebugWin_AddItemValueU32( m_pDebugGroup, m_pHeap, L"★Frame", &pCommonWork->frame, 1, 30*30, 1)->SetIsSelectSkip( true);

  // メンバの初期化
  m_isMoving = false;
  m_pCameraControllerEventScript = NULL;

  // スクリプトカメラユニット取得
  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);

  // 位置調整用カメラコントローラーを生成
  m_pCameraControllerAdjustPosition = GFL_NEW( m_pHeap )Camera::Controller::ControllerTypeAdjustPosition( pCameraManager->GetTarget() );

  // スクリプトカメラコントローラーから位置調整カメラコントローラーに変更
  Camera::Controller::ControllerBase* pScriptController = pCameraUnit->SwapController( m_pCameraControllerAdjustPosition);
  m_pCameraControllerEventScript = reinterpret_cast<Camera::Controller::ControllerTypeEventScript*>( pScriptController);

  // 現在のカメラ位置、ターゲット位置をファーストフレームにする
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);

  // バンクセット
  m_pCameraControllerAdjustPosition->SetBank( pCommonWork->firstFrameWork.bank);

  // カメラ更新許可
  pCameraUnit->SetUpdateFlag( true);

  // 会話ウインドウの元の表示状態を保持しておく
  m_pManager->SaveTalkWindowVisible();
}

/**
 * @brief クリーン
 */ 
void DebugEventCameraEditorModeFraming::Clean()
{
  DebugEventCameraEditorCommonWork* pCommonWork = m_pManager->GetCommonWork();

  // 会話ウインドウの元の表示状態を元に戻す
  m_pManager->ResetTalkWindowVisible();

  // スクリプトカメラユニット取得
  Camera::CameraManager* pCameraManager = m_pFieldmap->GetCameraManager();
  Camera::CameraUnit* pCameraUnit = pCameraManager->GetCameraByUnitNo( Camera::UnitNo::EVENT);

  // カメラ更新禁止
  pCameraUnit->SetUpdateFlag( false);

  // カメラ位置、ターゲット位置をファーストフレームに戻す
  poke_3d::model::BaseCamera* pBaseCamera = const_cast<poke_3d::model::BaseCamera*>( pCameraUnit->GetBaseCamera()); // デバッグ用なので許して欲しい
  pBaseCamera->SetPosition( pCommonWork->firstFrameWork.pos);
  pBaseCamera->SetTargetPosition( pCommonWork->firstFrameWork.target);
  pBaseCamera->SetRotationQuat( pCommonWork->firstFrameWork.rotation);
  pBaseCamera->SetFovy( pCommonWork->firstFrameWork.fovy);

  // スクリプトカメラコントローラーに戻す
  pCameraUnit->SwapController( m_pCameraControllerEventScript);

  // 位置調整用カメラコントローラーをは破棄
  GFL_SAFE_DELETE( m_pCameraControllerAdjustPosition);

  // メンバの後始末
  m_pCameraControllerEventScript = NULL;
  m_isMoving = false;

  // デバッグ項目削除
  m_pDebugGroup->RemoveAllItem();
}

/**
 * @brief 初期化
 */ 
void DebugEventCameraEditorModeFraming::initialize()
{
}

/**
* @brief 情報表示 
*/ 
void DebugEventCameraEditorModeFraming::DrawTextInfo( gfl2::util::DrawUtilText* pDrawText )
{
  if( m_pCameraControllerEventScript->IsMoving())
  {
    return;
  }

  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  gfl2::math::Vector2 start( 5.0f, 5.0f );
  gfl2::math::Vector2 pos( 0.0f, 0.0f );
  pos = start;
  f32 lineDistance = 12.0f;
  pDrawText->SetTextScale( 0.75f, 0.75f );
  pDrawText->SetTextColor( gfl2::math::Vector4( 255.0f / 255.0f, 79.0f / 255.0f, 25.0f / 255.0f, 1.0f ) );

#if defined(GF_PLATFORM_WIN32)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawTextW( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#elif defined(GF_PLATFORM_CTR)
#define CAMERA_DRAW_TEXT( ... ) pDrawText->DrawText( pos.x, pos.y, __VA_ARGS__); pos.y += lineDistance;
#endif

  if( DebugEventCameraEditorModeManager::m_IsDrawTextInfo )
  {
    if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
    {
      if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
      {
        CAMERA_DRAW_TEXT( L"スティック:カメラポジション基準回転" );
        CAMERA_DRAW_TEXT( L"Rボタン:加速モード" );
        CAMERA_DRAW_TEXT( L"Lボタン:単軸モード" );
        CAMERA_DRAW_TEXT( L" " );
      }
      else
      {
        CAMERA_DRAW_TEXT( L"スティック:カメラターゲット垂直移動" );
        CAMERA_DRAW_TEXT( L"Rボタン:加速モード" );
        CAMERA_DRAW_TEXT( L"Lボタン:単軸モード" );
        CAMERA_DRAW_TEXT( L" " );
      }
    }
    else if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
    {
      CAMERA_DRAW_TEXT( L"スティック:カメラターゲット水平移動" );
      CAMERA_DRAW_TEXT( L"十字キー上下　:カメラポジション距離変更" );
      CAMERA_DRAW_TEXT( L"Rボタン:加速モード" );
      CAMERA_DRAW_TEXT( L"Lボタン:単軸モード" );
      CAMERA_DRAW_TEXT( L" " );
    }

    if( pButton->IsHold( gfl2::ui::BUTTON_X ) )
    {
      CAMERA_DRAW_TEXT( L"Xボタン + RTボタン:今のカメラにFirstFrameを適用" );
      CAMERA_DRAW_TEXT( L"Xボタン + LTボタン:今のカメラにLastFrameを適用" );
      CAMERA_DRAW_TEXT( L" " );
    }

    if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      CAMERA_DRAW_TEXT( L"Rボタン+Lボタン:カメラターゲット座標表示切り替え" );
      CAMERA_DRAW_TEXT( L" " );
    }
  }

  // 右上
  pos = gfl2::math::Vector2( 280.0f, 230.0f );
  pDrawText->SetTextScale( 0.6f, 0.6f );
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::VISIBLE_CAMERA_TARGET_POSITION ) )
  {
    CAMERA_DRAW_TEXT( L"カメラターゲット座標表示中" );
  }

}
GFL_NAMESPACE_END( Field )

#endif // GF_ENABLE_DEBUG_EDITOR
#endif // PM_DEBUG

