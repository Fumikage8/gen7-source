// ============================================================================
/*
 * @file GTSApplicationSystem.cpp
 * @brief GTSのアプリケーションシステムです。
 * @date 2015.06.01
 */
// ============================================================================
#include "NetApp/GTS/source/ApplicationSystem/GTSApplicationSystem.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSBuilder.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"


#include "NetApp/GTS/source/GTSResourceID.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"
#include <arc_def_index/arc_def.h>
#include <arc_index/GTS.gaix>
#include <arc_index/MenuCursor.gaix>
#include <arc_index/message.gaix>
#include <arc_index/boxLyt.gaix>
#include <debug/include/gfl2_Assert.h>
#include "Print/include/PrintSystem.h"
#include "System/include/PokemonLanguage.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>


#if PM_DEBUG
// gfl2
#include <Debug/include/gfl2_DebugWinItemTypes.h>
#include <debug/include/gfl2_DebugWinSystem.h>
// niji
#include "Debug/DebugWin/include/DebugWinSystem.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"
#endif // #if PM_DEBUG


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ApplicationSystem)

static const u32 STACK_SIZE = 4096;
static const int THREAD_PRIORITY = 18;

GTSApplicationSystem::GTSApplicationSystem() :
  m_pGTSWork( NULL ),
  m_pGTSViewObjects( NULL ),
  m_pMessageUtility( NULL ),
  m_pJapanMonsNameMessageUtility( NULL ),
  m_pEnglishMonsNameMessageUtility( NULL ),
  m_pRootUpperView( NULL ),
  m_pRootLowerView( NULL ),
  m_pModelResourceNode( NULL ),
  m_pMotionResourceNode( NULL ),
  m_pBackgroundModel( NULL ),
  m_pBoxUpperDrawEnvNode( NULL ),
  m_pBoxUpperCamera( NULL ),
  m_pUpperDrawEnvNodeOfCamera( NULL ),
  m_pUpperDrawEnvNode( NULL ),
  m_pUpperCamera( NULL ),
  m_pLowerDrawEnvNode( NULL ),
  m_pLowerCamera( NULL ),
  m_pPokeModelUtility( NULL ),
  m_vBGCameraUpperPosition( 0.0f, 11.0f, 125.0f ),
  m_vBGCameraUpperTarget( 0.0f, 11.0f, 0.0f ),
  m_vBGCameraUpperUpVector( 0.0f, 1.0f, 0.0f ),
  m_fBGCameraUpperFovY( 37.0f ),
  m_fBGCameraUpperNear( 1.1f ),
  m_fBGCameraUpperFar( 10000.0f ),
  m_vBGCameraLowerPosition( -30.0f, -84.0f, 71.0f ),
  m_vBGCameraLowerTarget( 0.0f, -76.0f, 0.0f ),
  m_vBGCameraLowerUpVector( 0.0f, 1.0f, 0.0f ),
  m_fBGCameraLowerFovY( 37.0f ),
  m_fBGCameraLowerNear( 1.1f ),
  m_fBGCameraLowerFar( 10000.0f ),
  m_nPokeInFrameTop( NetApp::GTS::POKE_IN_FRAME_TOP ),
  m_nPokeInFrameBottom( NetApp::GTS::POKE_IN_FRAME_BOTTOM ),
  m_nPokeInFrameLeft( NetApp::GTS::POKE_IN_FRAME_LEFT ),
  m_nPokeInFrameRight( NetApp::GTS::POKE_IN_FRAME_RIGHT )
{

#if PM_DEBUG
  m_FourceVisibleFlag = 1;
#endif // PM_DEBUG

}


GTSApplicationSystem::~GTSApplicationSystem()
{
  m_pGTSWork->SetPokeModelUtility( NULL );

  if( m_pGTSViewObjects )
  {
    m_pGTSViewObjects->RemoveFromSuperViewAll();
  }

  GFL_SAFE_DELETE( m_pPokeModelUtility );

  GFL_SAFE_DELETE( m_pLowerCamera );
  GFL_SAFE_DELETE( m_pLowerDrawEnvNode );
  GFL_SAFE_DELETE( m_pUpperCamera );
  GFL_SAFE_DELETE( m_pUpperDrawEnvNode );
  GFL_SAFE_DELETE( m_pUpperDrawEnvNodeOfCamera );
  GFL_SAFE_DELETE( m_pBoxUpperCamera );
  GFL_SAFE_DELETE( m_pBoxUpperDrawEnvNode );
  GFL_SAFE_DELETE( m_pBackgroundModel );
  GFL_SAFE_DELETE( m_pMotionResourceNode );
  GFL_SAFE_DELETE( m_pModelResourceNode );

  m_pRootLowerView->RemoveAllSubView();
  m_pRootUpperView->RemoveAllSubView();

  GFL_SAFE_DELETE( m_pRootLowerView );
  GFL_SAFE_DELETE( m_pRootUpperView );

  GFL_SAFE_DELETE( m_pEnglishMonsNameMessageUtility );
  GFL_SAFE_DELETE( m_pJapanMonsNameMessageUtility );
  GFL_SAFE_DELETE( m_pMessageUtility );
  GFL_SAFE_DELETE( m_pGTSViewObjects );

  m_pGTSWork->SetMessageUtility( NULL );
  m_pGTSWork->SetJapanMonsNameMessageUtility( NULL );
  m_pGTSWork->SetEnglishMonsNameMessageUtility( NULL );

#if PM_DEBUG
  gfl2::str::StrBuf* workStrBuf = GFL_SINGLETON_INSTANCE( gfl2::debug::DebugWin)->GetWorkStrBuf();
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  pMsgData->GetString( msg_dmenu_gts_top, *workStrBuf );  // "GTS"
  gfl2::debug::DebugWin_RemoveGroupByName( workStrBuf->GetPtr() );
#endif // #if PM_DEBUG

}


void GTSApplicationSystem::Build( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork, NetApp::GTS::ApplicationSystem::GTSBuilder* pApplicationSystemBuilder )
{
#if PM_DEBUG
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  gfl2::debug::DebugWinGroup* pGroup = gfl2::debug::DebugWin_AddGroup( pGTSWork->GetAppHeap()->GetDeviceHeap(), pMsgData, msg_dmenu_gts_top, NULL );  // "GTS"
  
  gfl2::debug::DebugWin_AddItemValueU32(  pGroup,  pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Layout_Render_Flag",       &m_FourceVisibleFlag,              0,        1, 1    );
  
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Position", &m_vBGCameraUpperPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Target",   &m_vBGCameraUpperTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_UpVector", &m_vBGCameraUpperUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_FovY",     &m_fBGCameraUpperFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Near",     &m_fBGCameraUpperNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Upper_BG_Camera_Far",      &m_fBGCameraUpperFar,           1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Position", &m_vBGCameraLowerPosition, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Target",   &m_vBGCameraLowerTarget,   -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueVec3(  pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_UpVector", &m_vBGCameraLowerUpVector, -10000.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_FovY",     &m_fBGCameraLowerFovY,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Near",     &m_fBGCameraLowerNear,          1.0f, 10000.0f, 1.0f );
  gfl2::debug::DebugWin_AddItemValueFloat( pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Lower_BG_Camera_Far",      &m_fBGCameraLowerFar,           1.0f, 10000.0f, 1.0f );

  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Top",        &m_nPokeInFrameTop,                0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Bottom",     &m_nPokeInFrameBottom,             0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Left",       &m_nPokeInFrameLeft,               0,      500, 1    );
  gfl2::debug::DebugWin_AddItemValueInt(   pGroup, pGTSWork->GetAppHeap()->GetDeviceHeap(), L"Poke_In_Frame_Right",      &m_nPokeInFrameRight,              0,      500, 1    );

#endif // #if PM_DEBUG

  m_pGTSWork = pGTSWork;

  app::util::Heap* pAppHeap = pGTSWork->GetAppHeap();

  m_pGTSViewObjects = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::GTS::ApplicationSystem::GTSViewObjects();
  pApplicationSystemBuilder->SetDrawManager( m_pGTSViewObjects );

  m_pMessageUtility = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::Message::MessageUtility( pAppHeap->GetDeviceHeap(), MESSAGE_NUM );
  m_pJapanMonsNameMessageUtility = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::Message::MessageUtility( pAppHeap->GetDeviceHeap(), 1 );
  m_pEnglishMonsNameMessageUtility = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::Message::MessageUtility( pAppHeap->GetDeviceHeap(), 1 );

  m_pRootUpperView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );
  m_pRootLowerView = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetAppLib::UI::NetAppEmptyView( pAppHeap );

  pGTSWork->SetRootUpperView( m_pRootUpperView );
  pGTSWork->SetRootLowerView( m_pRootLowerView );

  ApplicationSystemBase::Build( pApplicationSystemBuilder );
}


void GTSApplicationSystem::LoadResourceRequest()
{
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS, ARCID_GTS, GARC_GTS_GTS_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_BOX, ARCID_BOX, GARC_boxLyt_staupp_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_GTS, print::GetMessageArcId(), GARC_message_gts_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_INITIAL, print::GetMessageArcId(), GARC_message_initial_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_BOX, print::GetMessageArcId(), GARC_message_box_DAT, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR, ARCID_MENU_CURSOR, GARC_MenuCursor_MenuCursor_applyt_COMP, true, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_BG_MODEL, ARCID_GTS, GARC_GTS_GTS_BG_ctr_GFBMDLP, false, true );
  m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_BG_MOTION, ARCID_GTS, GARC_GTS_GTS_BG_GFBMOT, false, true );

  if( System::GetLang() == POKEMON_LANG_CHINA || System::GetLang() == POKEMON_LANG_TAIWAN )
  {
    m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_MONSNAME_JAPAN, ARCID_MESSAGE_JPN, GARC_message_monsname_DAT, false, true, gfl2::fs::ToolDefine::ArcLangType::JPN );
    m_pResourceManager->AddLoadRequest( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_MONSNAME_ENGLISH, ARCID_MESSAGE_ENG, GARC_message_monsname_DAT, false, true, gfl2::fs::ToolDefine::ArcLangType::USA );
  }

  m_pResourceManager->FontLoadRequest( NetAppLib::System::ResourceManager::FONT_NUMBER );
}


void GTSApplicationSystem::CreateMessageData()
{
  m_pMessageUtility->LoadFromMemory( GARC_message_gts_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_GTS ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_initial_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_INITIAL ) );
  m_pMessageUtility->LoadFromMemory( GARC_message_box_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_BOX ) );
  m_pGTSWork->SetMessageUtility( m_pMessageUtility );

  if( System::GetLang() == POKEMON_LANG_CHINA || System::GetLang() == POKEMON_LANG_TAIWAN )
  {
    m_pJapanMonsNameMessageUtility->LoadFromMemory( GARC_message_monsname_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_MONSNAME_JAPAN ) );
    m_pEnglishMonsNameMessageUtility->LoadFromMemory( GARC_message_monsname_DAT, m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_MESSAGE_MONSNAME_ENGLISH ) );
    m_pGTSWork->SetJapanMonsNameMessageUtility( m_pJapanMonsNameMessageUtility );
    m_pGTSWork->SetEnglishMonsNameMessageUtility( m_pEnglishMonsNameMessageUtility );
  }
}


void GTSApplicationSystem::CreateView()
{
  m_pGTSWork->SetTrainerIconUtility( m_pNetAppTrainerIconUtility );

  m_pGTSViewObjects->CreateView( m_pGTSWork );

  gfl2::heap::HeapBase* pDeviceHeap = m_pGTSWork->GetDeviceHeap();

  NetAppLib::Util::NetAppPokeModelUtility::InFrameParam inFrameParam;
  inFrameParam.screen         = System::Camera::CModelInFrame::SCREEN_UPPER;
  inFrameParam.left           = m_nPokeInFrameLeft;
  inFrameParam.top            = m_nPokeInFrameTop;
  inFrameParam.right          = m_nPokeInFrameRight;
  inFrameParam.bottom         = m_nPokeInFrameBottom;
  inFrameParam.turnDirection  = System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT;
  inFrameParam.drawPosition   = System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM;
  inFrameParam.renderPlace    = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;

  m_pPokeModelUtility = GFL_NEW( pDeviceHeap ) NetAppLib::Util::NetAppPokeModelUtility( m_pGTSWork );
  m_pPokeModelUtility->Setup( 1, &inFrameParam, 1 );
  m_pGTSWork->SetPokeModelUtility( m_pPokeModelUtility );
}


bool GTSApplicationSystem::IsCreateViewComplete()
{
  return m_pGTSViewObjects->IsCreateViewComplete();
}


void GTSApplicationSystem::AddModel()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();
  gfl2::heap::HeapBase* pDeviceHeap = m_pGTSWork->GetDeviceHeap();

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
  modelDataPack.SetModelData( reinterpret_cast<c8*>( m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_BG_MODEL ) ) );
  m_pModelResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAppHeap->GetDevGLHeapAllocator(), &modelDataPack );

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData motionData;
  motionData.SetMotionData( reinterpret_cast<c8*>( m_pResourceManager->GetResourceBuffer( NetApp::GTS::GTS_RESOURCE_ID_BG_MOTION ) ) );
  m_pMotionResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( pAppHeap->GetDevGLHeapAllocator(), &motionData );

  m_pBackgroundModel = GFL_NEW( pDeviceHeap ) poke_3d::model::BaseModel();
  m_pBackgroundModel->Create( pAppHeap->GetDevGLHeapAllocator(), pDeviceHeap, m_pModelResourceNode );
  m_pBackgroundModel->ChangeAnimationByResourceNode( m_pMotionResourceNode );
  m_pBackgroundModel->SetAnimationLoop( true );

  m_pAppRenderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pBackgroundModel, 0, false );
  m_pAppRenderingManager->AddModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBackgroundModel, 0, false );
}


void GTSApplicationSystem::RemoveModel()
{
  m_pAppRenderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pBackgroundModel );
  m_pAppRenderingManager->RemoveModel( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pBackgroundModel );
}


void GTSApplicationSystem::SetupCamera()
{
  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::util::AppRenderingManager::CreateDefaultCamera( pAppHeap, &m_pBoxUpperDrawEnvNode, &m_pBoxUpperCamera );
  m_pAppRenderingManagers[1].SetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pBoxUpperCamera );

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


void GTSApplicationSystem::UnsetCamera()
{
  m_pAppRenderingManagers[1].UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
  m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY );
  m_pAppRenderingManager->UnsetCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );
}


void GTSApplicationSystem::UpdateView()
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

  bool bRootVisible = ( m_FourceVisibleFlag == 1 ) ? true : false;

  m_pRootUpperView->SetVisible( bRootVisible );
  m_pRootLowerView->SetVisible( bRootVisible );

//  if( m_pPokeModelUtility )
//  {
//    m_pPokeModelUtility->SetFramePosition( m_nPokeInFrameLeft, m_nPokeInFrameTop, m_nPokeInFrameRight, m_nPokeInFrameBottom );
//  }

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
}


void GTSApplicationSystem::DrawView( gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pRootUpperView->DrawTree( displayNo );
  m_pRootLowerView->DrawTree( displayNo );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  GTSApplicationSystem::DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if( m_pPokeModelUtility )
  {
    m_pPokeModelUtility->DebugDraw(displayNo);
  }
}
#endif


bool GTSApplicationSystem::IsExitView()
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

  if( m_pGTSViewObjects )
  {
    if( m_pGTSViewObjects->IsDrawing() )
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
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
