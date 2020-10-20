//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   Viewer.h
 *  @author ito_isao
 *
 */
//====================================================================== 
#ifndef __APP_CAPTURE_TUTORIAL_VIEWER_H__
#define __APP_CAPTURE_TUTORIAL_VIEWER_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

//  battleのインクルード
#include "Battle/include/battle_def.h"
#include "Battle/include/battle_SetupParam.h"

#include "Battle/source/btl_common.h"
#include "Battle/source/btl_sound.h"
#include "Battle/source/btl_ZenryokuWazaStatus.h"

#include "Battle/source/btlv/btlv_effect_def.h"

#include "Battle/source/btl_pokeparam.h"

#include "Battle/source/btlv/btlv_ui.h"
#include "Battle/source/btlv/btlv_seq_sys.h"
#include "Battle/source/btlv/btlv_core.h"

#include "Print/include/wordset.h"

#include "AppLib/include/Tool/FingerCursor.h"

GFL_NAMESPACE_BEGIN( GameSys )
class GameProc;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(capture)

//  ククイの手持ちはイワンオ
#define DEMO_MONSNO_SELF MONSNO_OOKAMI1
//  野生ポケモンはツツケラ
//  momijiでは野生ポケモンはアゴジムシ
#define DEMO_MONSNO_ENEMY MONSNO_KUWAGATA1

//  全力技デモのククイの手持ちはイワンオ
#define ZENRYOKU_DEMO_MONSNO_SELF MONSNO_OOKAMI1
//  全力技デモの野生ポケモンはガーディー
#define ZENRYOKU_DEMO_MONSNO_ENEMY MONSNO_GAADHI

  class CaptureTutorialViewer : public btl::BattleViewSystem
  {
  public:
    enum
    {
      CURSOR_ACTION_SELECT,
      CURSOR_WAZA_SELECT,
      CURSOR_BAG_SELECT,
      CURSOR_ITEM_CATEGORY_SELECT,
      CURSOR_BALL_SELECT,
      CURSOR_BALL_USE
    };

    enum
    {
      POKE_INDEX_SELF,
      POKE_INDEX_ENEMY,
      POKE_INDEX_SELF_2,
      POKE_INDEX_MAX
    };

    enum TutorialType
    {
      CAPTURE,
      ZENRYOKU_WAZA,
    };

    CaptureTutorialViewer();
    virtual ~CaptureTutorialViewer();
  
      //システム初期化
    virtual bool Initialize( gfl2::heap::HeapBase* heap ){ GFL_ASSERT_STOP(0); return false; }
    bool Initialize(gfl2::heap::HeapBase* heap, TutorialType type );
    //  システム終了
    virtual bool Finalize(void);

    void SetUtilHeap( app::util::Heap* heap )
    {
      GFL_ASSERT( heap );
      m_util_heap = heap;
    }
  
    //----------------------------------------------------------------------------
    // オーバーライド系
    //----------------------------------------------------------------------------
    //更新(30フレーム前半用)
    virtual void Update();

    //描画更新
    void Draw( gfl2::gfx::CtrDisplayNo displayNo );

    virtual const BTL_FIELD_SITUATION* GetFieldSituation(void) { return &m_setup.fieldSituation; }

    virtual btl::BtlvMode GetBtlvMode(void)const{return btl::BTLV_MODE_CAPTURE;}
    //対戦種類(野生・トレーナー・サブウェイ
    virtual BtlCompetitor GetBattleCompetitor(void)           const { return BTL_COMPETITOR_WILD; };
    virtual BtlRule GetBattleRule(void)                       const { return m_rule; }
    virtual bool IsPlayerSide( btl::BtlvPos pos );

    virtual bool IsSkyBattle(void)              const { return false; }
    virtual btl::BtlSound* GetBattleSound(void)       { return &m_sound; }

    virtual btl::BtlvPos BtlPosToViewPos( btl::BtlPokePos pos ) const { return (btl::BtlvPos)pos; }
    virtual btl::BtlPokePos ViewPosToBtlPos( btl::BtlvPos pos ) const { return (btl::BtlPokePos)pos; }

    virtual btl::BTL_POKEPARAM* GetBattlePokeParam( btl::BtlvPos pos )const{return m_btl_param[pos%2];}
    virtual btl::BTL_PARTY* GetBattleParty_Self(void)const{return m_party[0];}
    virtual btl::BTL_PARTY* GetBattleParty_Enemy(int idx = 0)const{return m_party[1];}

    virtual GameSys::GameProc* GetBattleProc(void)const{return mProc;}
    void SetBattleProc(GameSys::GameProc *proc){mProc = proc;}

    //  チュートリアルでは場はない
    virtual const btl::FieldStatus* GetFieldStatus( void ) const { return NULL; }

    //  基本クラスで定義されていない
    virtual bool IsArBattle(void)const{return false;}

    virtual const Savedata::MyStatus* GetClientPlayerData(u8 clientID, int btlFesIdx = -1) const;

    // 全力技ステータスを取得する。
    virtual const btl::ZenryokuWazaStatus* GetZenryokuWazaStatus( u8 clientID ) const;

    
    //  基本クラスで定義されていない
    virtual bool IsQuickStart(void)                               { return false; }

    virtual u8 GetClientID(void)                            const { return 0; }
    virtual u8 GetFriendCleintID( void )                    const { return  btl::BTL_CLIENTID_NULL; }
    virtual u8 GetEnemyClientID( u8 idx )                   const { return 0; }
    virtual bool IsPlayerInLeftSide( void )                 const { return true; }
  
    //  基本クラスで定義されていない
    virtual btl::BtlPokePos GetClientPokePos(u8 pokeIndex)        { return btl::BTL_POS_1ST_0; }

    //  基本クラスで定義されていない
    virtual bool IsMegaEvolved_Self( void )                 const { return false; }

    virtual bool IsControllable()                           const { return false; }
    
    //  基本クラスで定義されていない
    virtual void GetStringUi( gfl2::str::StrBuf* dst, u16 strID );

    virtual bool IsZukanRegistered( const btl::BTL_POKEPARAM* bpp ) const{return false;}
        
    virtual Savedata::MyItem* GetMyItem() const;
    
    virtual bool IsMultiMode()  const { return false; }
    virtual bool IsItemEnable() const { return true; }
    virtual bool IsClientTrainerExist( u8 clientID ) const { return (clientID==0)?true:false; } //主人公のみ

    //  基本クラスで定義されていない
    virtual bool GetBattleTrainerIsPupil( u8 clientId ) { return false; }

    virtual btl::BtlvPos PokeIDToViewPos( int pos ) const { return (btl::BtlvPos)pos; }
  
    // GFSBTS[509]:MainModuleがない捕獲チュートリアルは継承して実装する
    virtual const BSP_ENCOUNT_EFFECT* GetEncountEffect( void );
    
    //  捕獲チュートリアルでおていれは不要
    virtual bool IsOteireEnable( void ) const { return false; }

    //バトルのメッセージがMainModuleに依存しているので、捕獲チュートリアルなどで出す場合の関数
    // @param msgId メッセージID(btl_std.h)
    // @param str   表示する文字列
    virtual void SeqComFunc_SetEffectMessage_Std(int msgId , gfl2::str::StrBuf* str);
    virtual void SeqComFunc_SetEffectMessage_Set(int msgId , gfl2::str::StrBuf* str);

    virtual bool IsWazaAffinityDisplayEnable( u8 defPokeID ) const { return true; }

  //----------------------------------------------------------------------------
  // シーンから呼ばれる便利関数系
  //----------------------------------------------------------------------------
    // 通常のメッセージ
    void PlayMessage(int id);
    // 
    void PlayWazaMessage(bool self, int waza);
    // デモでのみ使用する専用メッセージ
    void PlayDemoMessage(int id);
    // 睨みつけるの結果、攻撃力が下がるメッセージ
    void PlayDownMessage();
    // 倒れるときのメッセージ
    void PlayDyingMessage();
    // 全力技開始のメッセージ
    void PlayZenryokuMessage();
    // 上記のどれでも、メッセージが終了したかどうかを返す
    bool IsFinishMessage();
    bool IsFinishMessageWithoutHide(); // メッセージウインドウは消さない版

    // メッセージウインドウを消す
    void HideMessage();
    
    void RegisterPokeName(int id, MonsNo poke);
    void RegisterWazaName(int id, u32 wazaID);
    void RegisterWord( int id, gfl2::str::MsgData* msgData, u32 msgId );
    

//  行動選択画面関連

    //  動選択画面を開く
    void OpenActionSelect();
    //  行動選択画面が開いているか
    bool IsOpenActionSelect();
    //  行動選択のコマンド通知
    bool PushCmdActionSelect( const btl::BattleUIActSelectFrame::CmdType type );

//  技選択画面関連

    // 技選択画面を開く
    void OpenWazaSelect();
    //  技選択画面を開いているか
    bool IsOpenWazaSelect();
    //  技選択のコマンド通知
    bool PushCmdSkillSelect( const btl::BattleUISkillSelectFrame::CmdType type );

    void StartWazaSelectAnime();
    bool IsFinishWazaSelectAnime();
    
    // 上記画面を閉じてしまう
    void CloseActionSelect();
    // 上画面の閉じるアニメが終了したかどうか
    bool IsFinishCloseActionSelect();
    
    // 攻撃側の技エフェクトを開始
    void StartWazaEffectAttack( bool down_atk );
    // ダメージを受ける演出を開始
    void StartDamageAttack();
    
    // 防御側の技エフェクトを開始（にらみつける）
    void StartWazaEffectDefense();
    // 睨みつけるの演出
    void StartDamageDefense();
    
    // バッグが開いたかどうか
    bool IsFinishOpenBag();

    // バッグ画面（カテゴリ選択）を開く
    void OpenItemSelect();
    // バッグ画面（モンスターボール選択）を開く
    void OpenBallSelect();
    // バッグ画面（アイテム使用）を開く
    void OpenUseItem();
    // バッグ画面を閉じる
    void CloseBag();
    bool IsFinishCloseBag();
    bool IsFinishEndCamera();
    
    // ボールを投げる
    void StartThrowBall();
    // 捕獲完了かどうか
    bool IsFinishThrowBall();
    
    // 指アイコンを指定された位置に出す
    void StartFingerCursor( gfl2::math::VEC3& rPos, const app::tool::FingerCursor::FormType formType );

    // 指カーソルを押すアニメを開始する
    void StartPushFingerCursorAnime( const app::tool::FingerCursor::FormType formType );

    // 指カーソルの押すアニメの再生が終わったかどうかを返す
    bool IsEndAnimePushFingerCursor( const app::tool::FingerCursor::FormType formType );

    // 勝利BGMの取得
    u32 GetWinBGM() const { return m_setup.btlEffData.bgm_win; }

  private:

    // TutorialTypeごとのSetup
    void setupCapture( gfl2::heap::HeapBase* pHeap );
    void setupZenryokuWaza( gfl2::heap::HeapBase* pHeap );


    // ○○があらわれた！をBufferに入れる
    void _MakeYaseiEncountMessage( gfl2::str::StrBuf* pOutputStrBuf );

    // いけっ○○！をBufferに入れる
    void _MakeSingleGoMessage( gfl2::str::StrBuf* pOutputStrBuf );

    void _MakeSimplePokeNameExpandMessage( gfl2::str::StrBuf* pOutputStrBuf, gfl2::str::MsgData* pMsgData, int msgId, MonsNo monsNo );

    void _Clear()
    {
      m_util_heap   = NULL;
      m_item        = NULL;
      m_std_msg     = NULL;
      m_attack_msg  = NULL;
      m_demo_msg    = NULL;
      m_set_msg     = NULL;
      m_ui_msg      = NULL;
      m_str_buf     = NULL;
      m_str_buf_tmp = NULL;
      m_wordset     = NULL;
      m_pFingerCursor = NULL;
      m_pFingerCursorLytBuffer  = NULL;
      mProc = NULL;

      gfl2::std::MemClear( m_param, sizeof( m_param ) );
      gfl2::std::MemClear( m_party, sizeof( m_party ) );
      gfl2::std::MemClear( m_btl_param, sizeof( m_btl_param ) );

      m_rule  = BTL_RULE_SINGLE;
      m_init_seq = 0;

      m_TutorialType = CAPTURE;

      for( u32 i=0; i<POKE_INDEX_MAX; ++i )
      {
        m_zenryokuWazaStatus[i] = NULL;
      }
    }

    app::util::Heap* m_util_heap;
    
    //ダミーデータ
    pml::pokepara::PokemonParam *m_param[ POKE_INDEX_MAX ];
    btl::BTL_PARTY *m_party[ POKE_INDEX_MAX ];
    btl::BTL_POKEPARAM *m_btl_param[ POKE_INDEX_MAX ];
    BtlRule m_rule;
    BATTLE_SETUP_PARAM    m_setup;
    btl::BTL_ACTION_PARAM m_btl_action_param;
    Savedata::MyItem* m_item;
    GameSys::GameProc *mProc;
    btl::ZenryokuWazaStatus* m_zenryokuWazaStatus[ POKE_INDEX_MAX ];

    //  2D指カーソル
    app::tool::FingerCursor*  m_pFingerCursor;
    //  2D指カーソルのバイナリ受け取る変数
    void*                     m_pFingerCursorLytBuffer;
    
    // 文字列表示用
    gfl2::str::MsgData* m_std_msg;
    gfl2::str::MsgData* m_attack_msg;
    gfl2::str::MsgData* m_demo_msg;
    gfl2::str::MsgData* m_set_msg;
    gfl2::str::MsgData* m_ui_msg;
    gfl2::str::StrBuf*  m_str_buf;
    gfl2::str::StrBuf*  m_str_buf_tmp;
    print::WordSet*     m_wordset;
    
    // BtlSoundも持たせる必要がある
    btl::BtlSound m_sound;

    // TutorialType
    TutorialType m_TutorialType;

    int m_init_seq;
#if PM_DEBUG
    btl::BTLV_DEBUG_PARAM m_btlDebugParam;
#endif
  };

GFL_NAMESPACE_END(capture)
GFL_NAMESPACE_END(app)

#endif //__APP_CAPTURE_TUTORIAL_VIEWER_H__
