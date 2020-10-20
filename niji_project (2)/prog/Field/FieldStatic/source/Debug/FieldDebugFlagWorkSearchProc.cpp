//======================================================================
/**
* @file  FieldDebugFlagWorkSearchProc.cpp
* @date 15/05/15
* @author ikuta_junya 
*/
//======================================================================

#if (PM_DEBUG)

#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugFlagWorkSearchProc.h"

#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <arc_def_index/arc_def.h>

#include <System/include/Screenshot.h>
#include <gfx/include/gfl2_PCDebugUtil.h>


#include "Field/FieldRo/include/Fieldmap.h"
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>
#include <GameSys/include/DllProc.h>
#include <str/include/gfl2_StrBuf.h>
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "AppLib/include/Ui/UIView.h"
#include "Debug/DebugWin/include/DebugWinSystem.h"
#include <Debug/include/gfl2_DebugWinItemTypes.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)


/**
* @brief constructor
*
*/
FieldDebugFlagWorkSearchProc::FieldDebugFlagWorkSearchProc() :
  m_pHeap(NULL),
  m_pAllocator(NULL),
  m_InitSeq(0),
  m_UpdateSeq(0),
  m_EndSeq(0),
  m_pSearchWord( NULL )
{
}

/**
* @brief destructor
*
*/
FieldDebugFlagWorkSearchProc::~FieldDebugFlagWorkSearchProc()
{
}

//------------------------------------------------------------------
/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
*/
//------------------------------------------------------------------

gfl2::proc::Result FieldDebugFlagWorkSearchProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;
  result = gfl2::proc::RES_FINISH;
  return result;
}

/**
* @brief	  PROCのメイン更新を行う関数
* @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldDebugFlagWorkSearchProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  switch (m_UpdateSeq )
  {

  case 0:
    {
      // デバッグキーボードをコール
      gfl2::heap::HeapBase *eventDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
      m_pSearchWord = GFL_NEW( eventDeviceHeap ) gfl2::str::StrBuf( 512, eventDeviceHeap );

      GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      Field::Fieldmap* pFieldmap = pGameManager->GetFieldmap();
      debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
      proc->SetupParam( eventDeviceHeap, eventDeviceHeap, m_pSearchWord, 256, 512 );
      proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_ALPHA );
      proc->SetLayoutSystem( pFieldmap->GetFieldWindowManager()->GetUIView()->GetLayoutSystem() );

      m_UpdateSeq++;

      // このプロックは1度抜けた後スリープ状態になる

      break;
    }

  case 1:
    {
      // デバッグキーボード入力終了処理、入力された文字列を含むフラグワーク項目をデバッグメニューに追加
      if( m_pSearchWord->GetLength() >= 1 )
      {
        b32 isOver = false;
        size_t breakHeapFreeSize = 340 * 10;   // 161010時点で1項目あたり340バイト使用
        s32 hitCount = 0;

        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_EventFlag_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_VanishFlag_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount  ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_ItemFlag_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_EtcFlag_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_SceneWork_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_SysWork_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;
        isOver = Field::Debug::FieldDebugMenu::CreateFlagWorkItemForSearch( m_Param.userWork, m_Param.item, GARC_ScriptDebugSymbol_OtherWork_BIN, m_pSearchWord, breakHeapFreeSize, &hitCount ) ? true : isOver;

        if( isOver )
        {
          gfl2::debug::DebugWinGroup *pGropWork = reinterpret_cast<gfl2::debug::DebugWinGroup*>( m_Param.item ); 
          FieldDebugMenu *pDebug = reinterpret_cast<FieldDebugMenu*>( m_Param.userWork );

          wchar_t workStr[512];
          swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"(表示可能数=%d,検索結果=%d)", pGropWork->GetItemList()->Size(), hitCount );

          pGropWork->RemoveAllItem();
          gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , pDebug->GetHeap() , L"検索結果が多すぎて表示できません。", NULL, NULL, NULL );
          gfl2::debug::DebugWin_AddItemUpdateFunc( pGropWork , pDebug->GetHeap() , workStr, NULL, NULL, NULL );
        }

        // 選択項目を更新
        GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->UpdateCurrent();
      }

      GFL_SAFE_DELETE( m_pSearchWord );

      // デバッグメニューを有効化( 無効化はデバッグメニューの検索項目起動時に実行されます )
      GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin)->SetFoceStopFlag( false );
      return gfl2::proc::RES_FINISH;
    }
  }

  return result;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void FieldDebugFlagWorkSearchProc::PreDrawFunc (gfl2::proc::Manager* pManager)
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void FieldDebugFlagWorkSearchProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result FieldDebugFlagWorkSearchProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result FieldDebugFlagWorkSearchProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;
  result = gfl2::proc::RES_FINISH;

  return result;
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // PM_DEBUG