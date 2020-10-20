// ============================================================================
/*
 * @file StaffrollEventFrame.cpp
 * @brief スタッフロールアプリのプロセスです。
 * @date 2016.01.06
 * @author uchida_yuto
 */
// ============================================================================
#include "App/Staffroll/source/Frame/StaffrollEventFrame.h"
#include "App/Staffroll/source/StaffrollFrameResult.h"
#include "App/Staffroll/source/StaffrollResourceID.h"
#include "App/Staffroll/source/StaffrollTextBox.h"

//  gflib2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "System/include/PokemonVersion.h"
#include "System/include/ArcIdGetter.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

//  viewのインクルード
#include "App/Staffroll/source/System/StaffrollViewObjects.h"
#include <arc_def_index/arc_def.h>

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Staffroll)
GFL_NAMESPACE_BEGIN(Frame)

  const u32 StaffrollEventFrame::ms_fadeTime = 100;

  StaffrollEventFrame::StaffrollEventFrame(
    App::Staffroll::System::StaffrollWork* pStaffrollWork ) :
    NetAppLib::System::NetAppFrameBase( pStaffrollWork )
  {
    _Clear();

    m_pWork = pStaffrollWork;
  }

  StaffrollEventFrame::~StaffrollEventFrame()
  {
    GFL_SAFE_DELETE( m_pImgEvent );
    GFL_SAFE_DELETE( m_pTextEvent );
  }

  //-----------------------------------------------------------------------------
  // NetAppFrameBase
  //-----------------------------------------------------------------------------
  bool StaffrollEventFrame::startup()
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    if( m_initSeq == 0 )
    {
      //  イベントデータ作成
      m_pTextEvent  = GFL_NEW( m_pWork->GetDeviceHeap() ) TextEvent( m_pWork );

      //  初回しか一枚絵はでない
      if( m_pWork->GetFirstEvent() == true )
      {
        m_pImgEvent   = GFL_NEW( m_pWork->GetDeviceHeap() ) ImgEvent( m_pWork );
      }

      App::Staffroll::System::StaffrollViewList*     pViewList     = pViewSysAccessor->GetViewList();
      GFL_ASSERT( pViewList );

      //  view構築
      pViewList->RemoveAllView();

      pViewObjects->InputDisableAll();

      //  上Viewの構築
      {
        //  ImgのView設定
        {
          App::Staffroll::View::StaffrollImgView*  pUpperImgView  = pViewObjects->GetImgUpperView();
          GFL_ASSERT( pUpperImgView );
          pUpperImgView->SetVisible( true );
//          pUpperImgView->BeginFadeSkip();
          pViewList->AddUpperView( pUpperImgView );
        }

        //  テキストのView設定
        {
          App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
          GFL_ASSERT( pUpperTextScrollView );

          pUpperTextScrollView->SetVisible( true );
        //  pViewList->AddUpperView( pUpperTextScrollView );
        }
      }

      //  開始する
      m_bPlayFlg  = true;

      ++m_initSeq;
    }
    else
    {
      return  true;
    }

    return false;
  }


  bool StaffrollEventFrame::cleanup()
  {
#if PM_DEBUG
    
#endif
    //  デバッグでスキップしたときのことを考えてここで強制フェード
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

    return  true;
  }


  void StaffrollEventFrame::setListener()
  {
    //  フレーム開始
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );
  }


  void StaffrollEventFrame::removeListener()
  {
  }


  void StaffrollEventFrame::updateSequence()
  {
    u32 seq = GetSubSeq();

    ++m_updateFrameCount;

#if PM_DEBUG
    if( m_bForceEnd == false )
    {
      gfl2::ui::DeviceManager* pUIDevManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      const gfl2::ui::Button*       button   = pUIDevManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

      //  強制終了
      if( button->IsTrigger(gfl2::ui::BUTTON_B) )
      {
        seq = _StartUpdateSeq( UPDATE_SEQ_END );
      }

      // 速度を変える
      if( button->IsTrigger(gfl2::ui::BUTTON_X) )
      {
        m_pWork->AddPlaySpeed();
      }
      if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
      {
        m_pWork->SubPlaySpeed();
      }
    }
#endif

    //  テキストViewはコマンド更新前に呼ばないと画面範囲外判定の座標が描画で更新されるのでうまくいかない
    {
      App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
      GFL_ASSERT( pViewSysAccessor != NULL );

      App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();

      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();

      s32 playSpeed = m_pWork->GetPlaySpeed();
      for( s32 i = 0; i < playSpeed; ++i )
      {
        pUpperTextScrollView->UpdateTree();
      }
    }

    if( seq == UPDATE_SEQ_FADE_IN_START )
    {
      //  フェードイン
      GameSys::FrameMode  frameMode = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFrameMode();
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
//      pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, ms_fadeTime );
      pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 0 );

      ++seq;
    }

    if( seq == UPDATE_SEQ_FADE_IN_WAIT )
    {
      gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
      if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
      {
        //  BGM再生
        Sound::StartBGMReq( STRM_BGM_ENDING02, Sound::BGM_FADE_VERY_FAST );

        ++seq;
      }
    }

    if( seq == UPDATE_SEQ_COMMAND_PROC )
    {
      //  １フレーム中に更新回数を増やすことで倍速をしている
      s32 playSpeed = m_pWork->GetPlaySpeed();
      for( s32 i = 0; i < playSpeed; ++i )
      {
        if( m_bPlayFlg == true )
        {
          //  実行
          _UpdateEventSequence();
          _UpdateEventCommand();
        }
      }

      //  イベント終了しているか
      {
        bool  bEndCommand = true;
        if( m_pTextEvent->IsEndCommand() == false )
        {
          bEndCommand = false;
        }

        if( m_pImgEvent != NULL )
        {
          if( m_pImgEvent->IsEndCommand() == false )
          {
            bEndCommand = false;
          }
        }

        if( bEndCommand == true )
        {
          seq = _StartUpdateSeq( UPDATE_SEQ_FADE_OUT );
        }
      }
    }

    if( seq == UPDATE_SEQ_FADE_OUT )
    {
    }

    if( seq == UPDATE_SEQ_END )
    {
    }

    //  強制終了が実行可能ならフェードして終了させる(メインのシーケンスは動いたまま)
    {
      if( m_bForceEnd == true )
      {
        if( m_foceEndSeq == 0 )
        {
          //  フェードアウトをしてBGMなどをフェードさせる
          gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
          static const gfl2::math::Vector4 scou(0.0f,0.0f,0.0f,0.0f);
          static const gfl2::math::Vector4 ecou(0.0f,0.0f,0.0f,255.0f);

          GameSys::FrameMode  frameMode = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFrameMode();
          pFadeManager->RequestOut( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &scou, &ecou, ms_fadeTime );

          _EndBGM( ms_fadeTime );

          ++m_foceEndSeq;
        }

        if( m_foceEndSeq == 1 )
        {
          //  フェードアウト終了かチェックして終了しているのであれば終了シーケンスへ
          gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
          if( pFadeManager->IsEnd( gfl2::Fade::DISP_DOUBLE ) == true )
          {
            seq = _StartUpdateSeq( UPDATE_SEQ_END );
            m_bForceEnd = false;
          }
        }
      }
    }

    SetSubSeq( seq );

    {
      App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
      GFL_ASSERT( pViewSysAccessor != NULL );

      App::Staffroll::System::StaffrollViewList*  pViewList  = pViewSysAccessor->GetViewList();
      GFL_ASSERT( pViewList );

      s32 playSpeed = m_pWork->GetPlaySpeed();
      for( s32 i = 0; i < playSpeed; ++i )
      {
        pViewList->Update();
      }
    }
  }

  void StaffrollEventFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();

    App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();

    pUpperTextScrollView->DrawTree( no );    
  }

  void StaffrollEventFrame::_UpdateEventSequence()
  {
    m_pTextEvent->UpdateSequence();

    if( m_pImgEvent != NULL )
    {
      m_pImgEvent->UpdateSequence();
    }
  }

  void StaffrollEventFrame::_UpdateEventCommand()
  {
    m_pTextEvent->UpdateCommand();

    if( m_pImgEvent != NULL )
    {
      m_pImgEvent->UpdateCommand();
    }
  }

  //  更新処理中のシーケンス設定
  u32 StaffrollEventFrame::_StartUpdateSeq( const u32 seq )
  {
    m_updateSubSeq  = 0;
    if( seq == UPDATE_SEQ_COMMAND_PROC )
    {
    }
    else
    if( seq == UPDATE_SEQ_FADE_OUT )
    {
      m_bForceEnd = true;
    }
    else
    if( seq == UPDATE_SEQ_END )
    {
      //  終了

      _EndBGM( 0 );
#if PM_DEBUG
      {
        GameSys::FrameMode  frameMode = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFrameMode();
        f32 time  = 0.0f;
        if( frameMode == GameSys::FRAMEMODE_30 )
        {
          time = (f32)m_updateFrameCount / 30.0f;
        }
        else
        if( frameMode == GameSys::FRAMEMODE_60 )
        {
          time = (f32)m_updateFrameCount / 60.0f;
        }

        GFL_PRINT( "スタッフロール終了 開始から終了までの総フレーム数:%d 時間にして[ %lf ]秒\n", m_updateFrameCount, time );
      }
#endif

      exitFrame( App::Staffroll::RESULT_BACK );
      m_req = REQ_NONE;
    }
    else
    {
      GFL_ASSERT( 0 );
    }

    return  seq;
  }

  void StaffrollEventFrame::_EndBGM( const u32 fadeFrame )
  {
    if( m_bBGMEnd == true )
    {
      return;
    }

    Sound::EndBGMReq( (Sound::FadeFrame)fadeFrame );

    m_bBGMEnd = true;
  }

//  ここからはTextEventクラスの定義
  StaffrollEventFrame::TextEvent::TextEvent( App::Staffroll::System::StaffrollWork* pWork )
  {
    _Clear();

    GFL_ASSERT( pWork );

    m_pWork = pWork;

    //  データ本体取得
    {
      size_t size = m_pWork->GetResourceManager()->GetResourceSize( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_DATA );
      GFL_ASSERT( size != 0 );

      m_eventDataMax = size / sizeof(StaffRollData);
      GFL_ASSERT( size % sizeof(StaffRollData) == 0 );  // 開発中にエラーを見付けるためのASSERT。

      m_pEventData  = (StaffRollData*)m_pWork->GetResourceManager()->GetResourceBuffer( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_TEXT_DATA );
      GFL_ASSERT( m_pEventData != NULL );
    }
  }

  StaffrollEventFrame::TextEvent::~TextEvent()
  {
  }

  /**
   * @brief シーケンスの更新
   */
  void StaffrollEventFrame::TextEvent::UpdateSequence( void )
  {
    if( m_sequence )
    {
      if( (this->*m_sequence)() )
      {
        this->SetSequence( NULL );
      }
    }
  }
  /**
   * @brief シーケンスのセット
   * @param sequence  セットするシーケンス
   */
  void StaffrollEventFrame::TextEvent::SetSequence( Sequence sequence )
  {
    m_sequence  = sequence;
  }

  /**
   * @brief 待機
   *
   * @return trueで終了 falseで継続
   */
  bool StaffrollEventFrame::TextEvent::SequenceWait( void )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      pUpperTextScrollView->ResetPos();
    }

    return false;
  }

  /**
   * @brief スクロール
   *
   * @return trueで終了 falseで継続
   */
  bool StaffrollEventFrame::TextEvent::SequenceScroll( void )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      f32 speed = (f32)m_pWork->GetEventTextParam().configFormatData.txtScrollFrameSpeed;
      pUpperTextScrollView->MoveYPos( speed );
    }

    return false;
  }

  /**
   * @brief ラベルにあったコマンドを発行
   *
   * @param label ラベル
   *
   * @return  コマンド関数 
   */
  StaffrollEventFrame::TextEvent::CommandFunction StaffrollEventFrame::TextEvent::CommandFactory( u32 label )
  {
    static const CommandFunction command_list[]  =
    {
      &StaffrollEventFrame::TextEvent::Command_StartPut,
      &StaffrollEventFrame::TextEvent::Command_EndPut,
      &StaffrollEventFrame::TextEvent::Command_StartScroll,
      &StaffrollEventFrame::TextEvent::Command_EndScroll,
      &StaffrollEventFrame::TextEvent::Command_Wait,
      &StaffrollEventFrame::TextEvent::Command_Space,
      &StaffrollEventFrame::TextEvent::Command_String,
      &StaffrollEventFrame::TextEvent::Command_Clear,
      &StaffrollEventFrame::TextEvent::Command_StringX,
      &StaffrollEventFrame::TextEvent::Command_StringY,
      &StaffrollEventFrame::TextEvent::Command_End,
      &StaffrollEventFrame::TextEvent::Command_StartAlpha,
      &StaffrollEventFrame::TextEvent::Command_EndAlpha,
      &StaffrollEventFrame::TextEvent::Command_AlphaClear,
    };
  
    GFL_ASSERT( label < GFL_NELEMS( command_list ) );
    return command_list[ label ];
  }

  /**
   * @brief コマンド実行
   */
  void StaffrollEventFrame::TextEvent::UpdateCommand( void )
  {
    while(1)
    {
      m_bEndCommand = false;
      if( m_eventDataPos >= m_eventDataMax )
      {
        m_bEndCommand = true;
        return;
      }

      const StaffRollData & data = m_pEventData[ m_eventDataPos ];
      CommandFunction command = this->CommandFactory( data.label );
      //GFL_PRINT("data[%d] label=%d\n", m_data_pos, data.label);

      bool is_break = false;
      switch( (this->*command)( data, m_command_state ) )
      {
      case COMMAND_RESULT_CONTINUE:  //現在のデータ処理が終わっていない
        is_break  = true;
        break;
      default:
        GFL_ASSERT(0);  // 開発中にエラーを発見するためのASSERT
      case COMMAND_RESULT_FINISH:    //現在のデータ処理が終わった
        ++m_eventDataPos;
        // GFL_PRINT( "command %d\n", data.label );
        is_break  = true;
        break;
      case COMMAND_RESULT_NEXTDATA:  //現在のデータ処理が終わり、すぐに次のデータを連続読み込み
        ++m_eventDataPos;
        // GFL_PRINT( "command %d\n", data.label );
        is_break  = false;
        break;
      }
    
      if( is_break )
      {
        break;
      }
    }

#if 0
    //  クレジット終端にきているかチェック
    {
      if( m_bEndCredit == false )
      {
        if( STAFFROLL_CREDIT_END_DATA_POS <= m_eventDataPos )
        {
          //  テキスト表示している中で画面の一番下位置にあるのはどれか
          if( m_pCreditLastTextBox == NULL )
          {
            //  それが最後に表示したテキストになる
            App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
            GFL_ASSERT( pViewSysAccessor != NULL );

            App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
            GFL_ASSERT( pViewObjects );

            //  一番最後の位置に表示しているテキストを取得
            {
              App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
              GFL_ASSERT( pUpperTextScrollView );

              m_pCreditLastTextBox  = pUpperTextScrollView->GetLastPosYDrawTextBox();
            }
          }
        }

        //  最後に表示しているテキストが画面外にあればクレジット終了成立
        if( m_pCreditLastTextBox != NULL )
        {
          if( m_pCreditLastTextBox->InDisplay() == true )
          {
            m_bEndCredit  = true;
          }
        }
      }
    }
#endif
  }

  /**
   * @brief コマンドの状態を設定
   *
   * @param state 状態
   */
  void StaffrollEventFrame::TextEvent::SetCommandState( CommandState state )
  {
    m_command_state = state;
  }

  /**
   * @brief 一括描画開始コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StartPut( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    //  上画面のみ表示
    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      pUpperTextScrollView->SetWritePos( DISPLAY_UPPER_HEIGHT / 2 );
    }

    GFL_ASSERT_MSG( state == COMMAND_STATE_NORMAL, "2回「開始」がきている" );
    this->SetCommandState( COMMAND_STATE_LUMPPUT );
  
    this->SetSequence( &StaffrollEventFrame::TextEvent::SequenceWait );

    //EndPutがくるまでずっと内部で呼びつづける
    return COMMAND_RESULT_NEXTDATA;
  }

  /**
   * @brief 一括描画終了コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_EndPut( const StaffRollData & data, CommandState state )
  {
    GFL_ASSERT_MSG( state == COMMAND_STATE_LUMPPUT, "「開始」が呼ばれずに「終了」が呼ばれた" );
    this->SetCommandState( COMMAND_STATE_NORMAL );
    return COMMAND_RESULT_FINISH;
  }

  /**
   * @brief スクロール開始コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StartScroll( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    //  上下一つにした下側の位置に書き込み位置を設定
    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      pUpperTextScrollView->SetWritePos( pUpperTextScrollView->GetDispLowLimit() - 10.0f );
      pUpperTextScrollView->SetVisible( true );
      pUpperTextScrollView->ClearAllTextBox();
    }

    this->SetSequence( &StaffrollEventFrame::TextEvent::SequenceScroll );
  
    return COMMAND_RESULT_FINISH;
  }

  /**
   * @brief スクロール終了コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_EndScroll( const StaffRollData & data, CommandState state )
  {
    //すべてのテキストが流れたら
    bool bTextNone = true;

    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      //  まだ画面にテキストが残っている
      if( pUpperTextScrollView->IsTextAllOut() == false )
      {
        bTextNone = false;
      }
    }

    if( bTextNone )
    {
      this->SetSequence( &StaffrollEventFrame::TextEvent::SequenceWait );
      return COMMAND_RESULT_FINISH;
    }
    else
    {
      return COMMAND_RESULT_CONTINUE;
    }
  }

  /**
   * @brief 待機コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_Wait( const StaffRollData & data, CommandState state )
  {
    {
      m_counter += 1;
    }

    if( m_counter < data.wait )
    {
      return COMMAND_RESULT_CONTINUE;
    }
    else
    {
      m_counter = 0;
      return COMMAND_RESULT_FINISH;
    }
  }

  /**
   * @brief 空白コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_Space( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      pUpperTextScrollView->AddWritePos( -(f32)( data.y ) );
    }

    if( state == COMMAND_STATE_NORMAL )
    {
      return COMMAND_RESULT_FINISH;
    }
    else
    {
      return COMMAND_RESULT_NEXTDATA;
    }
  }

  /**
   * @brief 文字描画コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_String( const StaffRollData & data, CommandState state )
  {
    return this->Command_StringCore( data, state );
  }

  /**
   * @brief 画面消去コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_Clear( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );
      
      pUpperTextScrollView->ClearAllTextBox();
    }

    return COMMAND_RESULT_FINISH;
  }

  /**
   * @brief 文字描画コマンド  Xバージョン用
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StringX( const StaffRollData & data, CommandState state )
  {
    if( m_pWork->GetRomVersion() == VERSION_MOON2 )
    {
      return this->Command_StringCore( data, state );
    }
    else
    {
      if( state == COMMAND_STATE_NORMAL )
      {
        return COMMAND_RESULT_FINISH;
      }
      else
      {
        return COMMAND_RESULT_NEXTDATA;
      }
    }
  }

  /**
   * @brief 文字描画コマンド  Yバージョン用
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StringY( const StaffRollData & data, CommandState state )
  {
    if( m_pWork->GetRomVersion() == VERSION_SUN2 )
    {
      return this->Command_StringCore( data, state );
    }
    else
    {
      if( state == COMMAND_STATE_NORMAL )
      {
        return COMMAND_RESULT_FINISH;
      }
      else
      {
        return COMMAND_RESULT_NEXTDATA;
      }
    }
  }

  /**
   * @brief スタッフロールリスト終了コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_End( const StaffRollData & data, CommandState state )
  {
    m_eventDataPos  = m_eventDataMax;
    return COMMAND_RESULT_FINISH;
  }

  /**
   * @brief 一括アルファ表示開始コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StartAlpha( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
    GFL_ASSERT( pUpperTextScrollView );

    {
      pUpperTextScrollView->SetWritePos( DISPLAY_UPPER_HEIGHT / 2 );
      pUpperTextScrollView->SetAlpha( 0 );
    }

    GFL_ASSERT_MSG( state == COMMAND_STATE_NORMAL, "2回「開始」がきている" );
    this->SetCommandState( COMMAND_STATE_ALPHAPUT );
  
    this->SetSequence( &StaffrollEventFrame::TextEvent::SequenceWait );
  
    //Command_EndAlphaがくるまでずっと内部で呼びつづける
    return COMMAND_RESULT_NEXTDATA;
  }

  /**
   * @brief 一括アルファ表示終了コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_EndAlpha( const StaffRollData & data, CommandState state )
  {
    GFL_ASSERT_MSG( state == COMMAND_STATE_ALPHAPUT, "「開始」が呼ばれずに「終了」が呼ばれた" );
  
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
    GFL_ASSERT( pUpperTextScrollView );

    const StaffrollParamBinaryFormat& rParam  = m_pWork->GetEventTextParam();
    {
      m_counter += rParam.configFormatData.alphaFrameSpeed;
    }

    f32 alpha = 255.0f * (f32)m_counter / (f32)rParam.configFormatData.alphaInFrame;
    alpha = gfl2::math::Clamp( alpha, 0.0f, 255.0f );

    pUpperTextScrollView->SetAlpha( static_cast<u8>(alpha) );

    if( m_counter < rParam.configFormatData.alphaInFrame )
    {
      return COMMAND_RESULT_CONTINUE;
    }
    else
    {
      m_counter = 0;
      pUpperTextScrollView->SetAlpha( 255 );

      this->SetCommandState( COMMAND_STATE_NORMAL );
      return COMMAND_RESULT_FINISH;
    }
  }

  /**
   * @brief アルファで消える
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return trueならば次のデータを検索する falseならばしない
   */
  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_AlphaClear( const StaffRollData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
    GFL_ASSERT( pUpperTextScrollView );

    const StaffrollParamBinaryFormat& rParam  = m_pWork->GetEventTextParam();
    {
      m_counter += rParam.configFormatData.alphaFrameSpeed;
    }
    f32 alpha = 255.0f * (f32)(rParam.configFormatData.alphaOutFrame - m_counter) / (f32)rParam.configFormatData.alphaOutFrame;
    alpha = gfl2::math::Clamp( alpha, 0.0f, 255.0f );
    pUpperTextScrollView->SetAlpha( static_cast<u8>(alpha) );

    if( m_counter < rParam.configFormatData.alphaOutFrame )
    {
      return COMMAND_RESULT_CONTINUE;
    }
    else
    {
      m_counter = 0;
      pUpperTextScrollView->ClearAllTextBox();

      //クリアしたからアルファを戻す
      pUpperTextScrollView->SetAlpha( 255 );

      return COMMAND_RESULT_FINISH;
    }
  }

  StaffrollEventFrame::TextEvent::CommandResult StaffrollEventFrame::TextEvent::Command_StringCore( const StaffRollData & data, CommandState state )
  {
    CommandResult ret = COMMAND_RESULT_CONTINUE;

    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
    GFL_ASSERT( pUpperTextScrollView );

    u32 color = 0;
  
    if( m_pWork->GetRomVersion() == VERSION_MOON2 )
    {
      color = data.x_color;
    }
    else
    {
      color = data.y_color;
    }

    if( pUpperTextScrollView->SettingBoxText( data.message, data.x, data.xofs, -data.y, data.strpos, data.font, color ) == false )
    {
      return COMMAND_RESULT_CONTINUE;
    }
    else
    {
      if( state == COMMAND_STATE_NORMAL )
      {
        ret = COMMAND_RESULT_FINISH;
      }
      else
      {
        ret = COMMAND_RESULT_NEXTDATA;
      }
    }

    return ret;
  }

//  ここからはImgEventの定義
  StaffrollEventFrame::ImgEvent::ImgEvent( App::Staffroll::System::StaffrollWork* pWork )
  {
    _Clear();

    GFL_ASSERT( pWork );

    m_pWork = pWork;

    //  データ本体取得
    {
      size_t size = m_pWork->GetResourceManager()->GetResourceSize( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_IMG_DATA );
      GFL_ASSERT( size != 0 );

      m_eventDataMax = size / sizeof( StaffrollImgEventItemData );
      // 開発中にエラーを見付けるためのASSERT。
      GFL_ASSERT( size % sizeof( StaffrollImgEventItemData ) == 0 );

      m_pEventData  = (StaffrollImgEventItemData*)m_pWork->GetResourceManager()->GetResourceBuffer( App::Staffroll::STAFFROLL_RESOURCE_ID_EVENT_IMG_DATA );
      GFL_ASSERT( m_pEventData != NULL );
    }

    //  性別取得
    {
      Savedata::MyStatus* pStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
      m_nowTrainerSex = pStatus->GetSex();
    }
  }

  StaffrollEventFrame::ImgEvent::~ImgEvent()
  {
  }

  /**
   * @brief シーケンスの更新
   */
  void StaffrollEventFrame::ImgEvent::UpdateSequence( void )
  {
    if( m_sequence )
    {
      if( (this->*m_sequence)() )
      {
        this->SetSequence( NULL );
      }
    }
  }
  /**
   * @brief シーケンスのセット
   * @param sequence  セットするシーケンス
   */
  void StaffrollEventFrame::ImgEvent::SetSequence( Sequence sequence )
  {
    m_sequence  = sequence;
  }

  /**
   * @brief 待機
   *
   * @return trueで終了 falseで継続
   */
  bool StaffrollEventFrame::ImgEvent::SequenceWait( void )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    {
      App::Staffroll::View::StaffrollTextScrollView*  pUpperTextScrollView  = pViewObjects->GetTextScrollUpperView();
      GFL_ASSERT( pUpperTextScrollView );

      pUpperTextScrollView->ResetPos();
    }

    return false;
  }

  /**
   * @brief ラベルにあったコマンドを発行
   *
   * @param label ラベル
   *
   * @return  コマンド関数 
   */
  StaffrollEventFrame::ImgEvent::CommandFunction StaffrollEventFrame::ImgEvent::CommandFactory( u32 label )
  {
    static const CommandFunction command_list[]  =
    {
      &StaffrollEventFrame::ImgEvent::Command_Wait,
      &StaffrollEventFrame::ImgEvent::Command_FadeIn,
      &StaffrollEventFrame::ImgEvent::Command_FadeOut,
      &StaffrollEventFrame::ImgEvent::Command_SetImg,
    };
  
    GFL_ASSERT( label < GFL_NELEMS( command_list ) );
    return command_list[ label ];
  }

  /**
   * @brief コマンド実行
   */
  void StaffrollEventFrame::ImgEvent::UpdateCommand( void )
  {
    while(1)
    {
      m_bEndCommand = false;
      if( m_eventDataPos >= m_eventDataMax )
      {
        m_bEndCommand = true;
        return;
      }

      const StaffrollImgEventItemData & data = m_pEventData[ m_eventDataPos ];
      CommandFunction command = this->CommandFactory( data.label );
      UCHIDA_PRINT( "now event data[%d] label=%d\n", m_eventDataPos, data.label );

      bool is_break = false;
      switch( (this->*command)( data, m_command_state ) )
      {
      case COMMAND_RESULT_CONTINUE:  //現在のデータ処理が終わっていない
        is_break  = true;
        break;
      default:
        GFL_ASSERT(0);  // 開発中にエラーを発見するためのASSERT
      case COMMAND_RESULT_FINISH:    //現在のデータ処理が終わった
        ++m_eventDataPos;
        m_cmdSeq = 0;

        UCHIDA_PRINT( "end command %d\n", data.label );
        is_break  = true;
        break;
      case COMMAND_RESULT_NEXTDATA:  //現在のデータ処理が終わり、すぐに次のデータを連続読み込み
        ++m_eventDataPos;
        m_cmdSeq = 0;

        UCHIDA_PRINT( "end command %d\n", data.label );
        is_break  = false;
        break;
      }
    
      if( is_break )
      {
        break;
      }
    }
  }

  /**
   * @brief コマンドの状態を設定
   *
   * @param state 状態
   */
  void StaffrollEventFrame::ImgEvent::SetCommandState( CommandState state )
  {
    m_command_state = state;
  }


  /**
   * @brief 待機コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return COMMAND_RESULT_FINISHならば次のデータを検索する COMMAND_RESULT_CONTINUEならばしない
   */
  StaffrollEventFrame::ImgEvent::CommandResult StaffrollEventFrame::ImgEvent::Command_Wait( const StaffrollImgEventItemData & data, CommandState state )
  {
    {
      m_counter += 1;
    }

    if( m_counter < data.wait )
    {
      return COMMAND_RESULT_CONTINUE;
    }
    else
    {
      m_counter = 0;
      return COMMAND_RESULT_FINISH;
    }
  }

  /**
   * @brief フェードインコマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return COMMAND_RESULT_FINISHならば次のデータを検索する COMMAND_RESULT_CONTINUEならばしない
   */
  StaffrollEventFrame::ImgEvent::CommandResult StaffrollEventFrame::ImgEvent::Command_FadeIn( const StaffrollImgEventItemData & data, CommandState state )
  {
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollImgView* pImgView  = pViewObjects->GetImgUpperView();
    GFL_ASSERT( pImgView );

    if( m_cmdSeq == 0 )
    {
      if( pImgView->GetAnimeNo() != 0 )
      {
        pImgView->StartCharaAnimeOut();
      }
      m_cmdSeq++;
    }

    if( m_cmdSeq == 1 )
    {
      if( pImgView->IsCharaAnimeOut() != false )
      {
        pImgView->SetBgPatern();
        pImgView->StartBgAnime();
        m_cmdSeq++;
      }
    }

    if( m_cmdSeq == 2 )
    {
      if( pImgView->IsBgAnime() != false )
      {
        pImgView->StartCharaAnimeLoop();
        pImgView->StartCharaAnimeIn();
        m_cmdSeq++;
      }
    }

    if( m_cmdSeq == 3 )
    {
      if( pImgView->IsCharaAnimeIn() != false )
      {
        pImgView->SetNextAnimeNo();
        return COMMAND_RESULT_FINISH;
      }
    }

/*
    if( m_cmdSeq == 0 )
    {
      if( pImgView->IsEndFadeSkip() == true )
      {
        //  フェード終端に到達したので最初のクロスフェードからやり直す
        pImgView->BeginFadeSkip( 1 );
      }

      //  2枚のペインに画像反映してフリップ
      pImgView->FlipTexTranslate();

      pImgView->StartFadeAnime();

      ++m_cmdSeq;
    }

    if( m_cmdSeq == 1 )
    {
      if( pImgView->IsFadeAnime() == true )
      {
        pImgView->NextFadeSkip();
        ++m_cmdSeq;
      }
    }

    if( m_cmdSeq == 2 )
    {
      return COMMAND_RESULT_FINISH;
    }
*/
    
    return COMMAND_RESULT_CONTINUE;
  }

  /**
   * @brief フェードアウトコマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return COMMAND_RESULT_FINISHならば次のデータを検索する COMMAND_RESULT_CONTINUEならばしない
   */
  StaffrollEventFrame::ImgEvent::CommandResult StaffrollEventFrame::ImgEvent::Command_FadeOut( const StaffrollImgEventItemData & data, CommandState state )
  {
    //  呼んではならない
    GFL_ASSERT_STOP( 0 );

    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollImgView* pImgView  = pViewObjects->GetImgUpperView();
    GFL_ASSERT( pImgView );

    if( m_cmdSeq == 0 )
    {
      pImgView->SetVisible( true );
//      pImgView->PlayAnime( App::Staffroll::View::StaffrollImgView::ANIME_FADE_OUT );
      ++m_cmdSeq;
    }

    if( m_cmdSeq == 1 )
    {
//      if( pImgView->IsPlayAnime( App::Staffroll::View::StaffrollImgView::ANIME_FADE_OUT ) == false )
      {
        ++m_cmdSeq;
      }
    }

    if( m_cmdSeq == 2 )
    {
      return COMMAND_RESULT_FINISH;
    }
    
    return COMMAND_RESULT_CONTINUE;
  }

  /**
   * @brief 画像設定コマンド
   *
   * @param data  データ
   * @patam state 状態
   *
   * @return COMMAND_RESULT_FINISHならば次のデータを検索する COMMAND_RESULT_CONTINUEならばしない
   */
  StaffrollEventFrame::ImgEvent::CommandResult StaffrollEventFrame::ImgEvent::Command_SetImg( const StaffrollImgEventItemData & data, CommandState state )
  {
    return COMMAND_RESULT_FINISH;

/*
    App::Staffroll::System::StaffrollViewSystemAccessor* pViewSysAccessor  = m_pWork->GetViewSystemAccessor();
    GFL_ASSERT( pViewSysAccessor != NULL );

    App::Staffroll::System::StaffrollViewObjects*  pViewObjects  = pViewSysAccessor->GetViewObjects();
    GFL_ASSERT( pViewObjects );

    App::Staffroll::View::StaffrollImgView* pImgView  = pViewObjects->GetImgUpperView();
    GFL_ASSERT( pImgView );

    if( m_cmdSeq == 0 )
    {
      //  ロム、性別条件で設定可能かチェック
      bool  bSetLoad = true;
      //  ロムチェック
      {
        if( data.romVer == STAFFROLL_IMG_EVENT_ROM_VERSION_NONE )
        {
          //  条件なしなのでスルー
        }
        else
        if( data.romVer == STAFFROLL_IMG_EVENT_ROM_VERSION_SUN )
        {
          if( m_pWork->GetRomVersion() != VERSION_SUN2 )
          {
            //  指定バージョンでないので終了
            bSetLoad = false;
          }
        }
        else
        if( data.romVer == STAFFROLL_IMG_EVENT_ROM_VERSION_MOON )
        {
          if( m_pWork->GetRomVersion() != VERSION_MOON2 )
          {
            //  指定バージョンでないので終了
            bSetLoad = false;
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      //  性別チェック
      if( bSetLoad == true )
      {
        if( data.sexId == STAFFROLL_IMG_EVENT_SEX_NONE )
        {
          //  条件なしなのでスルー
        }
        else
        if( data.sexId == STAFFROLL_IMG_EVENT_SEX_MAN )
        {
          if( m_nowTrainerSex != pml::SEX_MALE )
          {
            //  男でないので終了
            bSetLoad  = false;
          }
        }
        else
        if( data.sexId == STAFFROLL_IMG_EVENT_SEX_FEMALE )
        {
          if( m_nowTrainerSex != pml::SEX_FEMALE )
          {
            //  女でないので終了
            bSetLoad  = false;
          }
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }

      if( bSetLoad == true )
      {
        pImgView->StartTexLoadAsync( data.imgId );
        ++m_cmdSeq;
      }
      else
      {
        return COMMAND_RESULT_FINISH;
      }
    }

    if( m_cmdSeq == 1 )
    {
      if( pImgView->IsTexLoaded() == true )
      {
        ++m_cmdSeq;
      }
    }

    if( m_cmdSeq == 2 )
    {
      return COMMAND_RESULT_FINISH;
    }
    
    return COMMAND_RESULT_CONTINUE;
*/
  }


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(Staffroll)
GFL_NAMESPACE_END(App)
