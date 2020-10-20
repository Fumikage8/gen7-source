//==============================================================================
/**
 @file    d_kawada.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.22
 @brief   川田テストランチャー
 */
//==============================================================================


#if PM_DEBUG


// niji
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_kawada.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <Debug/StartMenu/include/d_kawada.h>

#include <GameSys/include/DllProc.h>

#include <Test/KawadaTest/include/KawadaTestProc.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


const u32 KawadaTestLauncher::ItemList[LIST_MAX] =
{
  MS_LABEL_D_KAWADA_MENU_009,  // ポケモンインフレームカメラ　ツール
  MS_LABEL_D_KAWADA_MENU_000,  // モデルのサンプル
  MS_LABEL_D_KAWADA_MENU_001,  // レイアウトのサンプル
  MS_LABEL_D_KAWADA_MENU_002,  // レーダーチャート
  MS_LABEL_D_KAWADA_MENU_003,  // ずかん
  MS_LABEL_D_KAWADA_MENU_007,  // ずかんとうろく
  MS_LABEL_D_KAWADA_MENU_004,  // ポケモデルのサンプル
  MS_LABEL_D_KAWADA_MENU_005,  // インフォウィンドウ
  MS_LABEL_D_KAWADA_MENU_008,  // ゼンリョクリング
  MS_LABEL_D_KAWADA_MENU_010,  // キラカードサンプル
  MS_LABEL_D_KAWADA_MENU_011,  // クリアーじのセーブ
};


KawadaTestLauncher::KawadaTestLauncher(void)
{}
KawadaTestLauncher::~KawadaTestLauncher()
{}

gfl2::fs::ArcFile::ARCDATID KawadaTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_kawada_DAT;
}
const u32 * KawadaTestLauncher::GetListTable(void)
{
  return ItemList;
}
u32 KawadaTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

launcher::ListData::RequestID KawadaTestLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  Test::KawadaTest::KawadaTestProcParam param;

#if 0  // TEST_EVENT_KAWADA_CALL_ZUKAN_REGISTER_SAMPLE_MODE_ASING
  // case MS_LABEL_D_KAWADA_MENU_007:  // ずかんとうろく
  param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_ZUKAN_REGISTER;
  GameSys::CallKawadaTestProc(&param);
  return debug::launcher::ListData::REQID_FINISH;
#endif

  switch( ItemList[pos] )
  {
  case MS_LABEL_D_KAWADA_MENU_009:  // ポケモンインフレームカメラ　ツール
    GameSys::CallPokemonInframeTool();
    return debug::launcher::ListData::REQID_CALL;
  case MS_LABEL_D_KAWADA_MENU_000:  // モデルのサンプル
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_MODEL;
    GameSys::CallKawadaTestProc(&param);
    //GameSys::CallKawadaTest2SimpleAppTestProc(NULL);
    return debug::launcher::ListData::REQID_CALL;
  case MS_LABEL_D_KAWADA_MENU_001:  // レイアウトのサンプル
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_LAYOUT;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_002:  // レーダーチャート
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_RADER_CHART;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_CALL;
  case MS_LABEL_D_KAWADA_MENU_003:  // ずかん
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_ZUKAN;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_007:  // ずかんとうろく
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_ZUKAN_REGISTER;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_004:  // ポケモデルのサンプル
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_POKEMODEL;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_005:  // インフォウィンドウ
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_INFO_WINDOW;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_008:  // ゼンリョクリング画面アプリ
    break;
  case MS_LABEL_D_KAWADA_MENU_010:  // キラカードのサンプル
    param.sampleId = Test::KawadaTest::KawadaTestProcParam::SAMPLE_ID_KIRA_CARD;
    GameSys::CallKawadaTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_KAWADA_MENU_011:  // クリアー時のセーブ
    GameSys::CallGameClearSaveProc(NULL);
    return debug::launcher::ListData::REQID_CALL;
  }

  return debug::launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG
