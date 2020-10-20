// ============================================================================
/*
 * @file ViewObjects.cpp
 * @brief バトルツリーアプリのViewクラスを持っているクラスです。
 * @date 2015.12.07
 * @author yuto_uchida
 */
// ============================================================================
#include "App/BattleTree/source/System/BattleTreeViewObjects.h"
#include "App/BattleTree/source/BattleTreeResourceID.h"
#include "App/BattleTree/source/System/BattleTreeWork.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BattleTree)
GFL_NAMESPACE_BEGIN(System)

  ViewObjects::ViewObjects()
  {
    _Clear();
  }


  ViewObjects::~ViewObjects()
  {
    GFL_SAFE_DELETE( m_pUpperBGView );
    GFL_SAFE_DELETE( m_pLowerBGView );
    GFL_SAFE_DELETE( m_pLowerSelectConfirmView );
    GFL_SAFE_DELETE( m_pLowerNPCTrainerListView );
  }


  bool ViewObjects::InitFunc( App::BattleTree::System::BattleTreeWork* pWork )
  {
    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    if( m_initSeq == 0 )
    {
      if( m_pUpperBGView == NULL )
      {
        m_pUpperBGView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::View::UpperBGView( pWork );
      }

      if( m_pLowerBGView == NULL )
      {
        m_pLowerBGView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::View::LowerBGView( pWork );
      }

      if( m_pLowerSelectConfirmView == NULL )
      {
        m_pLowerSelectConfirmView              = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::View::LowerSelectConfirmView( pWork );
      }

      if( m_pLowerNPCTrainerListView == NULL )
      {
        m_pLowerNPCTrainerListView              = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::BattleTree::View::LowerNPCTrainerListView( pWork );
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNext = true;

      if( m_pUpperBGView->InitFunc() == false )
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

  bool ViewObjects::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNext = true;

      if( m_pUpperBGView->EndFunc() == false )
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

  void ViewObjects::InputDisableAll()
  {
    m_pUpperBGView->SetInputEnabled( false );
    m_pLowerBGView->SetInputEnabled( false );
    m_pLowerSelectConfirmView->SetInputEnabled( false );
    m_pLowerNPCTrainerListView->SetInputEnabled( false );
  }


  bool ViewObjects::IsDrawing()
  {
    bool bRet = false;
    if( m_pUpperBGView->IsDrawing() == true )
    {
      bRet  = true;
    }

    if( m_pLowerBGView->IsDrawing() == true )
    {
      bRet  = true;
    }

    if( m_pLowerSelectConfirmView->IsDrawing() == true )
    {
      bRet  = true;
    }

    if( m_pLowerNPCTrainerListView->IsDrawing() == true )
    {
      bRet  = true;
    }

    return bRet;
  }

  void ViewObjects::_Clear()
  {
    m_pUpperBGView              = NULL;
    m_pLowerBGView              = NULL;
    m_pLowerSelectConfirmView   = NULL;
    m_pLowerNPCTrainerListView  = NULL;
    m_initSeq = 0;
    m_endSeq  = 0;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(BattleTree)
GFL_NAMESPACE_END(App)
