/**
 * @file btl_BattleAiSystem.cpp
 * @brief 戦闘AIシステム
 * @author obata_toshihiro
 * @date 2015.04.02
 */

#include "../btl_Heap.h"
#include "./btl_BattleAiSystem.h"


GFL_NAMESPACE_BEGIN( btl )


bool BattleAiSystem::s_isTokuseiOpened[ BTL_POKEID_MAX ];
gfl2::heap::HeapBase*  BattleAiSystem::s_heapHandle = NULL;
u32 BattleAiSystem::s_commonRandValue = 0;



//---------------------------------------------------------------------------
/**
 * @brief システムを初期化する
 * @param parentHeap  戦闘AI用ヒープの親となるヒープ
 */
//---------------------------------------------------------------------------
void BattleAiSystem::InitSystem( HeapHandle parentHeap )
{
  // スクリプト用ヒープ作成
  if( s_heapHandle == NULL ) 
  {
    s_heapHandle =  GFL_CREATE_LOCAL_HEAP_NAME( parentHeap, Heap::HEAPSIZE_AI_SCRIPTBINARY, gfl2::heap::HEAP_TYPE_EXP, true, "BattleAi" );
  }

  // オープンになった「とくせい」の記録領域を初期化
  for( u32 i=0; i<GFL_NELEMS(s_isTokuseiOpened); ++i )
  {
    s_isTokuseiOpened[i] = false;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief システムを終了する
 */
//---------------------------------------------------------------------------
void BattleAiSystem::QuitSystem( void )
{
  if( s_heapHandle ) {
    GFL_DELETE_HEAP( s_heapHandle );
    s_heapHandle = NULL;
  }
}



//---------------------------------------------------------------------------
/**
 * @brief AI用ヒープハンドルを取得する
 */
//---------------------------------------------------------------------------
HeapHandle BattleAiSystem::GetHeapHandle( void )
{
  return s_heapHandle;
}


#if PM_DEBUG
//---------------------------------------------------------------------------
/**
 * @brief AI用ヒープをダンプする
 */
//---------------------------------------------------------------------------
void BattleAiSystem::DumpHeap( void )
{
  if( s_heapHandle )
  {
    BTL_PRINT(
      "[AI_Heap] Total:0x%08x, Free:0x%08x, Allocatable:0x%08x\n",
      s_heapHandle->GetTotalSize(), s_heapHandle->GetTotalFreeSize(), s_heapHandle->GetTotalAllocatableSize());
  }
  else
  {
    BTL_PRINT("[AI_Heap] heap isn't exist ...\n");
  }
}
#endif



//---------------------------------------------------------------------------
/**
 * @brief 「とくせい」がオープンになったことの通知を受け取る
 * @param pokeID  「とくせい」オープンになったポケモンID
 */
//---------------------------------------------------------------------------
void BattleAiSystem::NotifyTokuseiOpen( u8 pokeID )
{
  if( GFL_NELEMS(s_isTokuseiOpened) <= pokeID ) 
  {
    GFL_ASSERT(0);
    return;
  }

  s_isTokuseiOpened[ pokeID ] = true;
}

//---------------------------------------------------------------------------
/**
 * @brief 「とくせい」がオープンになっているポケモンかどうか判定
 * @param pokeID   チェックするポケモンID
 */
//---------------------------------------------------------------------------
bool BattleAiSystem::IsTokuseiOpened( u8 pokeID )
{
  if( pokeID < GFL_NELEMS(s_isTokuseiOpened) ){
    return s_isTokuseiOpened[ pokeID ];
  }
  else{
    GFL_ASSERT(0);
    return false;
  }
}



//---------------------------------------------------------------------------
/**
 * @brief 共通乱数を設定する
 * @param randValue  共通乱数として設定する値
 */
//---------------------------------------------------------------------------
void BattleAiSystem::SetCommonRand( u32 randValue )
{
  s_commonRandValue = randValue;
  BTL_PRINT( "[BattleAiSystem] s_commonRandValue=%d\n", s_commonRandValue );
}

//---------------------------------------------------------------------------
/**
 * @brief 共通乱数を取得する
 */
//---------------------------------------------------------------------------
u32 BattleAiSystem::GetCommonRand( void )
{
  return s_commonRandValue;
}






GFL_NAMESPACE_END( btl )