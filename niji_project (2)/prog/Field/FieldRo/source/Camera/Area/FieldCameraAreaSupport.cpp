/**
* @brief  フィールドカメラ、サポート機能
* @file   FieldCameraAreaSupport.cpp
* @author ikuta_junya
* @data   2015.05.21
*/

// niji
#include "GameSys/include/GameManager.h"

// field
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaSupport.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaFocus.h"
#include "Field/FieldRo/include/Camera/FieldCameraUiDeviceTranslator.h"

// debug
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>


#if PM_DEBUG
#if defined(GF_PLATFORM_WIN32)
#include <system/include/InputDevice/gfl2_Pad.h>
#endif // #if defined(GF_PLATFORM_WIN32)

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif // #if PM_DEBUG

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

const static u32 STOP_MASK_EVENT = 0x01;
const static u32 STOP_MASK_FOCUS = 0x01 << 1;

static const f32 MOVE_DIRECTION_CHANGE_SPEED( 1.0f / 15.0f );
static const f32 MOVE_DIRECTION_DISTANCE( 0.6f );

#if PM_DEBUG
static const f32 MOVE_DIRECTION_DEBUG_CHANGE_SPEED( 0.2f );
#endif // #if PM_DEBUG

struct SUPPORT_SETTING
{
  f32 defaultValue; 
  s32 scrollStopMoveType;
};

SUPPORT_SETTING supportSetting =
{
  1.0f,                        // defaultValue
  Support::MoveType::MAX,      // eupportMoveType
};

/**
* @brief コンストラクタ
*
* @param rInitDesc  初期化設定
*
* @return 無し
*/ 
Support::Support( const InitializeDescription& rInitDesc ) :

  // CAMERA_AREA_SUPPORT_SETTING_DATA  m_Data;   // 現在の設定

  m_State( State::STOP )
  ,m_StopBit( 0 )
  ,m_IsScrollStop( false )
  ,m_InWaitFrame( 0 )
  ,m_OutWaitFrame( 0 )
  ,m_pMoveModelPlayer( NULL )
#if PM_DEBUG
  ,m_pDebugWinGroup( NULL )
#endif
{
  m_pTarget = rInitDesc.m_pTarget;
  m_pMoveModelPlayer = rInitDesc.m_pMoveModelPlayer;
  m_pFocus = rInitDesc.m_pFocus;
  m_Data.type = SupportType::NONE;
}

/**
* @brief デストラクタ
*
* @return 無し
*/
Support::~Support( void )
{
#if PM_DEBUG
  UnResistDebugMenu();
#endif // #if PM_DEBUG
}

/**
* @brief 更新処理
*
* @return 無し
*/
void Support::Update( void )
{
#if PM_DEBUG
  // デバッグ調整機能、カメラサポート設定の強制上書き
  if( Debug::DebugTypes::s_CameraSupportEnable )
  {
    m_Data = Debug::DebugTypes::s_CameraSupport;
  }
#endif


  if( m_pMoveModelPlayer == NULL )
  {
    return;
  }

  if( m_StopBit )
  {
    return;
  }

  switch( m_Data.type )
  {
  case SupportType::NONE:
    {
      return;
    }

  case SupportType::DIRECTION:
  case SupportType::DISTANCE:
  case SupportType::ALL:
    {
      break;
    }
  default:
    {
      return;
    }
  }

  // 移動判定
  MoveType::Value moveType( MoveType::NONE );
  moveType = CheckPlayerMove( m_pMoveModelPlayer );

  // フォーカス発動中は必ずKEEP
  if( m_pFocus && m_pFocus->IsValid() )
  {
    moveType = MoveType::MOVE_KEEP;
  }
  else if( m_IsScrollStop )
  {
    // スクロールストップHIT時の挙動上書き
    if( supportSetting.scrollStopMoveType != MoveType::MAX )
    {
      moveType = static_cast< MoveType::Value >( supportSetting.scrollStopMoveType );
    }
  }

  // 移動係数を更新
  UpdateValue( moveType );
}

/**
* @brief サポート挙動が有効か取得する
*
* @return true 有効 false 無効
*/
b32 Support::IsValid( void ) const
{
  return ( m_Data.type != SupportType::NONE );
}

/**
* @brief 指定データの設定に変更
*
* @param  rData 設定
*
* @return なし
*/
void Support::Change( const CAMERA_AREA_SUPPORT_SETTING_DATA& rData, b32 face )
{
  if( face )
  {
    Reset();
    m_Data = rData;
  }
  else
  {
    // NONEへの遷移、NONEからの遷移はリセット
    if( rData.type == SupportType::NONE || m_Data.type == SupportType::NONE )
    {
      Reset();
    }
    else
    {
      // Valueを継承
      f32 t = m_ValueController.GetValue() / m_Data.maxValue;
      m_ValueController.Set( rData.maxValue * t );
      ResetWaitFrame();
    }

    m_Data = rData;
  }
}

/**
* @brief 距離を変換
*
* @param  src 元の距離
*
* @return 結果
*/
f32 Support::ConvertDistance( f32 src ) const
{
  // そのまま
  if( m_Data.type != SupportType::DISTANCE &&  m_Data.type != SupportType::ALL )
  {
    return src;
  }

  return src * m_ValueController.GetValue();
}

/**
* @brief カメラサポート挙動の制御アクションの実行
*
* @param  value     アクション種別
*
* @return 成否
*/
b32 Support::Action( SupportAction::Value value )
{
  switch( value )
  {
    // 再開
  case SupportAction::RESUME:
    {
      m_StopBit = ( m_StopBit & (~STOP_MASK_EVENT) );
      break;
    }

    // 一時停止
  case SupportAction::STOP:
    {
      ResetWaitFrame();
      m_StopBit = ( m_StopBit | STOP_MASK_EVENT );
      break;
    }
  }

  return true;
}

/**
* @brief 距離サポートが有効か取得する
*
* @return trueなら有効、falseなら無効
*/
b32 Support::IsDistanceSupport( void ) const
{
  switch( m_Data.type )
  {
  case SupportType::DISTANCE:
  case SupportType::ALL:
    {
      return true;
    }
  }

  return false;
}


/**
* @brief 現在のサポート係数を取得する
*
* @return サポート係数
*/
f32 Support::GetValue( void ) const
{
  return m_ValueController.GetValue();
}

void Support::UpdateValue( MoveType::Value moveType )
{
  if( moveType == MoveType::MOVE_KEEP )
  {
    ResetWaitFrame();
    return;
  }

  f32 defaultValue = supportSetting.defaultValue;

  // 更新
  switch( m_State )
  {

  case State::MOVE_IN:
  case State::MOVE_OUT:
    {
      b32 endFlag( false );

      if( 
        ( moveType == MoveType::MOVE && m_State == State::MOVE_OUT ) ||
        ( ( moveType == MoveType::NONE || moveType == MoveType::MOVE_IN ) && m_State == State::MOVE_IN )
        )
      {
        m_ValueController.Update();

        if( m_ValueController.IsEnd() )
        {
          endFlag = true;
        }
      }
      else
      {
        endFlag = true;
      }

      if( endFlag )
      {
        m_OutWaitFrame = 0;
        m_InWaitFrame = 0;
        m_State = State::STOP;
        // break;
      }
      else
      {
        break;
      }
    }

  case State::STOP:
    {
      if( moveType == MoveType::MOVE )
      {
        m_OutWaitFrame += 1;
        m_InWaitFrame = 0;

        // out開始
        if( m_OutWaitFrame > m_Data.outData.waitFrame )
        {
          m_ValueController.Move( 
            m_ValueController.GetValue(),
            m_Data.maxValue,
            m_Data.outData.moveFrame,
            static_cast< gfl2::math::Easing::EaseFunc >( m_Data.outData.easeFunc )
            );

          m_State = State::MOVE_OUT;
        }
      }
      else
      {
        m_OutWaitFrame = 0;
        m_InWaitFrame += 1;

        // in開始
        if( moveType == MoveType::MOVE_IN || m_InWaitFrame > m_Data.inData.waitFrame )
        {
          m_InWaitFrame = m_Data.inData.waitFrame;

          m_ValueController.Move( 
            m_ValueController.GetValue(),
            defaultValue,
            m_Data.inData.moveFrame,
            static_cast< gfl2::math::Easing::EaseFunc >( m_Data.inData.easeFunc )
            );

          m_State = State::MOVE_IN;
        }
      }

      break;
    }
  }

#if PM_DEBUG
  switch( Debug::DebugTypes::s_CameraSupportDebugMode )
  {
  case Debug::DebugTypes::CameraSupportDebugMode::MAX_FIX:
    {
      // max固定
      m_ValueController.Set( m_Data.maxValue );
      break;
    }

  case Debug::DebugTypes::CameraSupportDebugMode::MIN_FIX:
    {
      // min固定
      m_ValueController.Set( defaultValue );
      break;
    }
  }
#endif
}

// 自機の状態をチェックし、サポートの動作を取得
Support::MoveType::Value Support::CheckPlayerMove( const MoveModel::FieldMoveModelPlayer* pMoveModelPlayer )
{
  // 念のためNULLチェック
  if( pMoveModelPlayer == NULL )
  {
    return MoveType::NONE;
  }
  
  MoveModel::FieldMoveModelPlayer* pPlayer = const_cast< MoveModel::FieldMoveModelPlayer* >( pMoveModelPlayer );

  // ムーランドでサーチ中は特殊
  if( pPlayer->IsMuurandoSearchState() )
  {
    return MoveType::MOVE_IN;
  }

  // 停止中 @note 操作感を重視し、スティックが入力されていないときも停止扱い
  if( pPlayer->IsMoveState() == false || pPlayer->IsInputStick() == false )
  {
    return MoveType::NONE;
  }

  //
  // 以下、移動中の判定
  //

  // ライドで移動中 == MOVE
  if( pPlayer->IsRideMoveModel() )
  {
    return MoveType::MOVE;
  }
  // 徒歩の場合、走っている == MOVE 歩いている == KEEP
  else
  {
    if( pPlayer->IsRunLoopState() )
    {
      return MoveType::MOVE;
    }
    else
    {
      return MoveType::MOVE_KEEP;
    }
  }
}

void Support::Reset( void )
{
  ResetMoveFrame();
}

void Support::ResetMoveFrame( void )
{
  m_State = State::STOP;
  m_InWaitFrame = 0;
  m_OutWaitFrame = 0;
  m_ValueController.Set( 1.0f );
}

void Support::ResetWaitFrame( void )
{
  m_State = State::STOP;
  m_InWaitFrame = 0;
  m_OutWaitFrame = 0;
}

#if PM_DEBUG

//
// デバッグメニュー表示用関数
//
GFL_NAMESPACE_BEGIN( Disp );

wchar_t* SupportDebugMode( void* userWork, void* pValue, gfl2::debug::DebugWinItem *item )
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

wchar_t* SupportType( void* userWork, void* pValue, gfl2::debug::DebugWinItem *item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32 value = *(s32*)pValue;

  wchar_t* table[] =
  {
    L"NONE",
    L"DIRECTION",
    L"DISTANCE",
    L"ALL",
    L"ERROR",
  };

  if( value >= GFL_NELEMS( table ) )
  {
    value = GFL_NELEMS( table ) - 1;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), table[ value ] );
  return workStr;
}

wchar_t* MoveType( void* userWork, void* pValue, gfl2::debug::DebugWinItem *item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  s32 value = *(s32*)pValue;
  
  wchar_t* table[] =
  {
    L"STOP",
    L"MOVE",
    L"MOVE_KEEP",
    L"MOVE_IN",
    L"NONE",
  };

  if( value >= GFL_NELEMS( table ) )
  {
    value = GFL_NELEMS( table ) - 1;
  }

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%ls]", item->GetName(), table[ value ] );
  return workStr;
}

GFL_NAMESPACE_END( Disp );

//
// デバッグメニュー項目用関数
//
GFL_NAMESPACE_BEGIN( Item );

static void SupportTestReset( void* pUserWork, gfl2::debug::DebugWinItem* pItem )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) == false )
  {
    return;
  }

  Debug::DebugTypes::s_CameraSupport = Debug::DebugTypes::s_CameraSupportDefault;
}

GFL_NAMESPACE_END( Item );

// デバッグメニュー追加
void Support::ResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ取得」→「FieldDebugMenuから直接グループを取得」に変更。
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::debug::DebugWinGroup* pGroup = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup();
  if( pGroup )
  {
    gfl2::heap::HeapBase *pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    gfl2::debug::DebugWinGroup* pSupportGroup = gfl2::debug::DebugWin_AddGroup( pHeap, L"Support", pGroup );

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    gfl2::str::MsgData* pMsgData = pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetMessageData();

    // エリア設定
    {
      gfl2::debug::DebugWinGroup* pTestGrop = gfl2::debug::DebugWin_AddGroup( pHeap, L"AreaSetting", pSupportGroup );

      // enable
      gfl2::debug::DebugWin_AddItemValueInt( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_enable, &Debug::DebugTypes::s_CameraSupportEnable, 0, 1, 1 );

      // reset
      gfl2::debug::DebugWin_AddItemUpdateFunc( 
        pTestGrop, 
        pHeap, 
        pMsgData, msg_dmenu_camera_support_reset,
        NULL, 
        Item::SupportTestReset,
        NULL
        );

      // type
      {  
        gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
        pMsgData->GetString( msg_dmenu_camera_support_type, *workStrBuf );
        gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pTestGrop, pHeap, workStrBuf->GetPtr(), NULL, Disp::SupportType, (s32*)(&Debug::DebugTypes::s_CameraSupport.type), 0, Camera::Area::SupportType::MAX - 1, 1 );
      }

      // value
      gfl2::debug::DebugWin_AddItemValueFloat( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_max_val, &Debug::DebugTypes::s_CameraSupport.maxValue, 1.0f, 10000.0f, 0.01f );

      // out
      {
        gfl2::debug::DebugWin_AddItemUpdateFunc( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_out, NULL, NULL );
        gfl2::debug::DebugWin_AddItemValueU16( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_wait_frm, &Debug::DebugTypes::s_CameraSupport.outData.waitFrame, 0, 10000, 1 );
        gfl2::debug::DebugWin_AddItemValueU16( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_move_frm, &Debug::DebugTypes::s_CameraSupport.outData.moveFrame, 0, 10000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pTestGrop, pHeap, L"-EaseFunc", &Debug::DebugTypes::s_CameraSupport.outData.easeFunc );
      }

      // in
      {
        gfl2::debug::DebugWin_AddItemUpdateFunc( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_in, NULL, NULL );
        gfl2::debug::DebugWin_AddItemValueU16( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_wait_frm, &Debug::DebugTypes::s_CameraSupport.inData.waitFrame, 0, 10000, 1 );
        gfl2::debug::DebugWin_AddItemValueU16( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_move_frm, &Debug::DebugTypes::s_CameraSupport.inData.moveFrame, 0, 10000, 1 );
        Field::Debug::FieldDebugMenu::DebugWin_AddItemValueEaseFunc( pTestGrop, pHeap, L"-EaseFunc", &Debug::DebugTypes::s_CameraSupport.inData.easeFunc );
      }
    }
    
    // 共通設定
    {
      gfl2::debug::DebugWinGroup* pCommonGroup = gfl2::debug::DebugWin_AddGroup( pHeap, L"CommonSetting", pSupportGroup );
      gfl2::debug::DebugWin_AddItemValueFloat( pCommonGroup, pHeap, pMsgData, msg_dmenu_camera_support_def_val, &supportSetting.defaultValue, 0.01f, 100.0f, 0.01f );
      gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pCommonGroup, pHeap, L"scrollStopMoveType", NULL, Disp::MoveType, (s32*)(&supportSetting.scrollStopMoveType), 0, MoveType::MAX, 1 );
    }

    // テストモード
    {  
      // gfl2::debug::DebugWin_AddItemUpdateFunc( pTestGrop, pHeap, pMsgData, msg_dmenu_camera_support_load_test, NULL, Item::Camera::LoadTest, NULL );
      
      gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
      pMsgData->GetString( msg_dmenu_camera_support_dbgmode, *workStrBuf );
      gfl2::debug::DebugWinItemValueDispOverRide<s32>::AddItem( pSupportGroup, pHeap, workStrBuf->GetPtr(), NULL, Disp::SupportDebugMode, &Debug::DebugTypes::s_CameraSupportDebugMode, 0, Debug::DebugTypes::CameraSupportDebugMode::MAX - 1, 1 );
    }

    // 記憶
    m_pDebugWinGroup = pSupportGroup;
  }
}

// デバッグメニュー削除
void Support::UnResistDebugMenu( void )
{
  // @fix GFNMCat[1208]:デバッグメニュー：本来選択できない項目が登録され、選択すると停止する。対処。 グループ名の変更に追従するため、「ハードコーディングの名前指定によるグループ削除」→「記憶したグループクラスを指定して削除」に変更。
  if( m_pDebugWinGroup )
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->GetFieldmap()->GetFieldDebugMenu() && pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup() )
    {
      pGameManager->GetFieldmap()->GetFieldDebugMenu()->GetCameraRootGroup()->RemoveItem( m_pDebugWinGroup );
      m_pDebugWinGroup = NULL;
    }
    else
    {
      // @note 無いなら親ごと削除されている
      m_pDebugWinGroup = NULL;
    }
  }
}

#endif // PM_DEBUG

GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );