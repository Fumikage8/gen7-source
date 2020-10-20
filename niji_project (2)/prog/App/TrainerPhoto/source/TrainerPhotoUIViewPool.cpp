//==============================================================================
/**
 @file    TrainerPhotoUIViewPool.cpp
 @date    2015.09.02
 */
//==============================================================================
#include "App/TrainerPhoto/source/TrainerPhotoUIViewPool.h"

//  nijiのインクルード
#include "System/include/HeapDefine.h"
#include "AppLib/include/Util/app_util_heap.h"
#include "Applib/include/Tool/app_tool_MenuWindow.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_id_photo.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(photo)

  TrainerPhotoUIViewPool::TrainerPhotoUIViewPool( const struct INIT_PARAM& rInitParam )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pUtilHeap );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  TrainerPhotoUIViewPool::~TrainerPhotoUIViewPool()
  {
    GFL_SAFE_DELETE( m_pLowMenuWindowView );
    GFL_SAFE_DELETE( m_pUppShootFrameView );
    GFL_SAFE_DELETE( m_pUppRootView );
    GFL_SAFE_DELETE( m_pLowRootView );
  }

  //------------------------------------------------------------------
  /**
    * @brief  初期化
    * @return     初期化完了: true / 初期化中: false
    */
  //------------------------------------------------------------------
  bool TrainerPhotoUIViewPool::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      //  下画面のView作成
      {
        TrainerPhotoLowRootView::INIT_PARAM initParam;
        initParam.pAppMsgData     = m_initParam.pAppMsgData;
        initParam.pHeap           = m_initParam.pUtilHeap;
        initParam.pLytBinData     = m_initParam.pLytBinData;
        initParam.pRenderManager  = m_initParam.pRenderManager;
        initParam.pWordSet        = m_initParam.pWordSet;

        m_pLowRootView          = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) TrainerPhotoLowRootView( initParam );

        //  汎用メニューウィンドウ
        {
          m_pLowMenuWindowView = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap()) app::tool::MenuWindow( m_initParam.pUtilHeap, m_initParam.pRenderManager, NULL, app::util::G2DUtil::SETUP_LOWER );
        }
      }

      //  上画面のView作成
      {
        {
          TrainerPhotoUppRootView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;

          m_pUppRootView          = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) TrainerPhotoUppRootView( initParam );
        }

        {
          TrainerPhotoUppShootFrameView::INIT_PARAM initParam;
          initParam.pAppMsgData     = m_initParam.pAppMsgData;
          initParam.pHeap           = m_initParam.pUtilHeap;
          initParam.pLytBinData     = m_initParam.pLytBinData;
          initParam.pRenderManager  = m_initParam.pRenderManager;
          initParam.pWordSet        = m_initParam.pWordSet;

          m_pUppShootFrameView    = GFL_NEW( m_initParam.pUtilHeap->GetDeviceHeap() ) TrainerPhotoUppShootFrameView( initParam );
        }
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bRet  = true;
      if( m_pUppRootView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pUppShootFrameView->InitFunc() == false )
      {
        bRet  = false;
      }

      if( m_pLowRootView->InitFunc() == false )
      {
        bRet  = false;
      }

      m_pLowMenuWindowView->UpdateTree();
      if( m_pLowMenuWindowView->IsCreate() == false )
      {
        bRet  = false;
      }

      if( bRet == true )
      {
        //  メニューウィンドウのテキスト設定
        {
          const u32 aTextId[ MENU_WINDWOW_SELECT_MAX ] =
          {
            //  はい1
            msg_id_photo_03_02,
            //  いいえ
            msg_id_photo_03_03
          };

          u32 loopNum = GFL_NELEMS( aTextId );

          gfl2::str::StrBuf* pTempbuf = m_pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_initParam.pAppMsgData->GetString( aTextId[ i ], *pTempbuf );

            if( i == MENU_WINDWOW_SELECT_YES )
            {
              m_pLowMenuWindowView->AddItem( *pTempbuf, false, false, SEQ_SE_COMPLETE1 );
            }
            else
            {
              m_pLowMenuWindowView->AddItem( *pTempbuf, true );
            }
          }
        }

        ++m_initSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief  終了
    * @return 終了完了: true / 終了中: false
    */
  //------------------------------------------------------------------
  bool TrainerPhotoUIViewPool::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool  bRet  = true;

      if( m_pLowMenuWindowView != NULL )
      {
        if( m_pLowMenuWindowView->IsModuleFree() == false )
        {
          bRet  = false;
        }
      }

      if( m_pLowRootView != NULL )
      {
        if( m_pLowRootView->EndFunc() == false )
        {
          bRet  = false;
        }
      }

      if( m_pUppRootView != NULL )
      {
        if( m_pUppRootView->EndFunc() == false )
        {
          bRet  = false;
        }
      }

      if( m_pUppShootFrameView != NULL )
      {
        if( m_pUppShootFrameView->EndFunc() == false )
        {
          bRet  = false;
        }
      }

      if( bRet == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  true;
    }

    return  false;
  }

  //  シャット開始
  void TrainerPhotoUIViewPool::DoEventShutter()
  {
    m_pUppShootFrameView->DoEventShutter();
    m_pUppRootView->PlayAnime( TrainerPhotoUppRootView::ANIME_SHUTTER );
  }

  //  シャッター中か
  bool TrainerPhotoUIViewPool::IsEventShutter()
  {    
    if( m_pUppShootFrameView->IsEventShutter() == true )
    {
      return  true;
    }

    if( m_pUppRootView->IsPlayAnime( TrainerPhotoUppRootView::ANIME_SHUTTER ) == true )
    {
      return  true;
    }

    //  シャッターが終っている
    return  false;
  }

GFL_NAMESPACE_END(photo)
GFL_NAMESPACE_END(app)
