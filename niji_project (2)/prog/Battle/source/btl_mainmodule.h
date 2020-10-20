#if !defined( __BTL_MAINMODULE_H__ )
#define __BTL_MAINMODULE_H__
//=============================================================================================
/**
 * @file    btl_mainmodule.h
 * @brief   XY バトルシステムメインモジュール
 * @author  taya
 *
 * @date  2010.12.16  作成
 */
//=============================================================================================
#pragma once

#include <Savedata/include/ConfigSave.h>
#include <Savedata/include/Record.h>
#include <GameSys/include/GameManager.h>
#include <PokeTool/include/NatsukiManager.h>
#include <Battle/include/battle_SetupParam.h>

#include "./btl_common.h"
#include "./btl_field.h"
#include "./btl_calc.h"
#include "./btl_net.h"
#include "./btl_client.h"
#include "./btl_rec.h"
#include "./btl_EscapeInfo.h"
#include "./btl_Adapter.h"
#include "./btl_Sound.h"
#include "./btl_PokeIDRec.h"

#include "./btl_DebugTimer.h"

#include  "./btlv/btlv_core.h"



GFL_NAMESPACE_BEGIN( Savedata )
class ZukanData;
GFL_NAMESPACE_END( Savedata )

GFL_NAMESPACE_BEGIN( GameSys )
class GameProc;
class GameData;
GFL_NAMESPACE_END( GameSys )

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Encount )
class PokeSet;
GFL_NAMESPACE_END( Encount )
GFL_NAMESPACE_END( Field )

GFL_NAMESPACE_BEGIN( PokeTool )
class KawaigariParamCareCoreManager;
GFL_NAMESPACE_END( PokeTool )


class userInterface;

GFL_NAMESPACE_BEGIN(btl)

class BTL_SERVER;
class POKECON;
class ClientPublicInformation;
class ClientCommunicationObserver;


//-------------------------------------------------------------------------------
/**
 *  特殊位置ID指定子
 */
//-------------------------------------------------------------------------------
typedef enum {

  BTL_EXPOS_DEFAULT = 0,      ///< 自分だけ

  BTL_EXPOS_AREA_ENEMY,       ///< （攻撃範囲内の）相手全部
  BTL_EXPOS_AREA_OTHERS,      ///< （攻撃範囲内の）自分以外全部
  BTL_EXPOS_AREA_MYTEAM,      ///< （攻撃範囲内の）味方全部
  BTL_EXPOS_AREA_FRIENDS,     ///< （攻撃範囲内の）自分以外の味方全部
  BTL_EXPOS_AREA_ALL,         ///< （攻撃範囲内の）敵・味方全部

  BTL_EXPOS_FULL_ENEMY,       ///< （全フィールドの）相手ポケ全部
  BTL_EXPOS_FULL_FRIENDS,     ///< （全フィールドの）自分以外の味方ポケ全部
  BTL_EXPOS_FULL_ALL,         ///< （全フィールドの）ポケ全部

  BTL_EXPOS_NEXT_FRIENDS,     ///< （自分の隣の）味方全部

//  BTL_EXPOS_AREA_ENEMY

}BtlExPosType;

typedef u16 BtlExPos;

static inline BtlExPos  EXPOS_MAKE( BtlExPosType type, u8 basePos )
{
  return (type<<8) | basePos;
}
static inline u8 EXPOS_GET_TYPE( BtlExPos exPos )
{
  return (exPos >> 8) & 0xff;
}
static inline u8 EXPOS_GET_BASEPOS( BtlExPos exPos )
{
  return exPos & 0xff;
}


class MainModule {
  GFL_FORBID_COPY_AND_ASSIGN( MainModule );

public:

  typedef struct {
    Savedata::MyStatus*   playerStatus;
    STRBUF*     name;
    STRBUF*     trtype_name;
    STRBUF*     win_msg;
    STRBUF*     lose_msg;

    u16         trainerID;
    u16         trainerType;
    u8          trainerGroup;
    u8          trainerSex;
    u8          trainerGold;
    u8          pad1;
    u32         ai_bit;
    u16         useItem[ BSP_TRAINER_DATA::USE_ITEM_MAX ];

    u8          grammer_attr;
    u8          grammer_isPlural;

  }TRAINER_DATA;


  /**
   *  PGL 記録に必要なパラメータ（倒したポケモン＆ワザ情報）
   */
  class PGLRecParam {
    public:
      PGLRecParam( const BTL_POKEPARAM* _attacker, WazaID _wazaID )
      : attacker(_attacker), wazaID(_wazaID) {};
      ~PGLRecParam(){};

      const BTL_POKEPARAM* attacker;
      WazaID               wazaID;

    private:
      PGLRecParam() {};
      PGLRecParam( const PGLRecParam& p ) {};
  };


  MainModule( BATTLE_SETUP_PARAM* setupParam,
              HeapHandle heapSys,
              HeapHandle heapView,
              HeapHandle heapNet,
              GameSys::GameManager* gameMan,
              gfl2::ui::DeviceManager *uiMan,
              GameSys::GameProc *battleProc
            );
  ~MainModule();

  bool  Initialize( int* seq );
  bool  MainLoop( int* seq );
  void  PreDrawLoop( int* seq );
  void  DrawLoop( int* seq , gfl2::gfx::CtrDisplayNo displayNo );
  bool  Finalize( int* seq );

  void StartForceRemoveView( void );
  bool WaitForceRemoveView( void );

  PokeParty*        GetSrcParty( u8 clientID, bool fForServer ) const;
  const PartyParam* GetPartySetupParam( u8 clientID ) const;
  ServerFlow*       GetSVFWorkForAI( void );
  const BTL_CLIENT* GetClient( u8 clientID ) const;
  const BTL_CLIENT* GetClientByPokeID( u8 pokeID ) const;
  ZenryokuWazaStatus* GetZenryokuWazaStatus( u8 clientID ) const;

  BtlPokePos        GetClientPokePos( u8 clientID, u8 posIdx ) const;
  bool              IsRecordPlayMode() const;
  bool              IsRecordFightMode( void ) const;
  bool              IsRecordFightWithNPCData( void ) const;
  bool              CheckImServerMachine( void ) const;
  bool              IsClientTrainerExist( u8 clientID ) const;
  bool              IsClientNPC( u8 clientID ) const;
  u16               GetClientUseItem( u8 clientID, u8 itemIdx ) const;
  u32               GetClientAIBit( u8 clientID ) const;
  BtlCompetitor     GetCompetitor( void ) const;
  bool              GetSetupStatusFlag( BTL_STATUS_FLAG flag ) const;
  BtlRule           GetRule( void ) const;
  bool              IsWazaEffectEnable( void ) const;
  u8                GetMaxFollowPokeLevel( void ) const;
  bool              IsRecordEnable( void ) const;
  BtlPokePos        GetValidPosMax( void ) const;
  u32               GetFrontPosNum( u8 clientID ) const;
  u32               GetSidePosNum( BtlSide side ) const;
  u32               GetOpponentFrontPosNum( u8 clientID ) const;
  bool              IsPokePosExist( BtlPokePos pos ) const;
  bool              IsFrontPos( BtlPokePos pos ) const;
  u8                ExpandBtlPos( BtlExPos exPos, u8* dst ) const;
  u8                ExpandExistPokeID( const POKECON* pokeCon, BtlExPos exPos, u8* pokeIDAry ) const;
  BtlPokePos        GetFriendPokePos( BtlPokePos basePos, u8 idx ) const;
  bool              IsFriendPokePos( BtlPokePos pos1, BtlPokePos pos2 ) const;
  u8                GetPlayerClientID( void ) const;
  u8                GetPlayerFriendCleintID( void ) const;
  u8                GetFriendCleintID( u8 clientID ) const;
  u8                GetEnemyClientID( u8 idx ) const;
  u32               GetOpponentClientID( u8 clientID, u8 idx ) const;
  BtlPokePos        PokeIDtoPokePos( const POKECON* pokeCon, u8 pokeID ) const;
  BtlvPos           PokeIDtoViewPos( const POKECON* pokeCon, u8 pokeID ) const;
  u8                BtlPosToPosIdx( BtlPokePos pos ) const;
  BtlvPos           BtlPosToViewPos( BtlPokePos pos ) const;
  u8                BtlPosToClientID( BtlPokePos pos ) const;
  void              BtlPosToClientID_and_PosIdx( BtlPokePos pos, u8* clientID, u8* posIdx ) const;
  BtlvTrainerPos    ClientIDtoTrainerViewPos( u8 clientID ) const;
  u8                ClientIDtoRelation( u8 myClientID, u8 targetClientID ) const;
  BtlPokePos        ViewPosToBtlPos( u8 vpos ) const;
  const BTL_FIELD_SITUATION*
                    GetFieldSituation( void ) const;

  BtlWeather        GetDefaultWeather( void ) const;
  const BSP_ENCOUNT_EFFECT*
                    GetEncountData( void ) const;
  const BSP_BATTLE_EFFECT*
                    GetBattleEncountData( void ) const;

  bool              IsFriendPokeID( u8 pokeID1, u8 pokeID2 ) const;
  bool              IsFriendClientID( u8 clientID_1, u8 clientID_2 ) const;

  /**
   *  図鑑「見た」フラグセット
.  */
  void              RegisterZukanSeeFlag( const BTL_POKEPARAM* bpp ) const;

  void              SetIllusionForParty( BTL_PARTY* party, u8 clientID ) const;
  u8                GetClientFrontPosCount( u8 clientID ) const;
  u8                GetClientBenchPosIndex( u8 clientID ) const;
  u32               GetCommandLimitTime( void ) const;
  u32               GetClientLimitTime( void ) const;
  bool              IsClientLimitTimeExist( void ) const;
  u32               GetGameLimitTime( void ) const;
  bool              IsGameLimitTimeExist( void ) const;
  bool              CheckGameLimitTimeOver( void ) const;
  bool              CheckClientLimitTimeOver( void ) const;
  bool              GetEnableTimeStop( void ){return m_setupParam->bEnableTimeStop;}
  bool              GetEnableVoiceChat( void ){return m_setupParam->bVoiceChat;}
  const BSP_ENCOUNT_EFFECT *GetEncountEffect( void ){return &m_setupParam->btlEffData.enceff;}
  bool              IsLongEncount( void );
  bool              CheckRecPlayError( void );
  BtlCommMode       GetCommMode( void ) const;
  bool              IsMultiMode( void ) const;
  bool              IsPlayerRatingDisplay( void ) const;
  u32               GetWinBGMNo( void ) const;
  bool              IsIrekaeMode( void ) const;
  bool              IsCompetitorScenarioMode( void ) const;
  bool              IsExpSeqEnable( void ) const;
  bool              IsMoneySeqEnable( void ) const;
  bool              IsExistClient( u8 clientID ) const;
  BtlSide           GetClientSide( u8 clientID ) const;
  BtlSide           GetOpponentSide( BtlSide side ) const;
  BtlSide           PokeIDtoSide( u8 pokeID ) const;
  BtlSide           PokeIDtoOpponentSide( u8 pokeID ) const;
  bool              IsPlayerSide( BtlSide side ) const;
  bool              IsSideExist( BtlSide side ) const;
  void              ExpandSide( BtlSide* expandSide, u8* expandSideNum, BtlSide side ) const;
  BtlPokePos        GetSidePos( BtlSide side, u8 index ) const;
  u8                GetSideNum( void ) const;
  u8                GetEnemyClientNum( u8 clientID ) const;
  BtlEscapeMode     GetEscapeMode( void ) const;
  const Savedata::MyStatus*   GetPlayerStatus( void ) const;
  bool              GetDebugFlag( BtlDebugFlag flag ) const;
  void              ReflectNatsukiDead( BTL_POKEPARAM* bpp, bool fLargeDiffLevel ) const;
  const BTL_POKEPARAM*
                    GetFakeTargetPokeParam( POKECON* pokeCon, const BTL_POKEPARAM* bpp ) const;
  void              AddBonusMoney( u32 volume );
  u32               GetBonusMoney( void ) const;
  void              SetMoneyDblUp( MoneyDblUpCause cause );
  u32               FixLoseMoney( void );
  bool              DecrementPlayerItem( u8 clientID, u16 itemID ) const;
  bool              AddItem( u8 clientID, u16 itemID );
  void              NotifyPokemonLevelup( BTL_POKEPARAM* bpp );
  void              CalcNatsukiItemUse( BTL_POKEPARAM* bpp, u16 itemNo ) const;
  void              NotifyPokemonGetToGameSystem( const BTL_POKEPARAM* bpp );
  bool              IsZukanRegistered( MonsNo monsno ) const;
  bool              IsZukanRegistered( const BTL_POKEPARAM* bpp ) const;
  u32               GetZukanCapturedCount( void ) const;
  const Savedata::ZukanData* GetZukanSaveData( void ) const;
  const Savedata::ZukanData* GetZukanSaveDataOnBattleStart( void ) const;
  Savedata::MyItem* GetItemSaveData( void ) const;
  Savedata::ConfigSave* GetConfigSaveData( void ) const;
  bool              IsLongVersionMegaEvolve( const BTL_POKEPARAM* bpp ) const;
  void              RecLongVersionMegaEvolved( const BTL_POKEPARAM* bpp );
  bool              IsResultStrictMode( void ) const;
  BtlResult         GetBattleResult( void );
  ResultCause       GetBattleResultCause( void ) const;
  void              NotifyCmdCheckError( void );
  u32               FixRegularMoney( void );
  u32               GetClientCoverPosNum( u8 clientID ) const;
  BTL_CLIENT_ID     GetPosCoverClientId( BtlPokePos pos ) const;

  const STRBUF*      GetClientTrainerName( u8 clientID, print::GrammaticalAttribute * grammer ) const;
  const STRBUF*      GetClientTrTypeName( u8 clientID, print::GrammaticalAttribute * grammer ) const;
  u32                GetClientTrainerID( u8 clientID ) const;
  u16                GetClientTrainerType( u8 clientID ) const;
  trainer::TrTypeGroup   GetClientTrainerGroup( u8 clientID ) const;
  u8                 GetClientTrainerGold( u8 clientID ) const;
  u16                GetClientTrainerMegaItemId( u8 clientID ) const;
  const gfl2::str::StrBuf*    GetClientTrainerMsg( u8 clientID, BtlResult playerResult ) const;
  const Savedata::MyStatus*   GetClientPlayerData(u8 clientID) const;
  const Savedata::MyStatus*   GetBattleFesPlayerData(u8 idx) const;
  u16                         GetBattleFesPlayerWinMsgId(u8 idx) const;
  u16                         GetClientRating(u8 clientID) const;
  ClientPublicInformation*       GetClientPublicInformation( u8 clientID );
  const ClientPublicInformation* GetClientPublicInformation( u8 clientID ) const;

  void               NotifyBattleResult( BtlResult result, ResultCause judgeCause );
  void               NotifyCapturedPokePos( BtlPokePos pos );
  bool               IsCapturedPokeZukanNewRegistered( void ) const;
  void               NotifyTurnedLevelUpPokePos( u8 pokeID );
  bool               CheckTurnedLevelUp( u8 pokeID ) const;
  void               RECORDDATA_Inc( Savedata::Record::RECORD_ID recID ) const;
  void               RECORDDATA_Add( Savedata::Record::RECORD_ID recID, u32 value ) const;
  BtlPokePos         GetOpponentPokePos( BtlPokePos basePos, u8 idx ) const;
  void               StartFadeoutForRecPlay( void );
  void               ResetForRecPlay( u32 nextTurnNum );
  void               NotifyChapterSkipEnd( void );
  void               ReflectPokeWazaOboe( u8 pokeID );
  void               BGMFadeOutDisable( void );
  bool               IsShooterEnable( void ) const;
  bool               IsItemEnable( void ) const;
  bool               IsFriendshipActive( void ) const;
  u8                 GetPokeFriendship( const BTL_POKEPARAM* bpp ) const;
  bool               CanEvolveAfterBattle( const BTL_POKEPARAM& poke ) const;
  void               GetEvolveSituation( pml::pokepara::EvolveSituation* dest, u8 pokeId ) const;

  void               RecMegaEvolved( u8 clientID );
  bool               IsMegaEvolved( u8 clientID ) const;
  void               RecUltraBursted( const u8 clientID );
  bool               IsUltraBursted( const u8 clientID ) const;
  void               NotifyCommErrorToLocalClient( void );
  bool               CheckAllClientQuit( void );

  void               StartBGM( u32 bgmNo );
  void               StartWinBGM( void );


  static u8          PokeIDtoClientID( u8 pokeID );
  static BtlPokePos  GetFacedPokePos( BtlRule rule, BtlPokePos pos );
  static bool        IsFriendPokePos( BtlRule rule, BtlPokePos pos1, BtlPokePos pos2 );
  static BtlPokePos  GetFriendPokePos( BtlRule rule, BtlPokePos basePos, u8 idx );
  static BtlPokePos  GetOpponentPokePosByRule( BtlRule rule, BtlPokePos basePos, u8 idx );
  static BtlPokePos  GetSidePos( BtlRule rule, BtlSide side, u8 idx );
  static BtlSide     PosToSide( BtlRule rule, BtlPokePos pos );
  static u8          GetClientBasePokeID( u8 clientID );
  static u8          PokeIDtoShortID( u8 pokeID );
  static u8          ShortIDtoPokeID( u8 clientID, u8 shortID );

  GameSys::GameManager* GetGameManager( void ) const { return m_gameMan; };
  GameSys::GameData*    GetGameData( void ) const { return m_gameMan->GetGameData(); }
  gfl2::ui::DeviceManager*  GetUIManager( void ) const { return m_uiMan; };
  BtlSound*          GetBattleSound(void){return &m_soundSys;}
  GameSys::GameProc  *GetBattleProc(void)const{return m_battleProc;}

  bool          IsOpponentClientID( u8 clientID1, u8 clientID2 ) const;
  bool          IsSkyBattle(void)const{return m_setupParam->bSkyBattle;}
  bool          IsMustCaptureMode( void ) const { return m_setupParam->bMustCapture; }
  bool          HasPlayerGakusyuSouti( void ) const;

  BATTLE_CONVENTION_INFO* GetBattleConventionInfo( u8 id ) const;

  void NotifyPGLRecord( const BTL_POKEPARAM* bpp, const PGLRecParam* recParam ) const;

  void SetupBattleSE( void );

  // サーバーを介したシミュレーション
  BtlTypeAff SimulationWazaAffinityOnlyAttacker( u8 atkPokeID, u8 defPokeID, WazaID waza );

  // sango用テレビナビデータ関連
  void SetTvNaviData_FrontPoke( const BTL_POKEPARAM* bpp1, const BTL_POKEPARAM* bpp2 );
  void SetTvNaviData_UseWaza( const BTL_POKEPARAM *bpp , u16 wazaNo );
  void SetTvNaviData_MegaEvo( const BTL_POKEPARAM* bpp );

  // かわいがり
  bool IsOteireEnable( void ) const;
  bool IsOteireButtonPressed( void ) const;
  void SetOteireButtonDisable( void );
  u8 GetOteireTargetPokeID( void ) const;
  void NotifyOteireButtonPressed( void );
  void RemoveIllegalDirt( PokeTool::KawaigariParamCareCoreManager* kawaigariData, u8 clientID );

  // 乱入
  Field::Encount::PokeSet* GetIntrudePokeParamGenerator( void ) const;
  bool CanIntrudeBattle( void ) const;
  void StartIntrudeBattle( BtlPokePos intrudePokePos, const pml::pokepara::PokemonParam& intrudePokeParam, BtlIntrudeType intrudeType );

  // 援軍
  bool IsReinforceEnable( void ) const;
  const ReinforceBattleParam& GetReinforceBattleParam( void ) const;

  // ひんしポケモン記録（乱入ポケモンID生成に利用する）
  void NotifyPokemonDead( u8 deadPokeID ) const;

  //Rotom関連
  bool IsEnableRotom(void)const;  ///<ロトムが有効か？
  Savedata::FieldMenu::RotomRank GetRotomRank(void)const;///<ロトムの仲良し度

  //バトルフェスフィールド効果関連
  BtlFesField GetBtlFesField(void)const;///< バトルフェスフィールド効果


private:

//  HEAPID      heapID;                                                          //
  HeapHandle                           m_heapSys;
  HeapHandle                           m_heapView;
  HeapHandle                           m_heapNet;

  BATTLE_SETUP_PARAM*                  m_setupParam;                             // tmp
  BattleViewSystem*                    m_viewCore;                               // OK
  RandSys                              m_randSys;                                //
  u32                                  m_randomSeed;                          //
  GameSys::GameManager*                m_gameMan;
  gfl2::ui::DeviceManager*             m_uiMan;
  GameSys::GameProc*                   m_battleProc;

  //  BTL_PROC             subProc;                                                //
  typedef bool (MainModule::*pSubProc)(int*);
  pSubProc             m_subProc;
  int                  m_subSeq;                                                 //
  int                  m_setupStep;

  typedef bool (MainModule::*pMainProc)(void);
  pMainProc            m_mainLoop;

  PokeParty*                           m_srcParty[ BTL_CLIENT_MAX ];             //
  PokeParty*                           m_srcPartyForServer[ BTL_CLIENT_MAX ];    //
  PokeParty*                           m_tmpParty;
  const Savedata::MyStatus*            m_playerStatus;                           //
  u8                                   m_fClientQuit[ BTL_CLIENT_MAX ];          //
  u8                                   m_fMegaEvolved[ BTL_CLIENT_MAX ];
  u8                                   m_fUltraBursted[ BTL_CLIENT_MAX ];

  // サーバが計算時に書き換えても良い一時使用パラメータ領域と、
  // サーバコマンドを受け取ったクライアントが実際に書き換えるパラメータ領域
  POKECON*                             m_pokeconForClient;                       //
  POKECON*                             m_pokeconForServer;                       //

  pml::pokepara::PokemonParam*         m_ppIllusionZoroArc;                      //
  net::AIDCon*                         m_AIDataContainer;                        //

  EscapeInfo                           m_escapeInfo;                             //
  BtlSound                             m_soundSys;

  BtlRule                          m_rule;
  u32                              m_regularMoney;                               //
  u32                              m_bonusMoney;                                 //
  u32                              m_loseMoney;                                  //
  Savedata::ConfigSave::MSG_SPEED  m_msgSpeed;                                   //
  u16                              m_LimitTimeGame;                              //
  u16                              m_LimitTimeClient;                            //
  u16                              m_LimitTimeCommand;                           //
  BtlResult                        m_serverResult;                               //
  ResultCause                 m_serverResultCause;

  u8          m_myClientID;
  u8          m_myOrgPos;
  Savedata::ConfigSave::BATTLE_RULE          m_changeMode;
  u8          m_MultiAIDataSeq;
  u8          m_MultiAIClientNum;
  u8          m_MultiAIClientID;
  u8          m_fCommError           : 1;
  u8          m_fCommErrorMainQuit   : 1;
  u8          m_fWazaEffectEnable    : 1;
  u8          m_fGetMoneyFixed       : 1;
  u8          m_fLoseMoneyFixed      : 1;
  u8          m_fBGMFadeOutDisable   : 1;
  u8          m_fSleepSoundFunc      : 1;
  u8          m_fSoundPushed         : 1;

  bool m_isOteireButtonDisable;
  bool m_isOteireButtonPressed;
  u8 m_oteireTargetPokeID;

  BTL_SERVER*                          m_server;                                     //
  BTL_SERVER*                          m_cmdCheckServer;                             //
  BTL_CLIENT*                          m_client[ BTL_CLIENT_MAX ];                   // ローカルクライアント
  BTL_CLIENT*                          m_dummyClient[ BTL_CLIENT_MAX ];              // ダミークライアント
  ZenryokuWazaStatus*                  m_clientZenryokuWazaStatus[ BTL_CLIENT_MAX ]; // 各クライアントの全力技ステータス管理( ローカルクライアント・ダミークライアント共用 )
  TRAINER_DATA                         m_trainerParam[ BTL_CLIENT_MAX ];             //
  static const int BTL_FES_TRAINER_NUM = 3;
  TRAINER_DATA                         m_fesTrainerParam[BTL_FES_TRAINER_NUM];             //
  ClientPublicInformation*             m_clientPublicInformation[BTL_CLIENT_MAX];
  ClientCommunicationObserver*         m_clientCommunicationObserver;

  rec::Reader                          m_recReader;                            //
  net::SERVER_NOTIFY_PARAM             m_serverNotifyParam;
  u8                                   m_moneyDblUpCause[ MONEY_DBLUP_CAUSE_MAX ];
  
  bool                  m_checkSeeAmount[BTL_CLIENT_MAX][BTL_PARTY_MEMBER_MAX];  // 図鑑ナビ用見た回数カウントチェック用
  Savedata::ZukanData*  m_zukanDataOnBattleStart;                                // 戦闘開始時の図鑑データ

  PokeIDRec  m_deadPokeIDRec;
  bool m_isRotomPower; // ロトムパワーが使用されているか

private:



  /**
   *  マルチモード時の味方クライアントIDに変換（存在しなくても計算上の値を返す）
   */
  static inline u8 getFriendCrientID( u8 clientID )
  {
    return (clientID + 2) & 3;
  }

  // X をフォルムチェンジさせておく
  static void srcParty_FormChangeForX( PokeParty* party );
  static void srcParty_FromChange_OnBattleStart( pml::pokepara::PokemonParam* pokeParam );
  void setupUnknownUBNickName( PokeParty* pokeParam ) const;


  u8           btlPos_to_clientID( BtlPokePos btlPos ) const;
  void         btlPos_to_cliendID_and_posIdx( BtlPokePos btlPos, u8* clientID, u8* posIdx ) const;

  void         setSubProcForSetup( const BATTLE_SETUP_PARAM* setup_param );
  void         setSubProcForCleanUp( const BATTLE_SETUP_PARAM* setup_param );
  void         setSubProc( pSubProc proc );
  bool         callSubProc( void );
  void         setMainLoop( pMainProc proc );
  bool         callMainLoop( void );
  void         storeBattleResult( void );
  bool         isBgmFadeOutDisable( void ) const;

  bool         MainLoop_StandAlone( void );
  bool         MainLoop_Comm_Server( void );
  bool         MainLoop_Comm_NotServer( void );
  bool         MainLoop_Comm_Error( void );
  void         viewUpdate( void );

  void         watchRemoteClientCommunication( void );
  bool         canLaunchDammyClient( void ) const;
  void         launchDammyClient( BTL_CLIENT_ID clientId );

  void         watchRemoteServerLoss( void );
  void         changeServerSelf( void );

  BtlBagMode   checkBagMode( const BATTLE_SETUP_PARAM* sp ) const;
  BtlResult    checkWinner( void );

  void     srcParty_Create( void );
  void     srcParty_Delete( void );
  void     srcParty_Set( u8 clientID, const PokeParty* party );
  void     srcParty_SetSerial( u8 clientID, const void* data );
  void     setSrcPartyToPokecon( POKECON* pokecon, u8 clientID, bool forServer );

  Savedata::ZukanData* createZukanDataClone( HeapHandle heap, const Savedata::ZukanData* original ) const;

  bool setup_alone_single( int* seq );
  bool setup_alone_double( int* seq );
  bool setup_alone_double_multi( int* seq );
  bool setup_alone_double_multi_00( int* seq );
  bool setup_alone_double_multi_01( int* seq );
  bool setup_alone_double_multi_02( int* seq );
  bool setup_alone_double_multi_03( int* seq );
  bool setup_alone_double_multi_04( int* seq );
  bool setup_alone_double_multi_05( int* seq );
  bool setup_alone_double_multi_06( int* seq );
  bool setup_alone_double_multi_07( int* seq );
  bool setup_alone_double_multi_08( int* seq );
  bool setup_alone_double_multi_09( int* seq );
  bool setup_alone_royal( int* seq );
  bool setup_comm_single( int* seq );
  bool setup_comm_double( int* seq );
  bool setup_comm_royal( int* seq );
  bool cleanup_common( int* seq );

  void setup_alone_common_ClientID_and_srcParty( const BATTLE_SETUP_PARAM* sp );
  void setupCommon_CreateServerClient( HeapHandle heap, const BATTLE_SETUP_PARAM& setupParam );
  void setupCommon_srcParty( const BATTLE_SETUP_PARAM* sp );
  void setupCommon_TrainerParam( const BATTLE_SETUP_PARAM* sp );
  void setupCommon_SetRecplayerClientMode( const BATTLE_SETUP_PARAM* sp );
  void setupCommon_CreateViewModule( const BATTLE_SETUP_PARAM* sp, BtlBagMode bagMode );
  bool setupCommon_InitViewModule( void );
  void setupCommon_ClientPublicInformation( const BATTLE_SETUP_PARAM& setupParam );
  void setupCommon_ClientPublicInformation( ClientPublicInformation* publicInfo, u8 clientId, const MiseaiData& miseaiData );
  bool setupseq_comm_determine_server( int* seq );
  bool setupseq_comm_notify_party_data( int* seq );
  bool setupseq_comm_notify_player_data( int* seq );
  bool setupseq_comm_create_server_client( int* seq );
  bool setupseq_comm_start_server( int* seq );

  bool         checkRecReadComplete( void );
  u8           checkNumFrontPos( u8 clientID ) const;
  u32          calcMoneyDblUpRatio( void ) const;


  void         trainerParam_Init( void );
  void         trainerParam_Clear(void);
  void         trainerParam_ClearCore(TRAINER_DATA *data);
  void         trainerParam_StorePlayer(TRAINER_DATA* dst, HeapHandle heapHandle, const Savedata::MyStatus* playerData);
  void         trainerParam_StoreSelialData( TRAINER_DATA* dst, HeapHandle heapHandle, const void* serialData );
  void         trainerParam_StoreCore( TRAINER_DATA* dst, HeapHandle heapHandle );

  void         trainerParam_StoreNPCTrainer( TRAINER_DATA* dst, const BSP_TRAINER_DATA* trData );
  void         trainerParam_SetupForRecPlay( u8 clientID );
  void         trainerParam_StoreCommNPCTrainer( TRAINER_DATA* dst, const net::TRAINER_SEND_DATA* trSendData );
  bool         trainerParam_IsExist( const TRAINER_DATA* trData ) const;


  void         Bspstore_RecordData( void );
  void         Bspstore_KenteiData( void );
  void         Bspstore_Party( u8 clientID, const void* serialData );
  void         Bspstore_PlayerStatus( u8 clientID, const Savedata::MyStatus* playerStatus );

  void   Kentei_ClearField( BATTLE_SETUP_PARAM* setupParam );

  u8           expandPokePos_single( BtlExPos exType, u8 basePos, u8* dst ) const;
  u8           expandPokePos_double( BtlExPos exType, u8 basePos, u8* dst ) const;
  u8           expandPokePos_royal( BtlExPos exType, u8 basePos, u8* dst ) const;
  u8           expandPokePos_intrude( BtlExPos exType, u8 basePos, u8* dst ) const;
  void         natsukiPut( BTL_POKEPARAM* bpp, PokeTool::NatsukiType calcID ) const;
  bool         pokeID_to_PGL_RecPos( u8 pokeID, u8* pokeIdx ) const;

  void changeIntrudeClientAi( BtlPokePos intrudePokePos, BtlIntrudeType intrudeType );
  bool registerIntrudePokemon( POKECON* pokecon, BtlPokePos intrudePokePos, const pml::pokepara::PokemonParam& intrudePokeParam, bool fMemoryPokeID );
  u8   decideIntrudePokemonID( POKECON* pokeCon, u8 clientID, u8 posIndex ) const;

  void removeIllegalDirt( PokeTool::KawaigariParamCareCoreManager* kawaigariData, u8 clientID, POKECON* pokecon );


#if defined( DEBUG_ONLY_FOR_taya )
  void  test_pokeIDRec( void );
#endif

#if PM_DEBUG
public:
  const pml::pokepara::PokemonParam* DEBUG_GetIntrudePokeParam( void ) const;
  BattleViewSystem* GetBattleViewSystem( void ) const { return m_viewCore; }
  BATTLE_SETUP_PARAM* GetBattleSetupParam( void ) const { return m_setupParam; }
#endif


};


#include "./btl_party.h"
inline void CheckMemberFormCorePP( const PokeParty* party, const char* filename, u32 lineNo )
{
#if USE_BTLV_PRINT  //warningが多かったので対応
  BTL_PRINT("** check Party PP:%p ** @ %s(%d)\n", party, filename, lineNo);
  u32 cnt = party->GetMemberCount();
  for(u32 i=0; i<cnt; ++i)
  {
    const pml::pokepara::PokemonParam *pp = party->GetMemberPointerConst( i );
    BTL_PRINT("  %d: monsNo=%d, formNo=%d\n", i, pp->GetMonsNo(), pp->GetFormNo());
  }
#endif
}
inline void CheckMemberFormCoreBPP( const BTL_PARTY* party, const char* filename, u32 lineNo )
{
#if USE_BTLV_PRINT  //warningが多かったので対応
  BTL_PRINT("** check Party BPP:%p ** @ %s(%d)\n", party, filename, lineNo);
  u32 cnt = party->GetMemberCount();
  for(u32 i=0; i<cnt; ++i)
  {
    const BTL_POKEPARAM *bpp = party->GetMemberDataConst( i );
    BTL_PRINT("  %d: monsNo=%d, formNo=%d\n", i, bpp->GetMonsNo(), bpp->GetFormNo());
  }
#endif
}


GFL_NAMESPACE_END(btl)

#endif // __BTL_MAINMODULE_H__
