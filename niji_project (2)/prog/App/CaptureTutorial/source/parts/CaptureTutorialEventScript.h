//============================================================================================
/**
 * @file    CaptureTutorialEventScript.h
 * @brief   捕獲チュートリアルのイベントスクリプト
 * @author  yuto uchida.
 * @date    15.06.02
 */
//============================================================================================
#ifndef __APP_CAPTURE_TUTORIAL_EVENT_SCRIPT_H__
#define __APP_CAPTURE_TUTORIAL_EVENT_SCRIPT_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_assert.h>

#include "CaptureTutorialViewer.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

  //  スクリプトデータタイプ
  enum ScriptType
  {
    SCRIPT_TYPE_START_TURN  = 0,
    SCRIPT_TYPE_END_TURN,
    SCRIPT_TYPE_PUSH_FINGER_BUTTON,
    SCRIPT_TYPE_PUSH_CMD_ACTION_SELECT,
    SCRIPT_TYPE_PUT_SKILL_SELECT,
    SCRIPT_TYPE_PUSH_CMD_SKILL_ITEM,
    SCRIPT_TYPE_PUT_MESSAGE,
    SCRIPT_TYPE_ATTACK_SKILL,
    SCRIPT_TYPE_THORW_BALL,
    SCRIPT_TYPE_OPEN_BAG,
    SCRIPT_TYPE_CLOSE_BAG,
    SCRIPT_TYPE_PUSH_BAG_CATEGORY_BUTTON,
    SCRIPT_TYPE_PUSH_BAG_ITEM_BUTTON,
    SCRIPT_TYPE_PUSH_BAG_USE_ITEM_BUTTON,
    SCRIPT_TYPE_ZNRYOKU_WAZA_BEGIN,
    SCRIPT_TYPE_ZNRYOKU_WAZA_END,
    SCRIPT_TYPE_ZENRYOKU_SKILL_SELECT,
    SCRIPT_TYPE_DEAD_TURN,
    SCRIPT_TYPE_WAIT_TIME_FRAME,
    SCRIPT_TYPE_START_WIN_BGM,

    SCRIPT_TYPE_MAX,
  };

  //  基本スクリプト空データワーク
  //  ※スクリプトデータはこれを継承する
  class ScriptDataWork
  {
  public:
    ScriptDataWork()
    {
      m_bInheritanceData  = false;
      ::std::memset( m_aWork, 0, sizeof( m_aWork ) );
    }

  public:
    u32  m_aWork[ 32 ];

  protected:
    //  継承データがある場合はtrue
    bool m_bInheritanceData;
  };

  //  行動選択のコマンド通知スクリプトデータ
  class PushCmdActSelectScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BattleUIActSelectFrame::CmdType  cmdType;
    } DATA;

    PushCmdActSelectScriptData( btl::BattleUIActSelectFrame::CmdType type ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->cmdType  = type;
    }
  };

  //  わざ選択のコマンド通知スクリプトデータ
  class PushCmdSkillSelectScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BattleUISkillSelectFrame::CmdType  cmdType;
    } DATA;

    PushCmdSkillSelectScriptData( btl::BattleUISkillSelectFrame::CmdType type ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->cmdType  = type;
    }
  };

  // 全力技選択をオープンするコマンド通知スクリプトデータ
  class PushCmdZenryokuSkillSelectScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      u32 dummy;
    } DATA;

    PushCmdZenryokuSkillSelectScriptData() :
    ScriptDataWork()
    {
    }
  };

  //  メッセージ表示のスクリプトデータ
  class PutMessageScriptData : public ScriptDataWork
  {
  public:
    enum MessageType
    {
      MESSAGE_TYPE_SKILL = 0,
      MESSAGE_TYPE_POKE_CATCH,
      MESSAGE_TYPE_DEMO,
      MESSAGE_TYPE_RANK_DOWN,
      MESSAGE_TYPE_DYING,
      MESSAGE_TYPE_ZENRYOKU,
    };

    typedef struct
    {
      MessageType messageType;

      //  デモ用のデータ
      struct _tag_demo_
      {
        u32   msgId;
        u32   waitTimeFrame;
        bool  bAutoHide;
      };

      //  技用のデータ
      struct _tag_skill_
      {
        u32           skillidx;
        btl::BtlvPos  pokePos;
      };

      //  ポケモン捕獲用のデータ
      struct _tag_catch_poke_
      {
        btl::BtlvPos  pokePos;
        bool          bBattleWin;
      };

      //  ランクダウン
      struct _tag_rank_down_
      {
        btl::BtlvPos  pokePos;
      } RANK_DOWN;

      // 倒れた
      struct _tag_dying_
      {
        btl::BtlvPos  pokePos;
      };

      // 全力技開始
      struct _tag_zenryoku_
      {
        btl::BtlvPos  pokePos;
      };

      union
      {
        u32 aWork[ 6 ];

        _tag_demo_        demo;
        _tag_skill_       skill;
        _tag_catch_poke_  catchPoke;
        _tag_rank_down_   rankDown;
        _tag_dying_       dying;
        _tag_zenryoku_    zenryoku;
      } _data;

    } DATA;

    static DATA::_tag_demo_        CreateDataDemo( const u32 msgId, const f32 waitTimeFrame, const bool bAutoHide )
    {
      DATA::_tag_demo_  data;

      data.msgId          = msgId;
      data.waitTimeFrame  = waitTimeFrame;
      data.bAutoHide      = bAutoHide;

      return  data;
    }

    static DATA::_tag_skill_       CreateDataSkill( const u32 skillIdx, const btl::BtlvPos  pokePos )
    {
      DATA::_tag_skill_  data;
      data.skillidx = skillIdx;
      data.pokePos  = pokePos;

      return  data;
    }

    static DATA::_tag_catch_poke_  CreateDataCatchPoke( const bool bBattleWin, const btl::BtlvPos pokePos )
    {
      DATA::_tag_catch_poke_  data;
      data.bBattleWin = bBattleWin;
      data.pokePos    = pokePos;

      return  data;
    }

    static DATA::_tag_rank_down_   CreateDataRankDown( const btl::BtlvPos  pokePos )
    {
      DATA::_tag_rank_down_  data;

      data.pokePos  = pokePos;
      return  data;
    }

    static DATA::_tag_dying_       CreateDataDying( const btl::BtlvPos  pokePos )
    {
      DATA::_tag_dying_  data;

      data.pokePos  = pokePos;
      return  data;
    }

    static DATA::_tag_zenryoku_       CreateDataZenryoku( const btl::BtlvPos  pokePos )
    {
      DATA::_tag_zenryoku_  data;

      data.pokePos  = pokePos;
      return  data;
    }

    //  コンストラクタでデータ種別を分ける

    //  デモメッセージ
    PutMessageScriptData( const DATA::_tag_demo_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType        = MESSAGE_TYPE_DEMO;
      pData->_data.demo = data;
    }

    //  ランクメッセージ
    PutMessageScriptData( const DATA::_tag_rank_down_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType       = MESSAGE_TYPE_RANK_DOWN;
      pData->_data.rankDown    = data;
    }

    //  わざメッセージ
    PutMessageScriptData( const DATA::_tag_skill_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType            = MESSAGE_TYPE_SKILL;

      pData->_data.skill  = data;
    }

    //  ポケモン捕獲メッセージ
    PutMessageScriptData( const DATA::_tag_catch_poke_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType            = MESSAGE_TYPE_POKE_CATCH;
      pData->_data.catchPoke  = data;
    }

    //  ポケモンが倒れたメッセージ
    PutMessageScriptData( const DATA::_tag_dying_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType            = MESSAGE_TYPE_DYING;
      pData->_data.dying  = data;
    }

    //  全力技開始メッセージ
    PutMessageScriptData( const DATA::_tag_zenryoku_ data ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)m_aWork;
      pData->messageType            = MESSAGE_TYPE_ZENRYOKU;
      pData->_data.zenryoku  = data;
    }

  };

  //  指カーソル押しスクリプトデータ
  class PushFingerScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      u32 showWaitFrame;
      u32 enterWaitFrame;
      f32 posXVal;
      f32 posYVal;

      app::tool::FingerCursor::FormType formType;
    } DATA;

    PushFingerScriptData( u32 showWaitFrame, u32 enterWaitFrame, f32 posXVal, f32 posYVal, const app::tool::FingerCursor::FormType formType ) :
    ScriptDataWork()
    {
      //  ワーク領域を専用構造体に扱うに
      DATA*  pData  = (DATA*)(m_aWork);

      pData->showWaitFrame   = showWaitFrame;
      pData->enterWaitFrame  = enterWaitFrame;
      pData->posXVal         = posXVal;
      pData->posYVal         = posYVal;
      pData->formType        = formType;

      m_bInheritanceData  = true;
    }
  };

  //  ポケモン攻撃スクリプトデータ
  class AttackActionScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BtlvPos  pokePos;
      u32           skillIdx;
      float         nowHpRateVal;
      b32           isSkillWazaID;  // true のとき skillIdxをWazaIDとして使用する。
      pml::battle::TypeAffinity::AboutAffinityID aboutAffinityID;

    } DATA;

    //  現在のHPを元に変更する比率
    AttackActionScriptData( const u32 useSkillIdx, const btl::BtlvPos btlvPos, const float nowHpRateVal, b32 isSkillWazaID , const pml::battle::TypeAffinity::AboutAffinityID aboutAffinityID = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)(m_aWork);
      pData->pokePos      = btlvPos;
      pData->skillIdx     = useSkillIdx;
      pData->nowHpRateVal = nowHpRateVal;
      pData->isSkillWazaID = isSkillWazaID;
      pData->aboutAffinityID = aboutAffinityID;
    }
  };

  //  全力技　ポケモン攻撃開始スクリプトデータ
  class AttackZenryokuWazaBeginActionScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BtlvPos  pokePos;
      u32           wazaID;
    } DATA;

    //  現在のHPを元に変更する比率
    AttackZenryokuWazaBeginActionScriptData( const u32 wazaID, const btl::BtlvPos btlvPos ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)(m_aWork);
      pData->pokePos      = btlvPos;
      pData->wazaID       = wazaID;
    }
  };

  //  全力技　ポケモン攻撃終了スクリプトデータ
  class AttackZenryokuWazaEndActionScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BtlvPos  pokePos;
      u32           wazaID;
    } DATA;

    //  現在のHPを元に変更する比率
    AttackZenryokuWazaEndActionScriptData( const u32 wazaID, const btl::BtlvPos btlvPos ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)(m_aWork);
      pData->pokePos      = btlvPos;
      pData->wazaID     = wazaID;
    }
  };

  // ポケモンが倒れる。
  class DeadActionScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      btl::BtlvPos  pokePos;
    } DATA;

    //  現在のHPを元に変更する比率
    DeadActionScriptData( const btl::BtlvPos btlvPos ) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)(m_aWork);
      pData->pokePos      = btlvPos;
    }
  };

  // 指定フレーム待機
  class WaitTimeFrameScriptData : public ScriptDataWork
  {
  public:
    typedef struct
    {
      s32 waitTimeFrame;
    } DATA;
    WaitTimeFrameScriptData( const s32 waitTimeFrame) :
    ScriptDataWork()
    {
      DATA* pData = (DATA*)(m_aWork);
      pData->waitTimeFrame    = waitTimeFrame;
    }
  };

  //  スクリプトデータ
  //  このデータを１つで１ステップ実行データ
  typedef struct
  {
    ScriptType scriptType;
    ScriptDataWork   work;

  } SCRIPT_DATA;

  //------------------------------------------------------------------------------
  // スクリプト実行
  //------------------------------------------------------------------------------
  class CaptureTutorialEventScriptExecution
  {
  public:
    CaptureTutorialEventScriptExecution( gfl2::heap::HeapBase* pHeap, f32 frameRateVal, CaptureTutorialViewer* pViewr, SCRIPT_DATA* pScriptDataList, const u32 scriptDataListNum );
    ~CaptureTutorialEventScriptExecution();

    void Start();
    bool Update();

  private:
    //  次に実行するスクリプトを作成
    bool _NextEventScript();

    //  ゲーム内のフレーム数を取得
    u32  _GetFrameRate();

    void _Clear()
    {
      m_seq                 = 0;
      m_frameRate           = 0;
      m_time                = 0;
      m_scriptDataListNum   = 0;
      m_is_capture          = false;
      m_down_atk            = false;
      m_pFingerCursorEvent  = NULL;
      m_pRunEventScript     = NULL;
      m_pHeap               = NULL;
      m_pViewr              = NULL;
    }

    u32 m_seq;
    u32 m_frameRate;
    u32 m_time;

    bool m_is_capture;
    bool m_down_atk;

    class FingerCursorEvent*        m_pFingerCursorEvent;
    class SimpleEventScriptBase*    m_pRunEventScript;
    CaptureTutorialViewer*          m_pViewr;

    SCRIPT_DATA*                    m_pScriptDataList;
    u32                             m_scriptDataListNum;

    gfl2::heap::HeapBase*           m_pHeap;
  };

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

#endif //__APP_CAPTURE_TUTORIAL_EVENT_SCRIPT_H__
