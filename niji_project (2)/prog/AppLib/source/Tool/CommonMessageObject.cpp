//======================================================================
/**
 * @file CommonMessageObject.cpp
 * @date 2015/10/09 14:48:36
 * @author uchida_yuto
 * @brief 共通ウィンドウオブジェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "AppLib/include/Tool/CommonMessageObject.h"

//  nijiのインクルード
#include "AppLib/include/Util/app_util_2d.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  CommonMessageObject::CommonMessageObject()
  {
    _Clear();
  }

  //  初期化
  bool CommonMessageObject::InitFunc( const INIT_PARAM& rParam, gfl2::heap::HeapBase* pHeap )
  {
    m_initParam = rParam;

    if( pHeap == NULL )
    {
      GFL_ASSERT( 0 );
      return  true;
    }

    if( m_initParam.textBoxPaneNum <= 0 )
    {
      GFL_ASSERT( 0 );
      return  true;
    }
    else
    if( TEXT_BOX_PANE_MAX < m_initParam.textBoxPaneNum )
    {
      GFL_ASSERT( 0 );
      return  true;
    }

    gfl2::lyt::LytWk* pLayoutWork = m_initParam.pG2DUtil->GetLayoutWork( m_initParam.lytId );
    if( pLayoutWork == NULL )
    {
      GFL_ASSERT( 0 );
      return  true;
    }

    gfl2::ui::DeviceManager * man =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
    if( man == NULL )
    {
      GFL_ASSERT( 0 );
      return  true;
    }

    m_pMsgWin = GFL_NEW( pHeap ) print::MessageWindow( man, pLayoutWork, m_initParam.textBoxPaneNum, m_initParam.showWaitFrame, pHeap );

    for( u32 i = 0; i < m_initParam.textBoxPaneNum; ++i )
    {
      GFL_ASSERT( m_initParam.pTextBoxPaneArray[ i ] );
      m_pMsgWin->AddLinePaneIdxPtr( m_initParam.pTextBoxPaneArray[ i ] );
    }

    {
      //  アウトアニメの最終フレーム値を反映
      if( m_initParam.outAnimeId == gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR )
      {
      }
      else
      {
        m_initParam.pG2DUtil->BindAnime( m_initParam.lytId, m_initParam.outAnimeId );
        m_initParam.pG2DUtil->StartAnimeFrame( m_initParam.lytId, m_initParam.outAnimeId, m_initParam.pG2DUtil->GetAnimeMaxFrame( m_initParam.lytId, m_initParam.outAnimeId ) );
      }

      //  最初は非表示
      m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pWindowPane, false );
    }

    return  true;
  }

  //  終了
  bool CommonMessageObject::EndFunc()
  {
    if( m_pMsgWin != NULL )
    {
      m_pMsgWin->Clear();
    }
    GFL_SAFE_DELETE( m_pMsgWin );

    return  true;
  }

  //  更新
  void CommonMessageObject::Update()
  {
    if( m_bShow == true )
    {
      m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pWindowPane, m_bShow );
    }

    if( m_bMsgProc == true )
    {
      if( m_bIsImmediateMessage == false )
      {
        m_pMsgWin->Proc();
      }
      else
      {
        m_pMsgWin->ProcAtOnce();
      }
    }
  }

  //  表示
  void CommonMessageObject::ShowMessage( gfl2::str::StrBuf& rStr, const bool bIsImmediateMessage )
  {
    if( m_pMsgWin == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_pMsgWin->SetString( rStr );

    _InAnime();

    m_bIsImmediateMessage = bIsImmediateMessage;
    m_bMsgProc  = true;
  }

  //  ペインに直接設定
  //  INIT_PARAM::textBoxPnaeNum < listNum でないと動作しない
  void CommonMessageObject::ShowMessageTextPane( gfl2::str::StrBuf** pStrArray, const u32 listNum )
  {
    if( m_initParam.textBoxPaneNum < listNum )
    {
      GFL_ASSERT( 0 );
      return;
    }

    //  空のテキスト設定
    for( u32 i = 0; i < m_initParam.textBoxPaneNum; ++i )
    {
      m_initParam.pG2DUtil->SetTextBoxPaneString( m_initParam.pTextBoxPaneArray[ i ], L"" );
    }

    //  テキスト配列分設定
    for( u32 i = 0; i < listNum; ++i )
    {
      GFL_ASSERT( pStrArray[ i ] );
      m_initParam.pG2DUtil->SetTextBoxPaneString( m_initParam.pTextBoxPaneArray[ i ], pStrArray[ i ] );
      m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pTextBoxPaneArray[ i ], true );
    }

    m_bMsgProc  = false;

    _InAnime();
  }

  //  非表示
  void CommonMessageObject::HideMessage()
  {
    _OutAnime();
  }

  void CommonMessageObject::_InAnime()
  {
    if( m_bShow == false )
    {
      //  アニメ再生
      if( m_initParam.inAnimeId == gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR )
      {
        //  アニメがなければペインの表示設定
        m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pWindowPane, true );
      }
      else
      {
        // @fix GFNMCat[799] すでにOutアニメが再生しているとOutアニメ結果も反映されるので、ここでとめる
        m_initParam.pG2DUtil->StopAnime( m_initParam.lytId, m_initParam.outAnimeId );

        m_initParam.pG2DUtil->StopAnime( m_initParam.lytId, m_initParam.inAnimeId );
        m_initParam.pG2DUtil->StartAnime( m_initParam.lytId, m_initParam.inAnimeId );
      }

      m_bShow = true;
    }
  }

  void CommonMessageObject::_OutAnime()
  {
    if( m_bShow == true )
    {
      //  アニメ再生
      if( m_initParam.outAnimeId == gfl2::lyt::LYTWKBASE_ANIMEIDX_ERROR )
      {
        //  アニメがなければペインの非表示設定
        m_initParam.pG2DUtil->SetPaneVisible( m_initParam.pWindowPane, false );
      }
      else
      {
        // @fix GFNMCat[799] すでにInアニメが再生しているとInアニメ結果も反映されるので、ここでとめる
        m_initParam.pG2DUtil->StopAnime( m_initParam.lytId, m_initParam.inAnimeId );

        m_initParam.pG2DUtil->StopAnime( m_initParam.lytId, m_initParam.outAnimeId );
        m_initParam.pG2DUtil->StartAnime( m_initParam.lytId, m_initParam.outAnimeId );
      }

      m_bShow = false;
    }
  }

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
