// ============================================================================
/*
 * @file ViewObjects.cpp
 * @brief すごい特訓アプリのViewクラスを持っているクラスです。
 * @date 2015.12.07
 * @author uchida_yuto
 */
// ============================================================================
#include "App/SuperTraining/source/System/SuperTrainingViewObjects.h"
#include "App/SuperTraining/source/SuperTrainingResourceID.h"
#include "App/SuperTraining/source/System/SuperTrainingWork.h"

//  nijoのインクルード
#include "AppLib/include/Tool/AppCommonGrpIconData.h"

//  メッセージのインクルード
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(SuperTraining)
GFL_NAMESPACE_BEGIN(System)

  ViewObjects::ViewObjects()
  {
    _Clear();
  }


  ViewObjects::~ViewObjects()
  {
    GFL_SAFE_DELETE( m_pUpperStatusView );
    GFL_SAFE_DELETE( m_pLowerItemListView );
    GFL_SAFE_DELETE( m_pAppCommonGrpIconData );
  }


  bool ViewObjects::InitFunc( App::SuperTraining::System::SuperTrainingWork* pWork, NetAppLib::UI::NetAppMessageMenu* pMessageMenu )
  {
    GFL_ASSERT( pMessageMenu );

    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    m_pMessageMenu  = pMessageMenu;

    if( m_initSeq == 0 )
    {
      m_pAppCommonGrpIconData = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::AppCommonGrpIconData();
      m_pAppCommonGrpIconData->Initialize( pAppHeap );
      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      if( m_pAppCommonGrpIconData->IsReady() == true )
      {
        ++m_initSeq;
      }
    }
    else
    if( m_initSeq == 2 )
    {
      if( m_pLowerItemListView == NULL )
      {
        m_pLowerItemListView                = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::SuperTraining::View::LowerItemListView( pWork );
      }

      if( m_pUpperStatusView == NULL )
      {
        App::Tool::FieldStatusUpperDraw::CreateParam createParam;
        {
          NetAppLib::System::ResourceManager* pResourceManager    = pWork->GetResourceManager();
          NetAppLib::Message::MessageUtility* pMessageUtility = pWork->GetMessageUtility();

          createParam.heap      = pAppHeap;
          createParam.renderMan = pWork->GetAppRenderingManager();
          createParam.cmnIcon   = m_pAppCommonGrpIconData;
          createParam.msgData   = pMessageUtility->GetData( GARC_message_pokemon_status_DAT );
          createParam.wordSet   = pMessageUtility->GetWordSet();
          createParam.lytBuff   = pResourceManager->GetResourceBuffer( SUPER_TRAINING_RESOURCE_ID_UPPLER_STATUS_LAYOUT );
          createParam.helpFlag  = false;
        }

        m_pUpperStatusView = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Tool::FieldStatusUpperDraw( createParam );
        {
          GFL_ASSERT(m_pUpperStatusView);
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 3 )
    {
      bool  bNext = true;

      if( m_pUpperStatusView->InitFunc() == false )
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

      if( m_pUpperStatusView->EndFunc() == false )
      {
        bNext = false;
      }

      if( bNext == true )
      {
        ++m_endSeq;
      }
    }
    else
    if( m_endSeq == 1 )
    {
      m_pAppCommonGrpIconData->Terminate();
      ++m_endSeq;
    }
    else
    {
      return  true;
    }

    return false;
  }

  void ViewObjects::InputDisableAll()
  {
    m_pLowerItemListView->SetInputEnabled( false );
    m_pUpperStatusView->SetInputEnabled( false );
  }


  bool ViewObjects::IsDrawing()
  {
    bool bRet = true;

    if( m_pLowerItemListView->IsDrawing() == false )
    {
      bRet  = false;
    }
    else
    if( m_pUpperStatusView->IsDrawing() == false )
    {
      bRet  = false;
    }

    return bRet;
  }

  void ViewObjects::_Clear()
  {
    m_initSeq = 0;
    m_endSeq  = 0;

    m_pLowerItemListView  = NULL;
    m_pUpperStatusView    = NULL;
    m_pMessageMenu        = NULL;
    m_pAppCommonGrpIconData = NULL;
  }


GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(SuperTraining)
GFL_NAMESPACE_END(App)
