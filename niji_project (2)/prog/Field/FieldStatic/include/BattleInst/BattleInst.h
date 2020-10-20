//======================================================================
/**
 * @file BattleInst.h
 * @date 2015/01/12
 * @author saita_kazuki
 * @brief バトル施設 (sangoから移植)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BATTLE_INST_H_INCLUDED__
#define __BATTLE_INST_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Field/FieldStatic/include/BattleInst/BattleInstTool.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstData.h"

#include "Battle/include/battle_def.h" //for AI_BIT
#include "Battle/include/battle_SetupParam.h" //for BATTLE_SETUP_PARAM
#include "Battle/Regulation/include/Regulation.h" //for REGULATION for REG_XXX レギュレーションID


#include "NetStatic/NetEvent/include/RegulationSelectEvent.h"
#include "NetApp/TeamSelect/include/TeamSelectEventAppParam.h"
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/BattleTree/include/BattleTreeAppParam.h"
#include "NetStatic/NetEvent/include/QuickMatchEvent.h"

#include "niji_reference_files/script/FieldPawnTypes.h"  //BattleTreeType
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"

#include <pml/include/item/item.h>

namespace GameSys
{
  class GameManager;
}
namespace Savedata
{
  class MyStatus;
}
namespace ExtSavedata
{
  class BattleRecorderSaveData;
}
namespace NetLib { namespace P2P
{
  class P2pConnectionManager;
}}


GFL_NAMESPACE_BEGIN( BattleInst )

struct BINST_TRAINER_ROM_DATA;
struct BINST_POKEMON_ROM_DATA;

/**
 * @class 
 */
class BattleInst
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleInst );

public:

  //----------------------------------------------------------------------------
  /**
   *  @brief 生成
   *  @param  GameSys::GameManager *p_gman 
   */
  //-----------------------------------------------------------------------------
   void Create();

  //----------------------------------------------------------------------------
  /**
   *  @brief 破棄
   *  @param  GameSys::GameManager *p_gman 
   */
  //-----------------------------------------------------------------------------
  void Delete();

public:

  /**
   * @brief 
   */
  enum
  {
    BTL_INST_ENEMY1_NO = 0, ///<マルチ戦対戦相手一人目
    BTL_INST_ENEMY2_NO,     ///<マルチ戦対戦相手二人目
    BTL_INST_PARTNER_NO,    ///<マルチ戦パートナー
  };

  enum
  {
    BINST_MSG_OFFSET = 5, // トレーナー一人がもっているメッセージ数
  };

  /**
   * @brief シリアライズ通信データ
   */
  struct CommSendData
  {
    u8 SerializeMyStatus[Savedata::MyStatus::SERIALIZE_DATA_SIZE];
    pml::PokeParty::SERIALIZE_DATA Party;        ///<
    bool            SuperRank;    ///<
    int             WinNum;       ///<
  };

  /**
   * @brief 通信データ
   */
  struct CommData
  {
    pml::PokeParty* Party;        ///<
    bool            SuperRank;    ///<
    int             WinNum;       ///<
  };

  /**
   * @brief 通信受信データ
   */
  struct CommRecieveData
  {
    CommData            Data;     ///<
    Savedata::MyStatus* MyStatus; ///<
  };


  /**
   * @brief 通信対戦トレーナー
   */
  struct CommSendVsTrainer
  {
    int               no[2];
    pml::PokeParty::SERIALIZE_DATA   Party1;
    pml::PokeParty::SERIALIZE_DATA   Party2;
  };

  /**
   * @brief 通信対戦トレーナー
   */
  struct CommVsTrainer
  {
    int               no[2];      ///<
    pml::PokeParty*   Party1;     ///<
    pml::PokeParty*   Party2;     ///<
  };


  /**
   * @brief 動作モデルデータ
   */
  struct MoveModelData
  {
    bool isUsing;
    Field::BATTLE_INST_POS_INDEX index;
    gfl2::math::Vector3 pos;
    gfl2::math::Quaternion rot;
    int charaID;
    bool isDressup;
    Savedata::MyStatus* pMyStatus;
  };


  /**
   * @brief ボス出現データ
   */
  struct BossData
  {
    int tr_no;
    int flag_no;
  };

  /**
   * @brief ロイヤル敵トレーナー番号
   */
  enum RoyalEnemyIndex
  {
    BTL_INST_ROYAL_ENEMEY_INDEX_0,
    BTL_INST_ROYAL_ENEMEY_INDEX_1,
    BTL_INST_ROYAL_ENEMEY_INDEX_2,
  };

public:

  //----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
  *
   *  @param  p_gman    マネージャポインタ
   *  @param p_heap     ヒープ
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  BattleInst( GameSys::GameManager *p_gman, gfl2::heap::HeapBase* p_heap );

  //----------------------------------------------------------------------------
  /**
   *  @brief デストラクタ
   *
   */
  //-----------------------------------------------------------------------------
  ~BattleInst();

  //----------------------------------------------------------------------------
  /**
   *  @brief  ルールセット
   *
   *  @param  Regulation::BATTLE_CATEGORY category　カテゴリ
   *  @param  rule              ルール
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void SetRule( Regulation::BATTLE_CATEGORY category, Regulation::BATTLE_RULE rule );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  スーパーランクフラグセット
   *
   *  @param  bool super スーパーか？
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void SetRank( bool super );
 
  //----------------------------------------------------------------------------
  /**
   *  @brief  スーパーランクフラグ取得
   *  @return bool trueでスーパー
   */
  //-----------------------------------------------------------------------------
  bool GetRank( void ) { return mData.SuperRank; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  対戦相手の決定
   *
   *  @param  int set_slot_idx
   *  @param  data_idx 
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void MakeTrainerData( int set_slot_idx, int data_idx );
 
  //----------------------------------------------------------------------------
  /**
   *  @brief  対戦相手の決定
   *
   *  @param  int set_slot_idx  スロットインデックス
   *  @param  data_idx          データインデックス
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void MakeTrainerDataWithBoss( int set_slot_idx, int data_idx );

  //----------------------------------------------------------------------------
  /**
   *  @brief  バトル開始
   *
   *  @param  bool inLvLowerFix レベル補正をかけるか？
   *  @param  bgm_change ＢＧＭ変更するか？
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void StartBattle( bool inLvLowerFix, bool bgm_change );

  //----------------------------------------------------------------------------
  /**
   *  @brief  戦うトレーナーをバトルセットアップパラメータにセットする
   *
   *  @param  int tr_no       ハウストレーナーナンバー
   *  @param  dst             格納先トレーナーデータポインタ
   *  @param  pHeap           ヒープ
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void SetVsTrainer( int tr_no, BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap );

  //--------------------------------------------------------------
  /**
   * @brief 対戦ポケモンセット
   * @param tr_no     0～2
   * @param party     ポケパーティ
   * @param pHeap  ヒープ
   */
  //-------------------------------------------------------------- 
  void SetVsPokemon( int tr_no, pml::PokeParty* party, gfl2::heap::HeapBase* pHeap );


  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦トレーナーをグリッドに追加
  *
  *  @param  pos_index  モデルのインデックス
  *  @param  charID
  *  @param  dressup                           着せ替えモデルか？
  *  @param  x                                 Ｘ座標
  *  @param  y                                 Ｙ座標
  *  @param  z                                 Ｚ座標
  *
  *  @return none
  */
  //--------------------------------------------------------------------------
  void AddTrainerObj( Field::BATTLE_INST_POS_INDEX pos_index, int charID, float x, float y, float z );

  //----------------------------------------------------------------------------
  /**
  *  @brief  パートナートレーナーをグリッドに追加
  *
  *  @param  pos_index  モデルのインデックス
  *  @param  x                                 Ｘ座標
  *  @param  y                                 Ｙ座標
  *  @param  z                                 Ｚ座標
  *
  *  @return none
  */
  //--------------------------------------------------------------------------
  void AddDressupTrainerObj( Field::BATTLE_INST_POS_INDEX pos_index, float x, float y, float z );

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデル読み込み待ち
  *
  *  @return trueで完了
  */
  //--------------------------------------------------------------------------
  bool WaitModelLoad();

  //----------------------------------------------------------------------------
  /**
  *  @brief  モデル情報を破棄
  */
  //--------------------------------------------------------------------------
  void ClearModelData();

  //----------------------------------------------------------------------------
  /**
   *  @brief    対戦トレーナー決定
   *
   *  @param  int data_idx データインデックス
   *
   *  @return no  対戦トレーナーナンバー
   */
  //-----------------------------------------------------------------------------
  int SelectTrainer( int data_idx );

  //----------------------------------------------------------------------------
  /**
   *  @brief    連勝数加算
   *
   */
  //-----------------------------------------------------------------------------
  void AddWinCount( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief バトルに勝ったか？
   *
   *  @return　bool trueで勝利
   */
  //-----------------------------------------------------------------------------
  bool IsWin( void );
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  対戦者はボスか？
   *
   *  @return bool trueでボス
   */
  //----------------------------------------------------------------------------- 
  bool IsBoss( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  ＡＩパートナー作成
   *
   */
  //-----------------------------------------------------------------------------
  void MakeAiPartner( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  トレーナーキャラクターID取得
   *
   *  @param  int tr_no トレーナーナンバー
   *
   *  @return キャラクターID
   */
  //----------------------------------------------------------------------------- 
  int GetCharacterID( int tr_no );
  //----------------------------------------------------------------------------
  /**
  *  @brief  パートナートレーナーキャラクターID取得
  *
  *  @return キャラクターID
  */
  //----------------------------------------------------------------------------- 
  int GetPartnerCharacterID();
  //----------------------------------------------------------------------------
  /**
   *  @brief      トレーナーＩＤ取得
   *
   *  @param  int tr_no  選出トレーナー番号　0～1
   *
   *  @return トレーナーＩＤ
   */
  //----------------------------------------------------------------------------- 
  int GetTrainerID( int tr_no );

  //----------------------------------------------------------------------------
  /**
   *  @brief      パートナートレーナーＩＤ取得
   *
   *  @return トレーナーＩＤ
   */
  //----------------------------------------------------------------------------- 
  int GetPartnerTrainerID(){ return mData.PartnerData.trID; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  戦闘録画できるかチェック
  *  @return 録画OKのときにtrue
  *
  */
  //-----------------------------------------------------------------------------
  bool IsCanSaveBattleRecord();

  //----------------------------------------------------------------------
  /*
   *  @brief  戦闘開始メッセージＩＤを返す
   *  @param  no  0 or 1
   */
  //----------------------------------------------------------------------
  int GetBattleStartMsgId( int no );

  //----------------------------------------------------------------------
  /*
   *  @brief  送信バッファにデータセット
   */
  //----------------------------------------------------------------------
  void SetupSendData( void );

  //----------------------------------------------------------------------
  /*
  *  @brief  送信データ取得
  */
  //----------------------------------------------------------------------
  void GetSendCommData( CommSendData *pDest );

  //----------------------------------------------------------------------
  /*
  *  @brief  受信データを反映
  */
  //----------------------------------------------------------------------
  void SetCommData( CommSendData *pSrc );

#if 0
  //----------------------------------------------------------------------
  /*
   *  @brief  送信データ取得
   *  @return void* 送信データ
   */
  //----------------------------------------------------------------------
  void* GetCommSendData( void )
  {
    return static_cast<void*>(&mCommDataMine);
  }

  //----------------------------------------------------------------------
  /*
   *  @brief  送信データ取得
   */
  //----------------------------------------------------------------------
  void GetCommSendData( CommSendData *pDest );

#endif

  //----------------------------------------------------------------------
  /*
   *  @brief  受信パートナーデータデータ取得
   */
  //----------------------------------------------------------------------
  CommRecieveData* GetCommPartnerData( void )
  {
    return &mCommDataYours;
  }
  
  //----------------------------------------------------------------------
  /*
   *  @brief  受信データサイズを取得
   *  @return int サイズ
   */
  //----------------------------------------------------------------------
  int GetCommRecieveDataSize( void );

  //----------------------------------------------------------------------
  /*
   *  @brief  YES NO のセット
   */
  //----------------------------------------------------------------------
  void SetCommSendYesNoData( bool answer ) { mCommYesNoMine = answer; }

  //----------------------------------------------------------------------
  /*
   *  @brief  YES NO 送信データの取得
   *  @return void* 送信データ
   */
  //----------------------------------------------------------------------
  void* GetCommSendYesNoData( void )
  {
    return static_cast<void*>(&mCommYesNoMine);
  }

  //----------------------------------------------------------------------
  /*
   *  @brief  YES NO 受信データの取得
   *  @return void* 受信データ
   */
  //----------------------------------------------------------------------
  void* GetCommRecieveYesNoData( void )
  {
    return static_cast<void*>(&mCommYesNoYours);
  }

  //----------------------------------------------------------------------
  /*
   *  @brief  YES NO データサイズ取得
   *  @return int サイズ
   */
  //----------------------------------------------------------------------
  int GetCommYesNoDataSize( void ) { return sizeof(bool); }
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  P2PConnectionManager取得
   *
   *  @return マネージャポインタ
   */
  //-----------------------------------------------------------------------------
  NetLib::P2P::P2pConnectionManager* GetP2PConnectionManager( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  通信親かどうかを設定
   *  @param  isParent 親設定の時 true
   */
  //-----------------------------------------------------------------------------
  void SetIsParent( bool isParent ){ mIsParent = isParent; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  通信親かどうかを取得
   *  @return  bool 親設定の時 true
   */
  //-----------------------------------------------------------------------------
  bool GetIsParent( void ) const { return mIsParent; }

  //----------------------------------------------------------------------
  /*
   *  @brief  バトルルール取得
   *  @retval Regulation::BATTLE_RULE
   */
  //----------------------------------------------------------------------
  Regulation::BATTLE_RULE GetBattleRule( void ) { return mData.RegulationDrawInfo.GetRegulation().GetBattleRule(); }

  //----------------------------------------------------------------------
  /*
   *  @brief  マルチプレイ時の共通連勝数設定
   *  @param  カウント
   */
  //----------------------------------------------------------------------
  void SetCommWinCount( int count );

  //----------------------------------------------------------------------
  /*
   *  @brief  マルチプレイ時の共通連勝数取得
   *  @return  カウント
   */
  //----------------------------------------------------------------------
  int GetCommWinCount( void ){ return mData.CommWinCount; }

  //----------------------------------------------------------------------
  /*
   *  @brief  マルチプレイかどうかの設定
   *  @param  inComm true でマルチ
   */
  //----------------------------------------------------------------------
  void SetIsComm( bool isComm ){ mData.Comm = isComm; }

  //----------------------------------------------------------------------
  /*
   *  @brief  マルチプレイかどうかの取得
   *  @return bool    true でマルチ
   */
  //----------------------------------------------------------------------
  bool GetIsComm( void ){ return mData.Comm; }

  //----------------------------------------------------------------------
  /*
   *  @brief  通信切断済みフラグセット
   *  @return bool    true で切断済み
   */
  //----------------------------------------------------------------------
  bool GetIsCommExit( void ){ return mData.CommExit; }

  //----------------------------------------------------------------------
  /*
   *  @brief  通信切断済みフラグセット
   */
  //----------------------------------------------------------------------
  void SetIsCommExit( void ){ mData.CommExit = true; }

  //----------------------------------------------------------------------
  /*
  *  @brief  通信マルチ対戦トレーナーのセット
  *  @return データの出力先
  */
  //----------------------------------------------------------------------
  void SetCommVsTrainer( CommSendVsTrainer* pSrc );

  //----------------------------------------------------------------------
  /*
   *  @brief  通信マルチ対戦トレーナーの取得
   *  @return データの出力先
   */
  //----------------------------------------------------------------------
  void GetCommVsTrainer( CommSendVsTrainer* pDest );

  //----------------------------------------------------------------------
  /*
   *  @brief  通信マルチ対戦トレーナー番号設定
   *  @param data_idx1　一人目
   *  @param data_idx2　二人目
   */
  //----------------------------------------------------------------------
  void SetCommVsTrainerNo( int data_idx1, int data_idx2 )
  {
    ONOUE_PRINT("通信対戦相手セット\n");
    mCommVsTrainer.no[0] = data_idx1;
    mCommVsTrainer.no[1] = data_idx2;
  }

  //----------------------------------------------------------------------
  /*
   *  @brief  通信マルチ対戦トレーナー番号の取得
   *  @param data_idx1　一人目
   *  @param data_idx2　二人目
   */
  //----------------------------------------------------------------------
  void GetCommVsTrainerNo( int *data_idx1, int *data_idx2 )
  {
    ONOUE_PRINT("通信対戦相手取得\n");
    *data_idx1 = mCommVsTrainer.no[0];
    *data_idx2 = mCommVsTrainer.no[1];
  }

  //----------------------------------------------------------------------
  /**
   *  @brief  施設トレーナーID -> 勝ちメッセージIDに変換
   *  @param trID トレーナーＩＤ
   *  @return u32 勝ちメッセージID
   */
  //----------------------------------------------------------------------
  static u32   TrainerIDtoWinStrID( int trID ){   return trID * BINST_MSG_OFFSET + 1; }
  
  //----------------------------------------------------------------------
  /**
   *  @brief  施設トレーナーID -> 負けメッセージIDに変換
   *  @param trID トレーナーＩＤ
   *  @return u32 負けメッセージID
   */
  //----------------------------------------------------------------------
  static u32   TrainerIDtoLoseStrID( int trID ){  return trID * BINST_MSG_OFFSET + 2; }

  //----------------------------------------------------------------------
  /**
   *  @brief  施設トレーナーID -> パートナー戦闘開始前メッセージIDに変換
   *  @param trID トレーナーＩＤ
   *  @return u32 パートナー戦闘開始前メッセージID
   */
  //----------------------------------------------------------------------
  static u32   PartnerIDtoStartStrID( int trID ){  return trID * BINST_MSG_OFFSET + 3; }

  //----------------------------------------------------------------------
  /**
   *  @brief  施設トレーナーID -> パートナー戦闘開始前メッセージIDに変換
   *  @param trID トレーナーＩＤ
   *  @return u32 パートナー戦闘開始前メッセージID
   */
  //----------------------------------------------------------------------
  static u32   PartnerIDtoLoseStrID( int trID ){  return trID * BINST_MSG_OFFSET + 4; }

  //--------------------------------------------------------------
  /**
   * @brief  抽選の対象外とするトレーナーをセットする
   * @param   target  0 or 1
   * @param   tr_no   施設トレーナーナンバー
   */
  //--------------------------------------------------------------
  void SetNoEntryTrainer( int target, int trno );

  //----------------------------------------------------------------------------
  /**
   *  @brief    通信用対戦ポケモンセット
   *
   */
  //-----------------------------------------------------------------------------
  void SelectVsPokemonForComm( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief ポケパーティに通信用対戦ポケモンをセット
   *
   *  @param  int tr_no   トレーナー番号0～1
   *  @param  party       格納ポケパーティ
   *
   */
  //----------------------------------------------------------------------------- 
  void SetVsCommPokemonToParty( int tr_no, pml::PokeParty* party );

  //--------------------------------------------------------------
  /**
   * @brief 参加ポケモンにリボンをつける
   * @param ribbon_id
   * @param mode ツリーかロイヤルか
   * @retval true 1匹でもリボンをセットした
   * @retval false どのポケモンにもリボンをセットしていない(既にセットされていた)
   */
  //--------------------------------------------------------------
  bool SetRibbon( u32 ribbon_id, Savedata::BattleInstMode mode );

  //--------------------------------------------------------------
  /**
   * @brief レギュレーション情報ワーク取得
   */
  //--------------------------------------------------------------
  NetAppLib::UI::RegulationDrawInfo* GetRegulationDrawInfo() { return &mData.RegulationDrawInfo; }

  //--------------------------------------------------------------
  /**
   * @brief レギュレーション結果ワーク取得
   */
  //--------------------------------------------------------------
  NetApp::RegulationSelect::APP_RESULT* GetRegulationSelectResult() { return &mData.RegulationSelectResult; }

  //--------------------------------------------------------------
  /**
   * @brief ポケモン選択アプリパラメーターワーク取得
   */
  //--------------------------------------------------------------
  App::PokeList::CONTEXT_PARAM* GetPokeListContextParam() { return &mData.PokeListContextParam; }

  //--------------------------------------------------------------
  /**
   * @brief チーム選択アプリパラメーターワーク取得
   */
  //--------------------------------------------------------------
  NetApp::TeamSelect::EVENT_APP_PARAM* GetTeamSelectEventAppParam() { return &mData.TeamSelectEventAppParam; }

  //--------------------------------------------------------------
  /**
   * @brief バトルツリーアプリ(NPC選択)パラメーターワーク取得
   */
  //--------------------------------------------------------------
  App::BattleTree::APP_PARAM* GetBattleTreeAppParam() { return &mData.BattleTreeAppParam; }

  //--------------------------------------------------------------
  /**
   * @brief 選択出力用ポケパーティワーク取得
   */
  //--------------------------------------------------------------
  pml::PokeParty* GetSelectPokeParty() const { return mpSelectParty; }

  //--------------------------------------------------------------
  /**
   * @brief 通信クイック接続アプリパラメーターワーク取得
   */
  //--------------------------------------------------------------
  NetEvent::QuickMatch::EVENT_PARAM* GetQuickMatchEventParam() { return &mData.QuickMatchEventParam; }

  //--------------------------------------------------------------
  /**
   * @brief バトルロイヤル結果アプリパラメーターワーク取得
   */
  //--------------------------------------------------------------
  App::BattleRoyalResult::APP_PARAM* GetBattleRoyalResultParam() { return &mData.BattleRoyalResultParam; }

  //----------------------------------------------------------------------
  /*
  *  @brief  参加メンバー選択結果設定
  */
  //----------------------------------------------------------------------
  bool SetPokeSelectResult();

  //----------------------------------------------------------------------
  /*
  *  @brief  モデルデータ取得
  */
  //----------------------------------------------------------------------
  MoveModelData* GetModelData(int index){ return &mModelData[index]; }

  //----------------------------------------------------------------------
  /*
  *  @brief  トレーナーメッセージデータ取得
  */
  //----------------------------------------------------------------------
  gfl2::str::MsgData* GetTrainerMessageData() const { return mpTrainerNameMsgData; }

  //----------------------------------------------------------------------
  /*
  *  @brief  ロイヤル選択ランク取得
  */
  //----------------------------------------------------------------------
  Savedata::BattleRoyalRank GetSelectRankRoyal();

  //----------------------------------------------------------------------
  /*
  *  @brief  ロイヤル選択結果インスタンス取得
  */
  //----------------------------------------------------------------------
  const BattleRoyalResult& GetBattleRoyalResult() const { return mData.SetupParam.battleRoyalResult; };

  //----------------------------------------------------------------------------
  /**
   *  @brief ボス戦かどうか ロイヤル
   *  @retval true ボス戦
   *  @retval false それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsBossRoyal();

  //----------------------------------------------------------------------------
  /**
   *  @brief バトル開始 ロイヤル
   */
  //-----------------------------------------------------------------------------
  void StartBattleRoyal( void );

  //----------------------------------------------------------------------
  /*
   *  @brief ロイヤル用の生成処理
   */
  //----------------------------------------------------------------------
  void CreateForRoyal( void );

  //----------------------------------------------------------------------
  /*
   *  @brief ロイヤル用の破棄処理
   */
  //----------------------------------------------------------------------
  void DeleteForRoyal( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief バトルロイヤル結果アプリ用パラメーターの設定
   */
  //-----------------------------------------------------------------------------
  void SetupBattleRoyalResultParam( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief バトルロイヤルでエントリーした中で先頭のモンスターNoを取得
   */
  //-----------------------------------------------------------------------------
  u16 GetBattleRoyalEntoryFirstMonsNo( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief 戦うトレーナーをバトルセットアップパラメータにセットする ロイヤル
   *  @param int tr_no ハウストレーナーナンバー
   *  @param dst 格納先トレーナーデータポインタ
   *  @param pHeap ヒープ
   */
  //-----------------------------------------------------------------------------
  void SetVsTrainerRoyal( int tr_no, BSP_TRAINER_DATA* dst, gfl2::heap::HeapBase* pHeap );

  //--------------------------------------------------------------
  /**
   * @brief 対戦ポケモンセット ロイヤル
   * @param tr_no 0～2
   * @param party ポケパーティ
   * @param pHeap ヒープ
   */
  //--------------------------------------------------------------
  void SetVsPokemonRoyal( int tr_no, pml::PokeParty* party, gfl2::heap::HeapBase* pHeap );

  //----------------------------------------------------------------------
  /*
  *  @brief  現在の対戦相手をスカウトする
  */
  //----------------------------------------------------------------------
  void ScountCurrentTrainer();

  //----------------------------------------------------------------------
  /*
  *  @brief  リーリエをスカウトする
  */
  //----------------------------------------------------------------------
  void ScoutLilie();

  //----------------------------------------------------------------------
  /*
  *  @brief  対戦相手のトレーナータイプ名を登録
  */
  //----------------------------------------------------------------------
  void WordSetBinstTrainerTypeNameFromTrainerNo(u16 buf_id,u16 tr_no);

  //----------------------------------------------------------------------
  /*
  *  @brief  対戦相手のトレーナー名を登録
  */
  //----------------------------------------------------------------------
  void WordSetBinstTrainerNameFromTrainerNo(u16 buf_id,u16 tr_no);

  //----------------------------------------------------------------------
  /*
   * @brief  対戦相手のトレーナータイプ名とトレーナー名を登録
   */
  //----------------------------------------------------------------------
  void WordSetBinstTrainerTypeNameAndTrainerNameFromTrainerNo(u16 dst_buf_id,u16 type_name_buf_id,u16 name_buf_id,u16 tr_no);

  //----------------------------------------------------------------------
  /*
  *  @brief  バトルに利用したパラメーターを取得
  */
  //----------------------------------------------------------------------
  const BATTLE_SETUP_PARAM* GetBattleSetupParam() const { return &mData.SetupParam; };

  //----------------------------------------------------------------------------
  /**
   *  @brief レギュレーションを取得
   */
  //-----------------------------------------------------------------------------
  Regulation& GetRegulation(){ return mData.RegulationDrawInfo.GetRegulation(); }

  //----------------------------------------------------------------------------
  /**
   *  @brief パートナーNPCIDセット
   */
  //-----------------------------------------------------------------------------
  void SetPartnerNPCID( int id ){ mData.PartnerNPCID = id; }

  //----------------------------------------------------------------------------
  /**
   *  @brief バトル施設データチェック結果設定
   *  @param result バトル施設データチェック結果
   */
  //-----------------------------------------------------------------------------
  void SetCommCheckResult(Field::BattleInstCommCheckResult result){ mData.CommCheckResult = result; }

  //----------------------------------------------------------------------------
  /**
   *  @brief バトル施設データチェック結果取得
   */
  //-----------------------------------------------------------------------------
  Field::BattleInstCommCheckResult GetCommCheckResult(){ return mData.CommCheckResult; }

  //----------------------------------------------------------------------------
  /**
   *  @brief 参加ポケモンパーティー(バトルに渡したパーティー)内のポケモンindexをSelectParty内のポケモンindexに変換する
   *  @param 参加ポケモンパーティー(バトルに渡したパーティー)内のポケモンindex
   *  @return SelectParty内のポケモンindex
   */
  //-----------------------------------------------------------------------------
  s32 ConvertEntryPartyIndexToSelectPartyIndex( u32 index ) const ;

  //--------------------------------------------------------------
  /**
   * @brief バトルツリータイプの取得
   */
  //--------------------------------------------------------------
  Savedata::BattleTreeType GetBattleTreeType();

  //--------------------------------------------------------------
  /**
   * @brief バトルツリーランクの取得
   */
  //--------------------------------------------------------------
  Savedata::BattleTreeRank GetBattleTreeRank();

#if PM_DEBUG
  void DEBUG_SetupSingleParty();
#endif

private:

  enum{
    FEMALE_FRIEND_MSG_OFS     = 4,
    FRIEND_MSG_START          = 203,
    FRIEND_INDIVIDUAL_USER_ID = 255,
  };

  static const int AI_BIT = (BTL_AISCRIPT_BIT_WAZA_BASIC | BTL_AISCRIPT_BIT_WAZA_STRONG | BTL_AISCRIPT_BIT_WAZA_EXPERT | BTL_AISCRIPT_BIT_POKECHANGE_BASIC);

  struct BattlelInstTrainerParam
  {
    BtlRule                   rule;             ///<バトルルール
    bool                      isBoss;           ///<
    int                       trID[2];          ///<
    BINST_TRAINER_ROM_DATA*   romData[2];       ///<
    u16                       check_poke[6];    ///<マルチ時の一人目との重複チェック用
    u16                       check_item[6];    ///<マルチ時の一人目との重複チェック用
    u16                       select_poke_index[6];    ///<選出したポケモンインデックス パートナー用
  };

  struct PartnerData
  {
    int                       pokeSex;                                        ///<@fix BTS[2522]
    int                       tokuseiIndex[BattleInstTool::PARTNER_POKE_CNT];
    int                       trID;                                           ///<
    BINST_POKEMON_ROM_DATA*   pokeData[BattleInstTool::PARTNER_POKE_CNT];     ///<
    BINST_TRAINER_ROM_DATA*   romData;                                        ///<
  };

  struct EntryPokeData
  {
    int   entry_poke_pos[6];    ///<参加するポケモンの位置
    int   entry_num;            ///<参加数
    int   reg;                  ///<レギュレーション？
  };

  static const u32 ROYAL_TEMOCHI_NUM = 3;
  static const u32 ROYAL_ENEMY_NUM = BTL_CLIENT_ROYAL_NUM - 1;
  struct BattlelRoyalEnemyTrainerParam
  {
    bool                      isBoss;                     ///<ボスが参加しているか
    s32                       trID[ ROYAL_ENEMY_NUM ];    ///<トレーナーID(識別用)
    BINST_TRAINER_ROM_DATA*   romData[ ROYAL_ENEMY_NUM ]; ///<トレーナーROMデータ

    BattlelRoyalEnemyTrainerParam()
    {
      isBoss = false;
      for( int i = 0; i < ROYAL_ENEMY_NUM; ++i)
      {
        trID[i] = 0;
        romData[i] = NULL;
      }
    }
  };

  //ゲームシステムが持つデータ
  struct BattleInstData
  {
    BattlelInstTrainerParam                 TrainerParam;                                         ///<
    EntryPokeData                           EntryPokeData;                                        ///<
    PartnerData                             PartnerData;                                          ///<
    BATTLE_SETUP_PARAM                      SetupParam;                                           ///<
    bool                                    SuperRank;                                            ///<
    ExtSavedata::BattleRecorderSaveData*    RecData;                                              ///<
    bool                                    Comm;                                                 ///<通信か？
    bool                                    CommExit;                                                 ///<通信か？
    //gflnet2::p2p::NetGame*                  NetHandle;                                            ///<
    int                                     BtlResultPoint;                                       ///<
    int                                     PokeTotalHpMax;                                       ///<
    int                                     CommWinCount;                                         ///<通信用連勝数
    int                                     PartnerNPCID;
    //u8                                      Fashion[xy_system::dress_up::FASHION_BUFFER_SIZE];    ///<
    NetAppLib::UI::RegulationDrawInfo       RegulationDrawInfo;                                   ///< レギュレーション描画情報(レギュレーション実体)
    NetApp::RegulationSelect::APP_RESULT    RegulationSelectResult;                               ///< レギュレーション選択アプリ結果
    NetApp::TeamSelect::EVENT_APP_PARAM     TeamSelectEventAppParam;                              ///< チーム選択アプリパラメーター
    App::PokeList::CONTEXT_PARAM            PokeListContextParam;                                 ///< ポケモン選択アプリパラメーター
    App::BattleTree::APP_PARAM              BattleTreeAppParam;                                   ///< バトルツリーNPC選択アプリパラメーター
    NetEvent::QuickMatch::EVENT_PARAM       QuickMatchEventParam;                                 ///< 通信クイック接続アプリパラメーター
    BattlelRoyalEnemyTrainerParam           RoyalEnemyTrainerParam;                               ///< ロイヤル用トレーナーパラメーター
    App::BattleRoyalResult::APP_PARAM       BattleRoyalResultParam;                               ///< バトルロイヤル結果アプリパラメーター
    Field::BattleInstCommCheckResult               CommCheckResult;

    BattleInstData( gfl2::heap::HeapBase* pHeap)
      : TrainerParam()
      , EntryPokeData()
      , PartnerData()
      , SetupParam()
      , SuperRank( false)
      , RecData( NULL)
      , Comm( false)
      , CommExit( false)
      , BtlResultPoint( 0)
      , PokeTotalHpMax( 0)
      , CommWinCount( 0)
      , PartnerNPCID(0)
      , RegulationDrawInfo( pHeap)
      , RegulationSelectResult()
      , TeamSelectEventAppParam()
      , PokeListContextParam()
      , BattleTreeAppParam()
      , QuickMatchEventParam()
      , RoyalEnemyTrainerParam()
      , BattleRoyalResultParam()
      , CommCheckResult(Field::BATTLE_INST_CHECK_RESULT_NG_ETC)
    {}
  };

private:

  //--------------------------------------------------------------
  /**
   * @brief 自分のポケパーティを作成
   * @param inLvLowFix      レベル５０未満の自分のポケモンのレベルを５０まで引き上げるか？
   * @param entry_data      エントリーデータ
   * @param outParty        ポケパーティ格納場所
   */
  //--------------------------------------------------------------
  void MakeMyPokeParty( bool inLvLowerFix, const EntryPokeData *entry_data, pml::PokeParty* outParty );

  //--------------------------------------------------------------
  /**
  * @brief ポケパーティを作成
  * @param inLvLowFix      レベル５０未満の自分のポケモンのレベルを５０まで引き上げるか？
  * @param inParty        元のパーティー
  * @param entry_data      エントリーデータ
  * @param outParty        ポケパーティ格納場所
  */
  //--------------------------------------------------------------
  void MakePokeParty( bool inLvLowerFix, pml::PokeParty* inParty, const EntryPokeData *entry_data, pml::PokeParty* outParty );
  //----------------------------------------------------------------------------
  /**
   *  @brief  ＡＩパートナー作成
   *
   */
  //-----------------------------------------------------------------------------
  void MakeAiPokeParty( const PartnerData *entry_data, pml::PokeParty* outParty );

  //----------------------------------------------------------------------------
  /**
   *  @brief  AI相方ポケモン作成
   *
   *  @param  pHeap   ヒープ
   *  @param  pp_ary  ポケパラポインタ配列
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void MakePartnerPokemon( gfl2::heap::HeapBase* pHeap,
                           pml::pokepara::PokemonParam** pp_ary );

  //----------------------------------------------------------------------------
  /**
  *  @brief  対戦ポケモン作成
  *
  *  @param arc_id      アーカイブＩＤ
  *  @param td         ＲＯＭトレーナーデータポインタ
  *  @param tr_id      トレーナーＩＤ
  *  @param poke_cnt   ポケモン数
  *  @param set_poke_no  決定したポケモン配列
  *  @param set_item_no　決定したアイテム配列
  *  @param pHeap        ヒープ
  *  @param pp_ary       ポケパラポインタ配列
  *  @param select_poke_index      選出したポケモンインデックス
  *
  *  @return bool 50回抽選をした場合 true
  */
  //-----------------------------------------------------------------------------
  bool MakeTrainerPokemon( int arc_id,
    BINST_TRAINER_ROM_DATA *td,
    int tr_id,
    int poke_cnt,
    u16 *set_poke_no,
    u16 *set_item_no,
    gfl2::heap::HeapBase* pHeap,
    pml::pokepara::PokemonParam** pp_ary ,
    u16 *select_poke_index );

  //----------------------------------------------------------------------------
  /**
   *  @brief    レベル調整
   *
   *  @param  bool lv_lower_fix レベル調整するか？
   *  @param  pp    対象ポケモンパラム
   *
   *  @return none
   */
  //-----------------------------------------------------------------------------
  void AdjustLevel( bool lv_lower_fix, pml::pokepara::PokemonParam* pp );

  //--------------------------------------------------------------
  /**
   * バトル施設トレーナー抽選
   * @param   tr_no   マルチの一人目トレーナーナンバー
   * @param   start   開始インデックス
   * @param   width   抽選幅
   *
   * @return  抽選結果トレーナーナンバー
   *
   * @fix     BTS[2176]:連戦している間に同じトレーナが立て続けに出ないように処理を変更。連戦ででなければＯＫとする
   */
  //--------------------------------------------------------------
  int SelectTrainerCore( int tr_no, int start, int width );

  //----------------------------------------------------------------------
  /*
  *  @brief  トレーナー名メッセージデータを作成
  */
  //----------------------------------------------------------------------
  void InitTrainerMsgData( void );

  //----------------------------------------------------------------------
  /*
  *  @brief  トレーナー名メッセージデータを削除
  */
  //----------------------------------------------------------------------
  void FinalizeTrainerMsgData( void );

  //----------------------------------------------------------------------
  /*
  *  @brief ランク別トレーナーIDオフセット取得 ロイヤル
  */
  //----------------------------------------------------------------------
  u32 GetTrainerIDRankOffsetRoyal( Savedata::BattleRoyalRank rank ) const ;

  //----------------------------------------------------------------------
  /*
  *  @brief ランク別トレーナーID範囲サイズ取得 ロイヤル
  */
  //----------------------------------------------------------------------
  u32 GetTrainerIDRankRangeSizeRoyal( Savedata::BattleRoyalRank rank ) const ;

  //----------------------------------------------------------------------------
  /**
   *  @brief 対戦トレーナー決定 ロイヤル
   *  @param int data_idx データインデックス
   *  @param bool isBoss ボスかどうか
   *  @return no 対戦トレーナーナンバー
   */
  //-----------------------------------------------------------------------------
  int SelectTrainerRoyal( int data_idx, bool isBoss );

  //----------------------------------------------------------------------------
  /**
   *  @brief 対戦相手の決定 ロイヤル
   *  @param int set_slot_idx
   *  @param data_idx 
   */
  //-----------------------------------------------------------------------------
  void MakeTrainerDataRoyal( int set_slot_idx, int data_idx );

  //----------------------------------------------------------------------------
  /**
   *  @brief ボス戦フラグのセット ロイヤル
   */
  //-----------------------------------------------------------------------------
  void SetBossFlagRoyal( bool isBoss);

  //----------------------------------------------------------------------------
  /**
   *  @brief 対戦トレーナー全ての決定 ロイヤル
   */
  //-----------------------------------------------------------------------------
  void SelectTrainerAllRoyal( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief 対戦ポケモン作成 ロイヤル
   *  @param td ROMレーナーデータポインタ
   *  @param tr_id トレーナーID
   *  @param set_poke_no 決定したポケモン配列
   *  @param set_item_no 決定したアイテム配列
   *  @param pHeap ヒープ
   *  @param pp_ary ポケパラポインタ配列
   *  @return bool 50回抽選をした場合 true
   */
  //-----------------------------------------------------------------------------
  bool MakeTrainerPokemonRoyal(
    BINST_TRAINER_ROM_DATA *td,
    int tr_id,
    gfl2::heap::HeapBase* pHeap,
    pml::pokepara::PokemonParam** pp_ary
    );

  //----------------------------------------------------------------------------
  /**
   *  @brief 対戦トレーナーとポケモンを全員分作成 ロイヤル
   */
  //-----------------------------------------------------------------------------
  void SetVsTrainerAndPokemonAllRoyal( void );

  //--------------------------------------------------------------
  /**
  * ボスを選択
  * @param   data_index  データ番号
  * @param   winCount    勝利数
  * @param   isComm      通信かどうか
  * @param   rule        ルール
  * 
  * @return ボスと戦えない場合は、-1
  */
  //--------------------------------------------------------------
  int SelectSuperBoss( int data_idx, int winCount, bool isComm, Regulation::BATTLE_RULE rule );

  //----------------------------------------------------------------------------
  /**
   *  @brief トレーナーIDに応じたパワー乱数の取得 ロイヤル
   */
  //-----------------------------------------------------------------------------
  u8 GetPowerRndRoyal( u16 trID);

  //----------------------------------------------------------------------------
  /**
   *  @brief リボンのセット バトルチーム
   *  @param ribbonID リボンID
   *  @param selectTeamIdx 選択チーム番号
   *  @retval true 1匹でもリボンをセットした
   *  @retval false どのポケモンにもリボンをセットしていない(既にセットされていた)
   */
  //-----------------------------------------------------------------------------
  bool SetRibbon_BattleTeam( u32 ribbonID, u32 selectTeamIdx);

  //----------------------------------------------------------------------------
  /**
   *  @brief リボンのセット 自分のパーティ
   *  @param ribbonID リボンID
   *  @retval true 1匹でもリボンをセットした
   *  @retval false どのポケモンにもリボンをセットしていない(既にセットされていた)
   */
  //-----------------------------------------------------------------------------
  bool SetRibbon_MyParty( u32 ribbonID);

  //----------------------------------------------------------------------------
  /**
   *  @brief パートナーかチェック
   *  @param no チェックするトレーナーNO
   *  @retval パートナーの場合にtrue
   */
  //-----------------------------------------------------------------------------
  bool IsPartner(int no);

  //----------------------------------------------------------------------------
  /**
   *  @brief トレーナーロムデータから捕獲ボールIDを取得
   *  @param p_heap 一時確保用ヒープ
   *  @param td トレーナーデータ
   *  @return 捕獲ボールID
   */
  //-----------------------------------------------------------------------------
  item::BALL_ID GetBallIDByTrainerRomData( gfl2::heap::HeapBase* p_heap, const BINST_TRAINER_ROM_DATA* td );

  //----------------------------------------------------------------------------
  /**
   *  @brief トレーナーロムデータから捕獲ボールIDを取得
   *  @param win_count 勝利数
   *  @param tr_no トレーナーインデックス（一人目もしくは二人目）
   *  @return トレーナーID
   */
  //-----------------------------------------------------------------------------
  int SelectRandamTrainer( int win_count, int tr_no );


#if PM_DEBUG

  //----------------------------------------------------------------------------
  /**
   *  @brief デバッグ用。強制トレーナーNo取得
   *  @param slotIndex データ格納スロット番号
   *  @param dataIndex 抽選されたトレーナーNo
   *  @return 強制トレーナーNoが有効ならそれを返す。無効ならslotIndexを返す
   */
  //-----------------------------------------------------------------------------
  s32 Debug_GetRoyalForceVsTrainerNo( s32 slotIndex, s32 dataIndex );

#endif // PM_DEBUG

private:

  BattleInstData                                mData;                            ///<
  CommData                                      mCommDataMine;                    ///<
  CommRecieveData                               mCommDataYours;                   ///<
  bool                                          mCommYesNoMine;                   ///<
  bool                                          mCommYesNoYours;                  ///<
  CommVsTrainer                                 mCommVsTrainer;                   ///<
  gfl2::heap::HeapBase*                         mp_heap;                          ///<
  gfl2::heap::HeapBase*                         mp_heap_tmp;                      ///<
  GameSys::GameManager*                         mp_gman;                          ///<
  bool                                          mIsParent;                        ///<マルチプレイ時、親かどうか
  int                                           mPastTrNo[2];                     ///<連戦中の前戦のトレーナーナンバー
  pml::PokeParty*                               mpSelectParty;                    ///<選択されたパーティ
  gfl2::str::MsgData*                           mpTrainerNameMsgData;             ///<ツリー用トレーナー名メッセージデータ
  gfl2::str::MsgData*                           mpTrainerNameMsgDataRoyal;        ///<ロイヤル用トレーナー名メッセージデータ

  MoveModelData                                 mModelData[BattleInstDefine::BINST_MODEL_DISP_MAX];
};

GFL_NAMESPACE_END( BattleInst )

#endif // __BATTLE_INST_H_INCLUDED__

