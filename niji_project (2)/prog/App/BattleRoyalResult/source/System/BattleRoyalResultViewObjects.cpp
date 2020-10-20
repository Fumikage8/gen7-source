// ============================================================================
/*
 * @file BattleRoyalResultBattleRoyalResultViewObjects.cpp
 * @brief バトルロイヤル勝敗アプリのViewクラスを持っているクラスです。
 * @date 2015.10.26
 * @author uchida_yuto
 */
// ============================================================================
#include "App/BattleRoyalResult/source/System/BattleRoyalResultViewObjects.h"
#include "App/BattleRoyalResult/source/BattleRoyalResultResourceID.h"
#include "App/BattleRoyalResult/source/System/BattleRoyalResultWork.h"

//  Viewのインクルード
#include "App/BattleRoyalResult/source/View/BattleRoyalResultLowerBGView.h"
#include "App/BattleRoyalResult/source/View/BattleRoyalResultUpperRankingView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleRoyalResult)
GFL_NAMESPACE_BEGIN(System)

  BattleRoyalResultViewObjects::BattleRoyalResultViewObjects()
  {
    _Clear();
  }


  BattleRoyalResultViewObjects::~BattleRoyalResultViewObjects()
  {
    GFL_SAFE_DELETE( m_pUpperRankingView );
    GFL_SAFE_DELETE( m_pLowerBGView );
  }


  bool BattleRoyalResultViewObjects::InitFunc( App::BattleRoyalResult::System::BattleRoyalResultWork* pWork  )
  {
    GFL_ASSERT( pWork );

    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    if( m_initSeq == 0 )
    {
      if( m_pLowerBGView == NULL )
      {
        m_pLowerBGView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::View::BattleRoyalResultLowerBGView( pWork );
        m_pLowerBGView->SetModal();
      }

      if( m_pUpperRankingView == NULL )
      {
        m_pUpperRankingView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleRoyalResult::View::BattleRoyalResultUpperRankingView( pWork );
        m_pUpperRankingView->SetModal();
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNext = true;
      if( m_pUpperRankingView->InitFunc() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }

  bool BattleRoyalResultViewObjects::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNext = true;
      if( m_pUpperRankingView->EndFunc() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return false;
  }

  void BattleRoyalResultViewObjects::InputDisableAll()
  {
    m_pLowerBGView->SetInputEnabled( false );
    m_pUpperRankingView->SetInputEnabled( false );
  }


  bool BattleRoyalResultViewObjects::IsDrawing()
  {
    bool bRet = false;
    if( m_pLowerBGView->IsDrawing() == true )
    {
      bRet  = true;
    }

    if( m_pUpperRankingView->IsDrawing() == true )
    {
      bRet  = true;
    }

    return bRet;
  }
  
  void BattleRoyalResultViewObjects::_Clear()
  {
    m_pLowerBGView          = NULL;
    m_pUpperRankingView     = NULL;
    m_initSeq = 0;
    m_endSeq  = 0;
  }

//  ここから先は BattleRoyalResultViewSystemAccessor クラスの定義
  BattleRoyalResultViewSystemAccessor::BattleRoyalResultViewSystemAccessor(
  App::BattleRoyalResult::System::BattleRoyalResultViewList* pViewList,
  App::BattleRoyalResult::System::BattleRoyalResultViewObjects* pViewObjects )
  {
    _Clear();

    GFL_ASSERT( pViewList );
    GFL_ASSERT( pViewObjects );

    m_pViewList     = pViewList;
    m_pViewObjects  = pViewObjects;
  }

  BattleRoyalResultViewSystemAccessor::~BattleRoyalResultViewSystemAccessor()
  {
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleRoyalResult)
GFL_NAMESPACE_END(App)
