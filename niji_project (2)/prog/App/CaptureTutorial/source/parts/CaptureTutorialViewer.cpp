//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   CaptureTutorialViewer.cpp
 *  @author ito_isao
 *
 */
//====================================================================== 
#include "CaptureTutorialViewer.h"

//  nijiのインクルード
#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldStatic/include/Event/EventBattleCall.h"

//  メッセージのインクルード
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_capturedemo.h>
#include <niji_conv_header/message/msg_btl_attack.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  CaptureTutorialViewer::CaptureTutorialViewer() :
  btl::BattleViewSystem()
  {
    _Clear();
  }

  CaptureTutorialViewer::~CaptureTutorialViewer()
  {
    //ここでは行わずBattleViewSystemで一括して行います。
    //xy_snd::PopAndUnloadBattleGlobalSE();

    GFL_SAFE_DELETE( m_pFingerCursor );
    app::tool::FingerCursor::DeleteLytBinData( m_pFingerCursorLytBuffer );
    m_pFingerCursorLytBuffer  = NULL;

    // ダミーデータ片付け
    GFL_SAFE_DELETE( m_item );
  
    m_party[0]->Initialize();
    m_party[1]->Initialize();
  
    {
      u32 loopNum = GFL_NELEMS( m_btl_param );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_btl_param[ i ] );
      }
    }

    {
      u32 loopNum = GFL_NELEMS( m_param );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_param[ i ] );
      }
    }
  
    {
      u32 loopNum = GFL_NELEMS( m_party );
      for( u32 i = 0; i < loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_party[ i ] );
      }
    }

    {
      u32 loopNum = GFL_NELEMS( m_zenryokuWazaStatus );
      for( u32 i = 0; i< loopNum; ++i )
      {
        GFL_SAFE_DELETE( m_zenryokuWazaStatus[i] );
      }
    }
  
    // メッセージ用片付け
    GFL_SAFE_DELETE( m_str_buf );
    GFL_SAFE_DELETE( m_str_buf_tmp );
    GFL_SAFE_DELETE( m_wordset );
    GFL_SAFE_DELETE( m_demo_msg );
    GFL_SAFE_DELETE( m_std_msg );
    GFL_SAFE_DELETE( m_attack_msg );
    GFL_SAFE_DELETE( m_set_msg );
    GFL_SAFE_DELETE( m_ui_msg );
  }

  //------------------------------------------------------------------------------
  bool CaptureTutorialViewer::Initialize( gfl2::heap::HeapBase* heap, TutorialType type )
  {
    //  ドレスアップデータが必要なのだろうか？
 //   xy_system::dress_up::DressUpManagerCreate(heap,heap,dev);
    switch( m_init_seq )
    {
    case 0:
      m_TutorialType = type;

      BATTLE_SETUP_Init( &m_setup );


      {
        u32 loopNum = GFL_NELEMS( m_party );
        for(u32 i = 0; i < loopNum; ++i)
        {
          m_party[i] = GFL_NEW( heap ) btl::BTL_PARTY();
        }
      }

      switch( m_TutorialType )
      {
      case CAPTURE:
        this->setupCapture(heap);
        this->SetIsZenryokuTutorial(false);
        break;

      case ZENRYOKU_WAZA:
        this->setupZenryokuWaza(heap);
        this->SetIsZenryokuTutorial(true);
        break;
      }


      //  シングル用
      m_rule  = BTL_RULE_SINGLE;



      // ダミー用アイテムデータの作成
      {
        // @fix NMCat[1680] この後のイベントで主人公にあげるアイテムを、あげる数より多く所持させる
        const u32 MONSTERBALL_NUM = 50;
        const u32 KIZUGUSURI_NUM = 20;
        m_item = GFL_NEW( heap ) Savedata::MyItem();
        m_item->Clear( heap );
        m_item->Add( ITEM_MONSUTAABOORU, MONSTERBALL_NUM );
        m_item->Add( ITEM_KIZUGUSURI, KIZUGUSURI_NUM );
      }



      // デモ用メッセージデータの初期化
      {
        m_demo_msg    = GFL_NEW( heap ) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_capturedemo_DAT ,heap, gfl2::str::MsgData::LOAD_PART);
        m_std_msg     = GFL_NEW( heap ) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_btl_std_DAT ,heap, gfl2::str::MsgData::LOAD_PART);
        m_attack_msg  = GFL_NEW( heap ) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_btl_attack_DAT ,heap, gfl2::str::MsgData::LOAD_PART);
        m_set_msg     = GFL_NEW( heap ) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_btl_set_DAT ,heap, gfl2::str::MsgData::LOAD_PART);
        m_ui_msg      = GFL_NEW( heap ) gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_btl_ui_DAT ,heap, gfl2::str::MsgData::LOAD_PART);
      }

      // strbufを確保
      {
        m_str_buf     = GFL_NEW( heap ) gfl2::str::StrBuf( 128, heap );
        m_str_buf_tmp = GFL_NEW( heap ) gfl2::str::StrBuf( 128, heap );
      }

      // wordsetも初期化する
      m_wordset = GFL_NEW( heap ) print::WordSet( heap );

      // サウンドの初期化
      m_sound.Initialize();

      m_init_seq++;
      //break; //スルー(しないと止まる)
    case 1:
      //ここでは行わずBattleViewSystemで一括して行います。
      if( BattleViewSystem::Initialize( heap ) == false )
      {
        return false;
      }
      m_init_seq++;
      //break; //スルー
    case 2:

  #if PM_DEBUG
      BattleViewSystem::SetDebugParam( &m_btlDebugParam );
  #endif

      //  指カーソルレイアウト作る
      {
        m_pFingerCursorLytBuffer  = app::tool::FingerCursor::CreateLytBinData( m_util_heap );
        m_pFingerCursor = GFL_NEW( heap ) app::tool::FingerCursor( m_util_heap, m_pFingerCursorLytBuffer, mRenderSys->GetAppRenderingManager(), app::util::G2DUtil::SETUP_LOWER, 200 );
      }

      m_init_seq++;
      //break; //スルー
    case 3:
      return true;
    }
    return false;
  }

  //  システム終了
  bool CaptureTutorialViewer::Finalize(void)
  {
    bool  bRet  = true;
    
    if( btl::BattleViewSystem::Finalize() == false )
    {
      bRet  = false;
    }

    if( m_pFingerCursor->IsDrawing() == true )
    {
      bRet  = false;
    }

    return  bRet;
  }

  //更新(30フレーム前半用)
  void CaptureTutorialViewer::Update()
  {
    if( m_TutorialType == ZENRYOKU_WAZA )
    {
      m_sound.Main();
    }

    btl::BattleViewSystem::Update();

    if( m_pFingerCursor != NULL )
    {
      m_pFingerCursor->UpdateTree();
    }
  }

  //描画更新
  void CaptureTutorialViewer::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    //  手前に出すように描画優先度を高めに
    m_pFingerCursor->DrawTree( displayNo );

    BattleViewSystem::Draw( displayNo );
  }

  //------------------------------------------------------------------------------
  // 各種オーバーライド
  //------------------------------------------------------------------------------
  bool CaptureTutorialViewer::IsPlayerSide(btl::BtlvPos pos)
  {
    return ( pos == btl::BTL_VPOS_NEAR_1 );
  }

  void CaptureTutorialViewer::_MakeYaseiEncountMessage( gfl2::str::StrBuf* pOutputStrBuf )
  {
    if( pOutputStrBuf == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    // やせいの丸丸が飛び出してきた

    // メッセージを取得
    // ポケモン名をセット
    if( m_TutorialType == CAPTURE )
    {
      this->RegisterPokeName( 0, DEMO_MONSNO_ENEMY );
      m_std_msg->GetString( BTL_STRID_STD_Encount_Wild_Move, *m_str_buf_tmp );
    }
    else
    {
      this->RegisterPokeName( 0, ZENRYOKU_DEMO_MONSNO_ENEMY );
      m_std_msg->GetString( BTL_STRID_STD_Encount_Wild1, *m_str_buf_tmp );
    }

    // expand
    m_wordset->Expand( pOutputStrBuf, m_str_buf_tmp );
  }

  void CaptureTutorialViewer::_MakeSingleGoMessage( gfl2::str::StrBuf* pOutputStrBuf )
  {
    if( pOutputStrBuf == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( m_demo_msg == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    // ライバルは丸丸を繰り出した

    // メッセージを取得
    // ポケモン名をセット
    RegisterWord( 0, m_demo_msg, msg_capture_demo_00 );

    if( m_TutorialType == CAPTURE )
    {
      RegisterPokeName( 1, DEMO_MONSNO_SELF );
    }
    else
    {
      RegisterPokeName( 1, ZENRYOKU_DEMO_MONSNO_SELF );
    }

    m_std_msg->GetString( BTL_STRID_STD_PutSingle_Player, *m_str_buf_tmp );

    // expand
    m_wordset->Expand( pOutputStrBuf, m_str_buf_tmp );
  }

  void CaptureTutorialViewer::_MakeSimplePokeNameExpandMessage( gfl2::str::StrBuf* pOutputStrBuf, gfl2::str::MsgData* pMsgData, int msgId, MonsNo monsNo )
  {
    if( pOutputStrBuf == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( pMsgData == NULL )
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( monsNo == MONSNO_NULL)
    {
      GFL_ASSERT( 0 );
      return;
    }

    if( msgId >= pMsgData->GetStrCount() )
    {
      GFL_ASSERT( 0 );
      return;
    }

    RegisterPokeName( 0, monsNo );
    pMsgData->GetString( msgId, *m_str_buf_tmp );
    m_wordset->Expand( pOutputStrBuf, m_str_buf_tmp );
  }

  //------------------------------------------------------------------------------
  // 各種便利
  //------------------------------------------------------------------------------
  void CaptureTutorialViewer::OpenActionSelect()
  {
    BattleViewSystem::CMD_UI_SelectAction_Start( m_btl_param[POKE_INDEX_SELF], 0, false, &m_btl_action_param );
    mUiSys->StartAnime_SelectCommand();
  }

  //  行動選択画面が開いているか
  bool CaptureTutorialViewer::IsOpenActionSelect()
  {
    //  待機中==開いていると扱う
    if( mUiSys->GetStateActSelectFrame() == btl::BtlvUi::UI_FRAME_STAET_IDLE )
    {
      return  true;
    }

    return  false;
  }

  //  行動選択のコマンド通知
  bool CaptureTutorialViewer::PushCmdActionSelect( const btl::BattleUIActSelectFrame::CmdType type )
  {
    return  mUiSys->PushCmdActSelectCommnad( type );
  }

  //  わざ選択画面を開く
  void CaptureTutorialViewer::OpenWazaSelect()
  {
    BattleViewSystem::CMD_UI_SelectWaza_Start( m_btl_param[POKE_INDEX_SELF], 0, &m_btl_action_param );
    mUiSys->StartAnime_SelectSkill(0);
  }

  //  わざ選択画面がきっちり開いているか
  bool CaptureTutorialViewer::IsOpenWazaSelect()
  {
    //  待機中==開いていると扱う
    if( mUiSys->GetStateSkillSelectFrame() == btl::BtlvUi::UI_FRAME_STAET_IDLE )
    {
      return  true;
    }
    
    return false;
  }

  //  技選択のコマンド通知
  bool CaptureTutorialViewer::PushCmdSkillSelect( const btl::BattleUISkillSelectFrame::CmdType type )
  {
    return  mUiSys->PushCmdSkillSelectCommnad( type );
  }

  void CaptureTutorialViewer::StartWazaSelectAnime()
  {
    mUiSys->ForceSelect_Skill();
  }

  bool CaptureTutorialViewer::IsFinishWazaSelectAnime()
  {
    return BattleViewSystem::CMD_UI_SelectWaza_Wait();
  }

  void CaptureTutorialViewer::CloseActionSelect()
  {
    BattleViewSystem::CMD_UI_Restart();
  }

  bool CaptureTutorialViewer::IsFinishCloseActionSelect()
  {
    return BattleViewSystem::CMD_UI_WaitRestart();
  }

  void CaptureTutorialViewer::StartWazaEffectAttack( bool down_atk)
  {
    const pml::pokepara::PokemonParam* pokePara = m_btl_param[POKE_INDEX_ENEMY]->GetSrcData( );
    u16 minusHp = 0;

    // 捕獲チュートリアルでは
    // 攻撃後、鳴き声をされ、もう一度攻撃という流れが確定なので適当にダメージ量を調節…。(sangoの場合)
    if( down_atk)
    {
      minusHp = ( static_cast<u16>(pokePara->GetMaxHp()) / 2 ) - 3;
    }
    else
    {
      minusHp = ( static_cast<u16>(pokePara->GetMaxHp()) / 2 ) - 1;
    }

    m_btl_param[POKE_INDEX_ENEMY]->HpMinus( minusHp);
    BattleViewSystem::CMD_ACT_WazaEffect_Start( POKE_INDEX_SELF, POKE_INDEX_ENEMY, WAZANO_TAIATARI, pml::wazadata::TARGET_OTHER_SELECT, btl::BTLV_WAZAEFF_INDEX_DEFAULT, 0, true );

    // STS[484]:PP減らす
    m_btl_param[POKE_INDEX_SELF]->WAZA_DecrementPP( 0, 1 );
  }

  void CaptureTutorialViewer::StartDamageAttack()
  {
    BattleViewSystem::CMD_ACT_DamageEffectSingle_Start(WAZANO_TAIATARI, 1, pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL);
  }

  void CaptureTutorialViewer::StartWazaEffectDefense()
  {
    BattleViewSystem::CMD_ACT_WazaEffect_Start(1, 0, WAZANO_NAKIGOE, pml::wazadata::TARGET_OTHER_SELECT, btl::BTLV_WAZAEFF_INDEX_DEFAULT, 0, true );
  }

  void CaptureTutorialViewer::StartDamageDefense()
  {
    BattleViewSystem::CMD_StartRankDownEffect((btl::BtlvPos)0, 0);
  }

  void CaptureTutorialViewer::OpenItemSelect()
  {
    mUiSys->ForceSelect_Bag();
  }

  bool CaptureTutorialViewer::IsFinishOpenBag()
  {
  /*
    if(BattleViewSystem::CMD_UI_SelectAction_Wait())
    {
      BattleViewSystem::CMD_ITEMSELECT_Start(0,0,0,0,0);
      return true;
    }
    return false;
    */

    //  待機中==開いていると扱う
    if( mUiSys->GetStateBagFrame() == btl::BtlvUi::UI_FRAME_STAET_IDLE )
    {
      return  true;
    }
    
    return false;
  }

  void CaptureTutorialViewer::OpenBallSelect()
  {
    mUiSys->ForceSelect_BallCategory();
  }

  void CaptureTutorialViewer::OpenUseItem()
  {
    mUiSys->ForceSelect_ItemUse();
  }

  void CaptureTutorialViewer::CloseBag()
  {
    mUiSys->ForceQuit_Bag();
//    mUiSys->ForceSelect_ItemUse();
  }

  bool CaptureTutorialViewer::IsFinishCloseBag()
  {
    if(BattleViewSystem::CMD_ITEMSELECT_Wait())
    {
//      BattleViewSystem::CMD_UI_Restart();
//      BattleViewSystem::CMD_UI_WaitRestart();
      return true;
    }
    return false;
  }

  bool CaptureTutorialViewer::IsFinishEndCamera()
  {
    if(BattleViewSystem::CMD_UI_WaitRestart())
    {
      return true;
    }
    return false;
  }

  void CaptureTutorialViewer::StartThrowBall()
  {
    BattleViewSystem::CMD_EFFECT_BallThrow(1, ITEM_MONSUTAABOORU, 3, true, false);
  }

  bool CaptureTutorialViewer::IsFinishThrowBall()
  {
    return  BattleViewSystem::CMD_EFFECT_WaitBallThrow();
  }

  //  指カーソルの開始
  void CaptureTutorialViewer::StartFingerCursor( gfl2::math::VEC3& rPos, const app::tool::FingerCursor::FormType formType )
  {
    GFL_ASSERT( m_pFingerCursor );

    m_pFingerCursor->SetVisible( true );
    m_pFingerCursor->StartAnime( formType );
    m_pFingerCursor->SetPos( rPos );
  }

  //  指カーソルの押すアニメ開始
  void CaptureTutorialViewer::StartPushFingerCursorAnime( const app::tool::FingerCursor::FormType formType )
  {
    GFL_ASSERT( m_pFingerCursor );

    m_pFingerCursor->SetVisible( true );
    m_pFingerCursor->PushAnime( formType );
  }

  //  指カーソルの押すアニメ終了しているか
  bool CaptureTutorialViewer::IsEndAnimePushFingerCursor( const app::tool::FingerCursor::FormType formType )
  {
    GFL_ASSERT( m_pFingerCursor );
    if( m_pFingerCursor->IsEndPushAnime( formType ) )
    {
      m_pFingerCursor->SetVisible( false );
      return  true;
    }

    return false;
  }

  void CaptureTutorialViewer::RegisterPokeName(int id, MonsNo poke)
  {
    m_wordset->RegisterPokeMonsNameNo(id, poke);
  }

  void CaptureTutorialViewer::RegisterWazaName(int id, u32 waza)
  {
    m_wordset->RegisterWazaName(id, waza);
  }

  void CaptureTutorialViewer::RegisterWord( int id, gfl2::str::MsgData* msgData, u32 msgId)
  {
    m_wordset->RegisterWord( id, msgData, msgId);
  }

  void CaptureTutorialViewer::GetStringUi( gfl2::str::StrBuf* dst, u16 strID )
  {
    m_ui_msg->GetString(strID, *dst);
  }

  void CaptureTutorialViewer::PlayMessage(int id)
  {
    // メッセージを取得
    m_std_msg->GetString( id, *m_str_buf );
    // expand
    m_wordset->Expand( m_str_buf_tmp, m_str_buf );
    mUiSys->StartDispMessage( m_str_buf_tmp );
  }

  void CaptureTutorialViewer::PlayDemoMessage(int id)
  {
    if( m_demo_msg == NULL )
    {
      return;
    }

    // メッセージを取得
    m_demo_msg->GetString( id, *m_str_buf );
    // expand
    m_wordset->Expand( m_str_buf_tmp, m_str_buf );
    mUiSys->StartDispMessage( m_str_buf_tmp );
  }

  void CaptureTutorialViewer::PlayWazaMessage(bool self, int waza)
  {
    int id = 0;
    if(self)
    {
      id = waza * 4 + 0;
    }
    else
    {
      id = waza * 4 + 1;
    }

    // メッセージを取得
    m_attack_msg->GetString(id, *m_str_buf);
    // expand
    m_wordset->Expand(m_str_buf_tmp, m_str_buf);
    mUiSys->StartDispMessage(m_str_buf_tmp);
  }

  void CaptureTutorialViewer::PlayDownMessage()
  {
    // メッセージを取得
    m_set_msg->GetString( BTL_STRID_SET_Rankdown_ATK, *m_str_buf );
    // expand
    m_wordset->Expand( m_str_buf_tmp, m_str_buf );
    mUiSys->StartDispMessage( m_str_buf_tmp );
  }

  void CaptureTutorialViewer::PlayDyingMessage()
  {
    // メッセージを取得
    m_set_msg->GetString( BTL_STRID_SET_Dead_Y, *m_str_buf );
    // expand
    m_wordset->Expand( m_str_buf_tmp, m_str_buf );
    mUiSys->StartDispMessage( m_str_buf_tmp );
  }

  void CaptureTutorialViewer::PlayZenryokuMessage()
  {
    // メッセージを取得
    m_set_msg->GetString( BTL_STRID_SET_ZenryokuPower, *m_str_buf );
    // expand
    m_wordset->Expand( m_str_buf_tmp, m_str_buf );
    mUiSys->StartDispMessage( m_str_buf_tmp );
  }

  bool CaptureTutorialViewer::IsFinishMessage()
  {
    return BattleViewSystem::CMD_WaitMsg();
  }

  bool CaptureTutorialViewer::IsFinishMessageWithoutHide()
  {
    return BattleViewSystem::CMD_WaitMsg_WithoutHide();
  }

  void CaptureTutorialViewer::HideMessage()
  {
    mUiSys->HideMessage();
  }

  Savedata::MyItem* CaptureTutorialViewer::GetMyItem() const
  {
    return m_item;
  }

  const Savedata::MyStatus* CaptureTutorialViewer::GetClientPlayerData(u8 id, int btlFesIdx) const
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    GameSys::GameData* pGameData        = pGameManager->GetGameData();
    GFL_ASSERT( pGameData );

    return  pGameData->GetPlayerStatus();
  }


  // 全力技ステータスを取得する。
  const btl::ZenryokuWazaStatus* CaptureTutorialViewer::GetZenryokuWazaStatus( u8 clientID ) const
  {
    if( clientID < POKE_INDEX_MAX )
    {
      return m_zenryokuWazaStatus[clientID];
    }
    else
    {
      GFL_ASSERT_STOP(clientID < POKE_INDEX_MAX);
      return NULL;
    }
  }

  // GFSBTS[509]:MainModuleがない捕獲チュートリアルは継承して実装する
  const BSP_ENCOUNT_EFFECT* CaptureTutorialViewer::GetEncountEffect( void )
  {
    return &m_setup.btlEffData.enceff;
  }
 
  //バトルのメッセージがMainModuleに依存しているので、捕獲チュートリアルなどで出す場合の関数
  // @param msgId メッセージID(btl_std.h)
  // @param str   表示する文字列
  void CaptureTutorialViewer::SeqComFunc_SetEffectMessage_Std(int msgId , gfl2::str::StrBuf* str)
  {
    GFL_ASSERT_STOP( str != NULL );    // 全力技開始のメッセージ

    if( msgId == BTL_STRID_STD_Encount_Wild1 )
    {
      //野生の○○が現れた
      _MakeYaseiEncountMessage( str );
    }
    else
    if( msgId == BTL_STRID_STD_PutSingle )
    {
      _MakeSingleGoMessage( str );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }

  void CaptureTutorialViewer::SeqComFunc_SetEffectMessage_Set(int msgId , gfl2::str::StrBuf* str)
  {
    GFL_ASSERT_STOP( str != NULL );

    // ○○はZパワーを体に纏った
    if( msgId == BTL_STRID_SET_AuraEffect_None )
    {
      _MakeSimplePokeNameExpandMessage( str , m_set_msg, BTL_STRID_SET_AuraEffect_None, ZENRYOKU_DEMO_MONSNO_SELF );
    }
    else
    {
      GFL_ASSERT( 0 );
    }
  }


// TutorialTypeごとのSetup
/**
 *  @brief 捕獲デモの設定
 */
void CaptureTutorialViewer::setupCapture( gfl2::heap::HeapBase* heap )
{
  //=====================================================================================================

  // GFSBTS[509]:バトルエフェクトを取得
  BATTLE_EFFECT_GetParam( BATTLE_EFFECT_WILD_GRASS, &m_setup.btlEffData );

  // 捕獲デモでは、プレイヤーは草の外
  {
    const Field::ZoneDataLoader* pZoneDataLoader  = NULL;
    Field::SetUpFieldSituation( &m_setup.fieldSituation , GFL_SINGLETON_INSTANCE(GameSys::GameManager), &pZoneDataLoader, Field::ATTR_ERROR, btl::bg::FAR_TYPE_Z_F0102_S0201, btl::bg::NEAR_TYPE_BUSH1 );
  }


  // 手持ちとかバトルの設定を行う

  //  SBTS[2586]:レアカラーを出さない設定でポケモンパラメータを作成する
  //  味方側のポケモンパラメータ設定
  {
    pml::pokepara::InitialSpec initialSpec_mons1;
    initialSpec_mons1.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE; // レアにしない
    initialSpec_mons1.monsno = DEMO_MONSNO_SELF;
    initialSpec_mons1.level = 7;
    initialSpec_mons1.id = 0;

    m_param[ POKE_INDEX_SELF ] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, initialSpec_mons1);

    // チュートリアルキャラのイワンオのオス
    m_param[ POKE_INDEX_SELF ]->ChangeSex( pml::SEX_MALE );

    // 技変更
    m_param[ POKE_INDEX_SELF ]->SetWaza( 0, WAZANO_TAIATARI);
    // @fix NMCat[343] なきごえ　－＞　にらみつける　に変更
    m_param[ POKE_INDEX_SELF ]->SetWaza( 1, WAZANO_NIRAMITUKERU);
    m_param[ POKE_INDEX_SELF ]->SetWaza( 2, WAZANO_SUNAKAKE);
    m_param[ POKE_INDEX_SELF ]->SetWaza( 3, WAZANO_KAMITUKU);

    // 固体値
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_HP, 0);
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_ATK, 31);
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_DEF, 0);
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_SPATK, 0);
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_SPDEF, 0);
    m_param[POKE_INDEX_SELF]->ChangeTalentPower(pml::pokepara::POWER_AGI, 0);

    // 努力値
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_HP, 0);
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_ATK, 255);
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_DEF, 0);
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_SPATK, 0);
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_SPDEF, 0);
    m_param[POKE_INDEX_SELF]->ChangeEffortPower(pml::pokepara::POWER_AGI, 0);

    btl::BTL_POKEPARAM::SetupParam setupParam;
    setupParam.srcParam    = m_param[POKE_INDEX_SELF];
    setupParam.pokeID      = 0;
    setupParam.friendship  = 0;
    setupParam.fieldStatus = NULL;
    DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

    m_btl_param[POKE_INDEX_SELF] = GFL_NEW(heap) btl::BTL_POKEPARAM();
    m_btl_param[POKE_INDEX_SELF]->Setup( setupParam );

    m_party[POKE_INDEX_SELF]->AddMember(m_btl_param[POKE_INDEX_SELF]);
  }

  //  敵側のポケモンパラメータ設定
  {
    pml::pokepara::InitialSpec initialSpec_mons2;
    initialSpec_mons2.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE; // レアにしない
    initialSpec_mons2.monsno = DEMO_MONSNO_ENEMY;
    initialSpec_mons2.level = 3;
    initialSpec_mons2.id = 0;

    m_param[ POKE_INDEX_ENEMY ] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, initialSpec_mons2);

    // 技変更
    m_param[POKE_INDEX_ENEMY]->SetWaza( 0, WAZANO_HASAMU);

    // 野生のツツケラはオス
    m_param[ POKE_INDEX_ENEMY ]->ChangeSex( pml::SEX_MALE );

    btl::BTL_POKEPARAM::SetupParam setupParam;
    setupParam.srcParam    = m_param[POKE_INDEX_ENEMY];
    setupParam.pokeID      = 1;
    setupParam.friendship  = 0;
    setupParam.fieldStatus = NULL;
    DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

    m_btl_param[POKE_INDEX_ENEMY] = GFL_NEW(heap) btl::BTL_POKEPARAM();
    m_btl_param[POKE_INDEX_ENEMY]->Setup( setupParam );

    m_party[POKE_INDEX_ENEMY]->AddMember(m_btl_param[POKE_INDEX_ENEMY]);
  }

  //=====================================================================================================
}

/**
 *  @brief 全力技デモの設定
 */
void CaptureTutorialViewer::setupZenryokuWaza( gfl2::heap::HeapBase* heap )
{
  //=====================================================================================================


  // GFSBTS[509]:バトルエフェクトを取得
  BATTLE_EFFECT_GetParam( BATTLE_EFFECT_WILD_GRASS, &m_setup.btlEffData );

  // 背景設定
  // 外背景 : 2番道路
  // 内背景 : 草むら
  {
    const Field::ZoneDataLoader* pZoneDataLoader  = NULL;
    Field::SetUpFieldSituation( &m_setup.fieldSituation , GFL_SINGLETON_INSTANCE(GameSys::GameManager), &pZoneDataLoader, Field::ATTR_ERROR, btl::bg::FAR_TYPE_Z_F0102_S0401, btl::bg::NEAR_TYPE_BUSH1 );
  }

  // 手持ちとかバトルの設定を行う

  //  SBTS[2586]:レアカラーを出さない設定でポケモンパラメータを作成する
  //  味方側のポケモンパラメータ設定
  //
  // イワンコ レベル１０ 性別♂
  // レベル１ たいあたり （ウルトラダッシュアタック）
  // レベル１ にらみつける
  // レベル４ すなかけ
  // レベル７ かみつく
  // 性格 がんばりや
  // 固体値 オール０
  // 特性 するどいめ
  // 持ち物 ノーマルビーズ
  {
    pml::pokepara::InitialSpec initialSpec_mons1;
    initialSpec_mons1.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE; // レアにしない
    initialSpec_mons1.monsno = ZENRYOKU_DEMO_MONSNO_SELF;
    initialSpec_mons1.level = 10;
    initialSpec_mons1.id = 0;

    m_param[ POKE_INDEX_SELF ] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, initialSpec_mons1);

    // 性別変更
    m_param[ POKE_INDEX_SELF ]->ChangeSex( pml::SEX_MALE );

    // 技変更
    m_param[ POKE_INDEX_SELF ]->SetWaza( 0, WAZANO_TAIATARI);
    m_param[ POKE_INDEX_SELF ]->SetWaza( 1, WAZANO_NIRAMITUKERU);
    m_param[ POKE_INDEX_SELF ]->SetWaza( 2, WAZANO_SUNAKAKE);
    m_param[ POKE_INDEX_SELF ]->SetWaza( 3, WAZANO_KAMITUKU);
    
    // 持ち物
    m_param[ POKE_INDEX_SELF ]->SetItem( ITEM_NOOMARUBIIZU_1 );

    // 特性
    m_param[ POKE_INDEX_SELF ]->SetTokuseiIndex( 0 );

    // 固体値
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_HP, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_ATK, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_DEF, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_SPATK, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_SPDEF, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeTalentPower( pml::pokepara::POWER_AGI, 0);

    // 努力値
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_HP, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_ATK, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_DEF, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_SPATK, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 0);
    m_param[ POKE_INDEX_SELF ]->ChangeEffortPower( pml::pokepara::POWER_AGI, 0);

    btl::BTL_POKEPARAM::SetupParam setupParam;
    setupParam.srcParam    = m_param[POKE_INDEX_SELF];
    setupParam.pokeID      = 0;
    setupParam.friendship  = 0;
    setupParam.fieldStatus = NULL;
    DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

    m_btl_param[POKE_INDEX_SELF] = GFL_NEW(heap) btl::BTL_POKEPARAM();
    m_btl_param[POKE_INDEX_SELF]->Setup( setupParam );

    m_party[POKE_INDEX_SELF]->AddMember(m_btl_param[POKE_INDEX_SELF]);

    // 全力リングを持たせる
    btl::ZenryokuWazaStatus::SetupParam param;
    param.haveZenryokuRing = true;
    m_zenryokuWazaStatus[ POKE_INDEX_SELF ] = GFL_NEW(heap) btl::ZenryokuWazaStatus(param);
  }

  // ツツケラを手持ちに加える
  // 場に出ることはないので最低限の設定のみ行う
  {
    pml::pokepara::InitialSpec initialSpec_mons3;
    initialSpec_mons3.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE; // レアにしない
    initialSpec_mons3.monsno = MONSNO_KUTIBASI1;
    initialSpec_mons3.level = 3;
    initialSpec_mons3.id = 0;

    m_param[ POKE_INDEX_SELF_2 ] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, initialSpec_mons3);

    btl::BTL_POKEPARAM::SetupParam setupParam;
    setupParam.srcParam    = m_param[POKE_INDEX_SELF_2];
    setupParam.pokeID      = 1;
    setupParam.friendship  = 0;
    setupParam.fieldStatus = NULL;
    DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

    m_btl_param[POKE_INDEX_SELF_2] = GFL_NEW(heap) btl::BTL_POKEPARAM();
    m_btl_param[POKE_INDEX_SELF_2]->Setup( setupParam );

    m_party[POKE_INDEX_SELF]->AddMember( m_btl_param[POKE_INDEX_SELF_2] );
  }

  //  敵側のポケモンパラメータ設定
  //
  // ガーディ レベル７ 性別♀
  // レベル１ かみつく
  // レベル１ ほえる
  // レベル６ ひのこ
  // 性格 がんばりや
  // 固体値 オール０
  // 特性 もらいび
  // 持ち物 なし
  {
    pml::pokepara::InitialSpec initialSpec_mons2;
    initialSpec_mons2.rare_rnd = pml::pokepara::INIT_SPEC_RARE_FALSE; // レアにしない
    initialSpec_mons2.monsno = ZENRYOKU_DEMO_MONSNO_ENEMY;
    initialSpec_mons2.level = 7;
    initialSpec_mons2.id = 0;

    m_param[ POKE_INDEX_ENEMY ] = GFL_NEW(heap) pml::pokepara::PokemonParam(heap, initialSpec_mons2);

    // 技変更
    m_param[POKE_INDEX_ENEMY]->SetWaza( 0, WAZANO_KAMITUKU);
    m_param[POKE_INDEX_ENEMY]->SetWaza( 1, WAZANO_HOERU);
    m_param[POKE_INDEX_ENEMY]->SetWaza( 2, WAZANO_HINOKO);

    // 性別変更
    m_param[ POKE_INDEX_ENEMY ]->ChangeSex( pml::SEX_FEMALE );

    // 持ち物
    m_param[ POKE_INDEX_ENEMY ]->SetItem( ITEM_DUMMY_DATA );

    // 特性
    m_param[ POKE_INDEX_ENEMY ]->SetTokuseiIndex( 1 );

    // 固体値
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_HP, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_ATK, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_DEF, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_SPATK, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_SPDEF, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeTalentPower( pml::pokepara::POWER_AGI, 0);

    // 努力値
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_HP, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_ATK, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_DEF, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_SPATK, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_SPDEF, 0);
    m_param[ POKE_INDEX_ENEMY ]->ChangeEffortPower( pml::pokepara::POWER_AGI, 0);


    btl::BTL_POKEPARAM::SetupParam setupParam;
    setupParam.srcParam    = m_param[POKE_INDEX_ENEMY];
    setupParam.pokeID      = 1;
    setupParam.friendship  = 0;
    setupParam.fieldStatus = NULL;
    DEFAULT_POWERUP_DESC_Clear( &setupParam.defaultPowerUpDesc );

    m_btl_param[POKE_INDEX_ENEMY] = GFL_NEW(heap) btl::BTL_POKEPARAM();
    m_btl_param[POKE_INDEX_ENEMY]->Setup( setupParam );

    m_party[POKE_INDEX_ENEMY]->AddMember(m_btl_param[POKE_INDEX_ENEMY]);
  }

  //=====================================================================================================

}




GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)
