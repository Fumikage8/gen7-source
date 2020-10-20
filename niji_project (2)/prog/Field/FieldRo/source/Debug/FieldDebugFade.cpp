//======================================================================
/**
 * @file FieldDebugFade.cpp
 * @date 2015/07/24 16:17:10
 * @author miyachi_soichi
 * @brief デバッグメニュー：フェード
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <ro/include/gfl2_RoManager.h>
// System
#include "System/include/HeapDefine.h"
// niji
#include "GameSys/include/GameManager.h"
// Field
#include "Field/FieldRo/include/Debug/FieldDebugFade.h"
// App
#include "AppLib/include/Fade/DemoFade.h"

// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

static const int FADE_SYNC_FRAME = 30;
static const char *const moduleTable[] =
{
  //"SeqEditor.cro",      /*  MODULE_SeqEditor, */  //Object,Background,SeqEditorはmomijiで統合されました。
  "SequencePlayer.cro", /*  MODULE_SeqPlayer, */
  "Object.cro",         /*  MODULE_Object,    */
};
static const u32 moduleTableItems = GFL_NELEMS(moduleTable);

// Disp
static void UpdateChangeDispFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  u32 disp = pDebug->GetDisp();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && disp > gfl2::Fade::DISP_UPPER )
  {
    -- disp;
    pDebug->SetDisp( disp );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && disp < gfl2::Fade::DISP_DOUBLE )
  {
    ++ disp;
    pDebug->SetDisp( disp );
  }
}
static wchar_t* DrawChangeDispFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetDisp() );
  return workStr;
}
// Type
static void UpdateChangeOutTypeFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  u32 type = pDebug->GetOutType();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    f32 color = 255.0f;
    if( pDebug->GetColor() )
    {
      color = 0.0f;
    }
    gfl2::math::Vector4 start_col( color, color, color, 0.0f );
    gfl2::math::Vector4 end_col( color, color, color, 255.0f );
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(
      static_cast<gfl2::Fade::DISP>( pDebug->GetDisp() ),
      static_cast<gfl2::Fade::FADE_TYPE>( pDebug->GetOutType() ),
      &start_col,
      &end_col,
      FADE_SYNC_FRAME
      );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && type > 1 )
  {
    -- type;
    pDebug->SetOutType( type );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && type < gfl2::Fade::FADE_TYPE_MAX - 1 )
  {
    ++ type;
    pDebug->SetOutType( type );
  }
}
static wchar_t* DrawChangeOutTypeFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetOutType() );
  return workStr;
}
static void UpdateChangeInTypeFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  u32 type = pDebug->GetInType();
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(
      static_cast<gfl2::Fade::DISP>( pDebug->GetDisp() ),
      static_cast<gfl2::Fade::FADE_TYPE>( pDebug->GetInType() ),
      FADE_SYNC_FRAME
      );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && type > 1 )
  {
    -- type;
    pDebug->SetInType( type );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && type < gfl2::Fade::FADE_TYPE_MAX - 1 )
  {
    ++ type;
    pDebug->SetInType( type );
  }
}
static wchar_t* DrawChangeInTypeFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%d]", item->GetName(), pDebug->GetInType() );
  return workStr;
}
// color
static void UpdateChangeColorFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_LEFT ) && !pDebug->GetColor() )
  {
    pDebug->SetColor( true );
  }
  else if( pButton->IsTrigger( gfl2::ui::BUTTON_RIGHT ) && pDebug->GetColor() )
  {
    pDebug->SetColor( false );
  }
}
static wchar_t* DrawChangeColorFunc( void* userWork, gfl2::debug::DebugWinItem* item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );

  wchar_t*workStr = gfl2::debug::DebugWin_GetWorkStr();

  if( pDebug )
  {
    gfl2::heap::HeapBase* pHeap = pDebug->GetHeap();
    gfl2::str::StrBuf   *workStrBuf = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->GetWorkStrBuf();
    if( pDebug->GetColor() )
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_fade_layout_black, *workStrBuf );
    }
    else
    {
      pDebug->GetMessageData()->GetString( msg_dmenu_field_fade_layout_white, *workStrBuf );
    }
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls%ls", item->GetName(), workStrBuf->GetPtr() );
  }
  return workStr;
}
// ポーズ
static void UpdateChangePauseFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    gfl2::Fade::DISP disp = static_cast<gfl2::Fade::DISP>( pDebug->GetDisp() );
    bool bPause = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->IsPause( disp );
    GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->SetPause( disp, !bPause );
  }
}
static void UpdateInitDemoFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->InitDemoTest();
  }
}
static void UpdateEndDemoFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->EndDemoTest();
  }
}
static void UpdateRequestDemoFunc(void* userWork, gfl2::debug::DebugWinItem* item )
{
  GameSys::GameManager*    pGameManager   = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button*	       pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);

  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    pDebug->RequestDemoTest();
  }
}

void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}
// Layout
void CreateLayoutGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_layout_disp, pDebug, UpdateChangeDispFunc, DrawChangeDispFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_layout_outtype, pDebug, UpdateChangeOutTypeFunc, DrawChangeOutTypeFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_layout_intype, pDebug, UpdateChangeInTypeFunc, DrawChangeInTypeFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_layout_color, pDebug, UpdateChangeColorFunc, DrawChangeColorFunc );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_layout_pause, pDebug, UpdateChangePauseFunc, NULL );
  }
}
void CreateDemoGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_demo_init, pDebug, UpdateInitDemoFunc, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_demo_request, pDebug, UpdateRequestDemoFunc, NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_fade_demo_finish, pDebug, UpdateEndDemoFunc, NULL );
  }
}
// Fade
void CreateFadeGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugFade *pDebug = reinterpret_cast<DebugFade*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData* pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWinGroup *pLayout = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_fade_layout_top, pGroup );
    pLayout->SetCreateDestroyGroupFunc( pDebug, CreateLayoutGroupFunc, DestroyDebugGroupFunc );

    gfl2::debug::DebugWinGroup *pDemo = gfl2::debug::DebugWin_AddGroup( pHeap, pMsgData, msg_dmenu_field_fade_demo_top, pGroup );
    pDemo->SetCreateDestroyGroupFunc( pDebug, CreateDemoGroupFunc, DestroyDebugGroupFunc );
  }
}

/**
  * @class DebugFade
  * @brief フェード用デバッグ
  */
/**
 *  @brief  コンストラクタ
 */
DebugFade::DebugFade( void )
: m_pHeap( NULL )
, m_nDisp( gfl2::Fade::DISP_UPPER )
, m_nOutType( gfl2::Fade::FADE_TYPE_ALPHA )
, m_nInType( gfl2::Fade::FADE_TYPE_ALPHA )
, m_bBlack( true )
, m_ppRoModules( NULL )
, m_pDemoFade( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
DebugFade::~DebugFade( void )
{
  Terminate();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void DebugFade::Initialize( gfl2::heap::HeapBase *pHeap, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
}

/**
 *  @brief  破棄
 */
void DebugFade::Terminate( void )
{
}

/**
 *  @brief  メニューの作成
 */
void DebugFade::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pFadeGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_fade_top, root );
  pFadeGroup->SetCreateDestroyGroupFunc( this, CreateFadeGroupFunc, DestroyDebugGroupFunc );
}

void DebugFade::InitDemoTest( void )
{
  if( m_pDemoFade )
    return;

  gfl2::heap::HeapBase *pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  // Dll読込
  {
    gfl2::ro::RoManager         *pRoManager   = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::fs::AsyncFileManager  *pFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager)->GetAsyncFileManager();
    m_ppRoModules = GFL_NEW( pHeapBase ) gfl2::ro::Module*[moduleTableItems];
    for( u32 i = 0; i < moduleTableItems; ++ i )
    {
      m_ppRoModules[ i ] = pRoManager->LoadModule( pFileManager, moduleTable[ i ] );
      pRoManager->StartModule( m_ppRoModules[ i ] );
    }
  }
  // DemoFadeを作る
  m_pDemoFade = GFL_NEW( pHeapBase ) AppLib::Fade::DemoFade();
  m_pDemoFade->Initialize( pHeapBase , 0x50000, 0x18E000 );
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->AddFade( gfl2::Fade::DISP_CUSTOM_UPPER, m_pDemoFade );
}
void DebugFade::EndDemoTest( void )
{
  if( !m_pDemoFade )
    return;
  if( !m_pDemoFade->IsEndStatus() )
    return;

  // DemoFade破棄
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RemoveFade( gfl2::Fade::DISP_CUSTOM_UPPER );
  GFL_SAFE_DELETE( m_pDemoFade );
  // Dll破棄
  {
    gfl2::ro::RoManager         *pRoManager   = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    for( u32 i = 0; i < moduleTableItems; ++ i )
    {
      pRoManager->DisposeModule( m_ppRoModules[ i ] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppRoModules );
  }
  GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn( gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA );
}
void DebugFade::RequestDemoTest( void )
{
  if( !m_pDemoFade )
    return;

  m_pDemoFade->StartDemo( 10 );
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG
