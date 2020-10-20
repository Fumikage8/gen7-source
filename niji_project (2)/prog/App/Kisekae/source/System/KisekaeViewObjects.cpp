// ============================================================================
/*
 * @file KisekaeViewObjects.cpp
 * @brief きせかえアプリのViewクラスを持っているクラスです。
 * @date  2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Kisekae/source/System/KisekaeViewObjects.h"
#include "App/Kisekae/source/KisekaeResourceID.h"
#include "App/Kisekae/source/System/KisekaeWork.h"

//  Viewのインクルード
#include "App/Kisekae/source/View/KisekaeUpperBGView.h"
#include "App/Kisekae/source/View/KisekaeLowerItemSelectView.h"
#include "App/Kisekae/source/View/KisekaeLowerInfoView.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)
GFL_NAMESPACE_BEGIN(System)

ViewObjects::ViewObjects()
{
  _Clear();
}


ViewObjects::~ViewObjects()
{
  GFL_SAFE_DELETE( m_pUpperBGView );
  GFL_SAFE_DELETE( m_pLowerItemSelectView );
  GFL_SAFE_DELETE( m_pLowerInfoView );
}


bool ViewObjects::InitFunc( App::Kisekae::System::Work* pWork, NetAppLib::UI::NetAppMessageMenu* pMessageMenu )
{
  GFL_ASSERT( pMessageMenu );

  app::util::Heap* pAppHeap = pWork->GetAppHeap();

  m_pMessageMenu  = pMessageMenu;

  if( m_initSeq == 0 )
  {
    if( m_pUpperBGView == NULL )
    {
      m_pUpperBGView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::View::UpperBGView( pWork );
      m_pUpperBGView->SetModal();
    }

    if( m_pLowerItemSelectView == NULL )
    {
      m_pLowerItemSelectView        = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::View::LowerItemSelectView( pWork );
      m_pLowerItemSelectView->SetModal();
    }

    if( m_pLowerInfoView == NULL )
    {
      m_pLowerInfoView              = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Kisekae::View::LowerInfoView( pWork );
      m_pLowerInfoView->SetModal();
    }

    ++m_initSeq;
  }
  else
  if( m_initSeq == 1 )
  {
    bool  bNext = true;
    if( m_pLowerItemSelectView->InitFunc() == false )
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
    if( m_pLowerItemSelectView->EndFunc() == false )
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
  m_pLowerItemSelectView->SetInputEnabled( false );
  m_pLowerInfoView->SetInputEnabled( false );
}


bool ViewObjects::IsDrawing()
{
  bool bRet = false;
  if( m_pUpperBGView->IsDrawing() == true )
  {
    bRet  = true;
  }

  if( m_pLowerItemSelectView->IsDrawing() == true )
  {
    bRet  = true;
  }

  if( m_pLowerInfoView->IsDrawing() == true )
  {
    bRet  = true;
  }

  return bRet;
}

void ViewObjects::_Clear()
{
  m_pUpperBGView          = NULL;
  m_pLowerItemSelectView  = NULL;
  m_pLowerInfoView        = NULL;
  m_pMessageMenu          = NULL;
  m_initSeq = 0;
  m_endSeq  = 0;
}


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)
