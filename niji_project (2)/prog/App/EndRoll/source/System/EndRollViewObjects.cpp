// ============================================================================
/*
 * @file EndRollEndRollViewObjects.cpp
 * @brief エンドロールアプリのViewクラスを持っているクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/System/EndRollViewObjects.h"
#include "App/EndRoll/source/EndRollResourceID.h"
#include "App/EndRoll/source/System/EndRollWork.h"

//  Viewのインクルード

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(System)

  EndRollViewObjects::EndRollViewObjects()
  {
    _Clear();
  }


  EndRollViewObjects::~EndRollViewObjects()
  {
    GFL_SAFE_DELETE( m_pUpperTheEndView );
  }


  bool EndRollViewObjects::InitFunc( App::EndRoll::System::EndRollWork* pWork  )
  {
    GFL_ASSERT( pWork );

    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    if( m_initSeq == 0 )
    {
      //  TheEndのView作成
      {
        m_pUpperTheEndView = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::EndRoll::View::EndRollTheEndView( pWork, App::EndRoll::APPENDROLL_RESOURCE_ID_LAYOUT );
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNext = true;
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

  bool EndRollViewObjects::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNext = true;
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

  void EndRollViewObjects::InputDisableAll()
  {
  }


  bool EndRollViewObjects::IsDrawing()
  {
    bool bRet = false;

    if( m_pUpperTheEndView->IsDrawing() == true )
    {
      bRet  = true;
    }

    return bRet;
  }
  
  void EndRollViewObjects::_Clear()
  {
    m_initSeq = 0;
    m_endSeq  = 0;

    m_pUpperTheEndView      = NULL;
  }

//  ここから先は EndRollViewSystemAccessor クラスの定義
  EndRollViewSystemAccessor::EndRollViewSystemAccessor(
  App::EndRoll::System::EndRollViewList* pViewList,
  App::EndRoll::System::EndRollViewObjects* pViewObjects )
  {
    _Clear();

    GFL_ASSERT( pViewList );
    GFL_ASSERT( pViewObjects );

    m_pViewList     = pViewList;
    m_pViewObjects  = pViewObjects;
  }

  EndRollViewSystemAccessor::~EndRollViewSystemAccessor()
  {
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
