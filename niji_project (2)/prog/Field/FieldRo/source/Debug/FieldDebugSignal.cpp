//======================================================================
/**
 * @file FieldDebugSignal.cpp
 * @date 2015/07/27 18:17:51
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Debug/FieldDebugSignal.h"

// niji
#include "GameSys/include/GameManager.h"

#include <Field/FieldRo/include/MotionSignal/FieldMotionSignal.h>
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include <Field/FieldRo/include/PlacementData/actor/FieldTrainerActor.h>
#include <Field/FieldRo/include/Camera/FieldCameraManager.h>
#include <Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypeDebugTrainer.h>
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include <Field/FieldRo/include/Fieldmap.h>
#include <Field/FieldRo/include/Camera/Target/FieldCameraTargetTypeMoveModel.h>
#include <Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerEye.h>
#include <Field/FieldStatic/include/MoveCode/FieldMoveCodeTrainerPath.h>
#include <Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h>

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

// 描画：視線
static wchar_t *DrawTrainerEyeHeight( void *userWork, gfl2::debug::DebugWinItem *item )
{
  f32 eye_height = 6.5f;
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( pDebug->GetActor( pDebug->GetCnt() ) );
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pMoveModel->GetMoveCodeWork();
  if( pMoveCodeWork->moveCodeId == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_EYE )
  {
    Field::MoveModel::FieldMoveCodeTrainerEye::Work *pWork = 
      static_cast<Field::MoveModel::FieldMoveCodeTrainerEye::Work*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Field::MoveModel::FieldMoveCodeTrainerEye::Work) ) );
    eye_height = pWork->eyeHeightRate;
  }
  else if( pMoveCodeWork->moveCodeId == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_PATH )
  {
    Field::MoveModel::FieldMoveCodeTrainerPath::Work *pWork = 
      static_cast<Field::MoveModel::FieldMoveCodeTrainerPath::Work*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Field::MoveModel::FieldMoveCodeTrainerPath::Work) ) );
    eye_height = pWork->eyeHeightRate;
  }

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[ %f ]", item->GetName(), eye_height );
  return workStr;
}
// 更新：視線
static void UpdateTrainerEyeHeight( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  f32 add = 0.0f;
  bool bAdd = false;

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) ){ add = -1.0f; bAdd = true; }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) ){ add = 1.0f; bAdd = true; }
  if( pButton->IsHold( gfl2::ui::BUTTON_L) ){ add *= 0.1f; }
  if( pButton->IsHold( gfl2::ui::BUTTON_R) ){ add *= 10.0f; }

  if( !bAdd ){ return; }

  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( pDebug->GetActor( pDebug->GetCnt() ) );
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  Field::MoveModel::FieldMoveCodeWork *pMoveCodeWork = pMoveModel->GetMoveCodeWork();
  if( pMoveCodeWork->moveCodeId == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_EYE )
  {
    Field::MoveModel::FieldMoveCodeTrainerEye::Work *pWork = 
      static_cast<Field::MoveModel::FieldMoveCodeTrainerEye::Work*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Field::MoveModel::FieldMoveCodeTrainerEye::Work) ) );
    pWork->eyeHeightRate += add;
  }
  else if( pMoveCodeWork->moveCodeId == Field::MoveModel::FIELD_MOVE_CODE_TRAINER_PATH )
  {
    Field::MoveModel::FieldMoveCodeTrainerPath::Work *pWork = 
      static_cast<Field::MoveModel::FieldMoveCodeTrainerPath::Work*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Field::MoveModel::FieldMoveCodeTrainerPath::Work) ) );
    pWork->eyeHeightRate += add;
  }

}
//! キャラクタのリロード
static void UpdateReloadCharacterFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ){ return; }

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  pDebug->ReloadChara();
}
//! 選択キャラクタの変更：更新
static void UpdateChangeCharacterFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    pDebug->ChangeTarget( NULL );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( pDebug->GetActor( pDebug->GetCnt() ) );
    if( pActor )
    {
      Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
      pDebug->ChangeTarget( pMoveModel );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->ChangeCharacter( -1 );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->ChangeCharacter( 1 );
  }
}
//! 選択キャラクタの変更：描画
static wchar_t* DrawChangeCharacterFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d/%d]", item->GetName(), pDebug->GetCnt() + 1, pDebug->GetMax() );
  return workStr;
}
//! シグナルの配列番号の変更：更新
static void UpdateChangeDataIdxFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->ChangeSignalDataIdx( -1 );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->ChangeSignalDataIdx( 1 );
  }
}
//! シグナルの配列番号の変更：描画
static wchar_t* DrawChangeDataIdxFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetDataIdx() + 1 );
  return workStr;
}
//! モーション番号の変更：更新
static void UpdateChangeAnimeIDFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  u32 animeID = pDebug->GetAnimeID();
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) && animeID >= 1)
  {
    -- animeID;
    pDebug->SetAnimeID( animeID );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    ++ animeID;
    pDebug->SetAnimeID( animeID );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( pDebug->GetActor( pDebug->GetCnt() ) );
    if( pActor )
    {
      Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
      pMoveModel->GetCharaDrawInstance()->ChangeAnimation( animeID );
    }
  }
}
//! モーション番号の変更：描画
static wchar_t* DrawChangeAnimeIDFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetAnimeID() );
  return workStr;
}
//! イベント種別の変更：更新
static void UpdateChangeTypeFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  u32 type = pDebug->GetType();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && type >= 1 )
  {
    -- type;
    pDebug->SetType( type );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && type < SIGNAL_TYPE_MAX - 1 )
  {
    ++ type;
    pDebug->SetType( type );
  }
}
//! イベント種別の変更：描画
static wchar_t* DrawChangeTypeFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();

  switch( pDebug->GetType() )
  {
  case Field::SIGNAL_TYPE_SPD:
    pDebug->GetMessageData()->GetString( msg_dmenu_field_signal_edit_type_chg_spd, *workStrBuf );
    break;
  case Field::SIGNAL_TYPE_EYE_RATIO:
    pDebug->GetMessageData()->GetString( msg_dmenu_field_signal_edit_type_eye_ratio, *workStrBuf );
    break;
  case Field::SIGNAL_TYPE_FREEZE:
    pDebug->GetMessageData()->GetString( msg_dmenu_field_signal_edit_type_anime_frz, *workStrBuf );
    break;
  default:
    pDebug->GetMessageData()->GetString( msg_dmenu_field_signal_edit_type_meaningless, *workStrBuf );
    break;
  }
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls:%ls", item->GetName(), workStrBuf->GetPtr() );
  return workStr;
}
//! 起動フレームの変更：更新
static void UpdateChangeFrameFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  s32 frame = pDebug->GetFrame();
  s32 rev = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    rev *= 10;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    frame -= 1 * rev;
    if( frame < 0 ){ frame = 0; }
    pDebug->SetFrame( frame );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    frame += 1 * rev;
    pDebug->SetFrame( frame );
  }
}
//! 起動フレームの変更：描画
static wchar_t* DrawChangeFrameFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetFrame() );
  return workStr;
}
//! 起動パラメータの変更：更新
static void UpdateChangeValueFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  s32 value = pDebug->GetValue();
  s32 rev = 1;
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    rev *= 10;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    value -= 1 * rev;
    pDebug->SetValue( value );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    value += 1 * rev;
    pDebug->SetValue( value );
  }
}
//! 起動パラメータの変更：描画
static wchar_t* DrawChangeValueFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetValue() );
  return workStr;
}
//! シグナルの登録
static void UpdateRegistDataFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RegistData();
  }
}
//! シグナルの削除
static void UpdateClearDataFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->ClearData();
  }
}
//! データのリセット
static void UpdateResetAllFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->ResetAllData();
  }
}

/**
 *  @brief  Edit dtor
 */
static void DestroyEditFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}
/**
 *  @brief  Edit ctor
 */
static void CreateEditFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_no, pDebug, UpdateChangeDataIdxFunc, DrawChangeDataIdxFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_anime, pDebug, UpdateChangeAnimeIDFunc, DrawChangeAnimeIDFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_frame, pDebug, UpdateChangeFrameFunc, DrawChangeFrameFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_type, pDebug, UpdateChangeTypeFunc, DrawChangeTypeFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_value, pDebug, UpdateChangeValueFunc, DrawChangeValueFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_regist, pDebug, UpdateRegistDataFunc, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_edit_clear, pDebug, UpdateClearDataFunc, NULL );
  }
}
/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_reload, pDebug, UpdateReloadCharacterFunc, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_select, pDebug, UpdateChangeCharacterFunc, DrawChangeCharacterFunc );

    {
      gfl2::debug::DebugWinGroup *pEdit = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_signal_edit_top, pGroup );
      pEdit->SetCreateDestroyGroupFunc( pDebug, CreateEditFunc, DestroyEditFunc );
    }
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_signal_eye_hight, pDebug, UpdateTrainerEyeHeight, DrawTrainerEyeHeight );
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugSignal *pDebug = reinterpret_cast<DebugSignal*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    pDebug->ChangeTarget( NULL );
    pGroup->RemoveAllItem();
  }
}

/**
 *  @class  DebugSignal
 *  @brief  シグナルのデバッグ
 */
//! コンストラクタ
DebugSignal::DebugSignal()
{
  m_pHeap = NULL;
  m_nDataIdx = 0;
  m_pCtrler = NULL;
  m_pTarget = NULL;
}
//! デストラクタ
DebugSignal::~DebugSignal()
{
  Terminate();
}

/**
 *  @brief  初期化
 *  @param  pHeap   ヒープ
 */
void DebugSignal::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData *pMsgData )
{
  m_pHeap = pHeap;
  {
    GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    m_pFieldmap = pGameManager->GetFieldmap();
    Field::FieldResident *pResident = pGameManager->GetFieldResident();
    m_pPlacementMgr = pResident->GetPlacementDataManager();
    m_pCamMgr = pGameManager->GetFieldmap()->GetCameraManager();
    m_pMsgData = pMsgData;
    ReloadChara();
  }
}

/**
 *  @brief  解放
 */
void DebugSignal::Terminate()
{
}

/**
 *  @brief  デバッグメニューの作成
 *  @param  root  メニュー親
 */
void DebugSignal::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  m_pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_signal_top, root );
  m_pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
 *  @brief  カメラ対象の変更
 *  @param  pModel    カメラ注視対象：NULLの場合Playerにもどす
 */
void DebugSignal::ChangeTarget( Field::MoveModel::FieldMoveModel *pModel )
{
  if( !m_pCamMgr ){ return; }

  if( !pModel )
  {
    m_pCamMgr->GetMainGamePlayCamera()->ChangeController( NULL );

    GFL_SAFE_DELETE( m_pTarget );

    m_pCamMgr->ChangeMainCameraByUnitNo( Camera::UnitNo::FIELD );

    m_pCamMgr->SetUpForNormalMap();
    return;
  }
  else
  {
    m_pCamMgr->GetCameraByUnitNo( Camera::UnitNo::DEBUG )->CopyBaseParam( *m_pCamMgr->GetMainGamePlayCamera() );
    m_pCamMgr->ChangeMainCameraByUnitNo( Camera::UnitNo::DEBUG );

    GFL_SAFE_DELETE( m_pTarget );
    // @todo 現行のターゲットクラスが自機専用になったので一時的にコメントアウト、2月修正予定
    // m_pTarget = GFL_NEW( m_pHeap ) Field::Camera::Target::TargetTypeMoveModelPlayer( m_pHeap, pModel );
    m_pCtrler = GFL_NEW( m_pHeap ) Field::Camera::Controller::ControllerTypeDebugTrainer( m_pTarget );
    m_pCamMgr->GetMainGamePlayCamera()->ChangeController( m_pCtrler );

    m_pCamMgr->SetUpForNormalMap();
  }
}

/**
 *  @brief  選択キャラクタの変更
 *  @param  add   移動値
 */
void DebugSignal::ChangeCharacter( s32 add )
{
  m_nCnt += add;
  if( m_nCnt < 0 )
  {
    m_nCnt = m_nMax - 1;
  }
  else if( m_nMax <= m_nCnt )
  {
    m_nCnt = 0;
  }

  // 表示データの更新
  RefreshData();
}

/**
 *  @brief  キャラクタリストの再作成
 */
void DebugSignal::ReloadChara( void )
{
  m_nCnt = 0;
  m_nMax = 0;
  for( u32 i = 0; i < Field::PlacementDataManager::TRAINER_ACTOR_MAX; ++ i )
  {
    if( m_pPlacementMgr->GetTrainerActor( i ) )
    {
      m_pActor[ i ] = m_pPlacementMgr->GetTrainerActor( i );
      ++ m_nMax;
    }
  }
  m_nDataIdx = 0;
  RefreshData();
}

/**
 *  @brief  シグナルの登録
 */
void DebugSignal::RegistData( void )
{
  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( m_pActor[ m_nCnt ] );
  if( !pActor ){ return; }
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  MotionSignal *pSignal = pMoveModel->GetMotionSignal();
  if( !pSignal ){ return; }

  pSignal->RegistEvent( m_nDataIdx, m_nAnimeID, m_nTypeID, m_nValue, static_cast<f32>( m_nFrame ) );
}

/**
 *  @brief  シグナルの破棄
 */
void DebugSignal::ClearData( void )
{
  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( m_pActor[ m_nCnt ] );
  if( !pActor ){ return; }
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  MotionSignal *pSignal = pMoveModel->GetMotionSignal();
  if( !pSignal ){ return; }

  pSignal->UnRegistEvent( m_nDataIdx );
  RefreshData();
}

/**
 *  @brief  表示データを設定データに合わせる
 */
void DebugSignal::RefreshData()
{
  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( m_pActor[ m_nCnt ] );
  if( !pActor ){ return; }
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  MotionSignal *pSignal = pMoveModel->GetMotionSignal();
  if( !pSignal ){ return; }

  const SignalEventData *pData = pSignal->GetData( m_nDataIdx );
  m_nAnimeID  = pData->motion_id;
  m_nTypeID   = pData->signal_type;
  m_nValue    = pData->value;
  m_nFrame    = static_cast<f32>( pData->frame );
}

/**
 *  @brief  シグナルを全リセット(リソース側に合わせる)
 */
void DebugSignal::ResetAllData( void )
{
  Field::FieldTrainerActor *pActor = reinterpret_cast<Field::FieldTrainerActor*>( m_pActor[ m_nCnt ] );
  if( !pActor ){ return; }
  Field::MoveModel::FieldMoveModel *pMoveModel = pActor->GetMoveModel();
  MotionSignal *pSignal = pMoveModel->GetMotionSignal();
  if( !pSignal ){ return; }

  void *pResource = NULL;
  pSignal->Debug_Clear();
  pSignal->Debug_ReInitialize( pResource );

  // 表示データの更新
  RefreshData();
}

/**
 *  @brief  シグナル配列番号の変更
 *  @param  add   移動値
 */
void DebugSignal::ChangeSignalDataIdx( s32 add )
{
  m_nDataIdx += add;
  if( m_nDataIdx < 0 )
  {
    m_nDataIdx = Field::SIGNAL_EVENT_DATA - 1;
  }
  else if( Field::SIGNAL_EVENT_DATA <= m_nDataIdx )
  {
    m_nDataIdx = 0;
  }

  // 表示データの更新
  RefreshData();
}

#endif // PM_DEBUG

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
