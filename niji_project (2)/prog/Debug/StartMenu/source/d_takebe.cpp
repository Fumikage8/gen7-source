//==============================================================================
/**
 @file    d_takebe.h
 @author  makoto takebe
 @date    2015.05.22
 @brief   竹部テストランチャー
 */
//==============================================================================


#if PM_DEBUG


// niji
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_takebe.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <Debug/StartMenu/include/d_takebe.h>

#include <GameSys/include/DllProc.h>

#include <Test/TakebeTest/include/TakebeTestProc_CharaModel.h>
#include <Test/TakebeTest/include/TakebeTestProc_Kisekae.h>

#include <GameSys/include/GameProcManager.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


const u32 TakebeTestLauncher::ItemList[LIST_MAX] =
{
  TakebeDebugMenu_CharaModel,
  TakebeDebugMenu_DressUpModel,
};


TakebeTestLauncher::TakebeTestLauncher(void)
{}
TakebeTestLauncher::~TakebeTestLauncher()
{}

gfl2::fs::ArcFile::ARCDATID TakebeTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_takebe_DAT;
}
const u32 * TakebeTestLauncher::GetListTable(void)
{
  return ItemList;
}
u32 TakebeTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

launcher::ListData::RequestID TakebeTestLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case TakebeDebugMenu_CharaModel:
    NIJI_PROC_CALL< GameSys::GameProcManager, Test::TakebeTest::TakebeTestProc_CharaModel >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
    return debug::launcher::ListData::REQID_CALL;
  case TakebeDebugMenu_DressUpModel:
    NIJI_PROC_CALL< GameSys::GameProcManager, Test::TakebeTest::TakebeTestProc_Kisekae >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() );
    return debug::launcher::ListData::REQID_CALL;
  }

  return debug::launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

