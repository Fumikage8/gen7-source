//======================================================================
/**
 * @file FieldDebugPokeFinder.cpp
 * @date 2015/10/01 19:10:41
 * @author miyachi_soichi
 * @brief PokeFinderデバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// GameSys
#include "GameSys/include/GameManager.h"
// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
// event
#include "Field/FieldStatic/include/Event/FieldEventPokeFinderForPlacement.h"
#include "Field/FieldStatic/include/Debug/EventDebugAllPokeFinder.h"
// camera
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerTypePokeFinderForPlacement.h"
// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
// Model
#include "Field/FieldRo/include/TrialModel/FieldFinderModel.h"
// self
#include "Field/FieldRo/include/Debug/FieldDebugPokeFinder.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#if PM_DEBUG
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#endif

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

#if PM_DEBUG

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Local Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Group dtor
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

static void UpdateZoomMini( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getZoomMini();
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= 0.1f;
    pDebug->m_pCamera->debug_setZoomMini( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += 0.1f;
    pDebug->m_pCamera->debug_setZoomMini( y );
  }
}
static wchar_t *DrawZoomMini( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getZoomMini();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdateZoomMaxi( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getZoomMaxi();
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= 0.1f;
    pDebug->m_pCamera->debug_setZoomMaxi( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += 0.1f;
    pDebug->m_pCamera->debug_setZoomMaxi( x );
  }
}
static wchar_t *DrawZoomMaxi( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getZoomMaxi();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}
static void UpdateLimMaxY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getLimMaxY();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pDebug->m_pCamera->debug_setLimMaxY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pDebug->m_pCamera->debug_setLimMaxY( y );
  }
}
static wchar_t *DrawLimMaxY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getLimMaxY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdateLimMaxX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getLimMaxX();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= add;
    pDebug->m_pCamera->debug_setLimMaxX( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += add;
    pDebug->m_pCamera->debug_setLimMaxX( x );
  }
}
static wchar_t *DrawLimMaxX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getLimMaxX();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}
static void UpdateLimMinY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getLimMinY();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pDebug->m_pCamera->debug_setLimMinY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pDebug->m_pCamera->debug_setLimMinY( y );
  }
}
static wchar_t *DrawLimMinY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getLimMinY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdateLimMinX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getLimMinX();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= add;
    pDebug->m_pCamera->debug_setLimMinX( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += add;
    pDebug->m_pCamera->debug_setLimMinX( x );
  }
}
static wchar_t *DrawLimMinX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getLimMinX();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}
static void UpdateRotY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getRotY();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pDebug->m_pCamera->debug_setRotY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pDebug->m_pCamera->debug_setRotY( y );
  }
}
static wchar_t *DrawRotY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getRotY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdateRotX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getRotX();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= add;
    pDebug->m_pCamera->debug_setRotX( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += add;
    pDebug->m_pCamera->debug_setRotX( x );
  }
}
static wchar_t *DrawRotX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getRotX();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}
static void UpdateCamPosZ( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 z = pDebug->m_pCamera->debug_getCamPosZ();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    z -= add;
    pDebug->m_pCamera->debug_setCamPosZ( z );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    z += add;
    pDebug->m_pCamera->debug_setCamPosZ( z );
  }
}
static wchar_t *DrawCamPosZ( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 z = pDebug->m_pCamera->debug_getCamPosZ();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), z );
  return workStr;
}
static void UpdateCamPosY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getCamPosY();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pDebug->m_pCamera->debug_setCamPosY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pDebug->m_pCamera->debug_setCamPosY( y );
  }
}
static wchar_t *DrawCamPosY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 y = pDebug->m_pCamera->debug_getCamPosY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdateCamPosX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getCamPosX();
  f32 add = 0.1f;
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= add;
    pDebug->m_pCamera->debug_setCamPosX( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += add;
    pDebug->m_pCamera->debug_setCamPosX( x );
  }
}
static wchar_t *DrawCamPosX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  f32 x = pDebug->m_pCamera->debug_getCamPosX();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// MonsFunc
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
static void UpdatePokePosX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 x = pModel->debug_getPosX();
  f32 add = 0.1f;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    x -= add;
    pModel->debug_setPosX( x );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    x += add;
    pModel->debug_setPosX( x );
  }
}
static wchar_t *DrawPokePosX( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 x = pModel->debug_getPosX();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), x );
  return workStr;
}
static void UpdatePokePosY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 y = pModel->debug_getPosY();
  f32 add = 0.1f;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pModel->debug_setPosY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pModel->debug_setPosY( y );
  }
}
static wchar_t *DrawPokePosY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 y = pModel->debug_getPosY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdatePokePosZ( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 z = pModel->debug_getPosZ();
  f32 add = 0.1f;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    z -= add;
    pModel->debug_setPosZ( z );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    z += add;
    pModel->debug_setPosZ( z );
  }
}
static wchar_t *DrawPokePosZ( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 z = pModel->debug_getPosZ();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), z );
  return workStr;
}
static void UpdatePokeRotY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 y = pModel->debug_getRotY();
  f32 add = 0.1f;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10.0f;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    y -= add;
    pModel->debug_setRotY( y );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    y += add;
    pModel->debug_setRotY( y );
  }
}
static wchar_t *DrawPokeRotY( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 y = pModel->debug_getRotY();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), y );
  return workStr;
}
static void UpdatePokeWalkerTest( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  if( pButton->IsHold( gfl2::ui::BUTTON_A ) )
  {
    pModel->debug_createPath();
  }
}
static void UpdatePokeWalkerWayPoint1( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pModel->debug_getPrevWayPoint1();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pModel->debug_getNextWayPoint1();
  }
}
static wchar_t *DrawPokeWalkerWayPoint1( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  s32 wp = pModel->deb_1;
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), wp );
  return workStr;
}
static void UpdatePokeWalkerWayPoint2( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pModel->debug_getPrevWayPoint2();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pModel->debug_getNextWayPoint2();
  }
}
static wchar_t *DrawPokeWalkerWayPoint2( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  s32 wp = pModel->deb_2;
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), wp );
  return workStr;
}
static void UpdatePokeWalkerWayPoint3( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    pModel->debug_getPrevWayPoint3();
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    pModel->debug_getNextWayPoint3();
  }
}
static wchar_t *DrawPokeWalkerWayPoint3( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  s32 wp = pModel->deb_3;
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), wp );
  return workStr;
}
static void UpdatePokeShadowScale( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );

  f32 add = 0.01f;
  f32 scale = pModel->GetShadowScale();
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    scale = (scale <= add )? 0.0f: scale - add;
    pModel->DebugFunc_SetShadowScale( scale );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    scale += add;
    pModel->DebugFunc_SetShadowScale( scale );
  }
}
static wchar_t *DrawPokeShadowScale( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  f32 scale = pModel->GetShadowScale();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%0.3f]", item->GetName(), scale );
  return workStr;
}
static void UpdatePokeShadowXOfs( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );

  f32 add = 0.01f;
  gfl2::math::Vector3 offset = pModel->GetShadowOffset();
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    offset.x -= add;
    pModel->DebugFunc_SetShadowOffset( offset.x, offset.z );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    offset.x += add;
    pModel->DebugFunc_SetShadowOffset( offset.x, offset.z );
  }
}
static wchar_t *DrawPokeShadowXOfs( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  gfl2::math::Vector3 offset = pModel->GetShadowOffset();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%0.3f]", item->GetName(), offset.x );
  return workStr;
}
static void UpdatePokeShadowZOfs( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );

  f32 add = 0.01f;
  gfl2::math::Vector3 offset = pModel->GetShadowOffset();
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100.0f;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10.0f;
  }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    offset.z -= add;
    pModel->DebugFunc_SetShadowOffset( offset.x, offset.z );
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    offset.z += add;
    pModel->DebugFunc_SetShadowOffset( offset.x, offset.z );
  }
}
static wchar_t *DrawPokeShadowZOfs( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  gfl2::math::Vector3 offset = pModel->GetShadowOffset();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%0.3f]", item->GetName(), offset.z );
  return workStr;
}
static void UpdatePokeOnewayOnemore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pModel->debug_onemore();
  }
}
static wchar_t *DrawPokeOnewayDelay( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  u32 delay = pDebug->m_oneway_delay;
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), delay );
  return workStr;
}
static void UpdatePokeStatueChange( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  u32 pos = pModel->debug_getStatuePosition();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) )
  {
    if( pos > 0 )
    {
      -- pos;
      pModel->debug_setStatuePosition( pos );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) )
  {
    ++ pos;
    pModel->debug_setStatuePosition( pos );
  }
}
static wchar_t *DrawPokeStatueChange( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  u32 pos = pModel->debug_getStatuePosition();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pos );
  return workStr;
}
static void UpdatePokeStatueMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  u32 pos = pModel->debug_getStatueMotion();

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    if( pos > 0 )
    {
      -- pos;
      pModel->debug_setStatueMotion( pos );
    }
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    if( pos <= 56 )
    {
      ++ pos;
      pModel->debug_setStatueMotion( pos );
    }
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pModel->debug_requestStatueMotion();
  }
}
static wchar_t *DrawPokeStatueMotion( void *userWork, gfl2::debug::DebugWinItem *item )
{
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  u32 pos = pModel->debug_getStatueMotion();
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pos );
  return workStr;
}
static void UpdatePokeStatueGetAABB( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  TrialModel::FinderModel *pModel = reinterpret_cast<TrialModel::FinderModel*>( userWork );
  u32 pos = pModel->debug_getStatueMotion();

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    const Field::TrialModel::FinderPokeData *data = pModel->GetPokeData();

    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_x = data->min.x;
    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_y = data->min.y;
    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_z = data->min.z;
    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_x = data->max.x;
    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_y = data->max.y;
    Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_z = data->max.z;
  }
}
// Mons1 ctor
static void CreateMons1Func( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pMons1 )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    // pos
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_00, pDebug->m_pMons1, UpdatePokePosX, DrawPokePosX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_01, pDebug->m_pMons1, UpdatePokePosY, DrawPokePosY );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_02, pDebug->m_pMons1, UpdatePokePosZ, DrawPokePosZ );
    // rot
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_03, pDebug->m_pMons1, UpdatePokeRotY, DrawPokeRotY );
    // 影
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_04, pDebug->m_pMons1, UpdatePokeShadowScale, DrawPokeShadowScale );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_04, pDebug->m_pMons1, UpdatePokeShadowXOfs,  DrawPokeShadowXOfs );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_04, pDebug->m_pMons1, UpdatePokeShadowZOfs,  DrawPokeShadowZOfs );
    if( pDebug->m_pMons1->GetCtrlType() == Field::TrialModel::FinderModel::CTRL_TYPE_Walker )
    {
      // force set path
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_05, pDebug->m_pMons1, UpdatePokeWalkerTest, NULL );

      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_06, pDebug->m_pMons1, UpdatePokeWalkerWayPoint1, DrawPokeWalkerWayPoint1 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_07, pDebug->m_pMons1, UpdatePokeWalkerWayPoint2, DrawPokeWalkerWayPoint2 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_08, pDebug->m_pMons1, UpdatePokeWalkerWayPoint3, DrawPokeWalkerWayPoint3 );
    }
    if( pDebug->m_pMons1->GetCtrlType() == Field::TrialModel::FinderModel::CTRL_TYPE_Oneway )
    {
      pDebug->m_oneway_delay = pDebug->m_pMons1->debug_getOnewayDelay();
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_onemore, pDebug->m_pMons1, UpdatePokeOnewayOnemore, NULL );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_oneway_delay, pDebug, NULL, DrawPokeOnewayDelay );
    }
    if( pDebug->m_pMons1->GetCtrlType() == Field::TrialModel::FinderModel::CTRL_TYPE_Statue )
    {
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_change, pDebug->m_pMons1, UpdatePokeStatueChange, DrawPokeStatueChange );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_01, pDebug->m_pMons1, UpdatePokeStatueMotion, DrawPokeStatueMotion );
      gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_02, pDebug->m_pMons1, UpdatePokeStatueGetAABB, NULL );
      gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_03,  &(Field::Debug::DebugTypes::s_DebugFinderParam.bEnableAABB), 0, 1, 1 );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_04, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_x, -1000.0f, 1000.0f,  0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_05, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_y, -1000.0f, 1000.0f,  0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_06, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_min_z, -1000.0f, 1000.0f,  0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_07, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_x, -1000.0f, 1000.0f,  0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_08, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_y, -1000.0f, 1000.0f,  0.1f );
      gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_statue_09, &Field::Debug::DebugTypes::s_DebugFinderParam.fAABB_max_z, -1000.0f, 1000.0f,  0.1f );
    }
  }
}
// Mons2 ctor
static void CreateMons2Func( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pMons2 )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    // pos
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_00, pDebug->m_pMons2, UpdatePokePosX, DrawPokePosX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_01, pDebug->m_pMons2, UpdatePokePosY, DrawPokePosY );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_02, pDebug->m_pMons2, UpdatePokePosZ, DrawPokePosZ );
    // rot
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_mons_03, pDebug->m_pMons2, UpdatePokeRotY, DrawPokeRotY );
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// DebugRaise
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
static void UpdateEventRaise( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && !pDebug->m_bExec )
  {
    Event::EventPokeFinderForPlacement::SetupData data;
    data.spot_id = 0xFFFFFFFF;
    Event::EventPokeFinderForPlacement *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Event::EventPokeFinderForPlacement>( pGameManager->GetGameEventManager() );
    pEvent->Initialize( data );
    pDebug->m_bExec = true;

    pGameManager->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
  }
}
static void UpdateRaisePosRot( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::MoveModel::FieldMoveModel *pPlayer = pGameManager->GetFieldmap()->GetPlayerCharacter();
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    // プレイヤー座標を取得
    {
      s32 joint_index = pBaseModel->GetJointIndex( "CamTgtHead" );
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *pNode = pBaseModel->GetModelInstanceNode();
      gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *pJointNode = pNode->GetJointInstanceNode( joint_index );
      pDebug->m_pos = pJointNode->GetWorldMatrix().GetElemPosition();
    }
    // プレイヤー角度を取得
    {
      gfl2::math::Matrix34 rot;
      gfl2::math::Vector3 vec( 0.0f, 0.0f, 1.0f );
      pBaseModel->GetRotationQuat().QuaternionToMatrix( rot );
      vec = rot.TransformCoord( vec );
      f32 tmpRot = atan2( vec.z, vec.x ) * -1.0f + PI * 0.5f;
      tmpRot = gfl2::math::NormalizeRadianAngle( tmpRot );
      pDebug->m_rot = gfl2::math::ConvRadToDeg( tmpRot );
    }
  }
}
static wchar_t *DrawRaisePos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f,%f,%f]", item->GetName(), pDebug->m_pos.x, pDebug->m_pos.y, pDebug->m_pos.z );
  return workStr;
}
static wchar_t *DrawRaiseRot( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), pDebug->m_rot );
  return workStr;
}
static void UpdateMonsNo( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  s32 add = 1;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->m_mons -= add;
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->m_mons += add;
  }
}
static wchar_t *DrawMonsNo( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_mons );
  return workStr;
}
static void UpdateForm( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  s32 add = 1;

  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_Y ) )
  {
    add *= 10;
  }

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->m_form -= add;
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->m_form += add;
  }
}
static wchar_t *DrawForm( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_form );
  return workStr;
}
static void UpdateSex( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    pDebug->m_sex = 0;
  }
  else if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    pDebug->m_sex = 1;
  }
}
static wchar_t *DrawSex( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_sex );
  return workStr;
}
static void UpdatePokePosRot( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::MoveModel::FieldMoveModel *pPlayer = pGameManager->GetFieldmap()->GetPlayerCharacter();
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    // プレイヤー座標を取得
    {
      pDebug->m_poke_pos = pBaseModel->GetPosition();
    }
    // プレイヤー角度を取得
    {
      gfl2::math::Matrix34 rot;
      gfl2::math::Vector3 vec( 0.0f, 0.0f, 1.0f );
      pBaseModel->GetRotationQuat().QuaternionToMatrix( rot );
      vec = rot.TransformCoord( vec );
      f32 tmpRot = atan2( vec.z, vec.x ) * -1.0f + PI * 0.5f;
      tmpRot = gfl2::math::NormalizeRadianAngle( tmpRot );
      pDebug->m_poke_rot = gfl2::math::ConvRadToDeg( tmpRot );
    }
  }
}
static wchar_t *DrawPokePos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f,%f,%f]", item->GetName(), pDebug->m_poke_pos.x, pDebug->m_poke_pos.y, pDebug->m_poke_pos.z );
  return workStr;
}
static wchar_t *DrawPokeRot( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), pDebug->m_poke_rot );
  return workStr;
}
static void CreateDebugMonsFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    // Raise
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_event_raise, pDebug, UpdateEventRaise, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_00, pDebug, UpdateRaisePosRot, DrawRaisePos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_01, pDebug, UpdateRaisePosRot, DrawRaiseRot );
    // MosSetup
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_02, pDebug, UpdateMonsNo, DrawMonsNo );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_03, pDebug, UpdateForm, DrawForm );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_04, pDebug, UpdateSex, DrawSex );
    // Poke
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_debugraise_05, pDebug, UpdatePokePosRot, DrawPokePos );
  }
}

// camera ctor
static void CreateCameraFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pCamera )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    // cam_pos
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_00, pDebug, UpdateCamPosX, DrawCamPosX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_01, pDebug, UpdateCamPosY, DrawCamPosY );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_02, pDebug, UpdateCamPosZ, DrawCamPosZ );
    // rot_base
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_03, pDebug, UpdateRotX, DrawRotX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_04, pDebug, UpdateRotY, DrawRotY );
    // lim
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_05, pDebug, UpdateLimMinX, DrawLimMinX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_06, pDebug, UpdateLimMinY, DrawLimMinY );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_07, pDebug, UpdateLimMaxX, DrawLimMaxX );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_08, pDebug, UpdateLimMaxY, DrawLimMaxY );
    // zoom
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_09, pDebug, UpdateZoomMini, DrawZoomMini );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_camera_10, pDebug, UpdateZoomMaxi, DrawZoomMaxi );

    pDebug->m_pCamera->debug_LockZoom = true;
  }
}
static void DestroyCameraFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pCamera )
  {
    pGroup->RemoveAllItem();
    pDebug->m_pCamera->debug_LockZoom = false;
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Score
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
static wchar_t *DrawScore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  f32 *score = reinterpret_cast<f32*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), *score );
  return workStr;
}
static void CreateDebugScore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pCamera )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    // score
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_00, &pDebug->m_score_total,       NULL, DrawScore );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_01, &pDebug->m_score_basepoint_1, NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_02, &pDebug->m_score_motion_1,    NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_03, &pDebug->m_score_dir_1,       NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_04, &pDebug->m_score_screen_1,    NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_06, &pDebug->m_score_out_1,       NULL, DrawScore );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_01, &pDebug->m_score_basepoint_2, NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_02, &pDebug->m_score_motion_2,    NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_03, &pDebug->m_score_dir_2,       NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_04, &pDebug->m_score_screen_2,    NULL, DrawScore );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_06, &pDebug->m_score_out_2,       NULL, DrawScore );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_score_07, &pDebug->m_score_spot,        NULL, DrawScore );
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 待機型編集
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
static void UpdateEditPos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  f32 *pos = reinterpret_cast<f32*>( userWork );
  f32 ratio = 1.0f;
  if( pButton->IsHold(   gfl2::ui::BUTTON_L     ) ) { ratio *= 100.0f; }
  if( pButton->IsHold(   gfl2::ui::BUTTON_R     ) ) { ratio *=  10.0f; }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT  ) ) { (*pos) -= ratio; }
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) ) { (*pos) += ratio; }
}
static wchar_t *DrawEditPos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  f32 *pos = reinterpret_cast<f32*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%f]", item->GetName(), *pos );
  return workStr;
}
static void UpdateWalkerAEditSet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pWalkerA )
  {
    pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.x = pDebug->m_pos_x;
    pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.y = pDebug->m_pos_y;
    pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.z = pDebug->m_pos_z;
  }
}
static void UpdateWalkerAEditGet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pWalkerA )
  {
    pDebug->m_pos_x = pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.x;
    pDebug->m_pos_y = pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.y;
    pDebug->m_pos_z = pDebug->m_pWalkerA->route[ pDebug->m_index ].pos.z;
  }
}
static void CreateDebugWalkerAEdit( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pWalkerA )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    pDebug->m_index = 0;
    pDebug->m_pos_x = 0.0f;
    pDebug->m_pos_y = 0.0f;
    pDebug->m_pos_z = 0.0f;
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_00, &pDebug->m_index, 0, pDebug->m_pWalkerA->pos_num-1, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_01, &pDebug->m_pos_x, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_02, &pDebug->m_pos_y, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_03, &pDebug->m_pos_z, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_04, pDebug, UpdateWalkerAEditSet, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_05, pDebug, UpdateWalkerAEditGet, NULL );
  }
}
static void UpdateWalkerBEditSet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pWalkerB )
  {
    pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.x = pDebug->m_pos_x;
    pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.y = pDebug->m_pos_y;
    pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.z = pDebug->m_pos_z;
  }
}
static void UpdateWalkerBEditGet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pWalkerB )
  {
    pDebug->m_pos_x = pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.x;
    pDebug->m_pos_y = pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.y;
    pDebug->m_pos_z = pDebug->m_pWalkerB->route[ pDebug->m_index ].pos.z;
  }
}
static void CreateDebugWalkerBEdit( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pWalkerB )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    pDebug->m_index = 0;
    pDebug->m_pos_x = 0.0f;
    pDebug->m_pos_y = 0.0f;
    pDebug->m_pos_z = 0.0f;
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_00, &pDebug->m_index, 0, pDebug->m_pWalkerB->pos_num-1, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_01, &pDebug->m_pos_x, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_02, &pDebug->m_pos_y, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_03, &pDebug->m_pos_z, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_04, pDebug, UpdateWalkerBEditSet, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_05, pDebug, UpdateWalkerBEditGet, NULL );
  }
}
static void UpdateStatueEditSet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pStatue )
  {
    pDebug->m_pStatue->data[ pDebug->m_index ].position.x = pDebug->m_pos_x;
    pDebug->m_pStatue->data[ pDebug->m_index ].position.y = pDebug->m_pos_y;
    pDebug->m_pStatue->data[ pDebug->m_index ].position.z = pDebug->m_pos_z;

    if( pDebug->m_pMons1 && pDebug->m_pMons1->GetCtrlType() == Field::TrialModel::FinderModel::CTRL_TYPE_Statue )
    {
      u32 pos = pDebug->m_pMons1->debug_getStatuePosition();
      if( pDebug->m_index == pos )
      {
        pDebug->m_pMons1->debug_setStatuePosition( pos );
      }
    }
  }
}
static void UpdateStatueEditGet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pStatue )
  {
    pDebug->m_pos_x = pDebug->m_pStatue->data[ pDebug->m_index ].position.x;
    pDebug->m_pos_y = pDebug->m_pStatue->data[ pDebug->m_index ].position.y;
    pDebug->m_pos_z = pDebug->m_pStatue->data[ pDebug->m_index ].position.z;
  }
}
static void CreateDebugStatueEdit( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pStatue )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    pDebug->m_index = 0;
    pDebug->m_pos_x = 0.0f;
    pDebug->m_pos_y = 0.0f;
    pDebug->m_pos_z = 0.0f;
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_00, &pDebug->m_index, 0, pDebug->m_pStatue->num-1, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_01, &pDebug->m_pos_x, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_02, &pDebug->m_pos_y, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_03, &pDebug->m_pos_z, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_04, pDebug, UpdateStatueEditSet, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_05, pDebug, UpdateStatueEditGet, NULL );
  }
}
static void UpdateOnewayEditSet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pOneway )
  {
    gfl2::math::Vector3 dst( pDebug->m_pos_x, pDebug->m_pos_y, pDebug->m_pos_z );
    dst = pDebug->m_spotMtx * dst;
    pDebug->m_pOneway->position[ pDebug->m_index ].x = dst.x;
    pDebug->m_pOneway->position[ pDebug->m_index ].y = dst.y;
    pDebug->m_pOneway->position[ pDebug->m_index ].z = dst.z;
  }
}
static void UpdateOnewayEditGet( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) && pDebug && pDebug->m_pOneway )
  {
    gfl2::math::Vector3 dst( pDebug->m_pOneway->position[ pDebug->m_index ].x, pDebug->m_pOneway->position[ pDebug->m_index ].y, pDebug->m_pOneway->position[ pDebug->m_index ].z );
    dst = pDebug->m_spotMtx.Inverse() * dst;
    pDebug->m_pos_x = dst.x;
    pDebug->m_pos_y = dst.y;
    pDebug->m_pos_z = dst.z;
  }
}
static void UpdateOnewayOutPos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  gfl2::math::Vector3* pos = reinterpret_cast<gfl2::math::Vector3*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Field::MoveModel::FieldMoveModel *pPlayer = pGameManager->GetFieldmap()->GetPlayerCharacter();
    poke_3d::model::BaseModel *pBaseModel = pPlayer->GetCharaDrawInstance();
    // プレイヤー座標を取得
    {
      pos->Set( pBaseModel->GetPosition() );
    }
  }
}
static wchar_t *DrawOnewayOutPos( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::math::Vector3* pos = reinterpret_cast<gfl2::math::Vector3*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%0.2f,%0.2f,%0.2f]", item->GetName(), pos->x, pos->y, pos->z );
  return workStr;
}
static void CreateDebugOnewayEdit( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup && pDebug->m_pOneway )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    pDebug->m_index = 0;
    pDebug->m_pos_x = 0.0f;
    pDebug->m_pos_y = 0.0f;
    pDebug->m_pos_z = 0.0f;
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_00, &pDebug->m_index, 0, pDebug->m_pOneway->num-1, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_01, &pDebug->m_pos_x, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_02, &pDebug->m_pos_y, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_03, &pDebug->m_pos_z, UpdateEditPos, DrawEditPos );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_04, pDebug, UpdateOnewayEditSet, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_edit_05, pDebug, UpdateOnewayEditGet, NULL );
  }
}
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// 高度な設定
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
static void UpdateAllFinder( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pDebug && !pDebug->m_bExec )
  {
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::Debug::EventDebugAllPokeFinder>( pGameManager->GetGameEventManager() );

      // ウィンドウを閉じる
      gfl2::debug::DebugWin_CloseWindow();
    }
    else if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
    {
      Field::Debug::EventDebugAllPokeFinder *pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::Debug::EventDebugAllPokeFinder>( pGameManager->GetGameEventManager() );
      pEvent->SetLotteryFlag( true );
      // ウィンドウを閉じる
      gfl2::debug::DebugWin_CloseWindow();
    }
  }
}
static void UpdateFinderVer( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && pDebug->m_cam_ver > 0 )
  {
    -- pDebug->m_cam_ver;
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && pDebug->m_cam_ver < Savedata::PokeFinderSave::MAX_POKE_FINDER_LEVEL )
  {
    ++ pDebug->m_cam_ver;
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    Savedata::PokeFinderSave *pSave = pGameManager->GetGameData()->GetPokeFinderSave();
    pSave->SetCameraVersion( pDebug->m_cam_ver );
  }
}
static wchar_t *DrawFinderVer( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->m_cam_ver );
  return workStr;
}
static void CreateDebugHighly( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();

    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_00,  &(Field::Debug::DebugTypes::s_DebugFinderParam.nTutorial), 0, 6, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_01,  &Field::Debug::DebugTypes::s_DebugFinderParam.bLoop, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_02,  &Field::Debug::DebugTypes::s_DebugFinderParam.bLotteryLock, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_03,  &Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx, -1, 20, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_03,  &Field::Debug::DebugTypes::s_DebugFinderParam.nPokeIdx2, -1, 20, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_19,  &Field::Debug::DebugTypes::s_DebugFinderParam.nSexIdx, -1, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_04,  &Field::Debug::DebugTypes::s_DebugFinderParam.nAppealRatio, -1, 100, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_05,  &Field::Debug::DebugTypes::s_DebugFinderParam.nAppealIdx, -1, 7, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_06, pDebug, UpdateAllFinder, NULL );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_07,  &Field::Debug::DebugTypes::s_DebugFinderParam.nInfinitPhoto, 0, 1, 1 );

    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_08,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerA, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_09,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawWalkerB, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_10,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawStatue, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_11,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawOneway, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_12,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawScore, 0, 2, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_13,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawRect, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_14, pDebug, UpdateFinderVer, DrawFinderVer );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_20,  &Field::Debug::DebugTypes::s_DebugFinderParam.bSnapLock, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_21,  &Field::Debug::DebugTypes::s_DebugFinderParam.nScoreLock, -1, 100000, 1 );

    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_22, pGroup );
      gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_23, &Field::Debug::DebugTypes::s_DebugFinderParam.bVoice, 0, 1, 1 );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_24, &Field::Debug::DebugTypes::s_DebugFinderParam.fLenStart, 0.0f, 10000.0f,  1.0f );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_25, &Field::Debug::DebugTypes::s_DebugFinderParam.fLenEnd,   0.0f, 10000.0f,  1.0f );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_26, &Field::Debug::DebugTypes::s_DebugFinderParam.fVolStart, 0.0f,     2.0f, 0.01f );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_27, &Field::Debug::DebugTypes::s_DebugFinderParam.fVolEnd,   0.0f,     2.0f, 0.01f );

      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_28, &Field::Debug::DebugTypes::s_DebugFinderParam.fPanStart, 0.0f,   180.0f, 1.0f );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_29, &Field::Debug::DebugTypes::s_DebugFinderParam.fPanEnd,   0.0f,   180.0f, 1.0f );
      gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_30, &Field::Debug::DebugTypes::s_DebugFinderParam.fPanPow,   0.0f,     1.0f, 0.01f );

      gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_31, &Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppRate, 0, 100, 1 );
      gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_32, &Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppCMin, 0, 100, 1 );
      gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_33, &Field::Debug::DebugTypes::s_DebugFinderParam.nNoAppCMax, 0, 100, 1 );
    }
    if( pDebug->m_pWalkerA )
    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_15, pGroup );
      group->SetCreateDestroyGroupFunc( pDebug, CreateDebugWalkerAEdit, DestroyDebugGroupFunc );
    }
    if( pDebug->m_pWalkerB )
    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_16, pGroup );
      group->SetCreateDestroyGroupFunc( pDebug, CreateDebugWalkerBEdit, DestroyDebugGroupFunc );
    }
    if( pDebug->m_pStatue )
    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_17, pGroup );
      group->SetCreateDestroyGroupFunc( pDebug, CreateDebugStatueEdit, DestroyDebugGroupFunc );
    }
    if( pDebug->m_pOneway )
    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_18, pGroup );
      group->SetCreateDestroyGroupFunc( pDebug, CreateDebugOnewayEdit, DestroyDebugGroupFunc );
    }

    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_34,  &Field::Debug::DebugTypes::s_DebugFinderParam.bShadowCheck, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_35,  &Field::Debug::DebugTypes::s_DebugFinderParam.bDrawCom, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_highly_36, &Field::Debug::DebugTypes::s_DebugFinderParam.fThreshold,   0.0f,     100.0f, 0.1f );

    {
      gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly_37, pGroup );
      gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_38, &pDebug->m_overwrite, 0, 6, 1 );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[0]), UpdateOnewayOutPos, DrawOnewayOutPos );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[1]), UpdateOnewayOutPos, DrawOnewayOutPos );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[2]), UpdateOnewayOutPos, DrawOnewayOutPos );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[3]), UpdateOnewayOutPos, DrawOnewayOutPos );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[4]), UpdateOnewayOutPos, DrawOnewayOutPos );
      gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_highly_39, &(pDebug->m_rare[5]), UpdateOnewayOutPos, DrawOnewayOutPos );
    }
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// PokeFinder
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

static void UpdateSaveScore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  Savedata::PokeFinderSave* pSave          = pGameManager->GetGameData()->GetPokeFinderSave();
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );

  u32 add = 0;

  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    add = 1;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    add *= 100;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    add *= 10;
  }

  if( pSave && add != 0 )
  {
    pSave->AddTotalValue( add );
  }
}
static wchar_t *DrawSaveScore( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*     pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Savedata::PokeFinderSave* pSave          = pGameManager->GetGameData()->GetPokeFinderSave();

  u32 score = pSave->GetTotalValue();

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), score );
  return workStr;
}

static void UpdataCommentRegist( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  s32 slot = reinterpret_cast<s32>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    s32 slot = Field::Debug::DebugTypes::s_DebugFinderParam.nCommentSlot;
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataType[slot] = Field::Debug::DebugTypes::s_DebugFinderParam.nCommentType;
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataID[slot]   = Field::Debug::DebugTypes::s_DebugFinderParam.nCommentID;
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataMsg[slot]  = Field::Debug::DebugTypes::s_DebugFinderParam.nCommentMsg;
  }
}
static void UpdataComment( void *userWork, gfl2::debug::DebugWinItem *item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  s32 slot = reinterpret_cast<s32>( userWork );

  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    s32 slot = reinterpret_cast<s32>( userWork );
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataType[slot] = 0;
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataID[slot]   = 0;
    Field::Debug::DebugTypes::s_DebugFinderParam.aComDataMsg[slot]  = -1;
  }
}
static wchar_t *DrawComment( void *userWork, gfl2::debug::DebugWinItem *item )
{
  s32 slot = reinterpret_cast<s32>( userWork );
  s32 type = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataType[slot];
  s32 id   = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataID[slot];
  s32 msg  = Field::Debug::DebugTypes::s_DebugFinderParam.aComDataMsg[slot];

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls:Type[%2d]:ID[%2d]:Msg[%9d]", item->GetName(), type, id, msg );

  return workStr;
}
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugPokeFinder *pDebug = reinterpret_cast<DebugPokeFinder*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::debug::DebugWinGroup *group = NULL;
    gfl2::str::MsgData* pMsgData  =pDebug->GetMessageData();
    
    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_debugraise, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateDebugMonsFunc, DestroyDebugGroupFunc );

    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_camera, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateCameraFunc, DestroyCameraFunc );

    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_mons1, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateMons1Func, DestroyDebugGroupFunc );
    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_mons2, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateMons2Func, DestroyDebugGroupFunc );
    
    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_score, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateDebugScore, DestroyDebugGroupFunc );

    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_pokefinder_savescore, pDebug, UpdateSaveScore, DrawSaveScore );

    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_highly, pGroup );
    group->SetCreateDestroyGroupFunc( pDebug, CreateDebugHighly, DestroyDebugGroupFunc );

    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_focus_00, pGroup );
    gfl2::debug::DebugWin_AddItemValueInt( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_01, &pDebug->m_debug_focus, 0, 1, 1 );
    gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_02, &pDebug->m_focus_offset,  0.0f,  1000.0f, 1.0f  );
    gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_03, &pDebug->m_focus_width,   0.0f, 10000.0f, 1.0f  );
    gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_04, &pDebug->m_focus_fpow,    0.0f,     1.0f, 0.05f );
    gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_05, &pDebug->m_focus_bpow,    0.0f,     1.0f, 0.05f );
    gfl2::debug::DebugWin_AddItemValueFloat( group, pHeap, pMsgData, msg_dmenu_pokefinder_focus_06, &pDebug->m_focus_powstep, 0.0f,     1.0f, 0.05f );

    group = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_pokefinder_comment_00, pGroup );
    gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_01, &Field::Debug::DebugTypes::s_DebugFinderParam.nCommentEnable, 0,  1, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_02,   &Field::Debug::DebugTypes::s_DebugFinderParam.nCommentSlot,   0, 15, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_03,   &Field::Debug::DebugTypes::s_DebugFinderParam.nCommentType,   0,  2, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_04,     &Field::Debug::DebugTypes::s_DebugFinderParam.nCommentID,     0, 99, 1 );
    gfl2::debug::DebugWin_AddItemValueInt(   group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_05,    &Field::Debug::DebugTypes::s_DebugFinderParam.nCommentMsg,    -1,  9, 1 );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_06,   NULL,                        UpdataCommentRegist, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 0), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 1), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 2), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 3), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 4), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 5), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 6), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 7), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 8), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>( 9), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>(10), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>(11), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>(12), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>(13), UpdataComment, DrawComment );
    gfl2::debug::DebugWin_AddItemUpdateFunc( group, pHeap, pMsgData, msg_dmenu_pokefinder_comment_07,   reinterpret_cast<void*>(14), UpdataComment, DrawComment );
  }
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @class  DebugPokeFinder
 *  @brief  PokeFinderデバッグ
 */
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  コンストラクタ
 */
DebugPokeFinder::DebugPokeFinder( void )
: m_pHeap( NULL )
, m_pMsgData( NULL )
{
  m_pos = gfl2::math::Vector3( 21300.0f, 100.0f, 15000.0f );
  m_rot = 180.0f;
  m_mons = MONSNO_NULL;
  m_form = 0;
  m_sex = 0;
  m_appeal01 = 0xFFFF;
  m_appeal02 = 0xFFFF;
  m_appeal03 = 0xFFFF;
  m_appeal04 = 0xFFFF;
  m_appeal05 = 0xFFFF;

  m_search_deg = 45.0f;
  m_search_frame = 10.0f;
  m_search_wait = 50.0f;
  m_search_count = 3;
  m_search_posy = 7.0f;
  m_search_posz = 100.0f;

  m_gyro_play = 0.005f;
  m_gyro_power = 0.03f;
  m_gyro_range = 0.4f;

  m_debug_focus = 0;
  m_focus_offset = 50.0f;
  m_focus_width = 280.0f;
  m_focus_fpow = 0.25f;
  m_focus_bpow = 0.25f;
  m_focus_powstep = 0.30f;

  m_pWalkerA = NULL;
  m_pWalkerB = NULL;
  m_pStatue = NULL;
  m_pOneway = NULL;

  m_bExec = false;

}

/**
 *  @brief  デストラクタ
 */
DebugPokeFinder::~DebugPokeFinder( void )
{
  Terminate();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugPokeFinder::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::debug::DebugWinGroup *root, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;

  gfl2::debug::DebugWinGroup *group = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_pokefinder_top, root );
  group->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}

/**
 *  @brief  破棄
 */
void DebugPokeFinder::Terminate( void )
{
}

#endif

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
