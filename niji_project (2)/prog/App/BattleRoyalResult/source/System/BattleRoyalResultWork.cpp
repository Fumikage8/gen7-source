// ============================================================================
/*
 * @file BattleRoyalResultWork.cpp
 * @brief バトルロイヤルの結果アプリのワーククラスです。
 * @date 2015.10.23
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)


  BattleRoyalResultWork::BattleRoyalResultWork( App::BattleRoyalResult::APP_PARAM* pAppParam ) :
    ApplicationWorkBase( APP_HEAP_SIZE, APP_DEVICE_HEAP_SIZE )
  {
    _Clear();

    GFL_ASSERT( pAppParam );

    app::util::Heap*  pAppHeap = ApplicationWorkBase::GetAppHeap();

    m_pAppParam = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::APP_PARAM();

    //  中身のコピー
    *m_pAppParam = *pAppParam;

    //  文字列バッファを作成
    {
      u32 loopNum = GFL_NELEMS( pAppParam->aTrainerData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( pAppParam->aTrainerData[ i ].pNameBuf != NULL )
        {
          m_pAppParam->aTrainerData[ i ].pNameBuf = GFL_NEW( pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( *pAppParam->aTrainerData[ i ].pNameBuf, pAppHeap->GetDeviceHeap() );
        }
        else
        {
          m_pAppParam->aTrainerData[ i ].pNameBuf = NULL;
        }
      }
    }
  }


  BattleRoyalResultWork::~BattleRoyalResultWork()
  {
    //  文字列バッファを開放
    {
      u32 loopNum = GFL_NELEMS( m_pAppParam->aTrainerData );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_pAppParam->aTrainerData[ i ].pNameBuf );
      }
    }

    GFL_SAFE_DELETE( m_pAppParam );
  }

  //  アプリパラメータ取得
  App::BattleRoyalResult::APP_PARAM*  BattleRoyalResultWork::GetAppParam()
  {
    return  m_pAppParam;
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
