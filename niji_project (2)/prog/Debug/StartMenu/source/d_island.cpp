//==============================================================================
/**
 @file    d_island.h
 @author  k.ohno
 @date    2015.05.25
 @brief   ポケモンアイランド用・・・沢山作らないかも
 */
//==============================================================================


#if PM_DEBUG


// niji
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_d_island.h>

#include <Debug/StartMenu/include/StartLauncher.h>

#include <Debug/StartMenu/include/d_island.h>

#include <GameSys/include/DllProc.h>


#include <System/include/HeapDefine.h>


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)


const u32 IslandTestLauncher::ItemList[LIST_MAX] =
{
  DEBUG_ISLAND_001,
  DEBUG_ISLAND_002,
};


IslandTestLauncher::IslandTestLauncher(void)
{}
IslandTestLauncher::~IslandTestLauncher()
{}

gfl2::fs::ArcFile::ARCDATID IslandTestLauncher::GetArcDataID(void)
{
  return GARC_debug_message_d_island_DAT;
}
const u32 * IslandTestLauncher::GetListTable(void)
{
  return ItemList;
}
u32 IslandTestLauncher::GetListTableSize(void)
{
  return LIST_MAX;
}

launcher::ListData::RequestID IslandTestLauncher::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == debug::launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<debug::startmenu::StartLauncher>();
    return debug::launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case DEBUG_ISLAND_001:
    return debug::launcher::ListData::REQID_CALL;
  case DEBUG_ISLAND_002:
    return debug::launcher::ListData::REQID_FINISH;
  }

  return debug::launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)


#endif  // PM_DEBUG

