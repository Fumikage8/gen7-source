//==============================================================================
/**
 @file    d_anai.h
 @author  anai_shigeo@gamefreak.co.jp
 @date    2015.07.24
 @brief   穴井テストランチャー
 */
//==============================================================================


#if PM_DEBUG


// niji
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_anai.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <Debug/StartMenu/include/d_Anai.h>

#include <GameSys/include/DllProc.h>

#include <Test/AnaiTest/include/AnaiTestProc.h>
#include <Test/AnaiTest/include/AnaiTestProcParam.h>

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


const u32 AnaiTestLauncher::ItemList[LIST_MAX] =
{
  Debug_anai_01_01
};


AnaiTestLauncher::AnaiTestLauncher(void)
{}
AnaiTestLauncher::~AnaiTestLauncher()
{}

gfl2::fs::ArcFile::ARCDATID AnaiTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_anai_DAT;
}
const u32 * AnaiTestLauncher::GetListTable(void)
{
  return ItemList;
}
u32 AnaiTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

launcher::ListData::RequestID AnaiTestLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  Test::AnaiTest::AnaiTestProcParam param;
  
/*
  switch( ItemList[pos] )
  {
  case MS_LABEL_D_ANAI_MENU_000:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_MODEL;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_CALL;
  case MS_LABEL_D_ANAI_MENU_001:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_LAYOUT;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_ANAI_MENU_002:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_RADER_CHART;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_CALL;
  case MS_LABEL_D_ANAI_MENU_003:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_ZUKAN;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_ANAI_MENU_004:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_POKEMODEL;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  case MS_LABEL_D_ANAI_MENU_005:
    param.sampleId = Test::AnaiTest::AnaiTestProcParam::SAMPLE_ID_INFO_WINDOW;
    GameSys::CallAnaiTestProc(&param);
    return debug::launcher::ListData::REQID_FINISH;
  }
*/
  return debug::launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG
