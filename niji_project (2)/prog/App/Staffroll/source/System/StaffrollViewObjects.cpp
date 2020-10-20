// ============================================================================
/*
 * @file StaffrollStaffrollViewObjects.cpp
 * @brief スタッフロールアプリのViewクラスを持っているクラスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/System/StaffrollViewObjects.h"
#include "App/Staffroll/source/StaffrollResourceID.h"
#include "App/Staffroll/source/System/StaffrollWork.h"

//  Viewのインクルード

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(System)

  StaffrollViewObjects::StaffrollViewObjects()
  {
    _Clear();
  }


  StaffrollViewObjects::~StaffrollViewObjects()
  {
    GFL_SAFE_DELETE( m_pUpperTextScrollView );
    GFL_SAFE_DELETE( m_pUpperImgView );
  }


  bool StaffrollViewObjects::InitFunc( App::Staffroll::System::StaffrollWork* pWork  )
  {
    GFL_ASSERT( pWork );

    app::util::Heap* pAppHeap = pWork->GetAppHeap();

    if( m_initSeq == 0 )
    {
      //  テキストスクロールの上下View作成
      {
        m_pUpperTextScrollView  = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::View::StaffrollTextScrollView( 
        pWork, 
        App::Staffroll::STAFFROLL_RESOURCE_ID_LAYOUT,
        app::util::G2DUtil::SETUP_UPPER, 
//        DISPLAY_UPPER_HEIGHT / 2, 
        DISPLAY_UPPER_HEIGHT / 2 + DISPLAY_UPPER_HEIGHT, 
        -DISPLAY_UPPER_HEIGHT / 2 );
      }

      //  一枚絵のView作成
      {
        m_pUpperImgView = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Staffroll::View::StaffrollImgView( pWork, App::Staffroll::STAFFROLL_RESOURCE_ID_LAYOUT );
        m_pUpperImgView->SetVisible( false );
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNext = true;
/*
      if( m_pUpperImgView->InitFunc() == false )
      {
        bNext = false;
      }
*/

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

  bool StaffrollViewObjects::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bNext = true;
/*
      if( m_pUpperImgView->EndFunc() == false )
      {
        bNext = false;
      }
*/

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

  void StaffrollViewObjects::InputDisableAll()
  {
  }


  bool StaffrollViewObjects::IsDrawing()
  {
    bool bRet = false;

    if( m_pUpperTextScrollView->IsDrawing() == true )
    {
      bRet  = true;
    }

    if( m_pUpperImgView->IsDrawing() == true )
    {
      bRet  = true;
    }

    return bRet;
  }
  
  void StaffrollViewObjects::_Clear()
  {
    m_initSeq = 0;
    m_endSeq  = 0;

    m_pUpperTextScrollView  = NULL;
    m_pUpperImgView         = NULL;
  }

//  ここから先は StaffrollViewSystemAccessor クラスの定義
  StaffrollViewSystemAccessor::StaffrollViewSystemAccessor(
  App::Staffroll::System::StaffrollViewList* pViewList,
  App::Staffroll::System::StaffrollViewObjects* pViewObjects )
  {
    _Clear();

    GFL_ASSERT( pViewList );
    GFL_ASSERT( pViewObjects );

    m_pViewList     = pViewList;
    m_pViewObjects  = pViewObjects;
  }

  StaffrollViewSystemAccessor::~StaffrollViewSystemAccessor()
  {
  }

GFL_NAMESPACE_END(System)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
