#if !defined(LIVEMATCHSAVE_H_INCLUDED)
#define LIVEMATCHSAVE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveMatchSave.h
 * @date   2015/11/26 Thu. 12:21:48
 * @author muto_yu
 * @brief  ライブ大会SaveData
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <pml/include/pml_PokeParty.h>

#include  "Savedata/include/SaveDataInterface.h"
#include  "Battle/Regulation/include/Regulation.h"
#include  "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"

namespace Savedata  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  ライブ大会SaveData
  *  @date   2015/11/26 Thu. 12:35:19
  *  @note   sangoではBattleMatchとしてBattleSpotやRegulationなどと共にまとめられていたが、
  *          nijiではregulationがExtSaveDataへ移動したこともありLive大会単体で分離することとした
  */
//==============================================================================
class LiveMatchData
  : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(LiveMatchData);
public:
  static const u32  CUPID_FOR_VALIDITY_CHECK_ROM  = 0xffffffff;   /**<  マッチング相手が不正チェックROMであることを判定するための予約大会ID  */


  /**
    大会成績
    8[B]
  */
  struct LIVE_RESULTS {
    u16   __cup_id;             /**<  大会ID（未使用）       */
    u16   battle_num;           /**<  対戦数(win+lose+draw)  */
    u16   win_num;              /**<  勝ち数  */
    u16   lose_num;             /**<  負け数  */

    /*  funcs  */
    void  Clear(void)
    {
//      cup_id      = competitionID;
      __cup_id    = 0;  /*  未使用  */
      battle_num  = 0;
      win_num     = 0;
      lose_num    = 0;
    }
    void  ApplyResult_Win(void)   {IncrementValue(battle_num);  IncrementValue(win_num);}
    void  ApplyResult_Lose(void)  {IncrementValue(battle_num);  IncrementValue(lose_num);}
    void  ApplyResult_Draw(void)  {IncrementValue(battle_num);}
  private:
    void  IncrementValue(u16& rTarget)  {if(rTarget < USHRT_MAX)  ++rTarget;}
  };

  /**
    対戦履歴
    13[B] + 3[B] = 16[B]
  */
  struct LIVE_RECORD {
    u32   reporter_id;          /**<  報告者の参加ID                                                            */
    u32   opponent_id;          /**<  対戦相手の参加ID                                                          */

    u8    reporter_poke;        /**<  報告者の残りポケモン数   0-6                                              */
    u8    opponent_poke;        /**<  対戦相手の残りポケモン数 0-6                                              */
    u8    battle_result;        /**<  対戦結果1 : 0)勝利                  1)敗北      2)引き分け                */
    u8    battle_result_reason; /**<  対戦結果2 : 0)全てのポケモンが瀕死  1)時間切れ  3)降参      ※nijiで追加  */

    u8    unconformity;         /**<  不整合フラグ                                                              */
    u8    padding[3];           /**<  パディング                                                  ※nijiで追加  */
  };

  static const int LIVE_RECORD_MAX  = 10;  /**<  大会履歴保存数  */


  //------------------------------------------------------------------
  /**
    * @breif   ライブ大会成績設定
    * @param   results 成績
    */
  //------------------------------------------------------------------
  void SetLiveResults( LIVE_RESULTS* results );

  //------------------------------------------------------------------
  /**
    *  @brief  ライブ大会対戦数カウントアップ
    *  @param  is_draw 引き分けかどうか
    *  @param  is_win  true:勝ち false:負け
    */
  //------------------------------------------------------------------
  void AddLiveResultCountUp( bool is_draw, bool is_win );

  //------------------------------------------------------------------
  /**
    * @breif   ライブ大会成績取得
    * @return  results 成績
    */
  //------------------------------------------------------------------
  const LIVE_RESULTS* GetLiveResults( void ) const;

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会対戦履歴設定
    * @param   record  ライブ大会履歴構造体
    */
  //------------------------------------------------------------------
  void SetLiveRecord( LIVE_RECORD* record );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会対戦履歴取得
    * @param   idx 取得したいデータのインデックス
    * 
    * @return  LIVE_RECORD   ライブ大会履歴構造体
    */
  //------------------------------------------------------------------
  LIVE_RECORD*  GetLiveRecord( int idx );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会対戦の最終履歴を取得
    * @return  LIVE_RECORD   ライブ大会履歴構造体
    */
  //------------------------------------------------------------------
  LIVE_RECORD*  GetLastLiveRecord( void );

  //------------------------------------------------------------------
  /**
    * @brief   ライブ大会参加ワーク設定
    */
  //------------------------------------------------------------------
  enum  EntryValue
  {
    ENTRYVAL_Initial,                 /**<  初期状態                        */
    ENTRYVAL_RegulationReceived,      /**<  選手証受信済み（Boxロック前）   */
    ENTRYVAL_Participating,           /**<  大会参加中（Boxロック後）       */
    ENTRYVAL_AllMatchOver,            /**<  指定対戦数消化終了              */
    ENTRYVAL_NotParticipate,          /**<  参加を解除した                  */
    
    ENTRYVAL_
  };

  void  SetLiveEntryWork( const u8 work );
  void  SetLiveEntryWork_Initial(void)            {SetLiveEntryWork(ENTRYVAL_Initial);}
  void  SetLiveEntryWork_RegulationReceived(void) {SetLiveEntryWork(ENTRYVAL_RegulationReceived);}
  void  SetLiveEntryWork_Participating(void)      {SetLiveEntryWork(ENTRYVAL_Participating);}
  void  SetLiveEntryWork_AllMatchOver(void)       {SetLiveEntryWork(ENTRYVAL_AllMatchOver);}
  void  SetLiveEntryWork_NotParticipate(void)     {SetLiveEntryWork(ENTRYVAL_NotParticipate);}


  //------------------------------------------------------------------
  /**
    *  @brief    ライブ大会参加ワーク取得
    */
  //------------------------------------------------------------------
  u8   GetLiveEntryWork( void ) const;


  //------------------------------------------------------------------
  /**
    *  @brief    シリアライズされたRegulationの取得
    */
  //------------------------------------------------------------------
  void*  GetSerializedRegulationData(void)          {return m_LiveMatchSaveData.live_regulation;}

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  u32  GetSerializedRegulationDataSize(void) const  {return sizeof(m_LiveMatchSaveData.live_regulation);}

  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  ResetResult(void)
  {
    m_LiveMatchSaveData.live_results.Clear();
  }



  //------------------------------------------------------------------
  /**
    *  @brief    使用中のPartyIDを取得
    */
  //------------------------------------------------------------------
  u16   GetPartyIndex(void) const             {return m_LiveMatchSaveData.live_party_index;}
  void  SetPartyIndex(const u16 partyIndex)   {m_LiveMatchSaveData.live_party_index  = partyIndex;}
  void  InvalidatePartyIndex(void)            {SetPartyIndex(NetApp::TeamSelect::TEAM_SELECT_ERROR);}
  bool  IsValidPartyIndex(void) const         {return (GetPartyIndex() != NetApp::TeamSelect::TEAM_SELECT_ERROR);}


  /*
    ctor/dtor
  */
  LiveMatchData(void);
  virtual ~LiveMatchData();

  /*
    implement SaveDataInterface I/F
  */
  virtual void*   GetData(void);
  virtual size_t  GetDataSize(void);
  virtual void    Clear(gfl2::heap::HeapBase* pTempHeap);

private:
  /**
    実データ
    @note  66856[B]
  */
  struct  LIVEMATCH_SAVEDATA
  {
    LIVE_RESULTS      live_results;                                           /**<  大会成績                                  */  /*    8[B]       */
    LIVE_RECORD       live_record[ LIVE_RECORD_MAX ];                         /**<  対戦履歴                                  */  /*   16[B] * 10  */

    u8                live_record_last;                                       /**<  履歴最終書き込み位置                      */  /*    1[B]       */
    u8                live_entry_work;                                        /**<  ライブ大会参加ワーク                      */  /*    1[B]       */
    u16               live_party_index;                                       /**<  boxのpartyIdx                             */  /*    2[B]       */

    u8                live_regulation[Regulation::REGULATION_MAX_DATA_SIZE_LIVE];  /**<  レギュレーション（ライブ大会専用サイズ） */  /* 1024*8[B]       */

    u8                reserve[66856 - (8 + (16*10) + 1+1+2 + Regulation::REGULATION_MAX_DATA_SIZE_LIVE)];       /**<  ※予約領域*/
  }m_LiveMatchSaveData;

  static const u32  LIVEMATCH_SAVEDATA_SIZE = sizeof(LIVEMATCH_SAVEDATA);

};





} /*  namespace Savedata  */
#endif  /*  #if !defined(LIVEMATCHSAVE_H_INCLUDED)  */
