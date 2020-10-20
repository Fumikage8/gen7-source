//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugEventBattleInstTest.cpp
 *	@brief  バトル施設テスト
 *	@author	onoue masayuki
 *	@date		2016.03.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if PM_DEBUG

#include "FieldDebugEventBattleInstTest.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "System/include/HeapDefine.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

GFL_NAMESPACE_BEGIN(Field);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
DebugEventBattleInstTest::DebugEventBattleInstTest( gfl2::heap::HeapBase* heap ) : 
  GameSys::GameEvent(heap)
{
}

DebugEventBattleInstTest::~DebugEventBattleInstTest()
{
}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool DebugEventBattleInstTest::BootChk(GameSys::GameManager* gmgr)
{
  return true;
}

void DebugEventBattleInstTest::SetDesc( const DebugEventBattleInstTest::InitializeDescription& rDesc )
{
  m_InitDesc = rDesc;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugEventBattleInstTest::InitFunc(GameSys::GameManager* gmgr)
{
  // イベントステータスを立てる。
  gmgr->GetGameData()->GetFieldCharaModelManager()->AddEventRequestAll();
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT DebugEventBattleInstTest::MainFunc(GameSys::GameManager* gmgr)
{
  enum
  {
    SEQ_TEST,
    SEQ_END,
  };


  switch( this->GetSeqNo() )
  {
  case SEQ_TEST:
    {
      // @todo チェック処理をエフェクトモジュール分割後のチェックに対応完了次第復旧
      //GFL_PRINT("call DebugEventBattleInstTest");
      //TestDllSize();
      // 終了
      this->SetSeqNo( SEQ_END );
      break;
    }
  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   gmgr       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void DebugEventBattleInstTest::EndFunc(GameSys::GameManager* gmgr)
{
  // イベントステータスを立てる。
  gmgr->GetGameData()->GetFieldCharaModelManager()->PullEventRequestAll();
}

//--------------------------------------------------------------
/**
 * @brief   DLLサイズをチェック
 */
//--------------------------------------------------------------
void DebugEventBattleInstTest::TestDllSize()
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ro::RoManager *pRoManager = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  gfl2::heap::HeapBase * pDllLoad = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );


  struct DllData
  {
    bool isLow; // メモリの後方確保を行なうか
    char* dllName;
  };

  // Roモジュール
  static const int RO_MODULE_NUM = 4;
  const DllData RO_MODULES[RO_MODULE_NUM] =
  {
    {false,"FieldGimmickBattleInst.cro"},
    {true,"FieldBattleInstEvent.cro"},
    //{true,"SeqEditor.cro"},//Object,Background,SeqEditorはmomijiで統合されました。
    {true,"Object.cro"},
    {true,"SequencePlayer.cro"},
  };
  gfl2::ro::Module* pModules[RO_MODULE_NUM];


  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    if( RO_MODULES[i].isLow )
    {
      pModules[i] = pRoManager->LoadModuleLower( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
    }else{
      pModules[i] = pRoManager->LoadModule( pGameManager->GetAsyncFileManager(), RO_MODULES[i].dllName, pDllLoad);
    }
    pRoManager->StartModule( pModules[i],true);
    GFL_RELEASE_PRINT( "load %s\n", RO_MODULES[i].dllName );
    pDllLoad->Dump();
  }
  GFL_RELEASE_PRINT( "BattleTree StartModule end\n" );
  GFL_RELEASE_PRINT( "write heap free size  when had loaded dll free[0x%x] allocatable[0x%x]\n", pDllLoad->GetTotalFreeSize(), pDllLoad->GetTotalAllocatableSize() );
  pDllLoad->Dump();

  for( u32 i = 0; i < RO_MODULE_NUM; ++i)
  {
    pRoManager->DisposeModule( pModules[i] );
  }
  GFL_RELEASE_PRINT( "BattleTree DisposeModule end\n" );
}

GFL_NAMESPACE_END(Field);

#endif // PM_DEBUG
