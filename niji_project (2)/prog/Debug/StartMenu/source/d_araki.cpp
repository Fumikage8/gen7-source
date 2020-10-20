//======================================================================
/**
 * @file d_araki.cpp
 * @date 2015/10/28 16:38:33
 * @author araki_syo
 * @brief 荒木テストランチャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG


// niji
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_araki.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <Debug/StartMenu/include/d_araki.h>

#include <GameSys/include/DllProc.h>

#include <Test/ArakiTest/include/ArakiTestProc.h>
#include <Test/ArakiTest/include/OteireTestProc.h>

#include <GameSys/include/GameProcManager.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


const u32 ArakiTestLauncher::ItemList[LIST_MAX] =
{
  ArakiDebugMenu_Kisekae,
  ArakiDebugMenu_Oteire,
};


ArakiTestLauncher::ArakiTestLauncher(void)
{}
ArakiTestLauncher::~ArakiTestLauncher()
{}

gfl2::fs::ArcFile::ARCDATID ArakiTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_araki_DAT;
}
const u32 * ArakiTestLauncher::GetListTable(void)
{
  return ItemList;
}
u32 ArakiTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

launcher::ListData::RequestID ArakiTestLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case ArakiDebugMenu_Kisekae:
    NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::ArakiTest::ArakiTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "ArakiTest.cro" );	 
    return debug::launcher::ListData::REQID_CALL;
  case ArakiDebugMenu_Oteire:
    NIJI_RO_PROC_CALL< GameSys::GameProcManager, Test::OteireTest::OteireTestProc >( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager() , GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager() , "ArakiTest.cro" );	 
    return debug::launcher::ListData::REQID_CALL;
  }

  return debug::launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG
