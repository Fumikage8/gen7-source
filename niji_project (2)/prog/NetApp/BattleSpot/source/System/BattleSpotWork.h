#if !defined(BATTLESPOTWORK_H_INCLUDED)
#define BATTLESPOTWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotWork.h
 * @date   2015/10/02 Fri. 12:57:35
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <str/include/gfl2_Str.h>
#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h>
#include  <NetStatic/NetAppLib/include/Util/NetAppTrainerIconUtility.h>

#include  "SaveData/include/MyStatus.h"

#include  "SaveData/include/BattleSpotSave.h"

#include  "../../include/BattleSpotProcParam.h"
#include  "../Views/BattleSpotViewContainer.h"


/*  country/region  */
#include  "System/include/PokemonVersion.h"
#include  "System/include/CTR/CTR_CountryNameCache.h"
#include  "System/include/CTR/CTR_RegionNameCache.h"

#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>
#include "NetStatic/NetLib/source/P2P/NexRatingClient.h"


#include <NetStatic/NetLib/source/P2P/NexRatingClientListener.h>

#if defined( GF_PLATFORM_CTR )
#else
namespace NetLib{
  namespace Validation{
    namespace PokemonValidation{
      static const u32 SIGNATURE_SIZE      = 256;   //!< 署名のサイズ
    }}}
#endif

namespace NetApp
{
  namespace BattleVideo
  {
    class BattleVideoRequestClient;
    class BattleVideoResponseListener;
  }
}



namespace NetApp    {
namespace BattleSpot {


  /// GBU用データ
  class Gbu{
  public:

    //----------------------------------------------------------
    /// 報告者の持ちポケモン（バトルに出していないの含む）  12byte
    //==========================================================
    struct TEMOTI_POKEMON{
      u16         pokeNumber;     ///< ポケモン番号   2
      u8          formNumber;     ///< フォルム番号   1
      u8          sex;            ///< 性別          1
      u16         waza[4];        ///< 持っている技   8
    };

    //-------------------------------------
    /// 対戦に出したポケモン情報    12byte
    //=====================================
    struct BATTLE_POKEMON{
      u32         mons_no       :16;  ///< ポケモン番号
      u32         form_no       : 8;  ///< フォルム番号
      u32         lv            : 8;  ///< レベル
      u32         sex           : 8;  ///< 性別
      u32         tokusei       :16;  ///< 特性
      u32         seikaku       : 8;  ///< 性格
      u32         item          :16;  ///< 持ち物
      u32         pad           : 2;  ///< 余り
    };

    //-------------------------------------
    /// 自分が倒された情報    10byte
    //=====================================
    struct LOSE_POKEMON_INFO{
      u16  deadPokeNumber;    ///< ひん死になった場合はポケモンナンバーが入る／ならなければ MONSNO_NULL
      u16  deadPokeForm;      ///< ひん死になった時のフォルムナンバー
      u16  finishWazaNumber;  ///< 相手のワザによってひん死になったらワザナンバーが入る／それ以外 WAZANO_NULL
      u16  attackPokeNumber;  ///< そのワザを出した相手ポケモンナンバー
      u16  attackPokeForm;    ///< そのワザを出した相手ポケモンフォルムナンバー
    };

    //-------------------------------------
    /// 自分が倒した情報      10bytr
    //=====================================
    struct WIN_POKEMON_INFO{
      u16  deadPokeNumber;    ///< ひん死になった場合はポケモンナンバーが入る／ならなければ MONSNO_NULL
      u16  deadPokeForm;      ///< ひん死になった時のフォルムナンバー
      u16  finishWazaNumber;  ///< 相手のワザによってひん死になったらワザナンバーが入る／それ以外 WAZANO_NULL
      u16  attackPokeNumber;  ///< そのワザを出した相手ポケモンナンバー
      u16  attackPokeForm;    ///< そのワザを出した相手ポケモンフォルムナンバー
    };

    //-------------------------------------
    /// GBU用データ
    //=====================================
    struct GBU_DATA{
      TEMOTI_POKEMON  temotiParty[ pml::PokeParty::MAX_MEMBERS ];  ///< 報告者の持ちポケモン
      BATTLE_POKEMON  myParty[ pml::PokeParty::MAX_MEMBERS ];      ///< 報告者が対戦に出したポケモン情報
      BATTLE_POKEMON  enemyParty[ pml::PokeParty::MAX_MEMBERS ];   ///< 対戦相手が対戦に出したポケモン情報

      LOSE_POKEMON_INFO losePokemonInfo[ pml::PokeParty::MAX_MEMBERS ];  ///< 自分が倒された情報
      WIN_POKEMON_INFO  winPokemonInfo[ pml::PokeParty::MAX_MEMBERS ];   ///< 自分が倒した情報

      u64             battleVideoNumber;            ///< バトルビデオナンバー
      u8              battleVideoAutoSend;          ///< バトルビデオが自動送信されたかどうか
      char            myNexUniqueId[16 + 1];        ///< 自分のNexUniqueIDを16進数表記で文字としていれる
      char            enemyNexUniqueId[16 + 1];     ///< 相手のNexUniqueIDを16進数表記で文字としていれる
      u8              pad1;
      u8              battleTeamID[12];             ///< バトルチームID
    };



  public:
    /// コンストラクタ
    Gbu( void ){
      STATIC_ASSERT( sizeof(TEMOTI_POKEMON) == 12 );
      STATIC_ASSERT( sizeof(BATTLE_POKEMON) == 12 );
      STATIC_ASSERT( sizeof(LOSE_POKEMON_INFO) == 10 );
      STATIC_ASSERT( sizeof(WIN_POKEMON_INFO) == 10 );
      STATIC_ASSERT( sizeof(GBU_DATA) == 392 );
    };

    /// デストラクタ
    virtual ~Gbu( void ){};

  public:

    /// PGL集計データ変換
    static void ConvertBattleData( GBU_DATA* gbu_data, BATTLE_PGL_RECORD_SET* pgl_record, pml::PokeParty *temotiParty, pml::PokeParty *myParty, pml::PokeParty *enemyParty );

    /// PGL集計データにNexUniqueID設定
    static void SetNexUniqueID( GBU_DATA* gbu_data, const u64 myNexUniqueId, const u64 enemyNexUniqueId );

#if PM_DEBUG
    /// 情報表示
    static void Dump( GBU_DATA& gbu_data );
#else
    static void Dump( GBU_DATA& gbu_data ){};
#endif

  private:

    /// 報告者の手持ちポケモンセット
    static void SetTemotiParty( GBU_DATA* gbu_data, pml::PokeParty *party );

    /// 報告者が対戦に出したポケモン情報セット
    static void SetBattleMyParty( GBU_DATA* gbu_data, pml::PokeParty *party );

    /// 対戦相手が対戦に出したポケモン情報セット
    static void SetBattleEnemyParty( GBU_DATA* gbu_data, pml::PokeParty *party );

    /// 自分が倒された情報セット
    static void SetLosePokemonInfo( LOSE_POKEMON_INFO* info, BATTLE_PGL_RECORD* pgl_record );
    /// 自分が倒した情報セット
    static void SetWinPokemonInfo( WIN_POKEMON_INFO* info, BATTLE_PGL_RECORD* pgl_record );

  };



  enum
  {
    MATCHING_TIMEOUT_MSEC = 30 * 1000,
    MATCHING_TIMEOUT_MSEC_FREE = 90 * 1000,
    MAX_MATCHING_TIMEOUT = 3,
    MULTI_PLAYER_NUM = 4,
    MAX_COMMUNITY_NUM = 5//レート４つとインターネット大会
  };

  //-------------------------------------
  /// インターネット公式大会参加ワーク定義
  //=====================================
  enum ENTRY_WORK
  {
    ENTRY_NOT         = 0,      ///< 未エントリー
    ENTRY_FINISH      = 1,      ///< エントリー完了
    ENTRY_REGULATION  = 2,      ///< レギュレーション取得済み
    ENTRY_BATTLEBOX   = 3,      ///< バトルボックス登録完了
    ENTRY_UPLOAD      = 4,      ///< 全セーブデータアップ完了
    ENTRY_END         = 5,      ///< 大会終了
    ENTRY_RETIRE      = 6,      ///< リタイア
    ENTRY_CANCEL      = 7       ///< 参加解除（失格など）
  };

//==============================================================================
/**
  */
//==============================================================================
class BattleSpotWork
  : public NetAppLib::System::ApplicationWorkBase
#if defined( GF_PLATFORM_CTR )
  , public NetLib::P2P::NexRatingClientListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleSpotWork );
public:

  //レギュレーションチェックデータ
  struct REGULATION_COMP
  {
    u8  btl_rule;
    u8  level;
    u8  level_range;
    u8  reverse;
    u8  time_limit_type;
    u8  time_vs;
    u8  time_command;
    u8  nickname;
    u8  background;
    u8  show_poke;
    u8  show_poke_time;
    //@fix[#3925 仲間大会：レギュレーションを改ざんしていてもマッチングが成立する]↓再報告追加分
    u8  category;                                           //! カテゴリ種別
    u8  team_num_lo;                                        //! バトルチーム下限
    u8  team_num_hi;                                        //! バトルチーム上限
    u8  select_num_lo;                                      //! 選出ポケモン下限
    u8  select_num_hi;                                      //! 選出ポケモン上限
    u8  legend_num_hi;                                      //! 伝説参加上限
    u16 must_poke_and[ Regulation::MUST_POKE_NUM ];                     //! 必須ポケモンAND 12byte
    u8  must_poke_form_and[ Regulation::MUST_POKE_NUM ];                //! 必須ポケモンANDフォルム 6byte
    u16 must_poke_or[ Regulation::MUST_POKE_NUM ];                      //! 必須ポケモンOR 12byte
    u8  must_poke_form_or[ Regulation::MUST_POKE_NUM ];                 //! 必須ポケモンORフォルム 6byte
    u8  qr_team;                                            //! QRバトルチーム使用可能フラグ 
  };

  //同期データ
  struct SYNC_DATA
  {
    pml::PokeParty::SERIALIZE_DATA m_serializeBuffer;
    u32 m_rate;
    u8  m_isQRTeam;
    u8  padding[3];
    u8  m_signatureBuffer[NetLib::Validation::PokemonValidation::SIGNATURE_SIZE];
    REGULATION_COMP regulationComp;
  };

  /*  country/region  */

  enum  RegionTargetID
  {
    REGIONTARGET_Player,
    REGIONTARGET_Opponent0,
    REGIONTARGET_Opponent1,
    REGIONTARGET_Opponent2,
  };

  enum
  {
    RULE_FREE_SINGLE = 0,
    RULE_FREE_DOUBLE,
    RULE_FREE_ROYAL,
    RULE_RATE_SINGLE,
    RULE_RATE_DOUBLE,
    RULE_RATE_SPECIAL,
    RULE_RATE_WCS,
    RULE_NET_CUP,
    RULE_FRIEND_CUP
  };

  enum
  {
    COMMUNITY_RATE_SINGLE = 0,
    COMMUNITY_RATE_DOUBLE,
    COMMUNITY_RATE_SPECIAL,
    COMMUNITY_RATE_WCS,
    COMMUNITY_INTERNET_CUP_START,
    COMMUNITY_INTERNET_CUP_END,
    COMMUNITY_NUM,
  };


  static const s32 DEFAULT_RATE = 1500;   ///< デフォルトのレート値
  static const u32 CUT_RANGE    = 5;      ///< 切断率の範囲
  static const u32 CONNECT_COMPLETE_TIMING = 70;

public:
  BattleSpotWork(void);
  virtual ~BattleSpotWork();


public:


  void                      SetupCountryRegionNameCache(void);
  u8                        GetPokemonLanguageID(void) const;
  Savedata::MyStatus*       GetTargetStatus(const RegionTargetID regionTargetID);
  const gfl2::str::STRCODE* GetCountry(const RegionTargetID regionTargetID);
  const gfl2::str::STRCODE* GetRegion(const RegionTargetID regionTargetID);

  Savedata::BattleSpotData::ENTRY_COMMUNITY_TYPE GetCategoryID( void );

  bool CheckPokeParty( u32 netID , u8* pSignatureBuffer , REGULATION_COMP* pRegulationComp );

  void SetupRegulationComp( REGULATION_COMP* pOutput );

  bool CheckP2PDisconnect( void );

  void SetRule( u32 rule ){ m_pAppParam->m_rule = rule; };
  u32 GetRule( void ) { return m_pAppParam->m_rule; };
  void SetLegendEnable( bool b ){ m_pAppParam->m_isLegendEnable = b; };
  bool GetLegendEnable( void ) { return m_pAppParam->m_isLegendEnable; };

  u32  GetOpponentRate( void ) { return m_pAppParam->m_opponentRate; };
  void SetOpponentRate( u32 rate )  { m_pAppParam->m_opponentRate = rate; };

  RANK_DATA& GetRankData( void ) { return m_pAppParam->m_rankData; };
  void       SetRankData( RANK_DATA& rank ){ m_pAppParam->m_rankData = rank; };

  COMMON_DATA& GetCommonData( void ) { return m_pAppParam->m_commonData; };
  void       SetCommonData( COMMON_DATA& rank ){ m_pAppParam->m_commonData = rank; };

  void SetBattleVideoAutoUpload( bool b ) { m_pAppParam->m_autoBattleVideoUpload = b; }
  bool GetBattleVideoAutoUpload( void ) { return m_pAppParam->m_autoBattleVideoUpload; }

  void SetGatheringID( u32 index , u32 id )
  {
    if( index >= MAX_COMMUNITY_NUM )
      return;

    m_gatheringID[index] = id;
  }

  bool IsSendPglRecordFinish( void ) { return m_isPGLReportEnd; }

#if defined(GF_PLATFORM_CTR)

  nn::nex::DateTime* GetCommunityEndDateTime( u32 index )
  {
    GFL_ASSERT( index < COMMUNITY_NUM );

    return &m_pAppParam->m_communityEndDateTime[index];
  }

  bool SendPglRecord( void );

  //------------------------------------------------------------------
  /**
   * @brief 自分の送信情報をセット
   * @param slot  セットするスロット
   * @param rate  勝敗レート
   * @param gbu_data  GBU用データ
   */
  //------------------------------------------------------------------
  void SetMyReportStatusInfo( NetLib::P2P::NexRatingClient::REPORT_SLOT_ID slot, NetLib::P2P::NexRatingClient::REPORT_RATE rate, Gbu::GBU_DATA *gbu_data );

  //------------------------------------------------------------------
  /**
   * @brief 相手の送信情報をセット
   * @param slot  セットするスロット
   * @param rate  勝敗レート
   * @param gbu_data  GBU用データ
   */
  //------------------------------------------------------------------
  void SetOtherReportStatusInfo( NetLib::P2P::NexRatingClient::REPORT_SLOT_ID slot, NetLib::P2P::NexRatingClient::REPORT_RATE rate, Gbu::GBU_DATA *gbu_data );

  //--------------------------------------------------------------------------
  /**
   * @brief   バトルビデオを自動送信するかチェック
   * @retval  true:送信する 
   * @retval  false:送信しない 
   */
  //--------------------------------------------------------------------------
  bool IsAutoSend( void );

  void SendBattleVideo( NetApp::BattleVideo::BattleVideoResponseListener* pListener );

  void EndBattleVideo();



  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバへの結果送信終了通知
  */
  //------------------------------------------------------------------
  virtual void OnRatingReportSuccess( void );

  //------------------------------------------------------------------
  /**
  * @brief Ratingサーバ通信のエラー通知
  *
  * @param[in] result    実行結果
  * @param[in] errorCode 通信エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnNexRatingClientError( const nn::nex::qResult& result, u32 errorCode );

  bool IsRegulationTimeOver( u8 hour );//過ぎている
  bool IsRegulationTimeBefore( u8 hour );//まだ始まってない
  bool IsRegulationTimeOut( u8 hour );//時間外


#endif

public:

  //Ranking
  //--------------------------------------------------------------------------
  /**
   * @brief   切断フラグをON
   * @param   data    ランキングサーバーの共通データ
   *
   * @note    バトル前に呼ばれる。
   *          バトル後は必ず切断フラグOFFを呼んでください
   */
  //--------------------------------------------------------------------------
  static void DisconnectFlagOn( COMMON_DATA& data );

  //--------------------------------------------------------------------------
  /**
   * @brief   切断フラグをOFF
   * @param   data    ランキングサーバーの共通データ
   */
  //--------------------------------------------------------------------------
  static void DisconnectFlagOff( COMMON_DATA& data );

  //--------------------------------------------------------------------------
  /**
   * @brief   切断率を求める（フリー）
   * @param   data    ランキングサーバーの共通データ
   * @return  切断率のパーセンテージ数値
   */
  //--------------------------------------------------------------------------
  static u32 GetDisconnectRate( COMMON_DATA * data );

  //--------------------------------------------------------------------------
  /**
   * @brief   切断率を求める（レート）
   * @param   data    ランキングサーバーの成績データ
   * @return  切断率のパーセンテージ数値
   */
  //--------------------------------------------------------------------------
  static u32 GetDisconnectRate( RANK_DATA * data );


  //--------------------------------------------------------------------------
  /**
   * @brief   マッチングに使用するレートのRange設定を求める
   * @param   rate    自分のレート値
   * @param   count   マッチング試行回数
   * @return  Range
   */
  //--------------------------------------------------------------------------
  static u32 GetRateRange( const u32 rate, const u32 count );

  // @fix MMCat[127]：リボンを除外したバトルチームの一致チェック
  bool BattleTeamMatchCheckIgnoreRibon( const u8* serverTeamData, u8* targetTeamData, const u8 pokeNum );
  


public:

  /*
    members
  */
  APP_PARAM*                                  m_pAppParam;
  BattleSpotViewContainer*                    m_pViewContainer;
  NetAppLib::Util::NetAppTrainerIconUtility*  m_pTrainerIconUtility;
  u32                          m_gpfEntryFlg;
  u32                          m_internetTempEntryWork;
  u32                          m_friendTempEntryWork;
  Savedata::BattleSpotData* m_pBattleSpotSave;
  bool m_retireRequest;

#if PM_DEBUG
  u32                             m_IsDebugForcedRegionName;
#endif



private:
  /*
    members
  */
  app::util::Heap                 m_RegionHeap;

  NetApp::BattleVideo::BattleVideoRequestClient* m_pBattleVideoClient;

#if defined( GF_PLATFORM_CTR )
  System::CTR::CountryNameCache*  m_pCountryNameCache;   /**<  全ての国名          */
  System::CTR::RegionNameCache*   m_pRegionNameCache;    /**<  ある国の全ての国名  */
  u32                             m_RegionNameWork[nn::cfg::CTR::CFG_SIMPLE_ADDRESS_WORKMEMORY_SIZE / sizeof(u32)];



#endif

  u32 m_gatheringID[MAX_COMMUNITY_NUM];

  bool m_isPGLReportEnd;
};




} /*  namespace BattleSpot */
} /*  namespace NetApp    */
#endif  /*  #if !defined(BATTLESPOTWORK_H_INCLUDED)  */

