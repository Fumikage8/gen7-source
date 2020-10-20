//=============================================================================
/**
 *
 *	@file		EventBattleDll.cpp
 *	@brief  バトルDLL非同期読み込み
 *	@author	tamada
 *	@date		2013.03.22
 *
    2015.12.13  sango から移植開始
 */
//=============================================================================
#include <macro/include/gfl2_Macros.h>
#include <System/include/HeapDefine.h>

#include "Field/FieldStatic/include/Event/EventBattleDll.h"
#include "GameSys/include/DllReadThread.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"


//btl
#include <Battle/include/battle_proc.h>

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventBattleDll::BootChk(GameSys::GameManager* p_gman)
{
  GFL_UNUSED(p_gman);
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleDll::InitFunc(GameSys::GameManager* p_gman)
{
  GFL_UNUSED(p_gman);
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleDll::MainFunc(GameSys::GameManager* p_gman)
{
  switch(m_Seq)
  {
  case 0:
#if defined(GF_PLATFORM_CTR)
    {
      const char *roNameArr[] = {
        "Battle.cro",
        //"Background.cro", //Object,Background,SeqEditorはmomijiで統合されました。
        //下記2個は後読み
        //"Bag.cro",
        //"WazaOshie.cro",
      };
      const char *battlefesRoNameArr[] = {
        "Battle.cro",
        "Object.cro",
        //"Background.cro", //Object,Background,SeqEditorはmomijiで統合されました。
        //下記2個は後読み
        //"Bag.cro",
        //"WazaOshie.cro",
      };
      gfl2::heap::HeapBase* pHeap  = p_gman->GetProcManager()->GetHeap();
      
      m_BattleDllRead = GFL_NEW( pHeap ) GameSys::DllReadThread(pHeap, 4096);
			// @fix GFMMCat[75] ジムオブカントーのバトルでアサート
#if PM_DEBUG
      m_BattleDllReadDebug = GFL_NEW( pHeap ) GameSys::DllReadThread(pHeap, 4096);
#endif // PM_DEBUG
      if (m_isBattleFes)
      {
        m_BattleDllRead->SetData(battlefesRoNameArr, elementof(battlefesRoNameArr));
      }
      else
      {
        m_BattleDllRead->SetData(roNameArr, elementof(roNameArr));
      }
      m_BattleDllRead->Start(gfl2::thread::Thread::THREAD_PRI_NORMAL );
      ++m_Seq;
    }
    break;
#elif defined(GF_PLATFORM_WIN32)
    ++m_Seq;
#endif  // GF_PLATFORM_ 

  case 1:
#if defined(GF_PLATFORM_CTR)
    {
      if(m_BattleDllRead->IsAlive()){
        break;
      }
#if PM_DEBUG
      gfl2::heap::HeapBase* pHeap  = p_gman->GetProcManager()->GetHeap();

      const char *roDebugNameArr[] = {
        "BattleDebug.cro",
      };
      m_BattleDllReadDebug->SetData(roDebugNameArr, elementof(roDebugNameArr),gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DLL_LOAD));
      m_BattleDllReadDebug->Start(gfl2::thread::Thread::THREAD_PRI_NORMAL );
#endif //PM_DEBUG

      ++m_Seq;
    }
    break;
#elif defined(GF_PLATFORM_WIN32)
    ++m_Seq;
#endif  // GF_PLATFORM_ 

  case 2:
#if defined(GF_PLATFORM_CTR)
    {
#if PM_DEBUG
      if (m_BattleDllReadDebug->IsAlive()){
        break;
      }
#endif //PM_DEBUG
      btl::BattleProc *p_battle_proc = NIJI_PROC_CALL< GameSys::GameProcManager, btl::BattleProc, btl::BATTLE_PROC_PARAM>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager(), &m_Param);
      p_battle_proc->CreateChildModulePtrBuffer(p_gman->GetProcManager()->GetHeap(), 5); //後読みの分を含めてバッファは5個
      m_BattleDllRead->Link(p_battle_proc,p_gman->GetProcManager()->GetHeap());

      GFL_DELETE m_BattleDllRead;

#if PM_DEBUG
      m_BattleDllReadDebug->Link(p_battle_proc, p_gman->GetProcManager()->GetHeap());
      GFL_DELETE m_BattleDllReadDebug;
#endif //PM_DEBUG

      ++ m_Seq;
    }
#elif defined(GF_PLATFORM_WIN32)
    {
      GameSys::SetupCallBattleProc( &m_Param );
    }
#endif  // GF_PLATFORM_ 
    // break through

  case 3:
    return GameSys::GMEVENT_RES_FINISH;
  }
  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleDll::EndFunc(GameSys::GameManager* p_gman)
{
  GFL_UNUSED(p_gman);
}


GFL_NAMESPACE_END( Field )
