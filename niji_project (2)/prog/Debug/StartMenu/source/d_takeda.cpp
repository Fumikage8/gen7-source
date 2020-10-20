#if PM_DEBUG
//==============================================================================
/**
 * @file        d_takeda.cpp
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 18:34:00
 */
//==============================================================================
#include "Debug/StartMenu/include/StartLauncher.h"
#include "Debug/StartMenu/include/d_takeda.h"

//#include "niji_conv_header/poke_lib/item/itemsym.h"
#include <pml/include/pmlib.h>

#include "arc_index/debug_message.gaix"
#include <niji_conv_header/message/debug/msg_d_nakahiro.h>

#include "System/include/HeapDefine.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "Savedata/include/MyItemSave.h"

#include "App/FieldBag/include/BagProc.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"
#include "App/FieldMenu/include/FieldMenuProc.h"

#include "App/AppEvent/include/Zukan/ZukanEvent.h"
#include "App/AppEvent/include/GameOver/GameOverEvent.h"
#include "App/GameOver/include/GameOverProc.h"

#include "Test/NakahiroTest/include/NakahiroTestProc.h"

#include "niji_conv_header/message/msg_gameover.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

// 項目文字列データテーブル
const u32 LauncherTakeda::ItemList[LauncherTakeda::LIST_MAX] =
{
  label_nakahiro_test_00,
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//-----------------------------------------------------------------------------
LauncherTakeda::LauncherTakeda(void)
{
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
LauncherTakeda::~LauncherTakeda()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetArcDataID
 * @brief   アークデータID取得
 * @date    2015.02.18
 *
 * @return  アークデータID
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID LauncherTakeda::GetArcDataID(void)
{
  return GARC_debug_message_d_nakahiro_DAT;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブル取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブル
 */
//-----------------------------------------------------------------------------
const u32 * LauncherTakeda::GetListTable(void)
{
  return ItemList;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListTable
 * @brief   項目文字列データテーブルの要素数取得
 * @date    2015.02.18
 *
 * @return  文字列データテーブルの要素数
 */
//-----------------------------------------------------------------------------
u32 LauncherTakeda::GetListTableSize(void)
{
  return LIST_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectListener
 * @brief   決定時のリスナー
 * @date    2015.02.18
 *
 * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
 * @param   launcher  呼び出し元のランチャークラス
 *
 * @return  ランチャーへのリクエストID
 */
//-----------------------------------------------------------------------------
launcher::ListData::RequestID LauncherTakeda::SelectListener( u32 pos, launcher::LauncherProc * launcher )
{
  // キャンセル
  if( pos == launcher::LauncherProc::CURSOR_POS_CANCEL )
  {
    launcher->CreateListData<StartLauncher>();
    return launcher::ListData::REQID_RESET;
  }

  switch( ItemList[pos] )
  {
  case label_nakahiro_test_00:
    {
      app::GameOver::GameOverProcParam param2;
      param2.m_iMsgId = msg_all_dead_01;
      GameSys::CallGameOverProc(&param2);
    }
    return launcher::ListData::REQID_CALL;
  }

  return launcher::ListData::REQID_NONE;
}


GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // PM_DEBUG
