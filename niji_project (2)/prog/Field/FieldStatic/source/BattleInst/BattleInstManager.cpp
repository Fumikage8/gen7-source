//======================================================================
/**
 * @file BattleInstManager.cpp
 * @date 2016/02/23 18:07:32
 * @author onoue_masayuki
 * @brief バトル施設常駐マネージャー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "System/include/HeapDefine.h"

GFL_NAMESPACE_BEGIN( BattleInst )

BattleInstManager::BattleInstManager()
  : pBattleInst(NULL),pModule(NULL)
{
}
BattleInstManager::~BattleInstManager()
{
}

void BattleInstManager::Create( GameSys::GameManager *p_gman )
{
  if( pBattleInst == NULL )
  {
    //  DLLを読み込み
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    pModule = roMgr->LoadModuleLower(p_gman->GetAsyncFileManager(), "FieldBattleInstEvent.cro");
    roMgr->StartModule(pModule, true);


    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    pBattleInst = GFL_NEW_LOW( p_heap ) BattleInst( p_gman, p_heap );
    pBattleInst->Create();
  }
}

void BattleInstManager::Delete()
{
  if( pBattleInst != NULL )
  {
    pBattleInst->Delete();
    GFL_SAFE_DELETE( pBattleInst );

    // DLLを破棄
    gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
    roMgr->DisposeModule( pModule );
    pModule = NULL;
  }
}

BattleInst::MoveModelData* BattleInstManager::GetModelData(int i)
{ 
  return pBattleInst->GetModelData(i); 
}

GFL_NAMESPACE_END(BattleInst)

