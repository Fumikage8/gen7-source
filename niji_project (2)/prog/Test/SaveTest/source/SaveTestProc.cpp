// ============================================================================
/*
 * @file SaveTestProc.cpp
 * @brief GameDataのセーブテストです。
 * @date 2015.10.02
 * @author Akira endo
 */
// ============================================================================
#if PM_DEBUG

#include "Test/SaveTest/include/SaveTestProc.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(SaveTest)


SaveTestProc::SaveTestProc() :
  m_bLoadError( false )
{
}


SaveTestProc::~SaveTestProc()
{
}


gfl2::proc::Result SaveTestProc::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result procResult = gfl2::proc::RES_CONTINUE;

  GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  switch( GetSubSeq() )
  {
    case 0:
    {
      gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      GFL_PRINT( "pGameData->LoadStart\n" );
      gfl2::fs::FsCtrBackupSystem::Result result = pGameData->LoadStart( pDeviceHeap );
      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        GFL_PRINT( "pGameData->LoadStart Success\n" );
        AddSubSeq();
      }
      else
      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
      {
        GFL_PRINT( "pGameData->LoadStart Error\n" );
        m_bLoadError = true;
        procResult = gfl2::proc::RES_FINISH;
      }
    }
    break;

    case 1:
    {
      GFL_PRINT( "pGameData->LoadMain\n" );
      gfl2::fs::FsCtrBackupSystem::Result result = pGameData->LoadMain();
      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
      {
        GFL_PRINT( "pGameData->LoadMain Success\n" );
        procResult = gfl2::proc::RES_FINISH;
      }
      else
      if( result == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
      {
        GFL_PRINT( "pGameData->LoadMain Error\n" );
        m_bLoadError = true;
        procResult = gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return procResult;
}


gfl2::proc::Result SaveTestProc::EndFunc( gfl2::proc::Manager* pManager )
{
  return gfl2::proc::RES_FINISH;
}


gfl2::proc::Result SaveTestProc::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result procResult = gfl2::proc::RES_CONTINUE;

  if( m_bLoadError )
  {
    procResult = gfl2::proc::RES_FINISH;
  }
  else
  {
    GameSys::GameData* pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

    switch( GetSubSeq() )
    {
      case 0:
      {
        gfl2::heap::HeapBase *pDeviceHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
        GFL_PRINT( "pGameData->SaveStart\n" );
        pGameData->SaveStart( pDeviceHeap );
        AddSubSeq();
      }
      break;

      case 1:
      {
        GFL_PRINT( "pGameData->SaveMain\n" );
        gfl2::fs::FsCtrBackupSystem::Result result = pGameData->SaveMain();
        if( result == gfl2::fs::FsCtrBackupSystem::RESULT_OK )
        {
          GFL_PRINT( "pGameData->SaveMain Success\n" );
          procResult = gfl2::proc::RES_FINISH;
        }
        else
        if( result == gfl2::fs::FsCtrBackupSystem::RESULT_NG_ERROR )
        {
          GFL_PRINT( "pGameData->SaveMain Error\n" );
          procResult = gfl2::proc::RES_FINISH;
        }
      }
      break;
    }
  }

  return procResult;
}

void SaveTestProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{

}



GFL_NAMESPACE_END(SaveTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
