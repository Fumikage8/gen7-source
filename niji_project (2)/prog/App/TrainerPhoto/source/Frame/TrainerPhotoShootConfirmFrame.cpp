//=============================================================================
/**
 * @file	 TrainerPhotoShootConfirmFrame.h
 * @brief	 証明写真撮影確認フレーム
 * @author yuto_uchida
 * @date	 2015.09.01
 */
//=============================================================================
#pragma once

#include "App/TrainerPhoto/source/frame/TrainerPhotoShootConfirmFrame.h"

//  gfl2のインクルード
#include <model/include/gfl2_BaseCamera.h>

//  nijiにインクルード
#include "App/TrainerPhoto/source/TrainerPhotoUIViewPool.h"
#include "Applib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/AppToolCharaSimpleModel.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"
#include "Savedata/include/Record.h"

//  リソースのインクルード
// 動的ヘッダシステムを利用するときは以下のヘッダをインクルードする
#include <debug/include/DynamicHeader/gfl2_DebugDynamicHeader.h>

//  アプリ全体で使う動的パラメータ
#include "niji_conv_header/app/id_photo/TrainerPhotoModelDynamic.h"

//  モデルビューアの動的パラメータ
#include "niji_conv_header/app/dynamicHeader/AppModelViewrDynamic.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_id_photo.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( photo )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  TrainerPhotoShootConfirmFrame::TrainerPhotoShootConfirmFrame()
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  TrainerPhotoShootConfirmFrame::~TrainerPhotoShootConfirmFrame()
  {
  }

  //------------------------------------------------------------------
  /**
    * @brief	  動作に必要な引数を登録
    * @note
    *    必ずインスタンス生成の段階で呼び出さないと後の動作保障はなし
    */
  //------------------------------------------------------------------
  void TrainerPhotoShootConfirmFrame::Initialize( FRAME_PARAM* pFrameParam )
  {
    GFL_ASSERT( pFrameParam );

    m_pFrameParam = pFrameParam;
    m_pFrameParam->out.Reset();
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoShootConfirmFrame::InitFunc()
  {
    enum
    {
      PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_START  = 0,
      PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_WAIT,
      PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_END
    };

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_START )
    {
      //  モデルは非表示に
      m_pFrameParam->in.pCharaModel->SetVisible( false );

      TrainerPhotoUIViewPool*   pUIViewPool         = m_pFrameParam->in.pUIViewPool;
      TrainerPhotoUppRootView*  pUIViewUppRootView  = pUIViewPool->GetRootUppView();

      //  UIViewの状態を初期化
      {
        //  下
        {
          TrainerPhotoLowRootView*  pLowRootView  = pUIViewPool->GetRootLowView();
          pLowRootView->RemoveAllSubView();

          // @fix NMCat[346] 下画面の「しゃしんをとる」などは押せないようにする
          pLowRootView->SetState( TrainerPhotoLowRootView::STATE_INPUT_STOP );
          pLowRootView->SetInputDelegate( NULL );
        }

        //  上
        {
          pUIViewUppRootView->SetState( TrainerPhotoUppRootView::STATE_IDLE );

          pUIViewUppRootView->RemoveAllSubView();

          //  トレーナーアイコン非表示
          pUIViewUppRootView->GetG2DUtil()->SetPaneVisible( pUIViewUppRootView->GetTrainerIconRootPane(), false );
        }
      }
      ++seq;
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_WAIT )
    {
      //  すでに先読みしているのでロードを待つ
      if( m_pFrameParam->in.pTrainerIconRender->IsLoadedObject( 0 ) == true )
      {
        //  ロードが終ってアイコンが表示
        TrainerPhotoUIViewPool*   pUIViewPool         = m_pFrameParam->in.pUIViewPool;

        TrainerPhotoUppRootView*  pUppRootView  = pUIViewPool->GetRootUppView();

        //  トレーナーアイコン非表示
        pUppRootView->GetG2DUtil()->SetPaneVisible( pUppRootView->GetTrainerIconRootPane(), true );
        pUppRootView->PlayAnime( TrainerPhotoUppRootView::ANIME_ICON_IN );

        ++seq;
      }
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_SEQ_INIT_FUNC_END )
    {
      return applib::frame::RES_FINISH;
    }

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoShootConfirmFrame::UpdateFunc()
  {
    enum
    {
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_ICON_ANIME  = 0,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_SHOW_SAVE_SELECT_MESSAGE,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_SAVE_SELECT_MESSAGE,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_OPEN_MENU_WINDOW,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_MENU_WINDOW,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_SHOW_SAVE_SUCCESS_MESSAGE,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_SAVE_SUCCESS_MESSAGE,
      PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_END
    };

    TrainerPhotoUIViewPool* pUIViewPool         = m_pFrameParam->in.pUIViewPool;
    TrainerPhotoLowRootView*  pLowRootView      = pUIViewPool->GetRootLowView();
    TrainerPhotoUppRootView*  pUppRootView  = pUIViewPool->GetRootUppView();
    app::tool::MenuWindow*  pMenuWindowLowView  = pUIViewPool->GetMenuWindowLowView();

    u32 seq = applib::frame::CellSuper::GetSubSeq();

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_ICON_ANIME )
    {
      if( pUppRootView->IsPlayAnime( TrainerPhotoUppRootView::ANIME_ICON_IN ) == false )
      {
        ++seq;
      }
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_SHOW_SAVE_SELECT_MESSAGE )
    {
      gfl2::str::StrBuf*  pTempStrBuf  = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
      pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_03_01, *pTempStrBuf );
      pLowRootView->ShowMessageDialog( pTempStrBuf, GFL_VALUE_DEFINE_IN_HEADER( PHOTO_MESSAGE_DIALOG_SHOW_FRAME ) );

      ++seq;
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_SAVE_SELECT_MESSAGE )
    {
      if( pLowRootView->IsEndUpdateMessageDialog() == true )
      {
        ++seq;
      }
    }

    //  メニューウィンドウを開く
    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_OPEN_MENU_WINDOW )
    {
      m_bEndMenuWindow  = false;

      pMenuWindowLowView->StartYesNoMenu( TrainerPhotoUIViewPool::MENU_WINDWOW_SELECT_YES );
      pMenuWindowLowView->SetListener( this );

      ++seq;
    }
    
    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_MENU_WINDOW )
    {
      if( m_bEndMenuWindow == true )
      {
        if( m_pFrameParam->out.pushBtn == PUSH_BTN_CANCEL )
        {
          //  保存をキャンセルした場合はすぐに終了
          seq = PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_END;
        }
        else
        {
          ++seq;
        }
      }
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_SHOW_SAVE_SUCCESS_MESSAGE )
    {
      //  メッセージを変える
      gfl2::str::StrBuf*  pTempStrBuf  = pLowRootView->GetG2DUtil()->GetTempStrBuf( 0 );
      pUIViewPool->GetInitParam().pAppMsgData->GetString( msg_id_photo_03_04, *pTempStrBuf );

      pLowRootView->ShowMessageDialog( pTempStrBuf, GFL_VALUE_DEFINE_IN_HEADER( PHOTO_MESSAGE_DIALOG_SHOW_FRAME ) );

      //  写真を取り直した
      Savedata::IncRecord( Savedata::Record::RECID_FINDER );

      ++seq;
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_WAIT_SAVE_SUCCESS_MESSAGE )
    {
      if( pLowRootView->IsEndUpdateMessageDialog() == true )
      {
        ++seq;
      }
    }

    if( seq == PHOTO_SHOOT_CONFIRM_FRAME_UPDATE_SEQ_END )
    {
      return applib::frame::RES_FINISH;
    }

    pMenuWindowLowView->UpdateTree();

    applib::frame::CellSuper::SetSubSeq( seq );

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void TrainerPhotoShootConfirmFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
    TrainerPhotoUIViewPool* pUIViewPool         = m_pFrameParam->in.pUIViewPool;
    app::tool::MenuWindow*  pMenuWindowLowView  = pUIViewPool->GetMenuWindowLowView();

    pMenuWindowLowView->DrawTree( no );
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result TrainerPhotoShootConfirmFrame::EndFunc()
  {
    TrainerPhotoUIViewPool* pUIViewPool     = m_pFrameParam->in.pUIViewPool;
    TrainerPhotoLowRootView*  pLowRootView  = pUIViewPool->GetRootLowView();
    app::tool::MenuWindow*  pMenuWindowLowView  = pUIViewPool->GetMenuWindowLowView();

    pMenuWindowLowView->RemoveFromSuperView();
    pMenuWindowLowView->SetListener( NULL );

    pLowRootView->HideMessageDialog();

    return applib::frame::RES_FINISH;
  }

  //-----------------------------------------------------------------------------
  /**
    * @func    SelectMenu
    * @brief   MenuWindwo項目選択時のリスナー
    *
    * @param   id  メニュー項目のID
    * @note    app::tool::MenuWindowListenerのリスナ
    */
  //-----------------------------------------------------------------------------
  void TrainerPhotoShootConfirmFrame::SelectMenu( u32 id )
  {
    TrainerPhotoUIViewPool*   pUIViewPool         = m_pFrameParam->in.pUIViewPool;
    app::tool::MenuWindow*    pMenuWindowLowView  = pUIViewPool->GetMenuWindowLowView();

    pMenuWindowLowView->EndMenu( false );
    m_bEndMenuWindow  = true;

    if( id == TrainerPhotoUIViewPool::MENU_WINDWOW_SELECT_YES )
    {
      m_pFrameParam->out.pushBtn  = PUSH_BTN_SHOOT_SAVE;
    }
    else
    if( id == TrainerPhotoUIViewPool::MENU_WINDWOW_SELECT_NO )
    {
      m_pFrameParam->out.pushBtn  = PUSH_BTN_CANCEL;
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

GFL_NAMESPACE_END( photo )
GFL_NAMESPACE_END( app )
