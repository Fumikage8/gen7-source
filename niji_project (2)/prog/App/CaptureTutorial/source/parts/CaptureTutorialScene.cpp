//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   Scene.cpp
 *  @author ito_isao
 *
 *  @author saita_kazuki sango修正
 *  @date 13.12.12
 *
 */
//====================================================================== 
#include "CaptureTutorialScene.h"
#include "CaptureTutorialViewer.h"
#include "CaptureTutorialEventScript.h"

//  gflb2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyStatus.h"
//#include "System/dress_up/DressUpInclude.h"

#include <niji_conv_header/message/msg_capturedemo.h>
#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  static const f32 FRAME_TO_DECIDE = 0;
  static const f32 FRAME_TO_SHOW_CURSOR = 30;

  static const gfl2::math::Vector4 s_fadeOutColor(0.0f,0.0f,0.0f,255.0f);

  // デモイベントスクリプト手順データ一覧
  static SCRIPT_DATA s_aScriptData[]  =
  {
    //  ターン開始(ターン開始をしたなら必ずターン終了を呼ばないと予期せぬエラー発生する)
    { SCRIPT_TYPE_START_TURN,               ScriptDataWork() },
    //  たたかうに指カーソルを押す
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 127, 10, app::tool::FingerCursor::FORM_TYPE_DOWN ) },
    //  たたかうを押す
    { SCRIPT_TYPE_PUSH_CMD_ACTION_SELECT,   PushCmdActSelectScriptData( btl::BattleUIActSelectFrame::CMD_TYPE_PUSH_BUTTON_FIGHT ) },
    //  わざ選択を開始
    { SCRIPT_TYPE_PUT_SKILL_SELECT,         ScriptDataWork() },
    //  わざ選択に指カーソルを押す
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 87, 82, app::tool::FingerCursor::FORM_TYPE_DOWN ) },
    //  わざ選択項目を押す
    { SCRIPT_TYPE_PUSH_CMD_SKILL_ITEM,      PushCmdSkillSelectScriptData( btl::BattleUISkillSelectFrame::CMD_TYPE_PUSH_SKILL_BUTTON_0 ) },
    //  ターン終了
    { SCRIPT_TYPE_END_TURN,                 ScriptDataWork() },

    // ○○のたいあたり攻撃！メッセージ
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataSkill( 0, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF ) ) },
    // 味方がわざを出す
    { SCRIPT_TYPE_ATTACK_SKILL,             AttackActionScriptData( 0, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF, 0.2f, false ) },
    // ○○の鳴き声！メッセージ
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataSkill( 0, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY ) ) },
    // 敵がわざを出す
    // @fix NMCat[4209] こうかはいまひとつ に変更
    { SCRIPT_TYPE_ATTACK_SKILL,             AttackActionScriptData( 0, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY, 0.95f, false, pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE ) },

    // @fix MMCat[95] 捕獲チュートリアルでアゴジムシがイワンコに「はさむ」を使用した際、「効果は今ひとつ」のメッセージが表示されないことについて
    // こうかはいまひとつ　メッセージ
    { SCRIPT_TYPE_PUT_MESSAGE, PutMessageScriptData(PutMessageScriptData::CreateDataDemo(msg_capture_demo_05, 30, false)) },

    // @msg ポケモンは弱らせてからボールを投げたほうがいいんだよね！本で読んだ
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataDemo( msg_capture_demo_01, 30, false ) ) },

    //  ターン開始(ターン開始をしたなら必ずターン終了を呼ばないと予期せぬエラー発生する)
    { SCRIPT_TYPE_START_TURN,               ScriptDataWork() },

    // バッグボタンを指押し
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, -125, -31, app::tool::FingerCursor::FORM_TYPE_DOWN ) },

    // バッグを選択
    { SCRIPT_TYPE_PUSH_CMD_ACTION_SELECT,   PushCmdActSelectScriptData( btl::BattleUIActSelectFrame::CMD_TYPE_PUSH_BUTTON_BAG ) },

// ここからはバッグ用の命令が必要
    //  バッグを開く(開いたら必ず閉じないとバグが発生)
    { SCRIPT_TYPE_OPEN_BAG,                 ScriptDataWork() },

    //  バッグのカテゴリーボタンを指押し
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 92, -90, app::tool::FingerCursor::FORM_TYPE_DOWN ) },

    //  バッグのカテゴリーを変更
    { SCRIPT_TYPE_PUSH_BAG_CATEGORY_BUTTON, ScriptDataWork() },

    //  バッグのアイテムリストを指押し
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 74, 78, app::tool::FingerCursor::FORM_TYPE_UP ) },

    //  バッグのアイテムボタンを押す
    { SCRIPT_TYPE_PUSH_BAG_ITEM_BUTTON,     ScriptDataWork() },

    //  アイテム使用確認画面のボタンを指押し
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 10, -81, app::tool::FingerCursor::FORM_TYPE_DOWN ) },

    //  アイテム使用確認の使うボタンを押す
    { SCRIPT_TYPE_PUSH_BAG_USE_ITEM_BUTTON, ScriptDataWork() },

    //  バッグを閉じる
    { SCRIPT_TYPE_CLOSE_BAG,                ScriptDataWork() },
// ここまで

     //  ターン終了
    { SCRIPT_TYPE_END_TURN,                 ScriptDataWork() },

    // @msg モンスターボールを使った！と表示
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataDemo( msg_capture_demo_02, 0, true ) ) },

    //  ボールを使用する
    { SCRIPT_TYPE_THORW_BALL,               ScriptDataWork() },

    // やったー！捕まえたぞ！メッセージ
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataCatchPoke( true, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY ) ) }
  };

  // デモイベントスクリプト手順データ一覧
  static SCRIPT_DATA s_aZenryokuWazaScriptData[]  =
  {
    //  ターン開始(ターン開始をしたなら必ずターン終了を呼ばないと予期せぬエラー発生する)
    { SCRIPT_TYPE_START_TURN,               ScriptDataWork() },
    //  たたかうに指カーソルを押す
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 127, 10, app::tool::FingerCursor::FORM_TYPE_DOWN ) },
    //  たたかうを押す
    { SCRIPT_TYPE_PUSH_CMD_ACTION_SELECT,   PushCmdActSelectScriptData( btl::BattleUIActSelectFrame::CMD_TYPE_PUSH_BUTTON_FIGHT ) },
    //  わざ選択を開始
    { SCRIPT_TYPE_PUT_SKILL_SELECT,         ScriptDataWork() },
    // @msg ノーマルビーズを もたせて ノーマルタイプの わざを えらべば
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataDemo( msg_capture_demo_03, 30, false ) ) },
    // @msg Zパワーが つかえる！ さあ みててごらん！！
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataDemo( msg_capture_demo_04, 30, false ) ) },
    //  Zパワーに指カーソルを押す
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, -110, 75, app::tool::FingerCursor::FORM_TYPE_DOWN ) },
    //  全力技を押す
    { SCRIPT_TYPE_ZENRYOKU_SKILL_SELECT,    PushCmdZenryokuSkillSelectScriptData() },
    // 指定フレーム待機する
    { SCRIPT_TYPE_WAIT_TIME_FRAME,          WaitTimeFrameScriptData( 60) },
    //  わざ選択に指カーソルを押す
    { SCRIPT_TYPE_PUSH_FINGER_BUTTON,       PushFingerScriptData( FRAME_TO_SHOW_CURSOR, FRAME_TO_DECIDE, 87, 82, app::tool::FingerCursor::FORM_TYPE_DOWN ) },
    //  全力技の項目を押す
    { SCRIPT_TYPE_PUSH_CMD_SKILL_ITEM,      PushCmdSkillSelectScriptData( btl::BattleUISkillSelectFrame::CMD_TYPE_PUSH_BUTTON_ZENRYOKU_SKILL_BUTTON_1 ) },
    //  ターン終了
    { SCRIPT_TYPE_END_TURN,                 ScriptDataWork() },

    // 全力技の準備
    { SCRIPT_TYPE_ZNRYOKU_WAZA_BEGIN,       AttackZenryokuWazaBeginActionScriptData( WAZANO_URUTORADASSYUATAKKU, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF ) },
    // ○○が解き放つ全力のZ技
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataZenryoku( (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF ) ) },
    // 味方がわざを出す
    { SCRIPT_TYPE_ATTACK_SKILL,             AttackActionScriptData( WAZANO_URUTORADASSYUATAKKU, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF, 0.0f, true ) },
    // 全力技の完了
    { SCRIPT_TYPE_ZNRYOKU_WAZA_END,         AttackZenryokuWazaBeginActionScriptData( WAZANO_URUTORADASSYUATAKKU, (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF ) },
    // やせいの○○は倒れた
    { SCRIPT_TYPE_PUT_MESSAGE,              PutMessageScriptData( PutMessageScriptData::CreateDataDying( (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY ) ) },
    //　倒れる
    { SCRIPT_TYPE_DEAD_TURN,                DeadActionScriptData( (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY ) },
    // @fix GFNMCat[5396] 勝利BGMは再生しない
  };

//  ここから先は CaptureTutorialInitScene の定義
  //------------------------------------------------------------------------------
  // 初めてProcに入ってきた時のフェードイン
  //------------------------------------------------------------------------------
  void CaptureTutorialInitScene::Enter()
  {
    GFL_PRINT("enter:initscene\n");
    m_seq = 0;
  }

  bool CaptureTutorialInitScene::Update()
  {
    //  シーケンス定義
    enum
    {
      SEQ_CREATE_VIEWER = 0,
      SEQ_CREATE_VIEWER_WAIT,
      SEQ_SETUP_VIEWR,
      SEQ_SETUP_WAIT,
      SEQ_END
    };

    CaptureTutorialViewer* pCore = m_owner->GetCore();
    
    if( m_seq == SEQ_CREATE_VIEWER )
    {
      m_owner->CreateViewer();

      ++m_seq;
    }
    else
    if( m_seq == SEQ_CREATE_VIEWER_WAIT )
    {
      if( pCore->Initialize(m_owner->GetHeap()->GetDeviceHeap(), m_owner->GetTutorialType()) )
      {
        //  Viewerを作成したので更新を有効に
        m_owner->SetEnableUpdateViewer( true );

        ++m_seq;
      }
    }
    else
    if( m_seq == SEQ_SETUP_VIEWR )
    {
      GFL_ASSERT( pCore );
      pCore->CMD_StartSetup();

      //  準備が整ったので描画をONに
      m_owner->SetEnableDrawViewer( true );

      ++m_seq;
    }

    if( m_seq == SEQ_SETUP_WAIT )
    {
      GFL_ASSERT( pCore );

      if( pCore->CMD_WaitSetup() == true )
      {
        ++m_seq;
      }
    }

    if( m_seq == SEQ_END )
    {
      GFL_PRINT("Initialize Finished\n");
      m_owner->SetNext<CaptureTutorialMainScene>();
    }

    return true;
  }

  void CaptureTutorialInitScene::Exit()
  {
    GFL_PRINT("exit:initscene\n");
  }

//  ここから先は CaptureTutorialMainScene の定義
  //------------------------------------------------------------------------------
  void CaptureTutorialMainScene::Enter()
  {
    _Clear();

    //  スクリプト実行
    switch( m_owner->GetTutorialType() )
    {

    case CaptureTutorialViewer::CAPTURE:
      m_pEventScriptExe =
        GFL_NEW( m_owner->GetHeap()->GetDeviceHeap() ) 
          CaptureTutorialEventScriptExecution( 
          m_owner->GetHeap()->GetDeviceHeap(),
          _GetFrameRate(),
          m_owner->GetCore(),
          s_aScriptData,
          GFL_NELEMS(s_aScriptData)
        );
      break;


    case CaptureTutorialViewer::ZENRYOKU_WAZA:
      m_pEventScriptExe =
        GFL_NEW( m_owner->GetHeap()->GetDeviceHeap() ) 
          CaptureTutorialEventScriptExecution( 
          m_owner->GetHeap()->GetDeviceHeap(),
          _GetFrameRate(),
          m_owner->GetCore(),
          s_aZenryokuWazaScriptData,
          GFL_NELEMS(s_aZenryokuWazaScriptData)
        );
      break;
    }

    m_pEventScriptExe->Start();
  }

  bool CaptureTutorialMainScene::Update()
  {
    //  イベント実行中か
    if( ( m_pEventScriptExe == NULL ) || ( m_pEventScriptExe->Update() == false ) )
    {
      m_owner->SetNext<CaptureTutorialExitScene>();
    }

    return true;
  }

  void CaptureTutorialMainScene::Exit()
  {
    GFL_SAFE_DELETE( m_pEventScriptExe );
  }

  //  ゲーム内のフレーム数を取得
  u32  CaptureTutorialMainScene::_GetFrameRate()
  {
    const s32 aFrameRateList[][ 2 ]  =
    {
      { GameSys::FRAMEMODE_60,  60 },   ///< 1/60動作モード
      { GameSys::FRAMEMODE_30,  30 },   ///< 1/30動作モード
      { GameSys::FRAMEMODE_MAX, -1 },   ///< システム内部で使用
    };

    u32 retFrameRate  = 30;

    GameSys::FrameMode  frameMode = m_owner->GetGameManaager()->GetFrameMode();
    for( u32 i = 0; aFrameRateList[ i ][ 1 ] != -1; ++i )
    {
      if( frameMode == aFrameRateList[ i ][ 0 ] )
      {
        retFrameRate  = aFrameRateList[ i ][ 1 ];
        break;
      }
    }

    return  retFrameRate;
  }

//  ここから先は CaptureTutorialExitScene の定義
  //------------------------------------------------------------------------------
  void CaptureTutorialExitScene::Enter()
  {
    m_seq = 0;
  }

  bool CaptureTutorialExitScene::Update()
  {
    CaptureTutorialViewer* pCore = m_owner->GetCore();

    //  更新のシーケンス一覧
    enum
    {
    //  SEQ_FADE_WAIT = 0,
      SEQ_CLEANUP_START = 0,
      SEQ_CLEANUP_WAIT,
      SEQ_FINAL_VIEWR,
      SEQ_END
    };

    if( m_seq == SEQ_CLEANUP_START )
    {
      GFL_ASSERT( pCore );

      pCore->CMD_StartCleanUp();

      ++m_seq;
    }
    else
    if( m_seq == SEQ_CLEANUP_WAIT )
    {
      GFL_ASSERT( pCore );

      if( pCore->CMD_WaitCleanUp() )
      {
        //  終了処理をするので更新もしない
        m_owner->SetEnableUpdateViewer( false );
        ++m_seq;
      }
    }
    else
    if( m_seq == SEQ_FINAL_VIEWR )
    {
      GFL_ASSERT( pCore );
      if( pCore->Finalize() )
      {
        m_owner->DeleteViewer();
        ++m_seq;
      }
    }

    if( m_seq == SEQ_END )
    {
      m_owner->SetFinished();
    }

    return true;
  }

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)
