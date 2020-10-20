//============================================================================================
/**
 * @file    DllProc.cpp
 * @brief   DLL対応したProcの呼び出し関数を定義
 * @author  Ariizumi Nobuhiko
 * @date    15/06/05
 *
 * namespace  GameSys
 */
//============================================================================================

#include <base/include/gfl2_Singleton.h>
#include <ro/include/gfl2_RoManager.h>

#include <System/include/HeapDefine.h>
#include <GameSys/include/DllProc.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameProcManager.h>

#include <Battle/include/battle_proc.h>
#include <Test/AriizumiTest/include/BattleMeasure.h>
#include <Test/AriizumiTest/include/BgViewer.h>
#include <Test/AriizumiTest/include/EffectViewer.h>
#include <Test/AriizumiTest/include/AriTestDraw.h>
#include <Test/AriizumiTest/include/PokeViewTest.h>
#include <Test/AriizumiTest/include/AutoBattleProc.h>
#include <NetApp/GTS/include/GTSProc.h>
#include <NetApp/MysteryGift/include/GiftProc.h>
#include <NetApp/Live/include/LiveProc.h>
#include <NetApp/Evolution/include/EvolutionProc.h>
#include <NetApp/BattleSpot/include/BattleSpotProc.h>
#include <NetApp/EggHatching/include/EggHatchingProc.h>
#include <NetApp/QRReader/include/QRReaderProc.h>
#include <NetApp/PhotoView/include/PhotoViewProc.h>
#include <NetApp/MiracleTrade/include/MiracleTradeProc.h>
#include <NetApp/RegulationSelect/include/RegulationSelectProc.h>
#include <NetApp/NBR/include/NBRProc.h>
#include <NetApp/TeamSelect/include/TeamSelectProc.h>
#include <NetApp/Connection/include/ConnectionProc.h>
#include <NetApp/QuickMatch/include/QuickMatchProc.h>
#include <NetApp/JoinFestaBeacon/include/JoinFestaBeaconProc.h>
#include <NetApp/JoinFestaPlayerListSelect/include/JoinFestaPlayerListSelectProc.h>
#include <NetApp/JoinFestaPlayerListSelectMulti/include/JoinFestaPlayerListSelectMultiProc.h>
#include <NetApp/JoinFestaPlayerListShow/include/JoinFestaPlayerListShowProc.h>
#include <NetApp/JoinFestaPlayerListSelectForScript/include/JoinFestaPlayerListSelectForScriptProc.h>
#include <NetApp/JoinFestaPlayerDetail/include/JoinFestaPlayerDetailProc.h>
#include <NetApp/JoinFestaAttractionListSelect/include/JoinFestaAttractionListSelectProc.h>
#include <NetApp/JoinFestaOpenList/include/JoinFestaOpenListProc.h>
#include <NetApp/JoinFestaAttractionResult/include/JoinFestaAttractionResultProc.h>
#include <NetApp/JoinFestaProfile/include/JoinFestaProfileProc.h>
#include <NetApp/JoinFestaSelectWord/include/JoinFestaSelectWordProc.h>
#include <NetApp/JoinFestaGlobalAttraction/include/JoinFestaGlobalAttractionProc.h>
#include <NetApp/P2PTrade/include/P2PTradeProc.h>
#include <NetApp/GameSync/include/GameSyncProc.h>
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>
#include <Debug/DebugBattle/include/DebugBattleProc.h>
#include <Debug/SoundTest/include/SoundTestProc.h>

#include <Field/FieldRo/include/FieldProc.h>
#include <App/Title/include/TitleProc.h>
#include <App/Config/include/ConfigProc.h>
#include <App/CaptureTutorial/include/CaptureTutorialProc.h>
#include <App/Resort/include/ResortProc.h>
#include <Movie/MovieLib/source/MovieProc.h>

#include <Test/KawadaTest/include/KawadaTestProc.h>
#include <Test/KawadaTest/include/Camera/test/pokemon_inframe_tool.h>
#include "Test/PeteTest/include/PeteTestProc.h"
#include <Test/FukushimaTest/include/FukushimaTestProc.h>
#include "Test/NakahiroTest/include/NakahiroTestProc.h"
#include <Test/BattleFontTest/include/BattleFontTestProc.h>
#include <Test/ImageDBTest/include/ImageDBTestProc.h>
#include <Test/FStudioDecoTest/include/FStudioDecoTestProc.h>
#include <Test/HaradaTest/include/FinderStudioProc.h>

#include <Test/AnaiTest/include/AnaiTestProc.h>

#include <App/FieldDemo/include/FieldDemoProc.h>

#include <App/Zukan/include/ZukanProc.h>
#include <App/GameOver/include/GameOverProc.h>

#include <App/TitleMenu/include/TitleMenuProc.h>
#include <App/WazaOshie/include/WazaOshieProc.h>
#include <App/LangSelect/include/LangSelectProc.h>
#include <App/BoxSearch/include/BoxSearchProc.h>
#include <App/HeroSelect/include/HeroSelectProc.h>
#include <App/Status/include/StatusProc.h>
#include <App/FinderMenu/include/FinderMenuProc.h>
#include <App/FinderStudioCapture/include/FinderStudioCaptureProc.h>
#include <App/FinderStudioDecoration/include/FinderStudioDecorationProc.h>
#include <App/StrInput/include/StrInputProc.h>

#include <App/AppDemo/include/AppDemoProc.h>

#include "App/FieldBag/include/BagProc.h"
#include "App/Box/include/BoxProc.h"
#include "App/Resort/include/ResortProc.h"
#include "App/Kawaigari/include/KawaigariProc.h"
#include "App/TrainerPhoto/include/TrainerPhotoProc.h"
#include "App/FieldMenu/include/FieldMenuProc.h"
#include "App/Shop/include/ShopProc.h"
#include "App/TownMap/include/TownMapProc.h"
#include "App/TrainerPass/include/TrainerPassProc.h"
#include "App/ParurePokeList/include/ParurePokeListProc.h"
#include "App/WazaOmoidasi/include/WazaOmoidasiProc.h"
#include "App/Staffroll/include/StaffrollProc.h"
#include "App/GameClearSave/include/GameClearSaveProc.h"
#include "App/DendouDemo/include/DendouDemoProc.h"
#include "App/EndRoll/include/EndRollProc.h"
#include "App/MantainSurf/include/MantainSurfProc.h"
#include "App/LunaSolDive/include/Proc.h"
#include "App/MyAlbum/include/MyAlbumProc.h"

#include <Test/KawadaTest2/SimpleAppTest/include/SimpleAppTestProc.h>

#if PM_DEBUG
#include "Test/MenuFrameTest/include/OhnoDebugStartProc.h"
#include "Debug/DebugWordSet/include/ncl_wordset_test.h"
#include "Debug/DebugItemMake/include/ItemMakeProc.h"
#include "Debug/PokeIconView/include/PokeIconViewProc.h"
#include "Debug/FontViewer/include/ncl_font_viewer.h"
#include <Test/MutoTest/include/CTROnly/TestProc_HIOView.h>

#endif

#include "App/StaffList/include/StaffListProc.h"


GFL_NAMESPACE_BEGIN(GameSys)

//-----------------------------------------------------------------------------
/**
*   @brief バトルプロセスを呼び出す関数
*/
//-----------------------------------------------------------------------------
void CallBattleProc( btl::BATTLE_PROC_PARAM *param )
{
  //単発呼び
  //NIJI_RO_PROC_CALL< GameSys::GameProcManager, btl::BattleProc , btl::BATTLE_PROC_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro" );

  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *btlModule       = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  gfl2::ro::Module *objModule       = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");
#if PM_DEBUG
  gfl2::heap::HeapBase*       debugDllHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DLL_LOAD);
  gfl2::ro::Module *debugModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "BattleDebug.cro", debugDllHeap);
#endif //PM_DEBUG
  //gfl2::ro::Module *bagModule       = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  //gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");

  roMgr->StartModule(btlModule);
  roMgr->StartModule(objModule);
#if PM_DEBUG
  roMgr->StartModule(debugModule);
#endif //PM_DEBUG
  //roMgr->StartModule(bagModule, false);   // @note FieldBag.croを読み込まないのでfalse
  //roMgr->StartModule(wazaOshieModule, true);

  btl::BattleProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, btl::BattleProc , btl::BATTLE_PROC_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 6 );  //追加で読み込まれるので多くする！
  proc->SetChildModulePtr( 0 , btlModule );
  proc->SetChildModulePtr( 1 , objModule );
#if PM_DEBUG
  proc->SetChildModulePtr(2, debugModule);
#endif //PM_DEBUG
  //proc->SetChildModulePtr( 4 , bagModule );
  //proc->SetChildModulePtr( 5 , wazaOshieModule );

}

//-----------------------------------------------------------------------------
/**
*   @brief バトル計測呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallBattleMeasureProc(void)
{
  //複数呼び
  gfl2::heap::HeapBase*       debugDllHeap         = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *btlModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro");
  gfl2::ro::Module *debugModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "BattleDebug.cro",debugDllHeap);
  //下記2個は後読み
  //gfl2::ro::Module *bagModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  //gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");

  roMgr->StartModule(btlModule);
  roMgr->StartModule(objModule);
  roMgr->StartModule(testAriModule);
  roMgr->StartModule(debugModule);
  //roMgr->StartModule(bagModule,false);  // @note FieldBag.croを読み込まないのでfalse
  //roMgr->StartModule( wazaOshieModule );

  Test::Ariizumi::BattleMeasureProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::BattleMeasureProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 6 );
  proc->SetChildModulePtr( 0 , btlModule );
  proc->SetChildModulePtr( 1 , objModule );
  proc->SetChildModulePtr( 2 , testAriModule );
  proc->SetChildModulePtr( 3 , debugModule );
  //proc->SetChildModulePtr( 4 , bagModule );
  //proc->SetChildModulePtr( 5 , wazaOshieModule );
}
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief 戦闘背景ビュワー呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallBackgroundViewerProc(void)
{
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *bgModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");//Object,Background,SeqEditorはmomijiで統合されました。
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro");
  roMgr->StartModule(bgModule);
  roMgr->StartModule(testAriModule);

  Test::Ariizumi::BgViewerProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::BgViewerProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , bgModule );
  proc->SetChildModulePtr( 1 , testAriModule );
}
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief エフェクトビュワー呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::Ariizumi::EffectViewerProc* CallEffectViewerProc(void)
{
  //複数呼び
  gfl2::heap::HeapBase*       dllHeap         = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *btlModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro",dllHeap);  //Heap足らないので、これだけ別呼び
  gfl2::ro::Module *debugModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "BattleDebug.cro",dllHeap);
  //下記2個は後読み
  //gfl2::ro::Module *bagModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  //gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");

  roMgr->StartModule(btlModule);
  roMgr->StartModule(objModule);
  roMgr->StartModule(testAriModule);
  roMgr->StartModule(debugModule);
  //roMgr->StartModule(bagModule,false);  // @note FieldBag.croを読み込まないのでfalse
  //roMgr->StartModule( wazaOshieModule );

  Test::Ariizumi::EffectViewerProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::EffectViewerProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 6 );
  proc->SetChildModulePtr( 0 , btlModule );
  proc->SetChildModulePtr( 1 , objModule );
  proc->SetChildModulePtr( 2 , testAriModule );
  proc->SetChildModulePtr( 3 , debugModule );
  //proc->SetChildModulePtr( 5 , bagModule );
  //proc->SetChildModulePtr( 6, wazaOshieModule );

  return proc;
}
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief 有泉描画テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallAriTestDrawProc(void)
{
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *btlModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro");
  gfl2::ro::Module *bgModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");
  gfl2::ro::Module *bagModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");

  roMgr->StartModule(testAriModule);
  roMgr->StartModule(btlModule);  //RenderPathを借りている
  roMgr->StartModule(bgModule);  //RenderPathを借りている
  roMgr->StartModule(bagModule,false);  // @note FieldBag.croを読み込まないのでfalse
  roMgr->StartModule( wazaOshieModule );

  Test::Ariizumi::AriTestDraw *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::AriTestDraw >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 5 );
  proc->SetChildModulePtr( 0 , testAriModule );
  proc->SetChildModulePtr( 1 , btlModule );
  proc->SetChildModulePtr( 2 , bgModule );
  proc->SetChildModulePtr( 3 , bagModule );
  proc->SetChildModulePtr( 4 , wazaOshieModule );
}
void CallAriTestPokeDrawProc(void)
{
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro");
  roMgr->StartModule(testAriModule);

  Test::Ariizumi::PokeViewTest *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::PokeViewTest >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 3 );
  proc->SetChildModulePtr( 0 , testAriModule );
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief バトル自動テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallAutoBattleProc(void)
{
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *testAriModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "AriizumiTest.cro");

  roMgr->StartModule(testAriModule);

  Test::Ariizumi::AutoBattleProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Ariizumi::AutoBattleProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DLL_LOAD) , 1 );
  proc->SetChildModulePtr( 0 , testAriModule );
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
  *   @brief GTSプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::GTS::GTSProc* CallGTSProc( NetApp::GTS::APP_PARAM *param )
{
  //return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::GTS::GTSProc, NetApp::GTS::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "GTS.cro" );

  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *strInputModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "StrInput.cro");
  roMgr->StartModule(strInputModule);

  NetApp::GTS::GTSProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::GTS::GTSProc, NetApp::GTS::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "GTS.cro" );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 1 );
  proc->SetChildModulePtr( 0 , strInputModule );

  return proc;
}

//-----------------------------------------------------------------------------
/**
  *   @brief MysteryGift（ふしぎなおくりもの）プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::Gift::GiftProc*  CallGiftProc( NetApp::Gift::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::Gift::GiftProc, NetApp::Gift::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "MysteryGift.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief movieプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------

movie::MovieProc* CallMovieProc( void )
{
#if defined(GF_PLATFORM_CTR)
  return NIJI_RO_PROC_SETNEXT< GameSys::GameProcManager, movie::MovieProc>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "MovieLib.cro" );
#else
  return NULL;
#endif
}


//-----------------------------------------------------------------------------
/**
  *   @brief Live（ライブ大会）プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::Live::LiveProc*  CallLiveProc( NetApp::Live::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::Live::LiveProc, NetApp::Live::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Live.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief 進化デモプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::Evolution::EvolutionProc*  CallEvolutionProc( NetApp::Evolution::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::Evolution::EvolutionProc, NetApp::Evolution::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Evolution.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief BattleSpotプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::BattleSpot::BattleSpotProc*  CallBattleSpotProc( NetApp::BattleSpot::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::BattleSpot::BattleSpotProc, NetApp::BattleSpot::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "BattleSpot.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief EggHatchingプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::EggHatching::EggHatchingProc*  CallEggHatchingProc( NetApp::EggHatching::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::EggHatching::EggHatchingProc, NetApp::EggHatching::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "EggHatching.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief QRReaderプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::QRReader::QRReaderProc*  CallQRReaderProc( NetApp::QRReader::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::QRReader::QRReaderProc, NetApp::QRReader::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "QRReader.cro" );
}


//-----------------------------------------------------------------------------
/**
  *   @brief プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::PhotoView::PhotoViewProc*  CallPhotoViewProc( NetApp::PhotoView::APP_PARAM *param )
{
  GameSys::GameManager*             pGameManager              = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager*              pROManager                = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  NetApp::PhotoView::PhotoViewProc* pProc                     = NULL;

  const char* const moduleFiles[] =
  {
    "FinderStudioViewer.cro",
    "KisekaeData.cro",
    "Object.cro",
  };
  const u32 modules = GFL_NELEMS(moduleFiles);


  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::PhotoView::PhotoViewProc, NetApp::PhotoView::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "PhotoView.cro" );

  /*  modules  */
  {
    pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), modules );
    for(u32 index = 0; index < GFL_NELEMS(moduleFiles); ++index)
    {
      gfl2::ro::Module* pModule = pROManager->LoadModule( pGameManager->GetAsyncFileManager(), moduleFiles[index] );

      pROManager->StartModule(pModule);
      pProc->SetChildModulePtr( index, pModule );
    }
  }
  return pProc;
}


//-----------------------------------------------------------------------------
/**
  *   @brief ミラクル交換プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::MiracleTrade::MiracleTradeProc*  CallMiracleTradeProc( NetApp::MiracleTrade::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::MiracleTrade::MiracleTradeProc, NetApp::MiracleTrade::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "MiracleTrade.cro" );
}


//-----------------------------------------------------------------------------
/**
  *   @brief レギュレーション選択(ダウンロードも可能)アプリのプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::RegulationSelect::RegulationSelectProc*  CallRegulationSelectProc( NetApp::RegulationSelect::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::RegulationSelect::RegulationSelectProc, NetApp::RegulationSelect::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "RegulationSelect.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief 通信対戦受付アプリのプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::NBR::NBRProc* CallNBRProc( NetApp::NBR::APP_PARAM *param )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::NBR::NBRProc, NetApp::NBR::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "NBR.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief チーム選択プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::TeamSelect::TeamSelectProc* CallTeamSelectProc( NetApp::TeamSelect::APP_PARAM* pParam )
{
  GFL_ASSERT( pParam );

  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  NetApp::TeamSelect::TeamSelectProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::TeamSelect::TeamSelectProc, NetApp::TeamSelect::APP_PARAM>( gameManager->GetProcManager(), pParam, gameManager->GetAsyncFileManager() , "TeamSelect.cro" );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
  *   @brief インターネット接続プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::Connection::ConnectionProc* CallConnectionProc( NetApp::Connection::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::Connection::ConnectionProc, NetApp::Connection::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Connection.cro" );
}

//-----------------------------------------------------------------------------
/**
  *   @brief クイックマッチを呼び出す関数
  */
//-----------------------------------------------------------------------------
NetApp::QuickMatch::QuickMatchProc* CallQuickMatchProc( NetApp::QuickMatch::APP_PARAM* pParam )
{
  NetApp::QuickMatch::QuickMatchProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::QuickMatch::QuickMatchProc, NetApp::QuickMatch::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "QuickMatch.cro" );

  if( pParam->m_isBattleTreeMode == true )
  {
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    gfl2::ro::Module *fieldMenuRoModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "FieldMenu.cro");
    roMgr->StartModule(fieldMenuRoModule);

    proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 1 );
    proc->SetChildModulePtr( 0 , fieldMenuRoModule );
  }

  return proc;
}


NetApp::JoinFestaBeacon::JoinFestaBeaconProc* CallJoinFestaBeaconProc( NetApp::JoinFestaBeacon::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaBeacon::JoinFestaBeaconProc, NetApp::JoinFestaBeacon::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaBeacon.cro" );
}


NetApp::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectProc* CallJoinFestaPlayerListSelectProc( NetApp::JoinFestaPlayerListSelect::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectProc, NetApp::JoinFestaPlayerListSelect::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaPlayerListSelect.cro" );
}


NetApp::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiProc* CallJoinFestaPlayerListSelectMultiProc( NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiProc, NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaPlayerListSelectMulti.cro" );
}


NetApp::JoinFestaPlayerListShow::JoinFestaPlayerListShowProc* CallJoinFestaPlayerListShowProc( NetApp::JoinFestaPlayerListShow::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaPlayerListShow::JoinFestaPlayerListShowProc, NetApp::JoinFestaPlayerListShow::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaPlayerListShow.cro" );
}


NetApp::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptProc* CallJoinFestaPlayerListSelectForScriptProc( NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM* pParam )
{
  GameSys::GameManager* pGameManager        = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager*  pROManager          = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module*     pModule_RadarChart  = pROManager->LoadModule( pGameManager->GetAsyncFileManager(), "RaderChart.cro" );

  NetApp::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptProc* pProc = NULL;

  pROManager->StartModule(pModule_RadarChart);
  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptProc, NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaPlayerListSelectForScript.cro" );
  pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 1 );
  pProc->SetChildModulePtr( 0, pModule_RadarChart );

  return pProc;
}


NetApp::JoinFestaPlayerDetail::JoinFestaPlayerDetailProc* CallJoinFestaPlayerDetailProc( NetApp::JoinFestaPlayerDetail::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaPlayerDetail::JoinFestaPlayerDetailProc, NetApp::JoinFestaPlayerDetail::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaPlayerDetail.cro" );
}


NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectProc* CallJoinFestaAttractionListSelectProc( NetApp::JoinFestaAttractionListSelect::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectProc, NetApp::JoinFestaAttractionListSelect::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaAttractionListSelect.cro" );
}


NetApp::JoinFestaOpenList::JoinFestaOpenListProc* CallJoinFestaOpenListProc( NetApp::JoinFestaOpenList::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaOpenList::JoinFestaOpenListProc, NetApp::JoinFestaOpenList::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaOpenList.cro" );
}


NetApp::JoinFestaAttractionResult::JoinFestaAttractionResultProc* CallJoinFestaAttractionResultProc( NetApp::JoinFestaAttractionResult::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaAttractionResult::JoinFestaAttractionResultProc, NetApp::JoinFestaAttractionResult::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaAttractionResult.cro" );
}


NetApp::JoinFestaProfile::JoinFestaProfileProc* CallJoinFestaProfileProc( NetApp::JoinFestaProfile::APP_PARAM* pParam )
{
  //return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaProfile::JoinFestaProfileProc, NetApp::JoinFestaProfile::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaProfile.cro" );

  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *strInputModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "StrInput.cro");
  roMgr->StartModule(strInputModule);

  NetApp::JoinFestaProfile::JoinFestaProfileProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaProfile::JoinFestaProfileProc, NetApp::JoinFestaProfile::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaProfile.cro" );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 1 );
  proc->SetChildModulePtr( 0 , strInputModule );

  return proc;
}


NetApp::JoinFestaSelectWord::JoinFestaSelectWordProc* CallJoinFestaSelectWordProc( NetApp::JoinFestaSelectWord::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaSelectWord::JoinFestaSelectWordProc, NetApp::JoinFestaSelectWord::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaSelectWord.cro" );
}


NetApp::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionProc* CallJoinFestaGlobalAttractionProc( NetApp::JoinFestaGlobalAttraction::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionProc, NetApp::JoinFestaGlobalAttraction::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "JoinFestaGlobalAttraction.cro" );
}


NetApp::P2PTrade::P2PTradeProc* CallP2PTradeProc( NetApp::P2PTrade::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::P2PTrade::P2PTradeProc, NetApp::P2PTrade::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "P2PTrade.cro" );
}


NetApp::GameSync::GameSyncProc* CallGameSyncProc( NetApp::GameSync::APP_PARAM* pParam )
{
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, NetApp::GameSync::GameSyncProc, NetApp::GameSync::APP_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , pParam , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "GameSync.cro" );
}


//-----------------------------------------------------------------------------
/**
  *   @brief タイトルプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
app::title::TitleProc* CallTitleProc( app::title::APP_PARAM* pAppParam )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  app::title::TitleProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, app::title::TitleProc>( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "Title.cro" );
  pProc->Initialize( pAppParam );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
  *   @brief コンフィグプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
app::config::ConfigProc* CallConfigProc()
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  app::config::ConfigProc*  pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, app::config::ConfigProc>( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "Config.cro" );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
 * @brief デバッグキーボード呼び出し
 * @return 呼び出したキーボードプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
debug::KeyboardProc* CallDebugKeyboardProc( void )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  debug::KeyboardProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, debug::KeyboardProc>( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "DebugKeyboard.cro" );
  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
 * @brief デバッグポケ作成　呼び出し
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
debug::PokeMake::DebugPokeMake* CallDebugPokeMakeProc( debug::PokeMake::APP_PARAM* param )
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  gfl2::ro::RoManager *roManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module* module_pokeMake = roManager->LoadModule( fileManager, "DebugPokeMake.cro" );
  gfl2::ro::Module* module_numInput = roManager->LoadModule( fileManager, "DebugNumInput.cro" );
  gfl2::ro::Module* module_keyboard = roManager->LoadModule( fileManager, "DebugKeyboard.cro" );
  roManager->StartModule( module_pokeMake, true );
  roManager->StartModule( module_numInput, true );
  roManager->StartModule( module_keyboard, true );

  debug::PokeMake::DebugPokeMake* proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::PokeMake::DebugPokeMake, debug::PokeMake::APP_PARAM>( gameManager->GetProcManager(), param );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 3 );
  proc->SetChildModulePtr( 0, module_pokeMake );
  proc->SetChildModulePtr( 1, module_numInput );
  proc->SetChildModulePtr( 2, module_keyboard );

  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
 * @brief デバッグバトルプロセスをコールする
 * @param systemHeapForBattle  バトルに渡すシステムヒープ
 * @param deviceHeapForBattle  バトルに渡すデバイスヒープ
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallDebugBattleProc( gfl2::heap::HeapBase* systemHeapForBattle, gfl2::heap::HeapBase* deviceHeapForBattle )
{
  gfl2::fs::AsyncFileManager* fileManager     = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
  GameSys::GameProcManager*   gameProcManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetProcManager();
  gfl2::ro::RoManager*        roManager       = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::heap::HeapBase*       dllHeap         = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG_DLL_LOAD );

  gfl2::ro::Module* module_debugBattle = roManager->LoadModule( fileManager, "DebugBattle.cro",   dllHeap );
//  gfl2::ro::Module* module_pokeMake    = roManager->LoadModule( fileManager, "DebugPokeMake.cro", dllHeap );
//  gfl2::ro::Module* module_numInput    = roManager->LoadModule( fileManager, "DebugNumInput.cro", dllHeap );
//  gfl2::ro::Module* module_keyboard    = roManager->LoadModule( fileManager, "DebugKeyboard.cro", dllHeap );
  roManager->StartModule( module_debugBattle );
//  roManager->StartModule( module_pokeMake );
//  roManager->StartModule( module_numInput );
//  roManager->StartModule( module_keyboard );

  debug::battle::DebugBattleProc* proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::battle::DebugBattleProc >( gameProcManager );
  proc->CreateChildModulePtrBuffer( dllHeap , 1 );
  proc->SetChildModulePtr( 0, module_debugBattle );
//  proc->SetChildModulePtr( 1, module_pokeMake );
//  proc->SetChildModulePtr( 2, module_numInput );
//  proc->SetChildModulePtr( 3, module_keyboard );
  proc->SetupParam( systemHeapForBattle, deviceHeapForBattle );
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief サウンドテストプロセスをコールする
 * @param systemHeapForBattle  システムヒープ
 * @param deviceHeapForBattle  デバイスヒープ
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallSoundTestProc( gfl2::heap::HeapBase* systemHeapForSoundTest, gfl2::heap::HeapBase* deviceHeapForSoundTest )
{
  gfl2::fs::AsyncFileManager* fileManager     = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
  GameSys::GameProcManager*   gameProcManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetProcManager();
  gfl2::ro::RoManager*        roManager       = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::heap::HeapBase*       dllHeap         = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );

  
  gfl2::ro::Module * module_soundTest = roManager->LoadModule( fileManager, "SoundTest.cro" );
  gfl2::ro::Module * module_numInput = roManager->LoadModule( fileManager, "DebugNumInput.cro" );

  roManager->StartModule( module_soundTest );
  roManager->StartModule( module_numInput );

  debug::sound::SoundTestProc* proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::sound::SoundTestProc >( gameProcManager );
  proc->CreateChildModulePtrBuffer( dllHeap , 2 );
  proc->SetChildModulePtr( 0, module_soundTest );
  proc->SetChildModulePtr( 1, module_numInput );
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief フィールドプロセスを呼び出す関数
*/
//-----------------------------------------------------------------------------
void CallFieldProc( Field::FieldmapDesc* desc )
{
  NIJI_PROC_CALL< GameSys::GameProcManager, Field::FieldProc, Field::FieldmapDesc>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , desc );
}

//-----------------------------------------------------------------------------
/**
  *   @brief 捕獲チュートリアルプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
app::capture::CaptureTutorialProc* CallCaptureTutorialProc( b32 isCapture )
{
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module* pCaptureTutorial  = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "CaptureTutorial.cro");
  gfl2::ro::Module* pBtlModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  gfl2::ro::Module* pObjModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");
#if PM_DEBUG
  gfl2::heap::HeapBase*       debugDllHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DLL_LOAD);
  gfl2::ro::Module *debugModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "BattleDebug.cro", debugDllHeap);
#endif //PM_DEBUG
  //下記2個は後読み
  //gfl2::ro::Module* pBagModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  //  捕獲チュートリアルではわざ忘れや教えの画面は使わない
  //gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");

  roMgr->StartModule( pCaptureTutorial );
  roMgr->StartModule( pBtlModule );
  roMgr->StartModule( pObjModule );
#if PM_DEBUG
  roMgr->StartModule(debugModule);
#endif //PM_DEBUG
  //roMgr->StartModule( pBagModule, false );  // @note FieldBag.croを読み込まないのでfalse
  //roMgr->StartModule( wazaOshieModule, true );

  app::capture::CaptureTutorialProc* pProc = NULL;
  if(isCapture)
  {
    pProc = NIJI_PROC_CALL< GameSys::GameProcManager, app::capture::CaptureTutorialProc>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  }
  else
  {
    pProc = NIJI_PROC_CALL< GameSys::GameProcManager, app::capture::ZenryokuWazaTutorialProc>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  }
  GFL_ASSERT( pProc );

  pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 6 );
  pProc->SetChildModulePtr( 0 , pCaptureTutorial );
  pProc->SetChildModulePtr( 1 , pBtlModule );
  pProc->SetChildModulePtr( 2 , pObjModule );
#if PM_DEBUG
  pProc->SetChildModulePtr(3, debugModule);
#endif //PM_DEBUG
  //pProc->SetChildModulePtr( 5 , pBagModule );
//  pProc->SetChildModulePtr( 6 , wazaOshieModule );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
*   @brief 川田テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::KawadaTest::KawadaTestProc* CallKawadaTestProc(Test::KawadaTest::KawadaTestProcParam* param)
{
  //複数呼び
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module* kawadaTestModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "KawadaTest.cro");
  gfl2::ro::Module* raderChartModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "RaderChart.cro");
  roMgr->StartModule(kawadaTestModule/*, true*/);  // IsAllSymbolResolvedに引っ掛かるのでfalseにした。
  roMgr->StartModule(raderChartModule/*, true*/);

  Test::KawadaTest::KawadaTestProc* proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::KawadaTest::KawadaTestProc, Test::KawadaTest::KawadaTestProcParam>(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() ,
    param );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , kawadaTestModule );
  proc->SetChildModulePtr( 1 , raderChartModule );

  return proc;
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief ポケモンインフレームカメラ　ツール呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
System::Camera::test::PokemonInframeTool* CallPokemonInframeTool(void)
{
  System::Camera::test::PokemonInframeTool* proc =
      NIJI_RO_PROC_CALL< GameSys::GameProcManager, System::Camera::test::PokemonInframeTool >(
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(),
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
          "KawadaTest.cro" );
  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
  *   @brief Boxプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
App::Box::Proc* CallBoxProc(void)
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager* pRoMgr       = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );


  //  Box用のdllを作成
  gfl2::ro::Module* pBoxModule        = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Box.cro");
  //  レーダーチャート用のdllを作成
  gfl2::ro::Module* pRaderChartModule = pRoMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "RaderChart.cro");

  App::Box::Proc* pProc = NIJI_PROC_CALL< GameSys::GameProcManager, App::Box::Proc>( gameManager->GetProcManager() );
  GFL_ASSERT( pProc );
  
  pRoMgr->StartModule( pBoxModule, true );
  pRoMgr->StartModule( pRaderChartModule, true );

  pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  pProc->SetChildModulePtr( 0 , pBoxModule );
  pProc->SetChildModulePtr( 1 , pRaderChartModule );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
  *   @brief リゾートプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
App::Resort::ResortProc* CallResortProc(void)
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::Resort::ResortProc* pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::Resort::ResortProc>
    ( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "Resort.cro" );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
  *   @brief 証明写真プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
app::photo::TrainerPhotoProc* CallTrainerPhotoProc()
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  app::photo::TrainerPhotoProc* pProc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, app::photo::TrainerPhotoProc>( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "TrainerPhoto.cro" );
  GFL_ASSERT( pProc );

  return  pProc;
}

//-----------------------------------------------------------------------------
/**
*   @brief PeteSequenceViewerテスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallPeteTestProc(void)
{ 
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager ); 
  //gfl2::ro::Module *seqEditModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "SeqEditor.cro");//Object,Background,SeqEditorはmomijiで統合されました。
  gfl2::ro::Module *seqPlayer = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "SequencePlayer.cro");
  gfl2::ro::Module *testPeteModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "PeteTest.cro"); 
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");

  roMgr->StartModule(objModule);
  //roMgr->StartModule(seqEditModule);
  roMgr->StartModule(seqPlayer);
  roMgr->StartModule(testPeteModule);


  Test::Pete::PeteTestProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Pete::PeteTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 3 );
  proc->SetChildModulePtr( 0 , objModule );
  //proc->SetChildModulePtr( 1 , seqEditModule );
  proc->SetChildModulePtr( 1 , seqPlayer );
  proc->SetChildModulePtr( 2 , testPeteModule );
  

  
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief FieldDemo呼び出し
*/
//-----------------------------------------------------------------------------
Field::Demo::FieldDemoProc* CallFieldDemoProc( s32 seqNo, int demoType /*=0*/ )
{ 
  //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager ); 
  //gfl2::ro::Module *seqEditModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "SeqEditor.cro"); //Object,Background,SeqEditorはmomijiで統合されました。
  gfl2::ro::Module *seqPlayer = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "SequencePlayer.cro");
  gfl2::ro::Module *fieldDemoModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "FieldDemo.cro");
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");

  roMgr->StartModule(objModule);
  //roMgr->StartModule(seqEditModule);
  roMgr->StartModule(seqPlayer);
  roMgr->StartModule(fieldDemoModule);

  Field::Demo::FieldDemoProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, Field::Demo::FieldDemoProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 3 );
  proc->SetSequence(seqNo);
  proc->SetDemoType(demoType);
  proc->SetChildModulePtr( 0 , objModule );
  //proc->SetChildModulePtr( 1 , seqEditModule );
  proc->SetChildModulePtr( 1 , seqPlayer );
  proc->SetChildModulePtr( 2 , fieldDemoModule );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  デバッグ道具作成画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Debug::ItemMakeProc * CallDebugItemMakeProc(void)
{
/*
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Debug::ItemMakeProc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Debug::ItemMakeProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "DebugItemMake.cro" );
  return proc;
*/
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager * fileManager = gameManager->GetAsyncFileManager();

  gfl2::ro::RoManager * roManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module * module_itemMake = roManager->LoadModule( fileManager, "DebugItemMake.cro" );
  gfl2::ro::Module * module_numInput = roManager->LoadModule( fileManager, "DebugNumInput.cro" );
  roManager->StartModule( module_itemMake );
  roManager->StartModule( module_numInput );

  Debug::ItemMakeProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Debug::ItemMakeProc >( gameManager->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, module_itemMake );
  proc->SetChildModulePtr( 1, module_numInput );

  return proc;
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief バトルプロセスを呼び出す関数 (DLL非同期読み込み)
*   デモ系のDLLはフィールドで呼んでいるのでこちらで呼んでいません。なので、それらが無いと動きません。
*/
//-----------------------------------------------------------------------------
void StartLoadBattleDll()
{
}
bool IsFinishedLoadBattleDll()
{
  return true;
}
void SetupCallBattleProc( btl::BATTLE_PROC_PARAM *param )
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *btlModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Battle.cro");
  //gfl2::ro::Module *bgModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Background.cro");//Object,Background,SeqEditorはmomijiで統合されました。
#if PM_DEBUG
  gfl2::heap::HeapBase*       debugDllHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DLL_LOAD);
  gfl2::ro::Module *debugModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "BattleDebug.cro", debugDllHeap);
#endif //PM_DEBUG
  //下記2個は後読み
  //gfl2::ro::Module *bagModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Bag.cro");
  //gfl2::ro::Module *wazaOshieModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "WazaOshie.cro");
  roMgr->StartModule(btlModule, true);
  //roMgr->StartModule(bgModule, true);
#if PM_DEBUG
  roMgr->StartModule(debugModule, true);
#endif //PM_DEBUG
  //roMgr->StartModule(bagModule, false);   // @note FieldBag.croを読み込まないのでfalse
  //roMgr->StartModule(wazaOshieModule, true);

  btl::BattleProc *proc = NIJI_PROC_CALL< GameSys::GameProcManager, btl::BattleProc , btl::BATTLE_PROC_PARAM>( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 4 );
  proc->SetChildModulePtr( 0 , btlModule );
  //proc->SetChildModulePtr( 1 , bgModule );
#if PM_DEBUG
  proc->SetChildModulePtr(1, debugModule);
#endif //PM_DEBUG
  //proc->SetChildModulePtr( 2 , bagModule );
  //proc->SetChildModulePtr( 3 , wazaOshieModule );

}


//-----------------------------------------------------------------------------
/**
*   @brief 図鑑呼び出し
*/
//-----------------------------------------------------------------------------
app::zukan::ZukanProc* CallZukanProc(app::zukan::ZukanProcParam* param)
{
  //複数呼び
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module* zukanModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Zukan.cro");
  gfl2::ro::Module* townMapPartsModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "TownMapParts.cro");
  gfl2::ro::Module* strInputModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "StrInput.cro");
  roMgr->StartModule(zukanModule, true);
  roMgr->StartModule(townMapPartsModule, true);
  roMgr->StartModule(strInputModule, true);

  app::zukan::ZukanProc* proc = NIJI_PROC_CALL< GameSys::GameProcManager, app::zukan::ZukanProc, app::zukan::ZukanProcParam>(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() ,
    param );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 3 );
  proc->SetChildModulePtr( 0 , zukanModule );
  proc->SetChildModulePtr( 1 , townMapPartsModule );
  proc->SetChildModulePtr( 2 , strInputModule );

  return proc;
}

//-----------------------------------------------------------------------------
/**
*   @brief 
*/
//-----------------------------------------------------------------------------
app::GameOver::GameOverProc* CallGameOverProc(app::GameOver::GameOverProcParam* param)
{
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module* gameoverModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "GameOver.cro");
  roMgr->StartModule(gameoverModule, true);

  app::GameOver::GameOverProc* proc = NIJI_PROC_CALL< GameSys::GameProcManager, app::GameOver::GameOverProc, app::GameOver::GameOverProcParam>(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() ,
    param );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , gameoverModule );

  return proc;
}


//-----------------------------------------------------------------------------
/**
 * @brief   福島テスト呼び出し
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::Fukushima::Proc * CallFukushimaTestProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module* strInputModule = roMgr->LoadModule(gm->GetAsyncFileManager() , "StrInput.cro");
  gfl2::ro::Module* numInputModule = roMgr->LoadModule(gm->GetAsyncFileManager() , "NumberInput.cro");

  roMgr->StartModule( strInputModule, true );
  roMgr->StartModule( numInputModule, true );

  Test::Fukushima::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, Test::Fukushima::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "FukushimaTest.cro" );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , strInputModule );
  proc->SetChildModulePtr( 1 , numInputModule );

  return proc;
}
#endif


//-----------------------------------------------------------------------------
/**
 * @brief   技教え・技忘れ呼び出し
 */
//-----------------------------------------------------------------------------
App::WazaOshie::Proc * CallWazaOshieProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::WazaOshie::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::WazaOshie::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "WazaOshie.cro" );

  return proc;
}


//-----------------------------------------------------------------------------
/**
 * @brief  バッグ画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
app::bag::BagProc * CallBagProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "Bag.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "FieldBag.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  app::bag::BagProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, app::bag::BagProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}
app::bag::BagProc * SetNextBagProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "Bag.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "FieldBag.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  app::bag::BagProc * proc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, app::bag::BagProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief   タイトルメニュー画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::TitleMenu::Proc* CallTitleMenuProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::TitleMenu::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::TitleMenu::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "TitleMenu.cro" );

  return proc;
}


//-----------------------------------------------------------------------------
/**
 * @brief   中村用テスト画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::Nakahiro::TestProc * CallNakahiroProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * roManager    = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * test  = roManager->LoadModule( gameManager->GetAsyncFileManager(), "NakahiroTest.cro" );
  gfl2::ro::Module * box   = roManager->LoadModule( gameManager->GetAsyncFileManager(), "Box.cro" );
  gfl2::ro::Module * rader = roManager->LoadModule( gameManager->GetAsyncFileManager(), "RaderChart.cro" );

  roManager->StartModule( test, true );
  roManager->StartModule( box, true );
  roManager->StartModule( rader, true );

  Test::Nakahiro::TestProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::Nakahiro::TestProc >( gameManager->GetProcManager() );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 3 );
  proc->SetChildModulePtr( 0, test );
  proc->SetChildModulePtr( 1, box );
  proc->SetChildModulePtr( 2, rader );

  return proc;
}

void DebagCallFieldMenu(void)
{
  gfl2::ro::RoManager * roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module * pMenu = roMgr->LoadModule( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), "FieldMenu.cro" );

  roMgr->StartModule( pMenu );

  App::FieldMenu::FieldMenuProc * pProc = NIJI_PROC_CALL< GameSys::GameProcManager, App::FieldMenu::FieldMenuProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );

  pProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 1 );
  pProc->SetChildModulePtr( 0, pMenu );
}
#endif


//-----------------------------------------------------------------------------
/**
*   @brief 川田テスト2呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::KawadaTest2::SimpleAppTest::SimpleAppTestProc* CallKawadaTest2SimpleAppTestProc(Test::KawadaTest2::SimpleAppTest::SimpleAppTestProcParam* param)
{
  //単発呼び
  Test::KawadaTest2::SimpleAppTest::SimpleAppTestProc* proc =
      NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::KawadaTest2::SimpleAppTest::SimpleAppTestProc, Test::KawadaTest2::SimpleAppTest::SimpleAppTestProcParam>(
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(),
          param,
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),
          "SimpleAppTest.cro" );
  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
*   @brief anaiTest呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
void CallAnaiTestProc(void)
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *bgModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");//Object,Background,SeqEditorはmomijiで統合されました。
  roMgr->StartModule(bgModule);

  Test::AnaiTest::AnaiTestProc *proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::AnaiTest::AnaiTestProc >
        ( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() ,
          GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() ,
          "AnaiTest.cro" );	 
  
  //proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  //proc->SetChildModulePtr( 0 , bgModule );
  //proc->SetChildModulePtr( 1 , testAnaiModule );
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief   言語選択画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::LangSelect::Proc* CallLangSelectProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::LangSelect::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::LangSelect::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "LangSelect.cro" );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ボックス検索画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::BoxSearch::Proc* CallBoxSearchProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::BoxSearch::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::BoxSearch::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "BoxSearch.cro" );

  return proc;
}

//------------------------------------------------------------------
/**
  *  @func    アプリケーションから呼び出すためのシーケンスデモ再生Proc
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc* CallAppDemoProc( App::AppDemo::AppDemoProcParam* pParam )
{
  GameSys::GameManager*       pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc*  pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc, App::AppDemo::AppDemoProcParam>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
}


//------------------------------------------------------------------
/**
  *  @func    ふしぎなおくりもの受信用シーケンスデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_Gift* CallAppDemoProc_Gift( App::AppDemo::AppDemoProcParam_Gift* pParam )
{
  GameSys::GameManager*             pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_Gift*   pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_Gift, App::AppDemo::AppDemoProcParam_Gift>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
}



//------------------------------------------------------------------
/**
  *  @func    進化用シーケンスデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_Evolution* CallAppDemoProc_Evolution( App::AppDemo::AppDemoProcParam_Evolution* pParam )
{
  GameSys::GameManager*                   pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_Evolution*    pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_Evolution, App::AppDemo::AppDemoProcParam_Evolution>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
};


//------------------------------------------------------------------
/**
  *  @func    GTSデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_GTS* CallAppDemoProc_GTS( App::AppDemo::AppDemoProcParam_GTS* pParam )
{
  GameSys::GameManager*                   pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_GTS*          pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_GTS, App::AppDemo::AppDemoProcParam_GTS>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
};


//------------------------------------------------------------------
/**
  *  @func    ミラクル交換デモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_MiracleTrade* CallAppDemoProc_MiracleTrade( App::AppDemo::AppDemoProcParam_MiracleTrade* pParam )
{
  GameSys::GameManager*                      pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_MiracleTrade*    pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_MiracleTrade, App::AppDemo::AppDemoProcParam_MiracleTrade>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
};


//------------------------------------------------------------------
/**
  *  @func    おめでとうデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_Congrats* CallAppDemoProc_Congrats( App::AppDemo::AppDemoProcParam_Congrats* pParam )
{
  GameSys::GameManager*                      pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_Congrats*    pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_Congrats, App::AppDemo::AppDemoProcParam_Congrats>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
};

//------------------------------------------------------------------
/**
  *  @func    VSデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_VS* CallAppDemoProc_VS( App::AppDemo::AppDemoProcParam_VS* pParam )
{
  GameSys::GameManager*                   pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::AppDemo::AppDemoProc_VS*           pProc;

  pProc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::AppDemo::AppDemoProc_VS, App::AppDemo::AppDemoProcParam_VS>
          (
            pGameManager->GetProcManager(),
            pParam,
            pGameManager->GetAsyncFileManager(),
            "AppDemo.cro"
          );
  return pProc;
}



//-----------------------------------------------------------------------------
/**
 * @brief   容姿・性別選択画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::HeroSelect::Proc* CallHeroSelectProc( void )
{
  GameSys::GameManager * gm  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module* strInputModule = roMgr->LoadModule(gm->GetAsyncFileManager() , "StrInput.cro");

  roMgr->StartModule( strInputModule, true );

  App::HeroSelect::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::HeroSelect::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "HeroSelect.cro" );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 1 );
  proc->SetChildModulePtr( 0 , strInputModule );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::Status::Proc* CallStatusProc( void )
{
  return CallStatusProcEx(true);
}

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::Status::Proc* CallStatusProcEx(const bool bRadarChartCROLoad)
{
  GameSys::GameManager * gm  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager* roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module* statusUpperModule = roMgr->LoadModule(gm->GetAsyncFileManager() , "StatusUpper.cro");
  gfl2::ro::Module* raderChartModule  = bRadarChartCROLoad ? roMgr->LoadModule(gm->GetAsyncFileManager() , "RaderChart.cro") : NULL;

  roMgr->StartModule(statusUpperModule, true);
  if(raderChartModule)  roMgr->StartModule(raderChartModule, true);

  App::Status::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::Status::Proc>(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "Status.cro" );

  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , statusUpperModule );
  if(raderChartModule)  proc->SetChildModulePtr( 1 , raderChartModule );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief   かわいがり呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::Kawaigari::KawaigariProc* CallKawaigariProc( App::Kawaigari::KawaigariProcParam *param )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    //複数呼び
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *kawaigariModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Kawaigari.cro");
  //gfl2::ro::Module *bgModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Background.cro");
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Object.cro");


  roMgr->StartModule(kawaigariModule, true);
  //roMgr->StartModule(bgModule, true);
  roMgr->StartModule(objModule);

  App::Kawaigari::KawaigariProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, App::Kawaigari::KawaigariProc , App::Kawaigari::KawaigariProcParam >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , param );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD) , 2 );
  proc->SetChildModulePtr( 0 , kawaigariModule );
  //proc->SetChildModulePtr( 1 , bgModule );
  proc->SetChildModulePtr( 1 , objModule );
  
  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  ショップ購入画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
App::Shop::ShopProc * CallShopProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::Shop::ShopProc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::Shop::ShopProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "Shop.cro" );
  return proc;
}
#endif // PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief   タウンマップ呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::TownMap::TownMapProc * CallTownMapProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "TownMap.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "TownMapParts.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  App::TownMap::TownMapProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, App::TownMap::TownMapProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}
App::TownMap::TownMapProc * SetNextTownMapProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "TownMap.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "TownMapParts.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  App::TownMap::TownMapProc * proc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::TownMap::TownMapProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  ファインダーメニュー画面アプリ呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::FinderMenu::Proc*  CallFinderMenuProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::fs::AsyncFileManager * afm = gm->GetAsyncFileManager();
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * fieldRo = rm->LoadModule( afm, "FieldRo.cro" );
  rm->StartModule( fieldRo );

  App::FinderMenu::Proc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::FinderMenu::Proc>( gm->GetProcManager() , afm , "FinderMenu.cro" );
  
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 1 );
  proc->SetChildModulePtr( 0, fieldRo );

  return proc;
}

//-----------------------------------------------------------------------------
/**
* @brief  ファインダースタジオ写真撮影アプリ呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
App::FinderStudio::Capture::Proc*  CallFinderStudioCaptureProc( void )
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *finderModule = roMgr->LoadModule( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), "FinderStudioCapture.cro" );
  gfl2::ro::Module *objModule = roMgr->LoadModule( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), "Object.cro" );
  gfl2::ro::Module *FSViewerModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),"FinderStudioViewer.cro");
  // @fix DLL統合のためBackGroundを削除
  //gfl2::ro::Module *bgModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Background.cro");
  roMgr->StartModule( finderModule );
  //roMgr->StartModule(bgModule);
  roMgr->StartModule( objModule );
  roMgr->StartModule(FSViewerModule);

  App::FinderStudio::Capture::Proc *finderProc = NIJI_PROC_CALL< GameSys::GameProcManager, App::FinderStudio::Capture::Proc>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetProcManager() );
  finderProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ), 3 );  // 登録するモジュールの個数
  finderProc->SetChildModulePtr( 0, finderModule );
  //finderProc->SetChildModulePtr( 1 , bgModule );
  finderProc->SetChildModulePtr( 1, objModule );
  finderProc->SetChildModulePtr(2,FSViewerModule);

  return finderProc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  ファインダースタジオデコレーションアプリ呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::FinderStudio::Decoration::Proc*  CallFinderStudioDecorationProc( void )
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::ro::Module *decoModule = roMgr->LoadModule( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), "FinderStudioDecoration.cro" );
  gfl2::ro::Module *objModule = roMgr->LoadModule( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), "Object.cro" );
  gfl2::ro::Module *FSViewerModule = roMgr->LoadModule( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), "FinderStudioViewer.cro" );
  roMgr->StartModule( decoModule );
  roMgr->StartModule( objModule );
  roMgr->StartModule( FSViewerModule );

  App::FinderStudio::Decoration::Proc *decoProc = NIJI_PROC_CALL< GameSys::GameProcManager, App::FinderStudio::Decoration::Proc>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetProcManager() );
  decoProc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ), 3 );  // 登録するモジュールの個数
  decoProc->SetChildModulePtr( 0, decoModule );
  decoProc->SetChildModulePtr( 1, objModule );
  decoProc->SetChildModulePtr( 2, FSViewerModule );

  return decoProc;
}

//-----------------------------------------------------------------------------
/**
* @brief  ファインダースタジオテストアプリ呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::HaradaTest::FinderStudioTestProc*  CallFinderStudioTestProc(void)
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE(gfl2::ro::RoManager);
  gfl2::ro::Module *procModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),"HaradaTest.cro");
  gfl2::ro::Module *FSViewerModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),"FinderStudioViewer.cro");
  gfl2::ro::Module *objModule = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(),"Object.cro");
  // @fix DLL統合のためBackGroundを削除
  //gfl2::ro::Module *bgModule        = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "Background.cro");
  roMgr->StartModule(procModule);
  //roMgr->StartModule(bgModule);
  roMgr->StartModule(objModule);
  roMgr->StartModule(FSViewerModule);

  Test::HaradaTest::FinderStudioTestProc *finderProc = NIJI_PROC_CALL< GameSys::GameProcManager,Test::HaradaTest::FinderStudioTestProc>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager());
  finderProc->CreateChildModulePtrBuffer(gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD),3);  // 登録するモジュールの個数
  finderProc->SetChildModulePtr(0,procModule);
  //finderProc->SetChildModulePtr( 1 , bgModule );
  finderProc->SetChildModulePtr(1,objModule);
  finderProc->SetChildModulePtr(2,FSViewerModule);

  return finderProc;
}
#endif

//-----------------------------------------------------------------------------
/**
 * @brief  トレーナーパス画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::TrainerPass::TrainerPassProc * SetNextTrainerPassProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::TrainerPass::TrainerPassProc * proc = NIJI_RO_PROC_SETNEXT< GameSys::GameProcManager, App::TrainerPass::TrainerPassProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "TrainerPass.cro" );
  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  ポケパルレポケモン選択画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::PokeParure::ParurePokeListProc * SetNextParurePokeListProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::PokeParure::ParurePokeListProc * proc = NIJI_RO_PROC_SETNEXT< GameSys::GameProcManager, App::PokeParure::ParurePokeListProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "ParurePokeList.cro" );
  return proc;
}
#if PM_DEBUG
App::PokeParure::ParurePokeListProc * CallParurePokeListProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::PokeParure::ParurePokeListProc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::PokeParure::ParurePokeListProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "ParurePokeList.cro" );
  return proc;
}
#endif // PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  技思い出し画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::WazaOmoidasi::WazaOmoidasiProc * SetNextWazaOmoidasiProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "WazaOmoidasi.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "RaderChart.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  App::WazaOmoidasi::WazaOmoidasiProc * proc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::WazaOmoidasi::WazaOmoidasiProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}
#if PM_DEBUG
App::WazaOmoidasi::WazaOmoidasiProc * CallWazaOmoidasiProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * main  = rm->LoadModule( gm->GetAsyncFileManager(), "WazaOmoidasi.cro" );
  gfl2::ro::Module * parts = rm->LoadModule( gm->GetAsyncFileManager(), "RaderChart.cro" );

  rm->StartModule( main );
  rm->StartModule( parts );

  App::WazaOmoidasi::WazaOmoidasiProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, App::WazaOmoidasi::WazaOmoidasiProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 2 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, parts );

  return proc;
}
#endif // PM_DEBUG



//-----------------------------------------------------------------------------
/**
 * @brief   MenuFrameTest呼び出し
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::MenuFrameTest::OhnoDebugStartProc* CallMenuFrameTestProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Test::MenuFrameTest::OhnoDebugStartProc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, Test::MenuFrameTest::OhnoDebugStartProc >(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "MenuFrameTest.cro" );

  return proc;
}
#endif

#if PM_DEBUG

test::ncl::WordsetTest* CallDebugWordSetTest()
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  test::ncl::WordsetTest * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, test::ncl::WordsetTest >(
    gm->GetProcManager(), gm->GetAsyncFileManager(), "DebugWordSet.cro" );

  return proc;  
}
#endif

//-----------------------------------------------------------------------------
/**
 *   @brief 文字入力プロセスを呼び出す関数
 */
//-----------------------------------------------------------------------------
App::StrInput::Proc* CallStrInputProc(void)
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::StrInput::Proc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::StrInput::Proc>
    ( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "StrInput.cro" );

  return  proc;
}

//-----------------------------------------------------------------------------
/**
 *   @brief スタッフロールプロセスを呼び出す関数
 */
//-----------------------------------------------------------------------------
App::Staffroll::StaffrollProc* CallStaffrollProc( const bool bFirstEvent )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  App::Staffroll::StaffrollProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::Staffroll::StaffrollProc>
    ( gameManager->GetProcManager(), gameManager->GetAsyncFileManager() , "Staffroll.cro" );

  proc->SetupParam( GET_VERSION(), bFirstEvent );

  return  proc;
}

//-----------------------------------------------------------------------------
/**
 *  @brief ゲームクリアー時のセーブ画面プロセスを呼び出す関数
 */
//-----------------------------------------------------------------------------
app::game_clear_save::Proc* CallGameClearSaveProc( app::game_clear_save::APP_PARAM* param )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  app::game_clear_save::Proc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, app::game_clear_save::Proc, app::game_clear_save::APP_PARAM>
    ( gameManager->GetProcManager(), param, gameManager->GetAsyncFileManager(), "GameClearSave.cro" );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 *  @brief おめでとう画面を呼び出す関数
 *  @param[in] param 渡す情報
 */
//-----------------------------------------------------------------------------
app::dendou_demo::Proc* CallDendouDemoProc( app::dendou_demo::APP_PARAM* param )
{
  GameSys::GameManager* gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  app::dendou_demo::Proc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, app::dendou_demo::Proc, app::dendou_demo::APP_PARAM>
    ( gameManager->GetProcManager(), param, gameManager->GetAsyncFileManager(), "DendouDemo.cro" );

  return proc;
}

//------------------------------------------------------------------
/**
 * @func    イントロデモ後半
 * @param   param   パラメータ
 * @return  呼び出したプロセス
 */
//------------------------------------------------------------------
App::AppDemo::AppDemoProc_Intro2nd * CallAppDemoProc_Intro2nd( App::AppDemo::AppDemoProcParam_Intro2nd * param )
{
  GameSys::GameManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  return NIJI_RO_PROC_CALL<GameSys::GameProcManager,App::AppDemo::AppDemoProc_Intro2nd,App::AppDemo::AppDemoProcParam_Intro2nd>( man->GetProcManager(), param, man->GetAsyncFileManager(), "AppDemo.cro" );
}

//-----------------------------------------------------------------------------
/**
 *  @brief スタッフロール終了後のエンドプロセスを呼び出す
 *  @param[in] pParam  今のところ中身は何もないのでNULLでよい
 */
//-----------------------------------------------------------------------------
App::EndRoll::EndRollProc * CallEndRollProc( App::EndRoll::APP_PARAM* pParam )
{
  GameSys::GameManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  return NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::EndRoll::EndRollProc, App::EndRoll::APP_PARAM>( man->GetProcManager(), pParam, man->GetAsyncFileManager(), "EndRoll.cro" );
}

//-----------------------------------------------------------------------------
/**
* @brief  マンタインサーフ呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
App::MantainSurf::MantainSurfProc * CallMantainSurfProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  App::MantainSurf::MantainSurfProc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, App::MantainSurf::MantainSurfProc>(gm->GetProcManager(), gm->GetAsyncFileManager(), "MantainSurf.cro");
  return proc;
}

//-----------------------------------------------------------------------------
/**
* @brief  ルナソルダイブ呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
LunaSolDive::Proc * CallLunaSolDiveProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  LunaSolDive::Proc * proc = NIJI_RO_PROC_CALL<GameSys::GameProcManager, LunaSolDive::Proc>(gm->GetProcManager(), gm->GetAsyncFileManager(), "LunaSolDive.cro");
  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief   マイアルバム呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
App::MyAlbum::MyAlbumProc * SetNextMyAlbumProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::fs::AsyncFileManager * fm = gm->GetAsyncFileManager();

  gfl2::ro::Module * main   = rm->LoadModule( fm, "MyAlbum.cro" );
  gfl2::ro::Module * object = rm->LoadModule( fm, "Object.cro" );
  gfl2::ro::Module * view   = rm->LoadModule( fm, "FinderStudioViewer.cro" );

  rm->StartModule( main );
  rm->StartModule( object );
  rm->StartModule( view );

  App::MyAlbum::MyAlbumProc * proc = NIJI_PROC_SETNEXT< GameSys::GameProcManager, App::MyAlbum::MyAlbumProc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DLL_LOAD), 3 );
  proc->SetChildModulePtr( 0, main );
  proc->SetChildModulePtr( 1, object );
  proc->SetChildModulePtr( 2, view );

  return proc;
}

//-----------------------------------------------------------------------------
/**
 * @brief  ポケモンアイコン確認画面呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Debug::PokeIconViewProc * CallDebugPokeIconViewProc(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Debug::PokeIconViewProc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Debug::PokeIconViewProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "PokeIconView.cro" );
  return proc;
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  フォントビューア呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Debug::FontViewer * CallDebugFontViewer(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Debug::FontViewer * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Debug::FontViewer >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "FontViewer.cro" );
  return proc;
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  バトルフォントテスト
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::BattleFont::BattleFontTestProc* CallBattleFontTestProc( void )
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  Test::BattleFont::BattleFontTestProc* proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::BattleFont::BattleFontTestProc >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "BattleFontTest.cro" );
  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
* @brief  ImageDBテスト
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::ImageDB::Proc* CallImageDBTestProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * imgModule = rm->LoadModule( gm->GetAsyncFileManager(), "ImageDBTest.cro" );
  gfl2::ro::Module * frModule  = rm->LoadModule( gm->GetAsyncFileManager(), "FieldRo.cro" );

  rm->StartModule( imgModule );
  rm->StartModule( frModule );

  Test::ImageDB::Proc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::ImageDB::Proc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ), 2 );
  proc->SetChildModulePtr( 0, imgModule );
  proc->SetChildModulePtr( 1, frModule );

  return proc;
}
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
* @brief  ファインダースタジオデコテスト
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
Test::FSDeco::Proc* CallFSDecoTestProc( void )
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager * rm = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );

  gfl2::ro::Module * decoModule = rm->LoadModule( gm->GetAsyncFileManager(), "FStudioDecoTest.cro" );
  gfl2::ro::Module * imgModule = rm->LoadModule( gm->GetAsyncFileManager(), "ImageDBTest.cro" );
  gfl2::ro::Module * frModule = rm->LoadModule( gm->GetAsyncFileManager(), "FieldRo.cro" );

  rm->StartModule( decoModule );
  rm->StartModule( imgModule );
  rm->StartModule( frModule );

  Test::FSDeco::Proc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, Test::FSDeco::Proc >( gm->GetProcManager() );
  proc->CreateChildModulePtrBuffer( gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD ), 3 );
  proc->SetChildModulePtr( 0, decoModule );
  proc->SetChildModulePtr( 1, imgModule );
  proc->SetChildModulePtr( 2, frModule );

  return proc;
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
 * @brief  モデルビューア呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::MutoTest::TestProc_HIOView* CallDebugModelViewer(void)
{
  GameSys::GameManager * gameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  return NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::MutoTest::TestProc_HIOView >( gameManager->GetProcManager(), gameManager->GetAsyncFileManager(), "MutoTest.cro" );
}
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
* @brief  スタッフロール（momiji追加分）呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
App::StaffList::StaffListProc * CallStaffListProc(void)
{
  GameSys::GameManager * gm = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  App::StaffList::StaffListProc * proc = NIJI_RO_PROC_CALL< GameSys::GameProcManager, App::StaffList::StaffListProc >( gm->GetProcManager(), gm->GetAsyncFileManager(), "StaffList.cro" );
  return proc;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(GameSys)
