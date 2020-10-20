//======================================================================
/**
 * @file BattleVideoPlayerUppRootView.h
 * @date 2015/09/24 16:23:10
 * @author uchida_yuto
 * @brief アプリの上画面ルートView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "NetApp/BattleVideoPlayer/source/View/BattleVideoPlayerUppRootView.h"

//  nijiのインクルード
#include "AppLib/include/util/app_util_GetPaneHelper.h"
#include "AppLib/include/Tool/CommonMessageObject.h"

//  レイアウトリソースのインクルード
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_anim_list.h"
#include "niji_conv_header/app/battle_video_player/BattleVideoPlayerLytData_pane.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(BattleVideoPlayer)

  UppRootView::UppRootView( INIT_PARAM& rInitParam ) :
  UIView( rInitParam.pHeap, rInitParam.pHeap->GetDeviceAllocator(), rInitParam.pHeap->GetDeviceAllocator() )
  {
    _Clear();

    GFL_ASSERT( rInitParam.pRenderManager );
    GFL_ASSERT( rInitParam.pLytBinData );
    GFL_ASSERT( rInitParam.pAppMsgData );
    GFL_ASSERT( rInitParam.pWordSet );

    m_initParam = rInitParam;
  }

  UppRootView::~UppRootView()
  {
    GFL_SAFE_DELETE( m_pOneLineCommonMessageObject );
    GFL_SAFE_DELETE( m_pTwoLineCommonMessageObject );
    GFL_SAFE_DELETE( m_pCenterCommonMessageObject );

    UIView::ReleaseButtonManager();
    UIView::Delete2D();
  }

  //------------------------------------------------------------------
  /**
    * @brief   初期化
    */
  //------------------------------------------------------------------
  bool UppRootView::InitFunc()
  {
    if( m_initSeq == 0 )
    {
      _Initialize2D( m_initParam.pLytBinData, m_initParam.pAppMsgData, m_initParam.pWordSet );

      m_pTwoLineCommonMessageObject = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::CommonMessageObject();
      m_pOneLineCommonMessageObject = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::CommonMessageObject();
      m_pCenterCommonMessageObject  = GFL_NEW( m_initParam.pHeap->GetDeviceHeap() ) app::tool::CommonMessageObject();

      //  BGのループアニメ
      {
        ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
        pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000__BG_WAVE_BLUE_UPP_00_BG_LOOP_00 );
      }

      ++m_initSeq;
    }
    else
    if( m_initSeq == 1 )
    {
      bool  bNextSeq  = true;

      gfl2::lyt::LytWk* pLayoutWork         = UIView::GetLayoutWork( LYT_WORK_NORMAL );
      ::app::util::G2DUtil* pG2DUtil        = UIView::GetG2DUtil();
      gfl2::lyt::LytMultiResID& rMultiResId = pG2DUtil->GetLayoutResourceID( 0 );

      //  上側のメッセージペイン取得
      {
        //  ２行用
        {
          app::util::GetPaneHelper  partsPaneHelper( pLayoutWork, &rMultiResId );
          partsPaneHelper.Push( PANENAME_BVP_MIN_UPP_000_PANE_PARTS_02 );

          app::tool::CommonMessageObject::INIT_PARAM  initParam;
          {
            initParam.lytId         = LYT_WORK_NORMAL;
            initParam.pG2DUtil      = pG2DUtil;
            initParam.inAnimeId     = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000__PARTS_02_IN;
            initParam.outAnimeId    = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000__PARTS_02_OUT;
            initParam.pWindowPane   = partsPaneHelper.Peek();
            initParam.showWaitFrame = 0;

            //  テキストペイン設定
            {
              initParam.pTextBoxPaneArray[ 0 ]  = partsPaneHelper.GetTextBoxPane( PANENAME_BVP_WIN_LOW_000_PANE_TEXTBOX_01 );
              initParam.pTextBoxPaneArray[ 1 ]  = partsPaneHelper.GetTextBoxPane( PANENAME_BVP_WIN_LOW_000_PANE_TEXTBOX_02 );
              initParam.textBoxPaneNum  = 2;
            }

            //  大本のテキスト表示のペインは表示しておく
            partsPaneHelper.GetPane( PANENAME_BVP_WIN_LOW_000_PANE_NULL_TEXT1 )->SetVisible ( true );
          }

          if( m_pTwoLineCommonMessageObject->InitFunc( initParam, m_initParam.pHeap->GetDeviceHeap() ) == false )
          {
            bNextSeq  = false;
          }
        }

        //  一行用
        {
          app::util::GetPaneHelper  partsPaneHelper( pLayoutWork, &rMultiResId );
          partsPaneHelper.Push( PANENAME_BVP_MIN_UPP_000_PANE_PARTS_01 );

          app::tool::CommonMessageObject::INIT_PARAM  initParam;
          {
            initParam.lytId         = LYT_WORK_NORMAL;
            initParam.pG2DUtil      = pG2DUtil;
            initParam.inAnimeId     = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000__PARTS_01_IN;
            initParam.outAnimeId    = LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000__PARTS_01_OUT;
            initParam.pWindowPane   = partsPaneHelper.Peek();
            initParam.showWaitFrame = 0;

            //  テキストペイン設定
            {
              initParam.pTextBoxPaneArray[ 0 ]  = partsPaneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00 );
              initParam.textBoxPaneNum  = 1;
            }
          }

          if( m_pOneLineCommonMessageObject->InitFunc( initParam, m_initParam.pHeap->GetDeviceHeap() ) == false )
          {
            bNextSeq  = false;
          }
        }
      }

      //  真ん中のメッセージペイン取得
      {
        app::util::GetPaneHelper  partsPaneHelper( pLayoutWork, &rMultiResId );
        partsPaneHelper.Push( PANENAME_BVP_MIN_UPP_000_PANE_PARTS_00 );

        app::tool::CommonMessageObject::INIT_PARAM  initParam;
        {
          initParam.lytId         = LYT_WORK_NORMAL;
          initParam.pG2DUtil      = pG2DUtil;
          initParam.inAnimeId     = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
          initParam.outAnimeId    = gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR;
          initParam.pWindowPane   = pLayoutWork->GetPane( PANENAME_BVP_MIN_UPP_000_PANE_WINDOW_L );
          initParam.showWaitFrame = 0;

          //  テキストペイン設定
          {
            initParam.pTextBoxPaneArray[ 0 ]  = partsPaneHelper.GetTextBoxPane( PANENAME_COMMON_MSG_UPP_004_PANE_TEXTBOX_00 );
            initParam.textBoxPaneNum  = 1;
          }
        }

        if( m_pCenterCommonMessageObject->InitFunc( initParam, m_initParam.pHeap->GetDeviceHeap() ) == false )
        {
          bNextSeq  = false;
        }
      }

      if( bNextSeq == true )
      {
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
    * @brief   終了処理
    */
  //------------------------------------------------------------------
  bool UppRootView::EndFunc()
  {
    if( m_endSeq == 0 )
    {
      bool bNextSeq = true;
      if( m_pTwoLineCommonMessageObject->EndFunc() == false )
      {
        bNextSeq  = false;
      }

      if( m_pOneLineCommonMessageObject->EndFunc() == false )
      {
        bNextSeq  = false;
      }

      if( m_pCenterCommonMessageObject->EndFunc() == false )
      {
        bNextSeq  = false;
      }

      if( bNextSeq == true )
      {
        ++m_endSeq;
      }
    }
    else
    {
      return  ( UIView::IsDrawing() == false );
    }

    return  false;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  void UppRootView::Update()
  {
    if( m_state == STATE_IDLE )
    {
    }

    m_pTwoLineCommonMessageObject->Update();
    m_pOneLineCommonMessageObject->Update();
    m_pCenterCommonMessageObject->Update();

    UIView::Update();
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  void UppRootView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( ( displayNo == gfl2::gfx::CtrDisplayNo::LEFT )
     || ( displayNo == gfl2::gfx::CtrDisplayNo::RIGHT ) )
    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      for( s32 i = 0; i < LYT_WORK_MAX; ++i )
      {
        pG2DUtil->AddDrawableLytWkForOneFrame( m_initParam.pRenderManager, displayNo, i );
      }
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   上タイトルメッセージ表示(1行用)
    */
  //------------------------------------------------------------------
  void UppRootView::ShowMessageUpperOneLine( gfl2::str::StrBuf* pStrBuf )
  {
    _HideMessageUpperTwoLine();

    gfl2::str::StrBuf*  pStrBufArray[] = 
    {
      pStrBuf
    };

    m_pOneLineCommonMessageObject->ShowMessageTextPane( pStrBufArray, GFL_NELEMS( pStrBufArray ) );

    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      //  1行用に画面変更
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000_8WIN_01 );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   上タイトルメッセージ表示(2行用)
    */
  //------------------------------------------------------------------
  void UppRootView::ShowMessageUpperTwoLine( gfl2::str::StrBuf* pStrBufOneLine, gfl2::str::StrBuf* pStrBufTwoLine )
  {
    _HideMessageUpperOneLine();

    gfl2::str::StrBuf*  pStrBufArray[] = 
    {
      pStrBufOneLine,
      pStrBufTwoLine
    };

    m_pTwoLineCommonMessageObject->ShowMessageTextPane( pStrBufArray, GFL_NELEMS( pStrBufArray ) );

    {
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

      //  2行用に画面変更
      pG2DUtil->StartAnime( LYT_WORK_NORMAL, LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000_8WIN_02 );
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   上タイトルメッセージ非表示
    */
  //------------------------------------------------------------------
  void UppRootView::HideMessageUpper()
  {
    _HideMessageUpperOneLine();
    _HideMessageUpperTwoLine();
  }

  //------------------------------------------------------------------
  /**
    * @brief   中央メッセージ表示
    */
  //------------------------------------------------------------------
  void UppRootView::ShowMessageCenter( gfl2::str::StrBuf* pStrBuf )
  {
    gfl2::str::StrBuf*  pStrBufArray[] = 
    {
      pStrBuf
    };

    m_pCenterCommonMessageObject->ShowMessageTextPane( pStrBufArray, GFL_NELEMS( pStrBufArray ) );
  }

  //------------------------------------------------------------------
  /**
    * @brief   中央メッセージ非表示
    */
  //------------------------------------------------------------------
  void UppRootView::HideMessageCenter()
  {
    m_pCenterCommonMessageObject->HideMessage();
  }

  //------------------------------------------------------------------
  /**
    * @brief   状態設定
    * @param[in] newStaet 設定する状態
    */
  //------------------------------------------------------------------
  void UppRootView::SetState( const State newStaet )
  {
    m_state = newStaet;

    if( m_state == STATE_IDLE )
    {
    }
  }

  //------------------------------------------------------------------
  /**
    * @brief   2D画面の作成
    * @param   pAppLytBuff レイアウトデータのバイナリポインタ
    */
  //------------------------------------------------------------------
  void UppRootView::_Initialize2D( void* pAppLytBuff, gfl2::str::MsgData* pAppMsgData, print::WordSet* pWordSet )
  {
    // アーカイブIDテーブル
    const ::app::util::G2DUtil::LytResData aResTbl[] =
    {
      {  pAppLytBuff, 16, app::util::G2DUtil::ATTACH_NONE },
    };

    //  作成するレイアウトワーク
    const ::app::util::G2DUtil::LytwkData aSetupData[ LYT_WORK_MAX ] =
    {
      {
        0,
        LYTRES_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000_BFLYT,
        LA_BATTLEVIDEOPLAYERLYTDATA_BVP_MIN_UPP_000___NUM,
        pAppLytBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      }
    };

    UIView::Create2D( m_initParam.pHeap, NULL, 8, aResTbl, GFL_NELEMS(aResTbl), aSetupData, GFL_NELEMS(aSetupData), pAppMsgData, pWordSet );
  }

  //------------------------------------------------------------------
  /**
    * @brief   上タイトルメッセージ非表示(1行)
    */
  //------------------------------------------------------------------
  void UppRootView::_HideMessageUpperOneLine()
  {
    m_pOneLineCommonMessageObject->HideMessage();
  }

  //------------------------------------------------------------------
  /**
    * @brief   上タイトルメッセージ非表示(2行)
    */
  //------------------------------------------------------------------
  void UppRootView::_HideMessageUpperTwoLine()
  {
    m_pTwoLineCommonMessageObject->HideMessage();
  }

GFL_NAMESPACE_END(BattleVideoPlayer)
GFL_NAMESPACE_END(NetApp)
