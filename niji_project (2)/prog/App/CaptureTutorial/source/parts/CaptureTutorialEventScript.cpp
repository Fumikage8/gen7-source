//============================================================================================
/**
 * @file    CaptureTutorialEventScript.h
 * @brief   捕獲チュートリアルのイベントスクリプト
 * @author  yuto uchida.
 * @date    15.06.02
 */
//============================================================================================
#include "CaptureTutorialEventScript.h"

#include "Battle/source/btlv/btlv_ui.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

    //  指カーソルステート
  class FingerCursorEvent
  {
  private:
    enum Stata
    {
      STATE_NONE  = 0,
      STATE_INIT,
      STATE_MOVE_POS,
      STATE_RUN_ANIM,
      STATE_WAIT_ANIM
    };

  public:
    FingerCursorEvent()
    {
      _Clear();
    }

    //  クリックイベントを開始
    void StartClick( f32 showFrame, f32 enterFrame, gfl2::math::VEC3& rMovePos, const app::tool::FingerCursor::FormType formType )
    {
      m_frameCount  = 0;
      m_showFrame   = showFrame;
      m_enterFrame  = enterFrame;
      m_pos         = rMovePos;
      m_state       = STATE_INIT;
      m_formType    = formType;
    }

    //  更新（必ず毎フレーム呼ぶ）
    void Update( CaptureTutorialViewer* pViewr )
    {
       GFL_ASSERT( pViewr );

      if( m_state == STATE_NONE )
      {
        return;
      }
        
      ++m_frameCount;

      if( m_state == STATE_INIT )
      {
        pViewr->StartFingerCursor( m_pos, m_formType );
        m_state = STATE_MOVE_POS;
      }

      if( m_state == STATE_MOVE_POS )
      {
        if( m_showFrame <= m_frameCount )
        {
          m_frameCount  = 0;

          m_state = STATE_RUN_ANIM;
        }
      }

      if( m_state == STATE_RUN_ANIM )
      {
        if( m_enterFrame <= m_frameCount )
        {
          pViewr->StartPushFingerCursorAnime( m_formType );

          m_frameCount  = 0;

          m_state = STATE_WAIT_ANIM;
        }
      }

      if( m_state == STATE_WAIT_ANIM )
      {
        if( pViewr->IsEndAnimePushFingerCursor( m_formType ) )
        {
          m_frameCount  = 0;
          m_state = STATE_NONE;
        }
      }
    }

    //  イベントが終了しているか
    bool IsEnd()
    {
      return ( m_state == STATE_NONE );
    }

  private:
    void _Clear()
    {
      m_frameCount  = 0;
      m_showFrame   = 0;
      m_enterFrame  = 0;
      m_state       = STATE_NONE;
      m_formType    = app::tool::FingerCursor::FORM_TYPE_DOWN;
    }

    u32                     m_frameCount;
    u32                     m_showFrame;
    u32                     m_enterFrame;
    gfl2::math::VEC3        m_pos;
    Stata                   m_state;
    
    app::tool::FingerCursor::FormType m_formType;
  };

//  チュートリアルの簡易スクリプトクラス作成

  //  簡易イベントスクリプト基本クラス
  class SimpleEventScriptBase
  {
  public:
    SimpleEventScriptBase() { _Clear(); GFL_ASSERT( 0 ); }

    SimpleEventScriptBase( CaptureTutorialViewer* pViewr )
    {
      _Clear();

      GFL_ASSERT( pViewr );
      m_pViewr = pViewr;
    }

    bool IsEnd() const { return m_bEnd; }
    virtual void Update() = 0;

  protected:
    CaptureTutorialViewer*  m_pViewr;

    u32   m_seq;
    bool  m_bEnd;

  private:
    void _Clear()
    {
      m_pViewr  = NULL;
      m_seq   = 0;
      m_bEnd  = false;
    }
  };

  //  バトルターン開始
  class StartBattleTurnEventScript : public SimpleEventScriptBase
  {
  public:
    StartBattleTurnEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
    }

    virtual void Update()
    {
      enum
      {
        SEQ_INIT_START_CAM_EFFECT  = 0,
        SEQ_WAIT_WAIT_CAM_EFFECT,
        SEQ_OPEN_ACT_SELECT,
        SEQ_WAIT_ACT_SELECT
      };

      //  初回演出を終わらせる
      //  この演出処理で行動選択の初期化関数が呼ばれている
      if( m_seq == SEQ_INIT_START_CAM_EFFECT )
      {
        if( m_pViewr->CMD_InitStartWaitCameraEffect() )
        {
          ++m_seq;
        }
      }
      else
      if( m_seq == SEQ_WAIT_WAIT_CAM_EFFECT )
      {
        if( m_pViewr->CMD_WaitStartWaitCameraEffect() )
        {
          ++m_seq;
        }
      }
      else
      if( m_seq == SEQ_OPEN_ACT_SELECT )
      {
        m_pViewr->OpenActionSelect();

        ++m_seq;
      }
      //  行動選択が使える状態かチェック
      if( m_seq == SEQ_WAIT_ACT_SELECT )
      {
        if( m_pViewr->IsOpenActionSelect() == true )
        {
          m_bEnd  = true;
          ++m_seq;
        }
      }
    }
  };

  //  バトルターン終了
  class EndBattleTurnEventScript : public SimpleEventScriptBase
  {
  public:
    EndBattleTurnEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
    }

    virtual void Update()
    {
      enum
      {
        SEQ_INIT_FINISH_CAM_EFFECT  = 0,
        SEQ_WAIT_FINISH_CAM_EFFECT
      };

      //  初回演出を終わらせる
      //  この演出処理で行動選択の初期化関数が呼ばれている
      if( m_seq == SEQ_INIT_FINISH_CAM_EFFECT )
      {
        m_pViewr->CMD_InitFinishWaitCameraEffect();
        ++m_seq;
      }
      else
      if( m_seq == SEQ_WAIT_FINISH_CAM_EFFECT )
      {
        if( m_pViewr->CMD_WaitFinishWaitCameraEffect() )
        {
          m_bEnd  = true;
          ++m_seq;
        }
      }
    }
  };

  //  行動選択のコマンド通知
  class PushCmdActionSelectEventScript : public SimpleEventScriptBase
  {
  public:
    PushCmdActionSelectEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pWorkData ) :
     SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pWorkData );

      //  行動選択が開いていないのに実行することはNG
      GFL_ASSERT( pViewr->IsOpenActionSelect() == true );

      PushCmdActSelectScriptData::DATA* pData = (PushCmdActSelectScriptData::DATA*)( pWorkData->m_aWork );
      m_data  = *pData;

      //  コマンド通知
      if( pViewr->PushCmdActionSelect( m_data.cmdType ) == false )
      {
        GFL_ASSERT( 0 );
      }
    }

    virtual void Update()
    {
      btl::BtlAction action = m_pViewr->CMD_UI_SelectAction_Wait();

      if( action != btl::BTL_ACTION_NULL )
      {
        m_bEnd  = true;

        //  ボタンが押して演出終了
        if( action == btl::BTL_ACTION_FIGHT )
        {
          //  指定したコマンドと違う結果になっていないかチェック
          GFL_ASSERT( m_data.cmdType == btl::BattleUIActSelectFrame::CMD_TYPE_PUSH_BUTTON_FIGHT );
        }
        else
        if( action == btl::BTL_ACTION_ITEM )
        {
          //  指定したコマンドと違う結果になっていないかチェック
          GFL_ASSERT( m_data.cmdType == btl::BattleUIActSelectFrame::CMD_TYPE_PUSH_BUTTON_BAG );
        }
        else
        if( action == btl::BTL_ACTION_ESCAPE )
        {
          //  コマンドを用意していない
          GFL_ASSERT( 0 );
        }
        else
        if( action == btl::BTL_ACTION_CHANGE )
        {
          //  コマンドを用意していない
          GFL_ASSERT( 0 );
        }
      }
    }

  private:
    PushCmdActSelectScriptData::DATA  m_data;
  };

  //  わざ選択画面を開く
  class OpenSkillSelectScreenEventScript : public SimpleEventScriptBase
  {
  public:
    OpenSkillSelectScreenEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      // 技セレクト画面を開く
      pViewr->OpenWazaSelect();
    }

    virtual void Update()
    {
      // 技セレクト画面が開ききるのを待つ
      if( m_pViewr->IsOpenWazaSelect() )
      {
        m_bEnd  = true;
      }
    }
  };

  //  わざ選択のコマンド通知
  class PushCmdSkillSelectEventScript : public SimpleEventScriptBase
  {
  public:
    PushCmdSkillSelectEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pWorkData ) :
     SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pWorkData );

      //  わざ選択が開いていないのに実行することはNG
      GFL_ASSERT( pViewr->IsOpenWazaSelect() == true );

      PushCmdSkillSelectScriptData::DATA* pData = (PushCmdSkillSelectScriptData::DATA*)( pWorkData->m_aWork );
      m_data  = *pData;

      //  コマンド通知
      if( pViewr->PushCmdSkillSelect( m_data.cmdType ) == false )
      {
        GFL_ASSERT( 0 );
      }

      m_seq = 0;
    }

    virtual void Update()
    {
      enum
      {
        SEQ_WAIT  = 0,
        SEQ_END
      };

      if( m_seq == SEQ_WAIT )
      {
        //  わざ選択が終了するまで待つ
        if( m_pViewr->CMD_UI_SelectWaza_Wait() == true )
        {
          ++m_seq;
        }
      }
      
      if( m_seq == SEQ_END )
      {
        //  わざ選択終了させて終了するまで待つ
        if( m_pViewr->CMD_UI_SelectWaza_End() == true )
        {
          m_bEnd  = true;
        }
      }
    }

  private:
    PushCmdSkillSelectScriptData::DATA  m_data;
    u32 m_seq;
  };


  //  全力わざ選択のコマンド通知
  class PushCmdZenryokuSkillSelectEventScript : public SimpleEventScriptBase
  {
  public:
    PushCmdZenryokuSkillSelectEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pWorkData ) :
     SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pWorkData );

      //  わざ選択が開いていないのに実行することはNG
      GFL_ASSERT( pViewr->IsOpenWazaSelect() == true );

      PushCmdZenryokuSkillSelectScriptData::DATA* pData = (PushCmdZenryokuSkillSelectScriptData::DATA*)( pWorkData->m_aWork );
      m_data  = *pData;

      //  コマンド通知
      if( pViewr->PushCmdSkillSelect(btl::BattleUISkillSelectFrame::CMD_TYPE_PUSH_BUTTON_ZENRYOKU ) == false )
      {
        GFL_ASSERT( 0 );
      }

      m_seq = 0;
    }

    virtual void Update()
    {
      enum
      {
        SEQ_END
      };

      if( m_seq == SEQ_END )
      {
        //  全力技のリストが出るまで待つ。
        if( m_pViewr->GetUiSys()->GetStateSkillSelectFrame() == btl::BtlvUi::UI_FRAME_STAET_IDLE )
        {
          m_bEnd  = true;
        }
      }
    }

  private:
    PushCmdZenryokuSkillSelectScriptData::DATA  m_data;
    u32 m_seq;
  };


  //  指カーソル押しイベント
  class PushFingerCursorEventScript : public SimpleEventScriptBase
  {
  public:
    PushFingerCursorEventScript( CaptureTutorialViewer* pViewr, FingerCursorEvent* pFingerCursorEvent, u32 frameRate, const ScriptDataWork* pScriptData ) :
     SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pFingerCursorEvent );
      GFL_ASSERT( pScriptData );
      GFL_UNUSED( frameRate );

      m_pFingerCursorEvent  = pFingerCursorEvent;

      //  作業領域を使用するデータ構造体に変更
      PushFingerScriptData::DATA*  pData = (PushFingerScriptData::DATA*)(pScriptData->m_aWork);

      //  指イベント開始
      gfl2::math::VEC3  pos = gfl2::math::VEC3( pData->posXVal, pData->posYVal, 0.0f );
      m_pFingerCursorEvent->StartClick( pData->showWaitFrame, pData->enterWaitFrame, pos, pData->formType );
    }

    virtual void Update()
    {
      m_pFingerCursorEvent->Update( m_pViewr );
      if( m_pFingerCursorEvent->IsEnd() )
      {
        m_bEnd  = true;
      }
    }

  private:
    FingerCursorEvent*      m_pFingerCursorEvent;
  };

  //  攻撃アクション
  class AttackActionEventScript : public SimpleEventScriptBase
  {
  public:
    AttackActionEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pScriptData ) :
      SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );

      //  攻撃開始
      {
        AttackActionScriptData::DATA* pData = (AttackActionScriptData::DATA*)pScriptData->m_aWork;

        m_aboutAffinityID = pData->aboutAffinityID;

        btl::BtlvPos  attackBtlvPos = btl::BTL_VPOS_NEAR_1;
        btl::BtlvPos  damageBtlvPos = btl::BTL_VPOS_NEAR_1;

        if( pData->pokePos == (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF )
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF;
          damageBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY;

        }
        else
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY;
          damageBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF;
        }

        btl::BTL_POKEPARAM* pAttackPokeParam  = pViewr->GetBattlePokeParam( attackBtlvPos );
        if( pData->isSkillWazaID )
        {
          m_useWazaNo  = static_cast<WazaNo>(pData->skillIdx);
        }
        else
        {
          m_useWazaNo  = pAttackPokeParam->WAZA_GetID( pData->skillIdx );
        }

        //  ダメージ側のHPを減らす
        {
          if( pData->nowHpRateVal < 1.0f )
          {
            btl::BTL_POKEPARAM* pDamagePokeParam  = pViewr->GetBattlePokeParam( damageBtlvPos );

            u16 nowHp = (u16)pDamagePokeParam->GetSrcData()->GetHp();
            u16 setHp = (u16)( (f32)( nowHp ) * pData->nowHpRateVal );
            pDamagePokeParam->HpMinus( nowHp - setHp );
          }
        }

        pViewr->CMD_ACT_WazaEffect_Start( attackBtlvPos, damageBtlvPos, m_useWazaNo, pml::wazadata::TARGET_OTHER_SELECT, btl::BTLV_WAZAEFF_INDEX_DEFAULT, 0, true );

        if( !pData->isSkillWazaID )
        {
          pAttackPokeParam->WAZA_DecrementPP( pData->skillIdx, 1 );
        }

        m_damagePokePos = damageBtlvPos;
      }

      m_state = 0;
    }

    virtual void Update()
    {
      if( m_state == STATE_WAIT_ATTACK )
      {
        // ダメージの終了待ち
        if( m_pViewr->CMD_ACT_WazaEffect_Wait() )
        {
          m_pViewr->CMD_ACT_DamageEffectSingle_Start( m_useWazaNo, m_damagePokePos, m_aboutAffinityID );

          ++m_state;
        }
      }
      
      if( m_state == STATE_DAMAGE_START_EFFECT )
      {
        if( m_pViewr->CMD_ACT_DamageEffectSingle_Wait() )
        {
          m_bEnd  = true;
        }
      }
    }

  private:
    enum State
    {
      STATE_WAIT_ATTACK = 0,
      STATE_DAMAGE_START_EFFECT
    };

    u32           m_state;
    WazaNo        m_useWazaNo;
    btl::BtlvPos  m_damagePokePos;
    pml::battle::TypeAffinity::AboutAffinityID m_aboutAffinityID;
  };


  //  全力技　ポケモン攻撃開始  処理
  class AttackZenryokuWazaBeginEventScript : public SimpleEventScriptBase
  {
  public:
    AttackZenryokuWazaBeginEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pScriptData ) :
      SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );

      //  攻撃開始
      {
        AttackZenryokuWazaBeginActionScriptData::DATA* pData = (AttackZenryokuWazaBeginActionScriptData::DATA*)pScriptData->m_aWork;

        btl::BtlvPos  attackBtlvPos = btl::BTL_VPOS_NEAR_1;
        btl::BtlvPos  targetBtlvPos = btl::BTL_VPOS_FAR_1;

        if( pData->pokePos == (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF )
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF;
          targetBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY;

        }
        else
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY;
          targetBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF;
        }

        m_useWazaNo  = static_cast<WazaNo>(pData->wazaID);

        pViewr->CMD_ACT_ZenryokuWazaBegin_Start( attackBtlvPos, targetBtlvPos , m_useWazaNo, BTL_STRID_SET_AuraEffect_None, 0, 0, 0 );
      }

      m_state = 0;
    }

    virtual void Update()
    {
      switch( m_state )
      {
      case STATE_WAIT:
        // ダメージの終了待ち
        if( m_pViewr->CMD_ACT_ZenryokuWazaBegin_Wait() )
        {
          m_bEnd = true;
          m_state ++;
        }
        break;

      case STATE_END:
        break;
      }
    }

  private:
    enum State
    {
      STATE_WAIT = 0,
      STATE_END,
    };

    u32           m_state;
    WazaNo        m_useWazaNo;
  };

  //  全力技　ポケモン攻撃開始  処理
  class AttackZenryokuWazaEndEventScript : public SimpleEventScriptBase
  {
  public:
    AttackZenryokuWazaEndEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pScriptData ) :
      SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );

      //  攻撃終了
      {
        AttackZenryokuWazaEndActionScriptData::DATA* pData = (AttackZenryokuWazaEndActionScriptData::DATA*)pScriptData->m_aWork;

        btl::BtlvPos  attackBtlvPos = btl::BTL_VPOS_NEAR_1;

        if( pData->pokePos == (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF )
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF;
        }
        else
        {
          attackBtlvPos = (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY;
        }

        pViewr->CMD_ACT_ZenryokuWazaEnd_Start( attackBtlvPos );
      }

      m_state = 0;
    }

    virtual void Update()
    {
      switch( m_state )
      {
      case STATE_WAIT:
        // ダメージの終了待ち
        if( m_pViewr->CMD_ACT_ZenryokuWazaEnd_Wait() )
        {
          m_bEnd = true;
          m_state ++;
        }
        break;

      case STATE_END:
        break;
      }
    }

  private:
    enum State
    {
      STATE_WAIT = 0,
      STATE_END,
    };

    u32           m_state;
  };

  //  ポケモンが倒れる演出
  class DeadActionEventScript : public SimpleEventScriptBase
  {
  public:
    DeadActionEventScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pScriptData ) :
      SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );

      //  攻撃開始
      {
        DeadActionScriptData::DATA* pData = (DeadActionScriptData::DATA*)pScriptData->m_aWork;
        pViewr->CMD_StartDeadAct( pData->pokePos, false );
      }

      m_state = 0;
    }

    virtual void Update()
    {
      switch( m_state )
      {
      case STATE_WAIT:
        // ダメージの終了待ち
        if( m_pViewr->CMD_WaitDeadAct() )
        {
          m_bEnd = true;
          m_state ++;
        }
        break;

      case STATE_END:
        break;
      }
    }

  private:
    enum State
    {
      STATE_WAIT = 0,
      STATE_END,
    };

    u32           m_state;
  };



  //  メッセージ表示
  class PutMessageEventScript : public SimpleEventScriptBase
  {
  public:
    PutMessageEventScript( CaptureTutorialViewer* pViewr, const u32 frameRate, const ScriptDataWork* pScriptData ) :
     SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );
      m_waitFrame = 0;

      //  作業領域を使用するデータ構造体に変更
      PutMessageScriptData::DATA*  pData = (PutMessageScriptData::DATA*)(pScriptData->m_aWork);

      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_SKILL )
      {
        btl::BTL_POKEPARAM* pPokeParam  = pViewr->GetBattlePokeParam( pData->_data.skill.pokePos );
        bool  bSelf = false;
        if( pData->_data.skill.pokePos == (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_SELF )
        {
          bSelf = true;
        }
        else if( pData->_data.skill.pokePos == (btl::BtlvPos)CaptureTutorialViewer::POKE_INDEX_ENEMY )
        {
          bSelf = false;
        }

        pViewr->RegisterPokeName( 0, (MonsNo)pPokeParam->GetMonsNo() );
        pViewr->PlayWazaMessage( bSelf, pPokeParam->WAZA_GetID( pData->_data.skill.skillidx ) );
      }
      else
      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_POKE_CATCH )
      {
        // 捕獲SE
        Sound::StartME( STRM_ME_POKEGET );

        btl::BTL_POKEPARAM* pPokeParam  = pViewr->GetBattlePokeParam( pData->_data.rankDown.pokePos );
        GFL_ASSERT( pPokeParam );

        pViewr->RegisterPokeName( 0, (MonsNo)pPokeParam->GetMonsNo() );
        pViewr->PlayMessage( BTL_STRID_STD_BallThrowS );
      }
      else
      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_DEMO )
      {
        pViewr->PlayDemoMessage( pData->_data.demo.msgId );

        m_waitFrame = pData->_data.demo.waitTimeFrame;
      }
      else
      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_RANK_DOWN )
      {
        btl::BTL_POKEPARAM* pPokeParam  = pViewr->GetBattlePokeParam( pData->_data.rankDown.pokePos );
        GFL_ASSERT( pPokeParam );

        pViewr->RegisterPokeName( 0, (MonsNo)pPokeParam->GetMonsNo() );
        pViewr->PlayDownMessage();
      }
      else
      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_DYING )
      {
        btl::BTL_POKEPARAM* pPokeParam  = pViewr->GetBattlePokeParam( pData->_data.dying.pokePos );
        GFL_ASSERT( pPokeParam );

        pViewr->RegisterPokeName( 0, (MonsNo)pPokeParam->GetMonsNo() );
        pViewr->PlayDyingMessage();
      }
      else
      if( pData->messageType == PutMessageScriptData::MESSAGE_TYPE_ZENRYOKU )
      {
        btl::BTL_POKEPARAM* pPokeParam  = pViewr->GetBattlePokeParam( pData->_data.zenryoku.pokePos );
        GFL_ASSERT( pPokeParam );

        pViewr->RegisterPokeName( 0, (MonsNo)pPokeParam->GetMonsNo() );
        pViewr->PlayZenryokuMessage();
      }

      m_seq   = 0;
      m_data  = *pData;
    }

    virtual void Update()
    {
      if( m_data.messageType == PutMessageScriptData::MESSAGE_TYPE_DEMO )
      {
        enum
        {
          SEQ_WAIT_MESSAGE  = 0,
          SEQ_WAIT_FRAME_TIME
        };

        if( m_seq == SEQ_WAIT_MESSAGE )
        {
          if( m_data._data.demo.bAutoHide == true )
          {
            if( m_pViewr->IsFinishMessage() )
            {
              ++m_seq;
            }
          }
          else
          {
            if( m_pViewr->IsFinishMessageWithoutHide() )
            {
              ++m_seq;
            }
          }
        }

        if( m_seq == SEQ_WAIT_FRAME_TIME )
        {
          if( m_waitFrame <= 0 )
          {
            if( m_data._data.demo.bAutoHide == false )
            {
              m_pViewr->HideMessage();
            }

            m_bEnd  = true;
          }
          else
          {
            --m_waitFrame;
          }
        }
      }
      else
      if( m_data.messageType == PutMessageScriptData::MESSAGE_TYPE_POKE_CATCH )
      {
        enum
        {
          SEQ_WAIT_SE = 0,
          SEQ_WAIT_MESSAGE
        };

        if( m_seq == SEQ_WAIT_SE )
        {
          if( Sound::IsMEFinished( STRM_ME_POKEGET ) )
          {
            ++m_seq;

            Sound::StartNewBGMFromME( STRM_BGM_WIN1 );
          }
        }

        if( m_seq == SEQ_WAIT_MESSAGE )
        {
          if( m_pViewr->IsFinishMessage() )
          {
            ++m_seq;

            m_bEnd  = true;
          }
        }
      }
      else
      {
        if( m_pViewr->IsFinishMessage() )
        {
          m_bEnd  = true;
        }
      }
    }

  private:
    PutMessageScriptData::DATA  m_data;
    u32                         m_waitFrame;
    u32                         m_seq;
  };

  //  ボール投げるスクリプトイベント
  class ThorwBallEventScript : public SimpleEventScriptBase
  {
  public:
    ThorwBallEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      pViewr->StartThrowBall();
    }

    virtual void Update()
    {
      if( m_pViewr->IsFinishThrowBall() )
      {
        m_bEnd  = true;
      }
    }
  };

  //  バッグを開くスクリプトイベント
  class OpenBagEventScript : public SimpleEventScriptBase
  {
  public:
    OpenBagEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      //  バッグ開始
      m_pViewr->CMD_ITEMSELECT_Start( 0, 0, 0, 1, 0 );
    }

    virtual void Update()
    {
      //  バッグが開いているかチェック
      if( m_pViewr->IsFinishOpenBag() == true )
      {
        m_bEnd  = true;
      }
    }
  };

  //  バッグを閉じるスクリプトイベント
  class CloseBagEventScript : public SimpleEventScriptBase
  {
  public:
    CloseBagEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      m_pViewr->CMD_ITEMSELECT_ForceQuit();
    }

    virtual void Update()
    {
      if( m_pViewr->IsFinishOpenBag() == false )
      {
        m_bEnd  = true;
      }
      else
      if( m_pViewr->CMD_ITEMSELECT_Wait() )
      {
        m_bEnd  = true;
      }
    }
  };

  //  バッグのカテゴリリストボタンを押すスクリプトイベント
  class PushButtonBagCategoriEventScript : public SimpleEventScriptBase
  {
  public:
    PushButtonBagCategoriEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      //  バッグを開いていないとNG

      //  ボールカテゴリーを開く
      pViewr->OpenItemSelect();
    }

    virtual void Update()
    {
      m_pViewr->CMD_ITEMSELECT_Wait();

      if( m_pViewr->IsFinishOpenBag() == true )
      {
        m_bEnd  = true;
      }
    }
  };

  //  バッグのアイテムリストボタンを押すスクリプトイベント
  class PushButtonBagItemEventScript : public SimpleEventScriptBase
  {
  public:
    PushButtonBagItemEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      //  バッグを開いていないとNG

      //  ボールアイテム選択
      pViewr->OpenBallSelect();
    }

    virtual void Update()
    {
      m_pViewr->CMD_ITEMSELECT_Wait();

      if( m_pViewr->IsFinishOpenBag() == true )
      {
        m_bEnd  = true;
      }
    }
  };

  //  バッグのアイテム使用ボタンを押すスクリプトイベント
  class PushButtonBagUseItemEventScript : public SimpleEventScriptBase
  {
  public:
    PushButtonBagUseItemEventScript( CaptureTutorialViewer* pViewr ) :
     SimpleEventScriptBase( pViewr )
    {
      //  バッグとアイテム使用画面を開いていないとNG
      m_seq = 0;
    }

    virtual void Update()
    {
      if( m_seq == 0 )
      {
        if( m_pViewr->IsFinishOpenBag() == true )
        {
          m_pViewr->OpenUseItem();

          ++m_seq;
        }
      }
      if( m_seq == 1 )
      {
        if( m_pViewr->CMD_ITEMSELECT_Wait() == true )
        {
          m_bEnd  = true;
        }
      }
    }

  private:
    u32 m_seq;
  };

  //  指定フレーム待機
  class WaitTimeFrameScript : public SimpleEventScriptBase
  {
  public:
    WaitTimeFrameScript( CaptureTutorialViewer* pViewr, const ScriptDataWork* pScriptData ) :
      SimpleEventScriptBase( pViewr )
    {
      GFL_ASSERT( pScriptData );

      WaitTimeFrameScriptData::DATA*  pData = (WaitTimeFrameScriptData::DATA*)(pScriptData->m_aWork);

      m_waitTimeFrame = pData->waitTimeFrame;
    }

    virtual void Update()
    {
      switch( m_seq )
      {
      case SEQ_WAIT:
        if( m_waitTimeFrame <= 0 )
        {
          m_bEnd = true;
          m_seq++;
        }
        else
        {
          --m_waitTimeFrame;
        }
        break;

      case SEQ_END:
        break;
      }
    }

  private:
    enum Seq
    {
      SEQ_WAIT = 0,
      SEQ_END,
    };

    s32           m_waitTimeFrame;
  };

  //  勝利BGM再生
  class StartWinBGMScript : public SimpleEventScriptBase
  {
  public:
    StartWinBGMScript( CaptureTutorialViewer* pViewr ) :
      SimpleEventScriptBase( pViewr )
    {
      u32 bgmNo = m_pViewr->GetWinBGM();
      m_pViewr->GetBattleSound()->SwitchBGM( bgmNo, Sound::BGM_FADE_VERY_FAST );

      m_bEnd = true;
    }

    virtual void Update()
    {
    }
  };

//  ここから先は CaptureTutorialEventScriptExecution の定義
  //------------------------------------------------------------------------------
  CaptureTutorialEventScriptExecution::CaptureTutorialEventScriptExecution( gfl2::heap::HeapBase* pHeap, f32 frameRateVal, CaptureTutorialViewer* pViewr, SCRIPT_DATA* pScriptDataList, const u32 scriptDataListNum )
  {
    GFL_ASSERT( pScriptDataList != NULL );
    GFL_ASSERT( 0 < scriptDataListNum );
    GFL_ASSERT( pHeap != NULL );
    GFL_ASSERT( pViewr != NULL );

    _Clear();

    m_pHeap             = pHeap;
    m_pScriptDataList   = pScriptDataList;
    m_scriptDataListNum = scriptDataListNum;
    m_pViewr            = pViewr;

    m_pFingerCursorEvent  = GFL_NEW( m_pHeap ) FingerCursorEvent();
  }

  CaptureTutorialEventScriptExecution::~CaptureTutorialEventScriptExecution()
  {
    GFL_SAFE_DELETE( m_pFingerCursorEvent );
  }

  void CaptureTutorialEventScriptExecution::Start()
  {
    //  スクリプト開始！
    _NextEventScript();
  }

  bool CaptureTutorialEventScriptExecution::Update()
  {
    //  イベント実行中か
    if( m_pRunEventScript != NULL )
    {
      m_pRunEventScript->Update();
      if( m_pRunEventScript->IsEnd() )
      {
        //  次のイベント実行
        _NextEventScript();
      }
    }

    if( m_pRunEventScript == NULL )
    {
      return false;
    }

    return true;
  }

  //  次に実行するスクリプトを作成
  bool CaptureTutorialEventScriptExecution::_NextEventScript()
  {
    GFL_SAFE_DELETE( m_pRunEventScript );

    //  スクリプト終端にきたので終了
    if( m_scriptDataListNum <= m_seq )
    {
      GFL_PRINT( "End EventScript\n" );

      return false;
    }

    GFL_PRINT( "Now EventScript SeqIdx = %d\n", m_seq );

    const SCRIPT_DATA*  pNowScriptData  = &m_pScriptDataList[ m_seq ];
    if( pNowScriptData->scriptType == SCRIPT_TYPE_START_TURN )
    {
      //  バトルターン開始
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) StartBattleTurnEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_END_TURN )
    {
      //  バトルターン終了
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) EndBattleTurnEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_FINGER_BUTTON )
    {
      //  ゆびカーソル押し
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushFingerCursorEventScript( m_pViewr, m_pFingerCursorEvent, m_frameRate, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUT_SKILL_SELECT )
    {
      //  わざ選択を開始
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) OpenSkillSelectScreenEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_CMD_ACTION_SELECT )
    {
      //  行動選択のコマンド通知
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushCmdActionSelectEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_CMD_SKILL_ITEM )
    {
      //  わざ選択のコマンド通知
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushCmdSkillSelectEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_ZENRYOKU_SKILL_SELECT )
    {
      //  わざ選択のコマンド通知
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushCmdZenryokuSkillSelectEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUT_MESSAGE )
    {
      //  メッセージ表示
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PutMessageEventScript( m_pViewr, m_frameRate, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_ATTACK_SKILL )
    {
      //  攻撃発動
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) AttackActionEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_ZNRYOKU_WAZA_BEGIN )
    {
      //  Zenryoku技　攻撃発動
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) AttackZenryokuWazaBeginEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_ZNRYOKU_WAZA_END )
    {
      //  Zenryoku技　攻撃終了
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) AttackZenryokuWazaEndEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_THORW_BALL )
    {
      //  ボールを投げる
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) ThorwBallEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_OPEN_BAG )
    {
        //  バッグを開くスクリプトイベント
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) OpenBagEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_CLOSE_BAG )
    {
      //  バッグを閉じるスクリプトイベント
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) CloseBagEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_BAG_CATEGORY_BUTTON )
    {
      //  バッグのカテゴリリストボタンを押すスクリプトイベント
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushButtonBagCategoriEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_BAG_ITEM_BUTTON )
    {
      //  バッグのアイテムリストボタンを押すスクリプトイベント
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushButtonBagItemEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_PUSH_BAG_USE_ITEM_BUTTON )
    {
      //  バッグのアイテム使用ボタンを押すスクリプトイベント
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) PushButtonBagUseItemEventScript( m_pViewr );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_DEAD_TURN )
    {
      // 倒れる処理
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) DeadActionEventScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_WAIT_TIME_FRAME )
    {
      // 指定フレーム待機処理
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) WaitTimeFrameScript( m_pViewr, &pNowScriptData->work );
    }
    else
    if( pNowScriptData->scriptType == SCRIPT_TYPE_START_WIN_BGM )
    {
      // 勝利BGM再生開始
      m_pRunEventScript = GFL_NEW_LOW( m_pHeap ) StartWinBGMScript( m_pViewr );
    }
    else
    {
      GFL_ASSERT( 0 );
    }

    ++m_seq;

    return  true;
  }

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)
