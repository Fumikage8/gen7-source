// ============================================================================
/*
 * @file BattleTreeWork.cpp
 * @brief バトルツリーアプリのワーククラスです。
 * @date 2015.11.30
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)


  BattleTreeWork::BattleTreeWork( App::BattleTree::APP_PARAM* pAppParam ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  {
    _Clear();

    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;
  }


  BattleTreeWork::~BattleTreeWork()
  {
  }

  //  トレーナーIDの割り当て
  u32 BattleTreeWork::GetAllocTrainerLoadID()
  {
    u32 id  = m_allockTrainerLoadID;
    if( TRAINER_LOAD_ICON_MAX <= m_allockTrainerLoadID )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    ++m_allockTrainerLoadID;

    return  id;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
