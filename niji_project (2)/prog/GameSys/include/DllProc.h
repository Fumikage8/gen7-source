//============================================================================================
/**
 * @file    DllProc.h
 * @brief   DLL対応したProcの呼び出し関数を定義
 * @author  Ariizumi Nobuhiko
 * @date    15/06/05
 *
 * namespace  GameSys
 */
//============================================================================================
#ifndef __DLL_PROC_H__
#define __DLL_PROC_H__
#pragma once

#include <Macro/include/gfl2_Macros.h>

// =============================================================================
// ここにインクルードの代わりのクラスを宣言してください
// =============================================================================
namespace btl { struct BATTLE_PROC_PARAM; }
namespace NetApp { namespace BattleFesCircleTrainerSelect { struct APP_PARAM; class BattleFesCircleTrainerSelectProc; } }
namespace NetApp { namespace GTS { struct APP_PARAM; class GTSProc; } }
namespace NetApp { namespace Gift         { struct APP_PARAM; class GiftProc; } }
namespace NetApp { namespace Live         { struct APP_PARAM; class LiveProc; } }
namespace NetApp { namespace Evolution    { struct APP_PARAM; class EvolutionProc;} }
namespace NetApp { namespace BattleSpot   { struct APP_PARAM; class BattleSpotProc;} }
namespace NetApp { namespace EggHatching  { struct APP_PARAM; class EggHatchingProc;} }
namespace NetApp { namespace QRReader     { struct APP_PARAM; class QRReaderProc;} }
namespace NetApp { namespace PhotoView    { struct APP_PARAM; class PhotoViewProc;} }
namespace NetApp { namespace MiracleTrade { struct APP_PARAM; class MiracleTradeProc; } }
namespace NetApp { namespace RegulationSelect { struct APP_PARAM; class RegulationSelectProc; } }
namespace NetApp { namespace NBR { struct APP_PARAM; class NBRProc; } }
namespace NetApp { namespace TeamSelect { struct APP_PARAM; class TeamSelectProc; } }
namespace NetApp { namespace Connection { struct APP_PARAM; class ConnectionProc; } }
namespace NetApp { namespace QuickMatch { struct APP_PARAM; class QuickMatchProc; } }
namespace NetApp { namespace JoinFestaBeacon { struct APP_PARAM; class JoinFestaBeaconProc; } }
namespace NetApp { namespace JoinFestaPlayerListSelect { struct APP_PARAM; class JoinFestaPlayerListSelectProc; } }
namespace NetApp { namespace JoinFestaPlayerListSelectMulti { struct APP_PARAM; class JoinFestaPlayerListSelectMultiProc; } }
namespace NetApp { namespace JoinFestaPlayerListShow { struct APP_PARAM; class JoinFestaPlayerListShowProc; } }
namespace NetApp { namespace JoinFestaPlayerListSelectForScript { struct APP_PARAM; class JoinFestaPlayerListSelectForScriptProc; } }
namespace NetApp { namespace JoinFestaPlayerDetail { struct APP_PARAM; class JoinFestaPlayerDetailProc; } }
namespace NetApp { namespace JoinFestaAttractionListSelect { struct APP_PARAM; class JoinFestaAttractionListSelectProc; } }
namespace NetApp { namespace JoinFestaOpenList { struct APP_PARAM; class JoinFestaOpenListProc; } }
namespace NetApp { namespace JoinFestaAttractionResult { struct APP_PARAM; class JoinFestaAttractionResultProc; } }
namespace NetApp { namespace JoinFestaProfile { struct APP_PARAM; class JoinFestaProfileProc; } }
namespace NetApp { namespace JoinFestaSelectWord { struct APP_PARAM; class JoinFestaSelectWordProc; } }
namespace NetApp { namespace JoinFestaGlobalAttraction { struct APP_PARAM; class JoinFestaGlobalAttractionProc; } }
namespace NetApp { namespace P2PTrade { struct APP_PARAM; class P2PTradeProc; } }
namespace NetApp { namespace GameSync { struct APP_PARAM; class GameSyncProc; } }
namespace debug { class KeyboardProc; }
namespace debug { namespace PokeMake { class DebugPokeMake;  struct APP_PARAM; } }
namespace Field{ struct FieldmapDesc; }
namespace Field{ namespace Demo{ class FieldDemoProc; } }
namespace app { namespace title { class TitleProc; struct APP_PARAM; } }
namespace app { namespace config { class ConfigProc; } }
namespace app { namespace capture { class CaptureTutorialProc; } }
namespace app { namespace capture { class ZenryokuWazaTutorialProc; } }
namespace app { namespace zukan { class ZukanProc;  class ZukanProcParam; } }
namespace app { namespace GameOver { class GameOverProc;  class GameOverProcParam; } }
namespace App { namespace WazaOshie { class Proc; } }
namespace app { namespace bag { class BagProc; } }
namespace App { namespace Box { class Proc; } }
namespace App { namespace Resort { class ResortProc; } }
namespace App { namespace TitleMenu { class Proc; } }
namespace App { namespace LangSelect { class Proc; } }
namespace App { namespace BoxSearch { class Proc; } }
namespace App { namespace AppDemo { class AppDemoProc;            struct AppDemoProcParam;} }
namespace App { namespace AppDemo { class AppDemoProc_Gift;       struct AppDemoProcParam_Gift;} }
namespace App { namespace AppDemo { class AppDemoProc_Evolution;       struct AppDemoProcParam_Evolution;} }
namespace App { namespace AppDemo { class AppDemoProc_GTS;  struct AppDemoProcParam_GTS;} }
namespace App { namespace AppDemo { class AppDemoProc_MiracleTrade;  struct AppDemoProcParam_MiracleTrade;} }
namespace App { namespace AppDemo { class AppDemoProc_Congrats;  struct AppDemoProcParam_Congrats;} }
namespace App { namespace AppDemo { class AppDemoProc_VS;  struct AppDemoProcParam_VS;} }
namespace App { namespace AppDemo { class AppDemoProc_Intro2nd;  struct AppDemoProcParam_Intro2nd;} }
namespace App { namespace HeroSelect { class Proc; } }
namespace App { namespace Status { class Proc; } }
namespace App { namespace PokeList { class Proc; } }
namespace App { namespace Kawaigari { class KawaigariProc; struct KawaigariProcParam;} }
namespace app { namespace photo { class TrainerPhotoProc; } }
namespace App { namespace Shop { class ShopProc; } }
namespace App { namespace TownMap { class TownMapProc; } }
namespace movie { class MovieProc; }
namespace App { namespace FinderMenu { class Proc; } }
namespace App { namespace FinderStudio { namespace Capture { class Proc; } } }
namespace App { namespace FinderStudio { namespace Decoration { class Proc; } } }
namespace App { namespace TrainerPass { class TrainerPassProc; } }
namespace App { namespace PokeParure { class ParurePokeListProc; } }
namespace App { namespace WazaOmoidasi { class WazaOmoidasiProc; } }
namespace App { namespace StrInput { class Proc; } }
namespace App { namespace Staffroll { class StaffrollProc; } }
namespace app { namespace game_clear_save { struct APP_PARAM; class Proc; } }
namespace app { namespace dendou_demo { struct APP_PARAM; class Proc; } }
namespace App { namespace EndRoll { struct APP_PARAM; class EndRollProc; } }
namespace App { namespace MantainSurf { class MantainSurfProc; } }
namespace App { namespace MyAlbum { class MyAlbumProc; } }
namespace LunaSolDive { class Proc; }

#if PM_DEBUG
namespace Test { namespace KawadaTest { class KawadaTestProc;  class KawadaTestProcParam; } }
namespace System { namespace Camera { namespace test { class PokemonInframeTool; } } }
namespace Test { namespace Island { class IslandTestProc; } }
namespace Test { namespace Ariizumi { class EffectViewerProc; } }
namespace Test { namespace Fukushima { class Proc; } }
namespace Test { namespace BattleFont { class BattleFontTestProc; } }
namespace Test { namespace ImageDB { class Proc; } }
namespace Test { namespace FSDeco { class Proc; } }
namespace Test { namespace Nakahiro { class TestProc; } }
namespace Test { namespace KawadaTest2 { namespace SimpleAppTest { class SimpleAppTestProc;  class SimpleAppTestProcParam; } } }
namespace Test { namespace AnaiTest{ class AnaiTestProc; }}
namespace Test { namespace MenuFrameTest { class OhnoDebugStartProc; } }
namespace test { namespace ncl { class WordsetTest; } }
namespace Debug { class ItemMakeProc; }
namespace Debug { class PokeIconViewProc; }
namespace Debug { class FontViewer; }
namespace Test  {namespace MutoTest  {class TestProc_HIOView;}  }
namespace Test { namespace HaradaTest { class FinderStudioTestProc; } }
namespace App { namespace StaffList { class StaffListProc; } }
#endif //PM_DEBUG



GFL_NAMESPACE_BEGIN(GameSys)

//-----------------------------------------------------------------------------
/**
  *   @brief バトルプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern void CallBattleProc( btl::BATTLE_PROC_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief デバッグバトルプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern void CallDebugBattleProc( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap );

//-----------------------------------------------------------------------------
/**
  *   @brief サウンドテストプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern void CallSoundTestProc( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap );

//-----------------------------------------------------------------------------
/**
*   @brief バトル計測呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallBattleMeasureProc(void);
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief 戦闘背景ビュワー呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallBackgroundViewerProc(void);
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief エフェクトビュワー呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::Ariizumi::EffectViewerProc*  CallEffectViewerProc(void);
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief 有泉描画テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallAriTestDrawProc(void);
extern void CallAriTestPokeDrawProc(void);
#endif //PM_DEBUG
//-----------------------------------------------------------------------------
/**
*   @brief バトル自動テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallAutoBattleProc(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief デバッグキーボード呼び出し
 * @return 呼び出したキーボードプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
debug::KeyboardProc* CallDebugKeyboardProc( void );
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief デバッグポケ作成　呼び出し
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
debug::PokeMake::DebugPokeMake* CallDebugPokeMakeProc( debug::PokeMake::APP_PARAM* param );
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
  *   @brief GTSプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::GTS::GTSProc* CallGTSProc( NetApp::GTS::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief Gift（ふしぎなおくりもの）プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::Gift::GiftProc*  CallGiftProc( NetApp::Gift::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief Live（ライブ大会）プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::Live::LiveProc*  CallLiveProc( NetApp::Live::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief movieプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern movie::MovieProc* CallMovieProc( void );

//-----------------------------------------------------------------------------
/**
  *   @brief 進化デモプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::Evolution::EvolutionProc*  CallEvolutionProc( NetApp::Evolution::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief BattleSpotプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::BattleSpot::BattleSpotProc*  CallBattleSpotProc( NetApp::BattleSpot::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief EggHatchingプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::EggHatching::EggHatchingProc*  CallEggHatchingProc( NetApp::EggHatching::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief QRReaderプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::QRReader::QRReaderProc*  CallQRReaderProc( NetApp::QRReader::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::PhotoView::PhotoViewProc*  CallPhotoViewProc( NetApp::PhotoView::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief ミラクル交換プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::MiracleTrade::MiracleTradeProc*  CallMiracleTradeProc( NetApp::MiracleTrade::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief レギュレーション選択(ダウンロードも可能)アプリのプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::RegulationSelect::RegulationSelectProc*  CallRegulationSelectProc( NetApp::RegulationSelect::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief 通信対戦受付アプリのプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::NBR::NBRProc* CallNBRProc( NetApp::NBR::APP_PARAM *param );

//-----------------------------------------------------------------------------
/**
  *   @brief チーム選択プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::TeamSelect::TeamSelectProc* CallTeamSelectProc( NetApp::TeamSelect::APP_PARAM* pParam );

//-----------------------------------------------------------------------------
/**
  *   @brief インターネット接続プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::Connection::ConnectionProc* CallConnectionProc( NetApp::Connection::APP_PARAM* pParam );

//-----------------------------------------------------------------------------
/**
  *   @brief クイックマッチアプリを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern NetApp::QuickMatch::QuickMatchProc* CallQuickMatchProc( NetApp::QuickMatch::APP_PARAM* pParam );

extern NetApp::JoinFestaBeacon::JoinFestaBeaconProc* CallJoinFestaBeaconProc( NetApp::JoinFestaBeacon::APP_PARAM* pParam );
extern NetApp::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectProc* CallJoinFestaPlayerListSelectProc( NetApp::JoinFestaPlayerListSelect::APP_PARAM* pParam );
extern NetApp::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiProc* CallJoinFestaPlayerListSelectMultiProc( NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM* pParam );
extern NetApp::JoinFestaPlayerListShow::JoinFestaPlayerListShowProc* CallJoinFestaPlayerListShowProc( NetApp::JoinFestaPlayerListShow::APP_PARAM* pParam );
extern NetApp::JoinFestaPlayerListSelectForScript::JoinFestaPlayerListSelectForScriptProc* CallJoinFestaPlayerListSelectForScriptProc( NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM* pParam );
extern NetApp::JoinFestaPlayerDetail::JoinFestaPlayerDetailProc* CallJoinFestaPlayerDetailProc( NetApp::JoinFestaPlayerDetail::APP_PARAM* pParam );
extern NetApp::JoinFestaAttractionListSelect::JoinFestaAttractionListSelectProc* CallJoinFestaAttractionListSelectProc( NetApp::JoinFestaAttractionListSelect::APP_PARAM* pParam );
extern NetApp::JoinFestaOpenList::JoinFestaOpenListProc* CallJoinFestaOpenListProc( NetApp::JoinFestaOpenList::APP_PARAM* pParam );
extern NetApp::JoinFestaAttractionResult::JoinFestaAttractionResultProc* CallJoinFestaAttractionResultProc( NetApp::JoinFestaAttractionResult::APP_PARAM* pParam );
extern NetApp::JoinFestaProfile::JoinFestaProfileProc* CallJoinFestaProfileProc( NetApp::JoinFestaProfile::APP_PARAM* pParam );
extern NetApp::JoinFestaSelectWord::JoinFestaSelectWordProc* CallJoinFestaSelectWordProc( NetApp::JoinFestaSelectWord::APP_PARAM* pParam );
extern NetApp::JoinFestaGlobalAttraction::JoinFestaGlobalAttractionProc* CallJoinFestaGlobalAttractionProc( NetApp::JoinFestaGlobalAttraction::APP_PARAM* pParam );
extern NetApp::P2PTrade::P2PTradeProc* CallP2PTradeProc( NetApp::P2PTrade::APP_PARAM* pParam );
extern NetApp::GameSync::GameSyncProc* CallGameSyncProc( NetApp::GameSync::APP_PARAM* pParam );

//-----------------------------------------------------------------------------
/**
  *   @brief タイトルプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern app::title::TitleProc* CallTitleProc( app::title::APP_PARAM* pAppParam );

//-----------------------------------------------------------------------------
/**
  *   @brief コンフィグプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern app::config::ConfigProc* CallConfigProc();

//-----------------------------------------------------------------------------
/**
  *   @brief フィールドプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern void CallFieldProc( Field::FieldmapDesc* desc );

//-----------------------------------------------------------------------------
/**
  *   @brief 捕獲チュートリアルプロセスを呼び出す関数
  *   @param  isCapture true:捕獲チュートリアル false:全力技チュートリアル
  */
//-----------------------------------------------------------------------------
extern app::capture::CaptureTutorialProc* CallCaptureTutorialProc( b32 isCapture = true );

//-----------------------------------------------------------------------------
/**
  *   @brief Boxプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern App::Box::Proc* CallBoxProc(void);
//-----------------------------------------------------------------------------
/**
  *   @brief リゾートプロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern App::Resort::ResortProc* CallResortProc(void);


//-----------------------------------------------------------------------------
/**
  *   @brief 証明写真プロセスを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern app::photo::TrainerPhotoProc* CallTrainerPhotoProc();

//-----------------------------------------------------------------------------
/**
*   @brief 川田テスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::KawadaTest::KawadaTestProc* CallKawadaTestProc(Test::KawadaTest::KawadaTestProcParam* param);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
*   @brief ポケモンインフレームカメラ　ツール呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern System::Camera::test::PokemonInframeTool* CallPokemonInframeTool(void);
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
*   @brief Peteテスト呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallPeteTestProc(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  デバッグ道具作成画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Debug::ItemMakeProc * CallDebugItemMakeProc(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
  *   @brief バトルプロセスを呼び出す関数 (DLL非同期読み込み)
  *   デモ系のDLLはフィールドで呼んでいるのでこちらで呼んでいません。なので、それらが無いと動きません。
  */
//-----------------------------------------------------------------------------
extern void StartLoadBattleDll();
extern bool IsFinishedLoadBattleDll();
extern void SetupCallBattleProc( btl::BATTLE_PROC_PARAM *param );

//-----------------------------------------------------------------------------
/**
*   @brief 図鑑呼び出し
*/
//-----------------------------------------------------------------------------
extern app::zukan::ZukanProc* CallZukanProc(app::zukan::ZukanProcParam* param);

//-----------------------------------------------------------------------------
/**
*   @brief 
*/
//-----------------------------------------------------------------------------
extern app::GameOver::GameOverProc* CallGameOverProc(app::GameOver::GameOverProcParam* param);

//-----------------------------------------------------------------------------
/**
  *   @brief FieldDemoを呼び出す関数
  */
//-----------------------------------------------------------------------------
extern Field::Demo::FieldDemoProc* CallFieldDemoProc( s32 seqNo, int demoType = 0 );


//-----------------------------------------------------------------------------
/**
 * @brief   福島テスト呼び出し
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::Fukushima::Proc* CallFukushimaTestProc( void );
#endif


//-----------------------------------------------------------------------------
/**
 * @brief   技教え・技忘れ呼び出し
 */
//-----------------------------------------------------------------------------
extern App::WazaOshie::Proc* CallWazaOshieProc( void );

//-----------------------------------------------------------------------------
/**
 * @brief  バッグ画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern app::bag::BagProc * CallBagProc(void);
extern app::bag::BagProc * SetNextBagProc(void);

//-----------------------------------------------------------------------------
/**
 * @brief   タイトルメニュー画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::TitleMenu::Proc* CallTitleMenuProc( void );


//-----------------------------------------------------------------------------
/**
 * @brief   中村用テスト画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::Nakahiro::TestProc * CallNakahiroProc(void);
extern void DebagCallFieldMenu(void);
#endif


//-----------------------------------------------------------------------------
/**
*   @brief 川田テスト2呼び出し
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::KawadaTest2::SimpleAppTest::SimpleAppTestProc* CallKawadaTest2SimpleAppTestProc(Test::KawadaTest2::SimpleAppTest::SimpleAppTestProcParam* param);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief   言語選択画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::LangSelect::Proc* CallLangSelectProc( void );


//-----------------------------------------------------------------------------
/**
 *  @brief  穴井テスト呼び出し
 *
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern void CallAnaiTestProc(void);
#endif

//-----------------------------------------------------------------------------
/**
 * @brief   ボックス検索画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::BoxSearch::Proc* CallBoxSearchProc( void );


//------------------------------------------------------------------
/**
  *  @func    アプリケーションから呼び出すためのシーケンスデモ再生Proc
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc* CallAppDemoProc( App::AppDemo::AppDemoProcParam* pParam );



//------------------------------------------------------------------
/**
  *  @func    ふしぎなおくりもの受信用シーケンスデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_Gift* CallAppDemoProc_Gift( App::AppDemo::AppDemoProcParam_Gift* pParam );


//------------------------------------------------------------------
/**
  *  @func    進化用シーケンスデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_Evolution* CallAppDemoProc_Evolution( App::AppDemo::AppDemoProcParam_Evolution* pParam );



//------------------------------------------------------------------
/**
  *  @func    GTSデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_GTS* CallAppDemoProc_GTS( App::AppDemo::AppDemoProcParam_GTS* pParam );



//------------------------------------------------------------------
/**
  *  @func    ミラクル交換デモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_MiracleTrade* CallAppDemoProc_MiracleTrade( App::AppDemo::AppDemoProcParam_MiracleTrade* pParam );


//------------------------------------------------------------------
/**
  *  @func    おめでとうデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_Congrats* CallAppDemoProc_Congrats( App::AppDemo::AppDemoProcParam_Congrats* pParam );

//------------------------------------------------------------------
/**
  *  @func    VSデモ
  *  @return  呼び出したプロセス
  */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_VS* CallAppDemoProc_VS( App::AppDemo::AppDemoProcParam_VS* pParam );

//-----------------------------------------------------------------------------
/**
 * @brief   容姿・性別選択画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::HeroSelect::Proc* CallHeroSelectProc( void );

//-----------------------------------------------------------------------------
/**
 * @brief   ステータス画面呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::Status::Proc* CallStatusProc( void );
extern App::Status::Proc* CallStatusProcEx(const bool bRadarChartCROLoad = true);

//-----------------------------------------------------------------------------
/**
 * @brief   かわいがり呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::Kawaigari::KawaigariProc* CallKawaigariProc( App::Kawaigari::KawaigariProcParam *param );

//-----------------------------------------------------------------------------
/**
 * @brief  ショップ購入画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern App::Shop::ShopProc * CallShopProc(void);
#endif

//-----------------------------------------------------------------------------
/**
 * @brief   タウンマップ呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::TownMap::TownMapProc * CallTownMapProc(void);
extern App::TownMap::TownMapProc * SetNextTownMapProc(void);

//-----------------------------------------------------------------------------
/**
 * @brief  ファインダーメニュー画面アプリ呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::FinderMenu::Proc* CallFinderMenuProc( void );

//-----------------------------------------------------------------------------
/**
 * @brief  ファインダースタジオ写真撮影アプリ呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::FinderStudio::Capture::Proc* CallFinderStudioCaptureProc( void );

//-----------------------------------------------------------------------------
/**
 * @brief  ファインダースタジオデコレーションアプリ呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::FinderStudio::Decoration::Proc* CallFinderStudioDecorationProc( void );

//-----------------------------------------------------------------------------
/**
* @brief  ファインダースタジオテストアプリ呼び出し
* @return 呼び出したプロセス
* @note   3Dビューワのみ起動
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::HaradaTest::FinderStudioTestProc* CallFinderStudioTestProc(void);
#endif

//-----------------------------------------------------------------------------
/**
 * @brief  トレーナーパス画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::TrainerPass::TrainerPassProc * SetNextTrainerPassProc(void);

//-----------------------------------------------------------------------------
/**
 * @brief  ポケパルレポケモン選択画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::PokeParure::ParurePokeListProc * SetNextParurePokeListProc(void);
#if PM_DEBUG
extern App::PokeParure::ParurePokeListProc * CallParurePokeListProc(void);
#endif // PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  技思い出し画面呼び出し
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::WazaOmoidasi::WazaOmoidasiProc * SetNextWazaOmoidasiProc(void);
#if PM_DEBUG
extern App::WazaOmoidasi::WazaOmoidasiProc * CallWazaOmoidasiProc(void);
#endif // PM_DEBUG


#if PM_DEBUG
Test::MenuFrameTest::OhnoDebugStartProc* CallMenuFrameTestProc( void );
#endif // PM_DEBUG

#if PM_DEBUG

extern test::ncl::WordsetTest* CallDebugWordSetTest();

#endif

//-----------------------------------------------------------------------------
/**
 *   @brief 文字入力プロセスを呼び出す関数
 */
//-----------------------------------------------------------------------------
extern App::StrInput::Proc* CallStrInputProc(void);

//-----------------------------------------------------------------------------
/**
 *  @brief スタッフロールプロセスを呼び出す関数
 *  @param  bFirstEvent:  初回か
 */
//-----------------------------------------------------------------------------
extern App::Staffroll::StaffrollProc* CallStaffrollProc( const bool bFirstEvent );

//-----------------------------------------------------------------------------
/**
 *  @brief ゲームクリアー時のセーブ画面プロセスを呼び出す関数
 *  @param[in] param  今のところ中身は何もないのでNULLでよい
 */
//-----------------------------------------------------------------------------
extern app::game_clear_save::Proc* CallGameClearSaveProc( app::game_clear_save::APP_PARAM* param );

//-----------------------------------------------------------------------------
/**
 *  @brief おめでとう画面を呼び出す関数
 *  @param[in] param 渡す情報
 */
//-----------------------------------------------------------------------------
extern app::dendou_demo::Proc* CallDendouDemoProc( app::dendou_demo::APP_PARAM* param );

//------------------------------------------------------------------
/**
 * @func    イントロデモ後半
 * @param   param   パラメータ
 * @return  呼び出したプロセス
 */
//------------------------------------------------------------------
extern App::AppDemo::AppDemoProc_Intro2nd * CallAppDemoProc_Intro2nd( App::AppDemo::AppDemoProcParam_Intro2nd * param );

//-----------------------------------------------------------------------------
/**
 *  @brief スタッフロール後のエンドプロセスを呼び出す
 *  @param[in] pParam  今のところ中身は何もないのでNULLでよい
 */
//-----------------------------------------------------------------------------
extern App::EndRoll::EndRollProc * CallEndRollProc( App::EndRoll::APP_PARAM* pParam );

//-----------------------------------------------------------------------------
/**
*  @brief マンタインサーフプロセスを呼び出す
*  @param[in] pParam  今のところ中身は何もないのでNULLでよい
*/
//-----------------------------------------------------------------------------
extern App::MantainSurf::MantainSurfProc * CallMantainSurfProc();

//-----------------------------------------------------------------------------
/**
*  @brief ルナソルダイブプロセスを呼び出す
*  @param[in] pParam  今のところ中身は何もないのでNULLでよい
*/
//-----------------------------------------------------------------------------
extern LunaSolDive::Proc * CallLunaSolDiveProc();

//-----------------------------------------------------------------------------
/**
 * @brief   マイアルバム呼び出し
 * @return  呼び出したプロセス
 */
//-----------------------------------------------------------------------------
extern App::MyAlbum::MyAlbumProc * SetNextMyAlbumProc(void);

//-----------------------------------------------------------------------------
/**
 * @brief  ポケモンアイコン確認画面呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Debug::PokeIconViewProc * CallDebugPokeIconViewProc(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief  フォントビューア呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Debug::FontViewer * CallDebugFontViewer(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
* @brief  バトルフォントテスト
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::BattleFont::BattleFontTestProc* CallBattleFontTestProc( void );
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
* @brief  ImageDBテスト
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::ImageDB::Proc* CallImageDBTestProc( void );
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
* @brief  ファインダースタジオデコテスト
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::FSDeco::Proc* CallFSDecoTestProc( void );
#endif //PM_DEBUG


//-----------------------------------------------------------------------------
/**
 * @brief  モデルビューア呼び出し（デバッグ）
 * @return 呼び出したプロセス
 */
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern Test::MutoTest::TestProc_HIOView* CallDebugModelViewer(void);
#endif //PM_DEBUG

//-----------------------------------------------------------------------------
/**
* @brief  スタッフロール（momiji追加分）呼び出し
* @return 呼び出したプロセス
*/
//-----------------------------------------------------------------------------
#if PM_DEBUG
extern App::StaffList::StaffListProc * CallStaffListProc(void);
#endif //PM_DEBUG


GFL_NAMESPACE_END(GameSys)

#endif //__DLL_PROC_H__
