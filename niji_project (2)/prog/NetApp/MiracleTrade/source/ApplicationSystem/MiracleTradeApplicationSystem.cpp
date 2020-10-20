// ============================================================================
/*
 * @file MiracleTradeApplicationSystem.cpp
 * @brief ミラクル交換のアプリケーションシステムです。
 * @date 2015.06.01
 */
// ============================================================================
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeApplicationSystem.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeViewObjects.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeBuilder.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetApp/MiracleTrade/source/ApplicationSystem/MiracleTradeWork.h"

#include "NetApp/MiracleTrade/source/MiracleTradeResourceID.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"
#include <arc_def_index/arc_def.h>
#include <arc_index/message.gaix>
#include <arc_index/MiracleTrade.gaix>

#include <arc_index/MenuCursor.gaix>
#include <debug/include/gfl2_Assert.h>
#include "Print/include/PrintSystem.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>


#if PM_DEBUG
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <debug/include/gfl2_DebugWinSystem.h>
// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif // #if PM_DEBUG


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(MiracleTrade)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

#if PM_DEBUG
static void MiracleTradeClearCacheUpdateFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::ui::Button *uiButton = gfl2::debug::DebugWin_GetUiButton();
  if( uiButton->IsTrigger( gfl2::ui::BUTTON_A ) )
  {
    GFL_PRINT( "NetLib::NijiNetworkSystem::SetClearCacheFlag( true );\n" );
    NetLib::NijiNetworkSystem::SetClearCacheFlag( true );
  }
}
static wchar_t* MiracleTradeClearCacheDrawFunc( void* userWork, gfl2::debug::DebugWinItem *item )
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", item->GetName() );
  return workStr;
}
#endif // #if PM_DEBUG


MiracleTradeApplicationSystem::MiracleTradeApplicationSystem() :
  m_pMiracleTradeWork( NULL ),
  m_pMiracleTradeViewObjects( NULL ),
  m_pMessageUtility( NULL ),
  m_pRootUpperView( NULL ),
  m_pRootLowerView( NULL ),
  m_pModelResourceNode( NULL ),
  m_pMotionResourceNode( NULL ),
  m_pBackgroundModel( NULL ),
  m_pUpperDrawEnvNodeOfCamera( NULL ),
  m_pUpperDrawEnvNode( NULL ),
  m_pUpperCamera( NULL ),
  m_pLowerDrawEnvNode( NULL ),
  m_pLowerCamera( NULL ),
  m_pPokeModelUtility( NULL ),
  m_EffectBG(),
  m_vBGCameraUpperPosition( 0.0f, 11.0f, 125.0f ),
  m_vBGCameraUpperTarget( 0.0f, 11.0f, 0.0f ),
  m_vBGCameraUpperUpVector( 0.0f, 1.0f, 0.0f ),
  m_fBGCameraUpperFovY( 37.0f ),
  m_fBGCameraUpperNear( 1.1f ),
  m_fBGCameraUpperFar( 10000.0f ),
  m_vBGCameraLowerPosition( 0.0f, -19.0f, 104.0f ),
  m_vBGCameraLowerTarget( 0.0f, -83.0f, 0.0f ),
  m_vBGCameraLowerUpVector( 0.0f, 1.0f, 0.0f ),
  m_fBGCameraLowerFovY( 37.0f ),
  m_fBGCameraLowerNear( 1.1f ),
  m_fBGCameraLowerFar( 10000.0f ),
  m_vEffectCameraUpperPosition( 0.0f, 131.0f, 180.0f ),
  m_vEffectCameraUpperTarget( 0.0f, 0.0f, 0.0f ),
  m_vEffectCameraUpperUpVector( 0.0f, 1.0f, 0.0f ),
  m_fEffectCameraUpperFovY( 37.0f ),
  m_fEffectCameraUpperNear( 1.1f ),
  m_fEffectCameraUpperFar( 10000.0f ),
  m_vEffectCameraLowerPosition( 0.0f, 0.0f, 180.0f ),
  m_vEffectCameraLowerTarget( 0.0f, 0.0f, 0.0f ),
  m_vEffectCameraLowerUpVector( 0.0f, 1.0f, 0.0f ),
  m_fEffectCameraLowerFovY( 37.0f ),
  m_fEffectCameraLowerNear( 1.1f ),
  m_fEffectCameraLowerFar( 10000.0f ),
  m_nPokeInFrameTop( NetApp::MiracleTrade::POKE_IN_FRAME_TOP ),
  m_nPokeInFrameBottom( NetApp::MiracleTrade::POKE_IN_FRAME_BOTTOM ),
  m_nPokeInFrameLeft( NetApp::MiracleTrade::POKE_IN_FRAME_LEFT ),
  m_nPokeInFrameRight( NetApp::MiracleTrade::POKE_IN_FRAME_RIGHT )
{

#if PM_DEBUG
  m_FourceVisibleFlag = 1;
#endif // PM_DEBUG

}


MiracleTradeApplicationSystem::~MiracleTradeApplicationSystem()
{
  if( m_pMiracleTradeViewObjects )
  {
    m_pMiracleTradeViewObjects->RemoveFromSuperViewAll();
  }

  m_EffectBG.PopClearColor();
  m_EffectBG.Cleanup();

  GFL_SAFE_DELETE( m_pPokeModelUtility );
  m_pMiracleTradeWork->SetPokeModelUtility( NULL );

  GFL_SAFE_DELETE( m_pLowerCamera );
  GFL_SAFE_DELETE( m_pLowerDrawEnvNode );
  GFL_SAFE_DELETE( m_pUpperCamera );
  GFL_SAFE_DELETE( m_pUpperDrawEnvNode );
  GFL_SAFE_DELETE( m_pUpperDrawEnvNodeOfCamera );
  GFL_SAFE_DELETE( m_pBackgroundModel );
  GFL_SAFE_DELETE( m_pMotionResourceNode );
  GFL_SAFE_DELETE( m_pModelResourceNode );

  m_pRootLowerView->RemoveAllSubView();
  m_pRootUpperView->RemoveAllSubView();

  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );

  GFL_SAFE_DELETE( m_pMessageUtility );
  GFL_SAFE_DELETE( m_pMiracleTradeViewObjects );


#if PM_DEBUG
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE( gfl2::debug::DebugWin)->GetWorkStrBuf();
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  pMsgData->GetString( msg_dmenu_mrcl_trade_top, *workStrBuf );  // "MiracleTrade"
  gfl2::debug::DebugWin_RemoveGroupByName( workStrBuf->GetPtr() );
#endif // #if PM_DEBUG

}


void MiracleTradeApplicationSystem::Build( NetApp::MiracleTrade::ApplicationSystem::MiracleTradeWork* pMiracleTradeWork, NetApp::MiracleTrade::ApplicationSystem::MiracleTradeBuilder* pApplicationSystemBuilder )
{
#if PM_DEBUG
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  gfl2::debug::DebugWinGroup* pGroup = gfl2::debug::DebugWin_AddGroup( pMiracleTradeWork->GetDeviceHeap(), pMsgData, msg_dmenu_mrcl_trade_top, NULL );  // "MiracleTrade"

  gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pMiracleTradeWork->GetDeviceHeap(), pMsgData, msg_dmenu_mrcl_trade_clear_cache, NULL, MiracleTradeClearCacheUpdateFunc, MiracleTradeClearCacheDrawFunc );    // "ClearCache"

  gfl2::debug::DebugWin_AddItemValueU32(  pGroup,  pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Layout_Render_Flag",       &m_FourceVisibleFlag,              0,        1, 1    );

  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Position", &m_vBGCameraUpperPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Target",   &m_vBGCameraUpperTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_UpVector", &m_vBGCameraUpperUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_FovY",     &m_fBGCameraUpperFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Near",     &m_fBGCameraUpperNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Far",      &m_fBGCameraUpperFar,           1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Position", &m_vBGCameraLowerPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Target",   &m_vBGCameraLowerTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_UpVector", &m_vBGCameraLowerUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_FovY",     &m_fBGCameraLowerFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Near",     &m_fBGCameraLowerNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Far",      &m_fBGCameraLowerFar,           1.0f, 10000.0f, 1.0f );

  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_Position", &m_vEffectCameraUpperPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_Target",   &m_vEffectCameraUpperTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_UpVector", &m_vEffectCameraUpperUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_FovY",     &m_fEffectCameraUpperFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_Near",     &m_fEffectCameraUpperNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Upper_Effect_Camera_Far",      &m_fEffectCameraUpperFar,           1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_Position", &m_vEffectCameraLowerPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_Target",   &m_vEffectCameraLowerTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_UpVector", &m_vEffectCameraLowerUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_FovY",     &m_fEffectCameraLowerFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_Near",     &m_fEffectCameraLowerNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Lower_Effect_Camera_Far",      &m_fEffectCameraLowerFar,           1.0f, 10000.0f, 1.0f );

  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Top",        &m_nPokeInFrameTop,                0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Bottom",     &m_nPokeInFrameBottom,             0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Left",       &m_nPokeInFrameLeft,               0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pMiracleTradeWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Right",      &m_nPokeInFrameRight,              0,      500, 1    );

#endif // #if PM_DEBUG


  m_pMiracleTradeWork = pMiracleTradeWork;

  app::util::Heap* pAppHeap = pMiracleTradeWork->GetAppHeap();

  m_pMiracleTradeViewObjects = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::MiracleTrade::ApplicationSystem::MiracleTradeViewObjects();
  pApplicationSystemBuilder->SetDrawManager( m_pMiracleTradeViewObjects );

  m_pMessageUtility = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::Message::MessageUtility( pAppHeap->GetDeviceHeap(), MESSAGE_NUM );

  m_pRootUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );
  m_pRootLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );

  pMiracleTradeWork->SetRootUpperView( m_pRootUpperView );
  pMiracleTradeWork->SetRootLowerView( m_pRootLowerView );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void MiracleTradeApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_LAYOUT, ARCID_MIRACLE_TRADE, GARC_MiracleTrade_MiracleTrade_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MESSAGE_MIRACLETRADE, print::GetMessageArcId(), GARC_message_miracletrade_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MESSAGE_PHRASE, print::GetMessageArcId(), GARC_message_jf_phrase_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_BG_MODEL, ARCID_MIRACLE_TRADE, GARC_MiracleTrade_miracletrade_BG_ctr_GFBMDLP, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_BG_MOTION, ARCID_MIRACLE_TRADE, GARC_MiracleTrade_miracletrade_BG_GFBMOT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_EFFECT_BALL, ARCID_MIRACLE_TRADE, GARC_MiracleTrade_fxp_miracletrade_ball_m_bptcl_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_EFFECT_WINDOW, ARCID_MIRACLE_TRADE, GARC_MiracleTrade_fxp_miracletrade_window_m_bptcl_COMP, true, true );

  m_pResourceManager->FontLoadRequest( NetAppLib::System::ResourceManager::FONT_NUMBER );
}


void MiracleTradeApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_miracletrade_DAT, m_pResourceManager->GetResourceBuffer( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MESSAGE_MIRACLETRADE ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_jf_phrase_DAT, m_pResourceManager->GetResourceBuffer( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_MESSAGE_PHRASE ) );
  m_pMiracleTradeWork->SetMessageUtility( m_pMessageUtility );
}


void MiracleTradeApplicationSystem::CreateView()
{
  m_pMiracleTradeWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pMiracleTradeViewObjects->CreateDraw( m_pMiracleTradeWork );

  gfl2::heap::HeapBase* pDeviceHeap = m_pMiracleTradeWork->GetDeviceHeap();

  NetAppLib::Util::NetAppPokeModelUtility::InFrameParam inFrameParam;
  inFrameParam.screen         = System::Camera::CModelInFrame::SCREEN_UPPER;
  inFrameParam.left           = m_nPokeInFrameLeft;
  inFrameParam.top            = m_nPokeInFrameTop;
  inFrameParam.right          = m_nPokeInFrameRight;
  inFrameParam.bottom         = m_nPokeInFrameBottom;
  inFrameParam.turnDirection  = System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT;
  inFrameParam.drawPosition   = System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM;
  inFrameParam.renderPlace    = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;

  m_pPokeModelUtility = GFL_NEW( pDeviceHeap ) NetAppLib::Util::NetAppPokeModelUtility( m_pMiracleTradeWork );
  m_pPokeModelUtility->Setup( 1, &inFrameParam, 1 );
  m_pMiracleTradeWork->SetPokeModelUtility( m_pPokeModelUtility );

  u32 resourceIDArray[2] = { NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_EFFECT_BALL, NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_EFFECT_WINDOW };
  u32 upperEffectGroupIDArray[2] = { 0, 1 };
  u32 lowerEffectGroupIDArray[1] = { 0 };
  m_EffectBG.Setup( m_pMiracleTradeWork, resourceIDArray, 2, upperEffectGroupIDArray, 2, lowerEffectGroupIDArray, 1 );
  m_EffectBG.SetUpperCamera(
    m_vEffectCameraUpperPosition.x, m_vEffectCameraUpperPosition.y, m_vEffectCameraUpperPosition.z,
    m_vEffectCameraUpperTarget.x, m_vEffectCameraUpperTarget.y, m_vEffectCameraUpperTarget.z,
    m_fEffectCameraUpperFovY, m_fEffectCameraUpperNear, m_fEffectCameraUpperFar,
    m_vEffectCameraUpperUpVector.x, m_vEffectCameraUpperUpVector.y, m_vEffectCameraUpperUpVector.z );
  m_EffectBG.SetLowerCamera(
    m_vEffectCameraLowerPosition.x, m_vEffectCameraLowerPosition.y, m_vEffectCameraLowerPosition.z,
    m_vEffectCameraLowerTarget.x, m_vEffectCameraLowerTarget.y, m_vEffectCameraLowerTarget.z,
    m_fEffectCameraLowerFovY, m_fEffectCameraLowerNear, m_fEffectCameraLowerFar,
    m_vEffectCameraLowerUpVector.x, m_vEffectCameraLowerUpVector.y, m_vEffectCameraLowerUpVector.z );
  m_EffectBG.PushClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
}


void MiracleTradeApplicationSystem::AddModel()
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();
  gfl2::heap::HeapBase* pDeviceHeap = pAppHeap->GetDeviceHeap();

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData( reinterpret_cast<c8*>( m_pResourceManager->GetResourceBuffer( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_BG_MODEL ) ) );
  m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAppHeap->GetDevGLHeapAllocator(), &resourceData );

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motionData;
  motionData.SetMotionData( reinterpret_cast<c8*>( m_pResourceManager->GetResourceBuffer( NetApp::MiracleTrade::MIRACLETRADE_RESOURCE_ID_BG_MOTION ) ) );
  m_pMotionResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAppHeap->GetDevGLHeapAllocator(), &motionData );

  m_pBackgroundModel = GFL_NEW( pDeviceHeap ) poke_3d::model::BaseModel();
  m_pBackgroundModel->Create( pAppHeap->GetDevGLHeapAllocator(), pDeviceHeap, m_pModelResourceNode );
  m_pBackgroundModel->ChangeAnimationByResourceNode( m_pMotionResourceNode );
  m_pBackgroundModel->SetAnimationLoop( true );

  m_pAppRenderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pBackgroundModel, 0, false );
  m_pAppRenderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBackgroundModel, 0, false );
}


void MiracleTradeApplicationSystem::RemoveModel()
{
  m_pAppRenderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pBackgroundModel );
  m_pAppRenderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBackgroundModel );
}


void MiracleTradeApplicationSystem::SetupCamera()
{
  app::util::Heap* pAppHeap = m_pMiracleTradeWork->GetAppHeap();

  app::util::AppRenderingManager::CreateDefaultCamera( pAppHeap, &m_pUpperDrawEnvNodeOfCamera, &m_pUpperCamera );
  m_pUpperDrawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv( pAppHeap );
  app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame( m_pUpperDrawEnvNode->GetLightSet(0)->GetLightNode(0), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT );
  m_pAppRenderingManager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pUpperCamera );
  m_pAppRenderingManager->AddDrawEnv( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pUpperDrawEnvNode, 1 );

  app::util::AppRenderingManager::CreateDefaultCamera( pAppHeap, &m_pLowerDrawEnvNode, &m_pLowerCamera );
  m_pAppRenderingManager->SetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pLowerCamera );

  m_pUpperCamera->SetupCameraLookAt(
    gfl2::math::Vector3( m_vBGCameraUpperPosition.x, m_vBGCameraUpperPosition.y, m_vBGCameraUpperPosition.z ),
    gfl2::math::Vector3( m_vBGCameraUpperTarget.x, m_vBGCameraUpperTarget.y, m_vBGCameraUpperTarget.z ),
    gfl2::math::Vector3( m_vBGCameraUpperUpVector.x, m_vBGCameraUpperUpVector.y, m_vBGCameraUpperUpVector.z ) );
  m_pUpperCamera->SetFovy( gfl2::math::ConvDegToRad( m_fBGCameraUpperFovY ) );
  m_pUpperCamera->SetNear( m_fBGCameraUpperNear );
  m_pUpperCamera->SetFar( m_fBGCameraUpperFar );

  m_pLowerCamera->SetupCameraLookAt(
    gfl2::math::Vector3( m_vBGCameraLowerPosition.x, m_vBGCameraLowerPosition.y, m_vBGCameraLowerPosition.z ),
    gfl2::math::Vector3( m_vBGCameraLowerTarget.x, m_vBGCameraLowerTarget.y, m_vBGCameraLowerTarget.z ),
    gfl2::math::Vector3( m_vBGCameraLowerUpVector.x, m_vBGCameraLowerUpVector.y, m_vBGCameraLowerUpVector.z ) );
  m_pLowerCamera->SetFovy( gfl2::math::ConvDegToRad( m_fBGCameraLowerFovY ) );
  m_pLowerCamera->SetNear( m_fBGCameraLowerNear );
  m_pLowerCamera->SetFar( m_fBGCameraLowerFar );

}


void MiracleTradeApplicationSystem::UnsetCamera()
{
  m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
  m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );
}


void MiracleTradeApplicationSystem::UpdateView()
{
#if PM_DEBUG
  if( m_pUpperCamera )
  {
    m_pUpperCamera->SetupCameraLookAt(
      gfl2::math::Vector3( m_vBGCameraUpperPosition.x, m_vBGCameraUpperPosition.y, m_vBGCameraUpperPosition.z ),
      gfl2::math::Vector3( m_vBGCameraUpperTarget.x, m_vBGCameraUpperTarget.y, m_vBGCameraUpperTarget.z ),
      gfl2::math::Vector3( m_vBGCameraUpperUpVector.x, m_vBGCameraUpperUpVector.y, m_vBGCameraUpperUpVector.z ) );
    m_pUpperCamera->SetFovy( gfl2::math::ConvDegToRad( m_fBGCameraUpperFovY ) );
    m_pUpperCamera->SetNear( m_fBGCameraUpperNear );
    m_pUpperCamera->SetFar( m_fBGCameraUpperFar );
  }

  if( m_pLowerCamera )
  {
    m_pLowerCamera->SetupCameraLookAt(
      gfl2::math::Vector3( m_vBGCameraLowerPosition.x, m_vBGCameraLowerPosition.y, m_vBGCameraLowerPosition.z ),
      gfl2::math::Vector3( m_vBGCameraLowerTarget.x, m_vBGCameraLowerTarget.y, m_vBGCameraLowerTarget.z ),
      gfl2::math::Vector3( m_vBGCameraLowerUpVector.x, m_vBGCameraLowerUpVector.y, m_vBGCameraLowerUpVector.z ) );
    m_pLowerCamera->SetFovy( gfl2::math::ConvDegToRad( m_fBGCameraLowerFovY ) );
    m_pLowerCamera->SetNear( m_fBGCameraLowerNear );
    m_pLowerCamera->SetFar( m_fBGCameraLowerFar );
  }

  m_EffectBG.SetUpperCamera(
    m_vEffectCameraUpperPosition.x, m_vEffectCameraUpperPosition.y, m_vEffectCameraUpperPosition.z,
    m_vEffectCameraUpperTarget.x, m_vEffectCameraUpperTarget.y, m_vEffectCameraUpperTarget.z,
    m_fEffectCameraUpperFovY, m_fEffectCameraUpperNear, m_fEffectCameraUpperFar,
    m_vEffectCameraUpperUpVector.x, m_vEffectCameraUpperUpVector.y, m_vEffectCameraUpperUpVector.z );

  m_EffectBG.SetLowerCamera(
    m_vEffectCameraLowerPosition.x, m_vEffectCameraLowerPosition.y, m_vEffectCameraLowerPosition.z,
    m_vEffectCameraLowerTarget.x, m_vEffectCameraLowerTarget.y, m_vEffectCameraLowerTarget.z,
    m_fEffectCameraLowerFovY, m_fEffectCameraLowerNear, m_fEffectCameraLowerFar,
    m_vEffectCameraLowerUpVector.x, m_vEffectCameraLowerUpVector.y, m_vEffectCameraLowerUpVector.z );

  bool bRootVisible = ( m_FourceVisibleFlag == 1 ) ? true : false;

  m_pRootUpperView->SetVisible( bRootVisible );
  m_pRootLowerView->SetVisible( bRootVisible );

  if( m_pPokeModelUtility )
  {
    m_pPokeModelUtility->SetFramePosition( m_nPokeInFrameLeft, m_nPokeInFrameTop, m_nPokeInFrameRight, m_nPokeInFrameBottom );
  }

#endif // #if PM_DEBUG

  if( m_pPokeModelUtility )
  {
    m_pPokeModelUtility->Update();
  }

#if PM_DEBUG

  if( bRootVisible )
  {
    m_pRootUpperView->UpdateTree();
    m_pRootLowerView->UpdateTree();
  }

#else

  m_pRootUpperView->UpdateTree();
  m_pRootLowerView->UpdateTree();

#endif // PM_DEBUG

  if( m_pBackgroundModel )
  {
    m_pBackgroundModel->UpdateAnimation();
  }

  m_EffectBG.Update();
}


void MiracleTradeApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}

#if PM_DEBUG
void  MiracleTradeApplicationSystem::DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if( m_pPokeModelUtility )
  {
    m_pPokeModelUtility->DebugDraw(displayNo);
  }
}
#endif


bool MiracleTradeApplicationSystem::IsExitView()
{
  bool bExit = true;

  if( m_pPokeModelUtility )
  {
    m_pPokeModelUtility->CleanupRequest();

    if( !m_pPokeModelUtility->IsStable() )
    {
      m_pPokeModelUtility->ReleaseRequest(0);
      m_pPokeModelUtility->Update();
      bExit = false;
    }
  }

  if( m_pMiracleTradeViewObjects )
  {
    if( m_pMiracleTradeViewObjects->IsDrawing() )
    {
      bExit = false;
    }
  }

  if( !m_pBackgroundModel->CanDestroy() )
  {
    bExit = false;
  }

  return bExit;
}


GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(MiracleTrade)
GFL_NAMESPACE_END(NetApp)
