//=============================================================================
/**
 * @file	 ConfigFrame.h
 * @brief	 コンフィグ制御フレーム
 * @author yuto_uchida
 * @date	 2015.05.19
 */
//=============================================================================
#pragma once

#include "App/config/include/ConfigFrame.h"
#include "App/config/source/parts/ConfigUpperUIDraw.h"

//  gflib2のインクルード
#include <str/include/gfl2_MsgData.h>
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "Applib/include/Frame/AppFrameManager.h"
#include "Applib/include/Util/AppRenderingManager.h"
#include "Applib/include/Util/AppUtilFileControlState.h"
#include "Applib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Util/app_util_KeyRepeatController.h"
#include "System/include/ArcIdGetter.h"
#include "System/include/HeapDefine.h"
#include "Savedata/include/ConfigSave.h"
#include "print/include/WordSet.h"

//  view
#include "App/Config/source/ConfigUIViewPool.h"

//  メッセージのインクルード
#include "niji_conv_header/message/msg_option.h"

//  リソースのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/configMenu.gaix>
#include <arc_index/message.gaix>
#include "arc_index/MenuWindow.gaix"

//  サウンドのインクルード
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  //------------------------------------------------------------------
  /**
   * @brief  コンストラクタ
   */
  //------------------------------------------------------------------
  ConfigFrame::ConfigFrame(void)
  {
    _Clear();
  }

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ConfigFrame::~ConfigFrame()
  {
  }

  void ConfigFrame::Initialize( APP_PARAM* pAppParam )
  {
    GFL_ASSERT( pAppParam );
    m_pAppParam = pAppParam;

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    m_pManagerHeapBase = GFL_CREATE_LOCAL_HEAP_NAME( m_pAppParam->in_param.pParentDevHeap, SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false, "ConfigFrame" );

    // このアプリ全体に必要なメモリを作成
    m_pAppHeap = GFL_NEW( m_pManagerHeapBase ) app::util::Heap();

    // ローカルヒープを作る
    m_pAppHeap->LocalHeapCreate( m_pAppParam->in_param.pParentAppHeap, m_pAppParam->in_param.pParentDevHeap, SIZE_MY_APP, SIZE_MY_APPDEV );

    m_pKeyRepeatCtrl  = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) app::util::KeyRepeatController( 0, 0 );

    _SetState( STATE_TYPE_LOAD );
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result ConfigFrame::InitFunc(void)
  {
    GFL_ASSERT( m_pAppHeap );
    GFL_ASSERT( m_pAppParam );
    GFL_ASSERT( m_pAppParam->in_param.pRenderingManager );

    return applib::frame::RES_FINISH;
  }

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  applib::frame::Result ConfigFrame::UpdateFunc(void)
  {
    if( m_state == STATE_TYPE_LOAD )
    {
      _SetState( STATE_TYPE_OPEN );
    }

    ConfigUIViewPool*   pUIViewPool   = m_pAppParam->in_param.pViewPool;
    ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();
    ConfigUpperUIDraw*  pUpperUIDraw  = pUIViewPool->GetRootUppView();

    if( m_state == STATE_TYPE_OPEN )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );

      ConfigUpperUIDraw::StateType  upperDrawState  = pUpperUIDraw->GetState();
      ConfigLowerUIDraw::StateType  lowerDrawState  = pLowerUIDraw->GetState();

      if( ( upperDrawState == ConfigUpperUIDraw::STATE_TYPE_UPDATE ) &&
          ( lowerDrawState == ConfigLowerUIDraw::STATE_TYPE_UPDATE ) )
      {
        if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) )
        {
          //  初回メッセージ設定
          _UpdateHelpText( *pLowerUIDraw->GetListItemCurrentData() );

          _SetState( STATE_TYPE_UPDATE );
        }
      }
    }

    //  リスト項目変更
    //  変更したら再びUpdateに戻って操作できるようにする
    {
      if( m_state == STATE_TYPE_CHANGE_ITEM_DATA )
      {
        if( pLowerUIDraw->IsButtonAnimation() == false )
        {
          //  リスト項目を変える
          const ConfigLowerUIDraw::LIST_ITEM_PARAM* pChangeListItemParam  = pLowerUIDraw->MoveListItemSelectDataPos( m_listItemChangeDataEvent.list_idx, m_listItemChangeDataEvent.select_move );

          //  漢字・かなが変わった場合はテキスト更新
          if( pChangeListItemParam->type == ConfigLowerUIDraw::CONFIG_TYPE_TEXT_MODE )
          {
            gfl2::str::MsgData* pSetMsgDataList[] =
            {
              m_pAppParam->in_param.pKanaMessage,
              m_pAppParam->in_param.pKanjiMessage
            };

            GFL_ASSERT( pChangeListItemParam->itemData.select_idx < GFL_NELEMS( pSetMsgDataList ) );

            gfl2::str::MsgData* pChangeMsgData  = pSetMsgDataList[ pChangeListItemParam->itemData.select_idx ];
            pLowerUIDraw->UpdateText( pChangeMsgData );
            pUpperUIDraw->UpdateText( pChangeMsgData );

            m_pUseMsgData  = pChangeMsgData;
          }
          //  テキスト速度が変わったらヘルプテキスト更新
          else
          if( pChangeListItemParam->type == ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED )
          {
            //  ヘルプテキスト更新
            _UpdateHelpText( *pChangeListItemParam );
          }

          //  セーブのコンフィグデータを更新しているかどうか
          m_bDataChange = _IsChangeConfigSaveData();

          _SetState( STATE_TYPE_UPDATE );
        }
      }
    }

    //  シーン戻るイベント
    //  ボタンアニメが終わったらフレームを終了させる
    {
      if( m_state == STATE_TYPE_SCENE_BACK_EVENT )
      {
        if( pLowerUIDraw->IsButtonAnimation() == false )
        {
          //  データ変更しているかチェックして変更しているのであれば、変更確認ウィンドウを出す
          if( m_bDataChange )
          {
            _SetState( STATE_TYPE_CHANGE_DATA_YESNO_WINDOW );
          }
          else
          {
            _SetState( STATE_TYPE_CLOSE );
          }
        }
      }
    }

    //  決定
    {
      if( m_state == STATE_TYPE_ENTER )
      {
        if( pLowerUIDraw->IsButtonAnimation() == false )
        {
          //  データ更新
          if( m_bDataChange )
          {
            //  データ更新処理をする
            if( _WriteConfigSaveData() == false )
            {
              //  設定が変更されているので書き込まれないとNG
              GFL_ASSERT( 0 );
            }
          }

          _SetState( STATE_TYPE_CLOSE );
        }
      }
    }

    //  はいいいえ
    {
      if( m_state == STATE_TYPE_CHANGE_DATA_YESNO_WINDOW )
      {
        if( ( m_lowerSystemMessageEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
        ||  ( m_lowerSystemMessageEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo ) )
        {
          App::Tool::MessageMenuView* pMessageMenuView  = pUIViewPool->GetMessageMenuLowView();

          pMessageMenuView->RemoveFromSuperView();
          pMessageMenuView->SetEventHandler( NULL );

          if( m_lowerSystemMessageEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedYes )
          {
            //  変更して終了
            if( _WriteConfigSaveData() == false )
            {
              //  設定が変更されているので書き込まれないとNG
              GFL_ASSERT( 0 );
            }

            _SetState( STATE_TYPE_CLOSE );
          }
          else
          if( m_lowerSystemMessageEventCode == App::Tool::MessageMenuView::IEventHandler::EVENT_OnSelectedNo )
          {
            //  変更しないで終了
            _SetState( STATE_TYPE_CLOSE );
          }
          else
          {
            //  論理バグ
            GFL_ASSERT( 0 );
          }
        }
      }
    }

    if( m_state == STATE_TYPE_CLOSE )
    {
      _SetState( STATE_TYPE_END );
    }

    if( m_state == STATE_TYPE_END )
    {
      return applib::frame::RES_FINISH;
    }

    return applib::frame::RES_CONTINUE;
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  void ConfigFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
  }

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  applib::frame::Result ConfigFrame::EndFunc(void)
  {
    enum
    {
      SEQ_FADE_OUT  = 0,
      SEQ_FADE_OUT_WAIT,
      SEQ_END
    };

    int seq = applib::frame::CellSuper::GetSubSeq();

    //  フェードで画面が完全に黒で覆った状態でレイアウトは破棄する
    //  フェード中にレイアウト破棄するとバグった画面が見える
    if( seq == SEQ_FADE_OUT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      pFadeManager->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );

      ++seq;
    }

    if( seq == SEQ_FADE_OUT_WAIT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
      {
        ++seq;
      }
    }

    applib::frame::CellSuper::SetSubSeq( seq );
    if( seq == SEQ_END )
    {
      ConfigUIViewPool*   pUIViewPool   = m_pAppParam->in_param.pViewPool;
      ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();
      pLowerUIDraw->SetInputListener( NULL );

      GFL_SAFE_DELETE( m_pKeyRepeatCtrl );
      m_pUseMsgData               = NULL;

      GFL_SAFE_DELETE( m_pAppHeap );

      gfl2::heap::Manager::DeleteHeap( m_pManagerHeapBase );

      return applib::frame::RES_FINISH;
    }

    return applib::frame::RES_CONTINUE;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return 同フレーム内での他入力イベントを許可するならtrue、
  *         他のイベントを排除するならfalseを返却すること。
  *                 ※このイベントは ConfigLowerUIDrawクラス限定
  */
  //--------------------------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult ConfigFrame::OnLayoutPaneEvent( const u32 buttonId )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return DISABLE_ACCESS;
    }

    //  リスト項目のデータ切り替えボタンにタッチしているどうかの判定
    {
      LIST_ITEM_CHANGE_DATA_EVENT pushEventData;
      if( _OutputChangeListItemDataFromButtonid( &pushEventData, buttonId ) )
      {
        _StartEventChangeItemData( pushEventData );
        return DISABLE_ACCESS;
      }
    }

    //  戻るボタンを押したか
    if( buttonId == ConfigLowerUIDraw::BUTTON_ID_BACK )
    {
      _SetState( STATE_TYPE_SCENE_BACK_EVENT );
      return DISABLE_ACCESS;
    }

    //  決定ボタンを押したか
    if( buttonId == ConfigLowerUIDraw::BUTTON_ID_ENTER )
    {
      _SetState( STATE_TYPE_ENTER );
      return DISABLE_ACCESS;
    }

    return ENABLE_ACCESS;
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペイン選択アニメーションの開始を検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return none
  */
  //--------------------------------------------------------------------------------------------
  void ConfigFrame::OnLayoutPaneStartingAnimeEvent( const u32 buttonId )
  {
    LIST_ITEM_CHANGE_DATA_EVENT pushEventData;
    if( _OutputChangeListItemDataFromButtonid( &pushEventData, buttonId ) )
    {
      if( Sound::IsSEFinished( SEQ_SE_SELECT1 ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインタッチ時を検知
  *       【ボタンマネージャー関係】
  *
  * @param  buttonId   通知を受けるペインＩＤ
  *
  * @return none
  * 
  * @note 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
  * 
  *                 TRIGER判定として使いたい場合は、ここを使わずに
  *                 ボタンマネージャーモードをTRIGGERにして
  *                 OnLayoutPaneEventを使うべき。
  *                 ※このイベントは ConfigLowerUIDrawクラス限定
  */
  //--------------------------------------------------------------------------------------------
  void ConfigFrame::OnLayoutPaneTouchTriggerEvent( const u32 buttonId )
  {
    // @fix NMCat[1434] 更新処理以外はスキップする
    if( m_state != STATE_TYPE_UPDATE )
    {
      return;
    }

    ConfigUIViewPool*   pUIViewPool   = m_pAppParam->in_param.pViewPool;
    ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

    if( pLowerUIDraw->IsButtonAnimation() == true )
    {
      return;
    }

    //  リスト項目のボタンであればカーソルのカレント位置を変える
    if( pLowerUIDraw->SetListItemCurrentPosFormButtonId( (ConfigLowerUIDraw::ButtonId)buttonId ) )
    {
      // @fix GFNMCat[3920] カーソル切り替えしたらSE再生
      if( Sound::IsSEFinished( SEQ_SE_SELECT1 ) )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }

      //  変更
      _UpdateHelpText( *pLowerUIDraw->GetListItemCurrentData() );
    }
  }

  //--------------------------------------------------------------------------------------------
  /**
    * @brief  キー入力の検知
    *         ※タッチパネルの入力が優先される
    *
    * @param  pButton  ボタン ( A,B,X,Y,L,R... )
    * @param  pKey     十字キー
    * @param  pStick   アナログスティック
    *
    * @return 同フレーム内での他入力イベントを許可するならtrue、
    *         他のイベントを排除するならfalseを返却すること。
    */
  //--------------------------------------------------------------------------------------------
  app::ui::UIInputListener::ListenerResult ConfigFrame::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
  {
    if( m_state != STATE_TYPE_UPDATE )
    {
      return DISABLE_ACCESS;
    }

    ConfigUIViewPool*   pUIViewPool   = m_pAppParam->in_param.pViewPool;
    ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      if( pLowerUIDraw->PushButton( ConfigLowerUIDraw::BUTTON_ID_BACK ) )
      {
        return DISABLE_ACCESS;
      }
    }
    else
    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      if( pLowerUIDraw->PushButton( ConfigLowerUIDraw::BUTTON_ID_ENTER ) )
      {
        return DISABLE_ACCESS;
      }
    }
    else
    {
      //  カーソルち項目は同期していないといけないので
      //  ボタンアニメが動いている時はカーソルを動かすとカレント項目がずれるのでアニメが終るまで待つ
      if( pLowerUIDraw->IsButtonAnimation() == true )
      {
        return DISABLE_ACCESS;
      }

      if( pKey->IsTrigger(  gfl2::ui::DIR_LEFT ) )
      {
        if( pLowerUIDraw->PushButtonCurrentListItem( ConfigLowerUIDraw::PUSH_BUTTON_LIST_ITEM_LEFT_ARROW ) )
        {
          return DISABLE_ACCESS;
        }
      }
      else
      if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
      {
        if( pLowerUIDraw->PushButtonCurrentListItem( ConfigLowerUIDraw::PUSH_BUTTON_LIST_ITEM_RIGHT_ARROW ) )
        {
          return DISABLE_ACCESS;
        }
      }
      else
      {
        s32 listItemCurrentPos  = pLowerUIDraw->GetListItemCurrentPos();
        u32 listItemMax         = pLowerUIDraw->GetInitParam().listMax;

        m_pKeyRepeatCtrl->SetMaxPos( listItemMax - 1, 0 );
        m_pKeyRepeatCtrl->UpdateInfo( pKey, listItemCurrentPos, 0 );

        bool bChangeCurrentListItem = false;

        if( m_pKeyRepeatCtrl->IsRepeat( gfl2::ui::DIR_UP ) )
        {
          --listItemCurrentPos;
          if( listItemCurrentPos < 0 )
          {
            listItemCurrentPos  = listItemMax - 1;
          }

          if( pLowerUIDraw->SetListItemCurrentPos( listItemCurrentPos ) != -1 )
          {
            bChangeCurrentListItem  = true;
          }
        }
        else
        if( m_pKeyRepeatCtrl->IsRepeat( gfl2::ui::DIR_DOWN ) )
        {
          ++listItemCurrentPos;
          if( (s32)listItemMax <= listItemCurrentPos )
          {
            listItemCurrentPos  = 0;
          }

          if( pLowerUIDraw->SetListItemCurrentPos( listItemCurrentPos ) != -1 )
          {
            bChangeCurrentListItem  = true;
          }
        }

        //  リスト項目のカレント変更した
        if( bChangeCurrentListItem )
        {
          Sound::PlaySE( SEQ_SE_SELECT1 );

          _UpdateHelpText( *pLowerUIDraw->GetListItemCurrentData() );
          return DISABLE_ACCESS;
        }
      }
    }

    return ENABLE_ACCESS;
  }

  //  MessageMenuWindowのイベント
  void  ConfigFrame::MessageMenuView_OnEvent(const App::Tool::MessageMenuView::IEventHandler::EventCode event)
  {
    m_lowerSystemMessageEventCode = event;
  }

  //------------------------------------------------------------------
  /**
    * @brief 状態ステート設定
  */
  //------------------------------------------------------------------
  void ConfigFrame::_SetState( const StateType newStateType )
  {
    m_state             = newStateType;
    m_subState          = 0;

    applib::frame::CellSuper::SetSubSeq( 0 );

    ConfigUIViewPool* pUIViewPool = m_pAppParam->in_param.pViewPool;

    if( m_state == STATE_TYPE_LOAD )
    {
    }
    else
    if( m_state == STATE_TYPE_OPEN )
    {
      //  使用するメッセージデータ
      m_pUseMsgData = m_pAppParam->in_param.pNormalMessage;

      pUIViewPool->GetRootLowView()->Start();
      pUIViewPool->GetRootUppView()->Start();

      //  処理が終った最後にフェード
      {
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
      }
    }
    else
    if( m_state == STATE_TYPE_UPDATE )
    {
      ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

      pLowerUIDraw->SetInputListener( this );
      pLowerUIDraw->SetInputEnabled( true );
    }
    else
    if( m_state == STATE_TYPE_CHANGE_ITEM_DATA )
    {
      pUIViewPool->GetRootLowView()->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_SCENE_BACK_EVENT )
    {
      // @fix NMCat[1434] もどるボタンを押したら各コンフィグの選択項目は入力を無効にする
      pUIViewPool->GetRootLowView()->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_ENTER )
    {
      pUIViewPool->GetRootLowView()->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_CHANGE_DATA_YESNO_WINDOW )
    {
      //  変更確認メッセージモードに切り替え
      ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();
      pLowerUIDraw->SetDrawMode( ConfigLowerUIDraw::DRAW_MODE_MESSAGE_CHANGE_DATA, false );

      //  メッセージ取得
      gfl2::str::StrBuf* pTextStrBuf = NULL;
      {
        app::util::G2DUtil*       pG2DUtil    = pLowerUIDraw->GetG2DUtil();

        pTextStrBuf = pG2DUtil->GetTempStrBuf( 0 );
        GFL_ASSERT( pTextStrBuf );
        m_pUseMsgData->GetString( msg_option_10_01, *pTextStrBuf );
      }

      App::Tool::MessageMenuView* pMessageMenuView  = pUIViewPool->GetMessageMenuLowView();

      //  Yes/Noにしてダイアログ設定
      pMessageMenuView->SetYNListMode( true );

      pMessageMenuView->SelectMenu( 0 );
      pMessageMenuView->SetModal( true );

      //  ShowMessageWithListMenu内のAddItemを使わないで個別でAddItem()
      {
        //  はい1
        pMessageMenuView->AddItem( m_pUseMsgData, msg_option_10_02, false, false, SEQ_SE_COMPLETE1 );
        //  いいえ
        pMessageMenuView->AddItem( m_pUseMsgData, msg_option_10_03, true, false, 0 );
      }

      pMessageMenuView->ShowMessageWithListMenu(
        /*  ウインドウ設定  */
        pTextStrBuf,
        false,

        /*  リスト設定  */
        NULL,
        NULL
      );

      pMessageMenuView->SetEventHandler( this );

      m_lowerSystemMessageEventCode = App::Tool::MessageMenuView::IEventHandler::EVENT_;

      pLowerUIDraw->AddSubView( pMessageMenuView );
    }
    else
    if( m_state == STATE_TYPE_CLOSE )
    {
      //m_pLowerUIDraw->SetInputEnabled( false );
      pUIViewPool->GetRootLowView()->SetInputEnabled( false );
    }
    else
    if( m_state == STATE_TYPE_END )
    {
    }
  }

  //  項目のデータ変更開始
  void ConfigFrame::_StartEventChangeItemData( const LIST_ITEM_CHANGE_DATA_EVENT& rData )
  {
    m_listItemChangeDataEvent = rData;

    //  下画面にデータ変更したので、矢印アニメをする
    {
      ConfigUIViewPool* pUIViewPool = m_pAppParam->in_param.pViewPool;
      ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

      //  変更項目は下画面のカーソル位置と同期しているので違いがある場合はNG
      GFL_ASSERT( (s32)m_listItemChangeDataEvent.list_idx == pLowerUIDraw->GetListItemCurrentPos() );

      if( m_listItemChangeDataEvent.select_move < 0 )
      {
        pLowerUIDraw->PlayPushAnimeListItem( m_listItemChangeDataEvent.list_idx, ConfigLowerUIDraw::PUSH_BUTTON_LIST_ITEM_LEFT_ARROW );
      }
      else if( 0 < m_listItemChangeDataEvent.select_move )
      {
        pLowerUIDraw->PlayPushAnimeListItem( m_listItemChangeDataEvent.list_idx, ConfigLowerUIDraw::PUSH_BUTTON_LIST_ITEM_RIGHT_ARROW );
      }
      else
      {
        GFL_ASSERT( 0 );
      }
    }

    _SetState( STATE_TYPE_CHANGE_ITEM_DATA );
  }

  //  ヘルプテキスト更新
  void ConfigFrame::_UpdateHelpText( const ConfigLowerUIDraw::LIST_ITEM_PARAM& rCurrentListItemParam )
  {
    Savedata::ConfigSave::MSG_SPEED msgSpeed  = Savedata::ConfigSave::MSG_NO_WAIT;

    ConfigUIViewPool* pUIViewPool = m_pAppParam->in_param.pViewPool;
    ConfigUpperUIDraw*  pUpperUIDraw  = pUIViewPool->GetRootUppView();

    //  メッセージスピードが変わった
    if( rCurrentListItemParam.type == ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED )
    {
      const Savedata::ConfigSave::MSG_SPEED aSpeedType[ Savedata::ConfigSave::MSG_SPEED_MAX ]  =
      {
        Savedata::ConfigSave::MSG_SLOW,
        Savedata::ConfigSave::MSG_NORMAL,
        Savedata::ConfigSave::MSG_FAST
      };

      GFL_ASSERT( rCurrentListItemParam.itemData.select_idx < GFL_NELEMS( aSpeedType ) );
      
      msgSpeed  = aSpeedType[ rCurrentListItemParam.itemData.select_idx ];
    }

    //  メッセージを流す
    pUpperUIDraw->SetHelpMessage( rCurrentListItemParam.helpMsgId, msgSpeed );
  }

  //  コンフィグデータを変更しているかどうか
  bool ConfigFrame::_IsChangeConfigSaveData()
  {
    ConfigUIViewPool* pUIViewPool = m_pAppParam->in_param.pViewPool;
    ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

    const ConfigLowerUIDraw::INIT_PARAM&  rLowerInitParam = pLowerUIDraw->GetInitParam();
    for( u32 i = 0; i < rLowerInitParam.listMax; ++i )
    {
      const ConfigLowerUIDraw::LIST_ITEM_PARAM* pListItemParam  = pLowerUIDraw->GetListItemData( i );
      GFL_ASSERT( pListItemParam );

      if( pListItemParam->firstSelectIdx != pListItemParam->itemData.select_idx )
      {
        //  変更している
        return  true;
      }
    }

    return  false;
  }

  //------------------------------------------------------------------
  /**
    * @brief	  コンフィグデータに書き込み
    * @return	書き込みした -> true / 書き込みしていない -> false
    */
  //------------------------------------------------------------------
  bool ConfigFrame::_WriteConfigSaveData()
  {
    bool  bWrite  = false;

    // saveDataに反映
    Savedata::ConfigSave* pConfigSaveData = m_pAppParam->in_param.pConfigData;

    //  リスト項目データを取得
      
    ConfigLowerUIDraw::ConfigType confgType = ConfigLowerUIDraw::CONFIG_TYPE_MAX;

    ConfigUIViewPool* pUIViewPool = m_pAppParam->in_param.pViewPool;
    ConfigLowerUIDraw*  pLowerUIDraw  = pUIViewPool->GetRootLowView();

    u32   loopNum     = pLowerUIDraw->GetInitParam().listMax;

    for( u32 i = 0; i < loopNum; ++i )
    {
      const ConfigLowerUIDraw::LIST_ITEM_PARAM* pListItemParam  = pLowerUIDraw->GetListItemData( i );
      GFL_ASSERT( pListItemParam );

      confgType = pListItemParam->type;

      //  設定が変更して書き込むかどうか
      if( pListItemParam->firstSelectIdx != pListItemParam->itemData.select_idx )
      {
      }
      else
      {
        continue;
      }

      //  値が変更された時には下の処理が進む
      bWrite  = true;
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_TALK_SPEED )
      {
        // はなしのはやさ
        Savedata::ConfigSave::MSG_SPEED speedType = (Savedata::ConfigSave::MSG_SPEED)pListItemParam->itemData.select_idx;
        pConfigSaveData->SetTalkingSpeed( speedType );

        // 文字送り対応メッセージウィンドウのメッセージ速度も変更
        print::MessageWindow::SetDefaultMessageSpeed( speedType );
      }
      else
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_TEXT_MODE )
      {
        // もじモード
        if( pConfigSaveData->IsJapanese() == true )
        {
          // 文字モードが変更されたか
          bool  bKanjiMode  = ( pListItemParam->itemData.select_idx == 1 );

          //  Configが漢字モードと選択データが漢字モードかで判定
          if( pConfigSaveData->IsKanjiMode() != bKanjiMode )
          {
            //  現在の文字モードと異なっているか
            m_pAppParam->out_param.bMoziChange  = true;
          }

          pConfigSaveData->SetKanjiMode( bKanjiMode );
        }
      }
      else
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_ANIM_SHOW )
      {
        // せんとうアニメ
        pConfigSaveData->SetBattleAnimation( (Savedata::ConfigSave::BATTLE_ANIMATION)pListItemParam->itemData.select_idx );
      }
      else
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_BATTLE_RULE )
      {
        // しあいのルール
        pConfigSaveData->SetBattleRule( (Savedata::ConfigSave::BATTLE_RULE)pListItemParam->itemData.select_idx );
      }
      else
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_BUTTOM_MODE )
      {
        // ボタンモード
        gfl2::ui::DeviceManager * pDeviceManager =  GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
        GFL_ASSERT( pDeviceManager );
        
        gfl2::ui::Button* pButtonStandard = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );
        GFL_ASSERT( pButtonStandard );

        gfl2::ui::Button* pButtonStickEmu = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STICK_EMU );
        GFL_ASSERT( pButtonStickEmu );

        pConfigSaveData->SetButtonMode( (Savedata::ConfigSave::BUTTON_MODE)pListItemParam->itemData.select_idx, pButtonStandard, pButtonStickEmu );
      }
      else
      if( confgType == ConfigLowerUIDraw::CONFIG_TYPE_BOX_STATUS )
      {
        // Boxの送信
        pConfigSaveData->SetBoxStatus( (Savedata::ConfigSave::BOX_STATUS)pListItemParam->itemData.select_idx );
      }
      else
      {
        //  論理バグ
        GFL_ASSERT( 0 );
      }
    }

    return  bWrite;
  }

  //------------------------------------------------------------------
  /**
    * @brief ボタンidからリスト項目切り替えのイベントデータを出力
    * @param pOutputData  : 出力データのアドレス
    * @param buttonId     : 出力対象のリストアイテムのボタンid
    * @return 成功 true / 失敗時 false
  */
  //------------------------------------------------------------------
  bool ConfigFrame::_OutputChangeListItemDataFromButtonid( LIST_ITEM_CHANGE_DATA_EVENT* pOutputData, const u32 buttonId )
  {
    GFL_ASSERT( pOutputData );

    u32 loopNum = ConfigLowerUIDraw::GetCreateListItemDataNum();
    for( u32 i = 0; i < loopNum; ++i )
    {
      const ConfigLowerUIDraw::LIST_ITEM_CREATE_DATA& rActiveListItemData  = ConfigLowerUIDraw::GetCreateListItemData( ( ConfigLowerUIDraw::ConfigType )i);
      if( rActiveListItemData.left_arrow_button_id == buttonId )
      {
        pOutputData->list_idx     = i;
        pOutputData->select_move  = -1;
        return  true;
      }
      else if( rActiveListItemData.right_arrow_button_id == buttonId )
      {
        pOutputData->list_idx     = i;
        pOutputData->select_move  = 1;
        return  true;
      }
    }

    return  false;
  }

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )
