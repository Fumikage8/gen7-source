//===================================================================
/**
 * @file    JoinFestaDataSave.h
 * @brief   JoinFestaデータ管理
 * @author  ichiraku_katsuhiko
 * @date    2015/10/16
 */
//===================================================================
#ifndef __JOIN_FESTA_DATA_SAVE_H__
#define __JOIN_FESTA_DATA_SAVE_H__

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>
#include <system/include/Date/gfl2_Date.h>

#include "SaveDataInterface.h"

// ジョインフェスタ定義
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
// パーソナルイベント定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEventDefine.h"
// 人物データ
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
// 施設データ
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesData.h"
// アトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionDefine.h"
// グローバルアトラクション定義
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaGlobalAttractionDefine.h"
// ブラックリストデータ
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaBlackPlayerData.h"
// 定型文
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaSelectWordTable.h"

GFL_NAMESPACE_BEGIN(Savedata)

/**
 *  @brief JoinFestaデータ管理クラス
 */
class JoinFestaDataSave : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaDataSave); //コピーコンストラクタ＋代入禁止

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
  JoinFestaDataSave( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  virtual ~JoinFestaDataSave(void);

public:
  //------------------------------------------------------------------------------------------
  /**
   * @brief    データの先頭ポインタを得る関数
   *
   * @return  先頭のポインタ
   */
  //------------------------------------------------------------------------------------------
  virtual void * GetData(void);

  //------------------------------------------------------------------------------------------
  /**
   * @brief    データサイズ
   *
   * @return  バイト
   */
  //------------------------------------------------------------------------------------------
  virtual size_t GetDataSize(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief セーブデータのクリア
   * @param[in] heap クリアに使用するテンポラリヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap );

public:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief 自分の定型文の初期値を設定
   * @param sex   性別
   *
   * @note  性別が決まらないと決めれない為、関数にして性別決まった後で呼ぶ
   */
  /* ----------------------------------------------------------------------------------------- */
  void InitializeDefaultPattern( const u8 sex );


public:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief 日付更新におけるリセット処理
   *
   * @note  ペナルティタイムの時は呼ばれない
   */
  /* ----------------------------------------------------------------------------------------- */
  void ResetWithDaytime();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief ペナルティタイム発動による更新（RTC改ざん検知時）
   */
  /* ----------------------------------------------------------------------------------------- */
  void PenaltyTimeUpdate();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief １分毎に更新される処理（フィールド中更新）
   * @param  diff_minute   変わった分数
   */
  /* ----------------------------------------------------------------------------------------- */
  void UpdateMinuteTimeAtField( s32 diff_minute );

public:

  /************************************************************************/
  /*  個人用                                                              */
  /*  ※個人情報を変更した場合、                                          */
  /*     PersonalDataの SetupMyselfを呼び出しビーコン送信し直す事         */
  /************************************************************************/
  //! 定型文設定
  void SetPattern( const JoinFestaScript::PatternType patternType, const u16 patternMsgId, const u16 patternCategoryMsgId );
  //! 定型文取得
  u16 GetPattern( const JoinFestaScript::PatternType patternType ) const;
  //! 定型文カテゴリ取得
  u16 GetPatternCateogry( const JoinFestaScript::PatternType patternType ) const;

  //! 友達との通信拒否フラグ設定
  void SetRejectFriend( const bool isReject );
  //! 友達との通信拒否フラグ取得
  bool IsRejectFriend() const;
  //! VIPとの通信拒否フラグ設定
  void SetRejectVip( const bool isReject );
  //! VIPとの通信拒否フラグ取得
  bool IsRejectVip() const;
  //! Guestとの通信拒否フラグ設定
  void SetRejectGuest( const bool isReject );
  //! Guestとの通信拒否フラグ取得
  bool IsRejectGuest() const;

  //! 通信対戦いつでもOKフラグ設定
  void SetAlwaysBattleOk( const bool isOk );
  //! 通信対戦いつでもOkフラグ取得
  bool IsAlwaysBattleOk() const;
  //! 通信交換いつでもOKフラグ設定
  void SetAlwaysTradeOk( const bool isOk );
  //! 通信交換いつでもOkフラグ取得
  bool IsAlwaysTradeOk() const;
  //! アトラクションいつでもOKフラグ設定
  void SetAlwaysAttractionOk( const bool isOk );
  //! アトラクションいつでもOkフラグ取得
  bool IsAlwaysAttractionOk() const;

  //! 現在のテーマを設定する
  void SetTheme( const u8 theme );
  //! 現在のテーマを取得する
  u8 GetTheme() const;

  //! 施設抽選用のランダムシード取得
  u32 GetFacilitiesLotterySeed();
  //! 施設抽選用のランダムシードをリセットする
  void ResetFacilitiesLotterySeed();

  //! アンケート回答を設定する
  void SetQuestionnaireAnswer( const u32 questionId, const u32 answer );
  //! アンケート回答を取得する
  u32 GetQuestionnaireAnswer( const u32 questionId ) const;
  //! パーソナルイベントで質問するアンケートID取得
  u8 GetQuestionnaireIdForPersonalEvent();

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /************************************************************************/
  /*  パーソナルイベント用                                                */
  /************************************************************************/
  //! パーソナルイベント抽選完了
  void SetPersonalEventLotteryOk( const u32 slotIndex, const JoinFestaFriendKey& friendKey );
  //! パーソナルイベント終了
  void SetPersonalEventFinish( const u32 slotIndex );
  //! パーソナルイベント取得
  NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::SlotData* GetPersonalEventSlotData( const u32 slotIndex );
  //! パーソナルイベント抽選時間を今の時間にする
  void SetPersonalEventLotteryTimeNow();
  //! パーソナルイベント抽選時間が経過したか
  bool IsPassedPersonalEventLotteryTime();
  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /************************************************************************/
  /*  施設用                                                              */
  /************************************************************************/
  //! 施設データ設定
  void SetFacilitiesData( const JoinFestaScript::FacilitiesIndex facilitiesIndex, NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* pData );
  //! 施設データ取得
  NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData* GetFacilitiesData( const JoinFestaScript::FacilitiesIndex facilitiesIndex );

  //! 施設を変更する
  void FacilitiesChange( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const u8 facilitiesId, const u32 characterId, const u8 color );

  //! 施設の場所を入れ替える
  void FacilitiesChangeIndex( const JoinFestaScript::FacilitiesIndex facilitiesIndex1, const JoinFestaScript::FacilitiesIndex facilitiesIndex2 );

  //! 施設を変更したふりをする
  void FacilitiesFakeChangeStart( const JoinFestaScript::FacilitiesIndex facilitiesIndex, const u8 facilitiesId, const u32 characterId, const u8 color );
  //! 施設を変更したふりを終了する
  void FacilitiesFakeChangeEnd();

  //! 起動するショップがどの施設Indexなのか設定（セーブデータではない）
  void SetFacilitiesIndexOfScriptCall( const JoinFestaScript::FacilitiesIndex facilitiesIndex );
  //! 施設の商品購入
  void SetFacilitiesBuy( const u32 itemNo );
  //! 施設の商品購入してあるかチェック
  bool IsFacilitiesBuy( const u32 itemNo );

  //! 占いの結果を設定
  void SetFortuneResult( const JoinFestaScript::FacilitiesIndex facilitiesIndex,
                         const JoinFestaScript::FortuneResultHit fortuneResultHit,
                         const JoinFestaScript::FortuneResultAction fortuneResultAction );

  //! 占いやの結果取得（当たり）
  JoinFestaScript::FortuneResultHit GetFortuneResultHitByAction( const JoinFestaScript::FortuneResultAction fortuneResultAction ) const;

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------


  /************************************************************************/
  /*  ジョインフェスタカスタム用                                          */
  /************************************************************************/
  //! コイン追加
  void AddCoin( const u32 addCoin );
  //! コイン減算
  void SubCoin( const u32 subCoin );
  //! コイン設定
  void SetCoin( const u32 subCoin );
  //! 獲得コイン取得
  u32 GetCoin() const;
  //! 獲得コイン累計取得
  u32 GetTotalCoin() const;
#if PM_DEBUG
  //! 獲得コイン累計設定
  void SetTotalCoin( u32 coin );
#endif

  //! ジョインフェスタ名設定
  void SetJoinFestaNameString( gfl2::str::StrBuf* src );
  //! ジョインフェスタ名取得
  void GetJoinFestaNameString( gfl2::str::StrBuf* dst ) const;

  //! フェスランク設定
  void SetRank( const u16 rank );
  //! フェスランク取得
  u16 GetRank() const;
  //! 次のランクまでに必要なコイン取得
  u32 GetCoinNecessaryForNextRank();

  //! ご褒美状態設定
  void SetRewardState( const JoinFestaScript::RewardType index, const JoinFestaScript::RewardStateType state );
  //! ご褒美状態取得
  JoinFestaScript::RewardStateType GetRewardState( const JoinFestaScript::RewardType index ) const;
  //! ご褒美イベント発動しているかチェック
  bool IsRewardEvent() const;
  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /************************************************************************/
  /*  ローカルアトラクション用                                            */
  /************************************************************************/
  //! 最高個人スコア取得
  u16 GetManHighScore( const JoinFestaScript::AttractionId attractionId ) const; 
  //! 最高個人スコア設定
  void SetManHighScore( const JoinFestaScript::AttractionId attractionId, const u16 score ); 
  //! 最高トータルスコア取得
  u32 GetTotalHighScore( const JoinFestaScript::AttractionId attractionId ) const; 
  //! 最高トータルスコア設定
  void SetTotalHighScore( const JoinFestaScript::AttractionId attractionId, const u32 score ); 

  //! プレイ回数加算
  void AddPlayCount( const JoinFestaScript::AttractionId attractionId );
  //! プレイ回数取得
  u16 GetPlayCount( const JoinFestaScript::AttractionId attractionId ) const;

#if PM_DEBUG
  // デバッグ用：アトラクション構造体データ取得
  NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* GetAttractionData( const u8 index );
  void SetAttractionData( const u8 index, NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData* pData );
#endif

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------


  /************************************************************************/
  /*  ブラックリスト用                                                    */
  /************************************************************************/
  //! ブラックリスト追加（空いている場所に自動追加）
  bool AddBlackList( const NetAppLib::JoinFesta::JoinFestaPersonalData& addPersonalData );
  //! ブラックリスト削除
  bool DeleteBlackList( const JoinFestaFriendKey deleteFriendKey );
  //! ブラックリストに追加されている人数取得
  u32 GetBlackListNum() const;
  //! ブラックリストプレイヤーデータ取得（登録されていない場合NULL）
  const NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData* GetBlackListPlayerData( const u32 index ) const;
  //! ブラックリストに含まれているフレンドキーかチェック
  bool IsBlackList( const JoinFestaFriendKey targetFriendKey ) const;

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /************************************************************************/
  /* 定型コメント用                                                       */
  /************************************************************************/
  //! 定型コメント開放チェック
  bool IsUseSelectWord( const u32 messageId ) const;
  //! 定型コメント開放する
  void SetUseSelectWord( const u32 messageId );

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  //----------------------------------------------------------------------------------
  /************************************************************************/
  /*  グローバルアトラクション用                                          */
  /************************************************************************/
  //! グローバルアトラクションデータ設定
  void SetGlobalAttractionData( const u8 index, NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* pData );
  //! グローバルアトラクションデータ取得
  NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData* GetGlobalAttractionData( const u8 index );

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------

  /************************************************************************/
  /*  その他用                                                            */
  /************************************************************************/
  //! 交換の電源切断ペナルティ用カウント設定
  void SetGradePenaltyCount( const u32 count ); 
  //! 交換の電源切断ペナルティ用カウント取得
  u32 GetGradePenaltyCount() const;
#if PM_DEBUG
  u32* GetTradePenaltyCount(){ return &m_body.m_tradePenaltyCount; }
#endif

  //! P2Pで最後に遊んだ人を設定
  void SetLastP2pPersonal( const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData ); 
  //! P2Pで最後に遊んだ人のパーソナルイベント終了
  void FinishLastP2pPersonalEvent();
  //! P2Pで最後に遊んだ人を取得
  NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData& GetLastP2pPersonal(); 
  //! P2Pで最後に遊んだ人のパーソナルイベント終了フラグを取得
  bool IsFinishLastP2pPersonalEvent() const;

  //! プロフィールアプリチュートリアルフラグを設定
  void SetProfileAppTutorialFlag();
  //! プロフィールアプリチュートリアルフラグを取得
  bool GetProfileAppTutorialFlag() const;
  //! プレイヤーリストアプリチュートリアルフラグを設定
  void SetPlayerListShowAppTutorialFlag();
  //! プレイヤーリストアプリチュートリアルフラグを取得
  bool GetPlayerListShowAppTutorialFlag() const;
  //! 定型文選択アプリチュートリアルフラグを設定
  void SetSelectWordAppTutorial();
  //! 定型文選択アプリチュートリアルフラグを取得
  bool GetSelectWordAppTutorial() const;

  //! プリセットNPC名前取得
  void GetPresetNpcNameString( const u32 sex, gfl2::str::STRCODE* dst ) const;
  //! プリセットNPCイベント状態取得
  u8 GetPresetNpcEventState( const u8 id ) const;
  //! プリセットNPCイベントID取得
  u8 GetPresetNpcEventId( const u8 id ) const;
  //! プリセットNPCイベントID設定
  void SetPresetNpcEventId( const u8 id, const u8 personalEventId );
  //! プリセットNPCイベント終了に変更
  void FnishPresetNpcEvent( const u8 id );

  //----------------------------------------------------------------------------------
  //----------------------------------------------------------------------------------
  //! 人物話しかけのカメラ決め用
  u8 GetPersonalTalkCameraCount();
  void AddPersonalTalkCameraCount();
  //! エアバルーンの注意喚起メッセージスキップフラグ
  void SetIsSkipMessageAirToy( bool flag );
  bool GetIsSkipMessageAirToy() const;
  //! 屋台の注意喚起メッセージスキップフラグ
  void SetIsSkipMessageStall( bool flag );
  bool GetIsSkipMessageStall() const;
  //! ホラーハウスの注意喚起メッセージスキップフラグ
  void SetIsSkipMessageHorrorHouse( bool flag );
  bool GetIsSkipMessageHorrorHouse() const;

  //! 現在のテーマ一時保存用
  void SetTempSaveTheme( u8 theme );
  u8 GetTempSaveTheme() const;

  /** UGCガイドラインメッセージ  */
  bool  GetUGCGuidelineMessageFlag_PhotoView_FromScriptTalk(void) const                   {return m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromScriptTalk;}
  bool  GetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelect(void) const                {return m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromTrainerSelect;}
  bool  GetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelectSlideShow(void) const       {return m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromTrainerSelectSlideShow;}

  void  SetUGCGuidelineMessageFlag_PhotoView_FromScriptTalk(const bool bFlag)             {m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromScriptTalk              = bFlag;}
  void  SetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelect(const bool bFlag)          {m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromTrainerSelect           = bFlag;}
  void  SetUGCGuidelineMessageFlag_PhotoView_FromTrainerSelectSlideShow(const bool bFlag) {m_body.m_UGCGuidelineMessageFlags.bPhotoView_FromTrainerSelectSlideShow  = bFlag;}

private:

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief 施設の初期値を設定
   */
  /* ----------------------------------------------------------------------------------------- */
  void InitializeDefaultFacilities();

private:
  /**
   * @brief セーブデータコア
   * 計5944byte
   */
  typedef struct
  {
    //----------------------------------------------------------------------------------
    // 個人情報 512byte
    struct{
      //----------------------------------------------------------------------------------
      u16     patternHello;                              //!< 定型：あいさつ
      u16     patternParting;                            //!< 定型：別れ
      u16     patternEmotion;                            //!< 定型：感動
      u16     patternRegret;                             //!< 定型：残念
      u16     patternSecret;                             //!< 定型：秘密
      u16     patternCategoryHello;                      //!< 定型カテゴリ：あいさつ
      u16     patternCategoryParting;                    //!< 定型カテゴリ：別れ
      u16     patternCategoryEmotion;                    //!< 定型カテゴリ：感動
      u16     patternCategoryRegret;                     //!< 定型カテゴリ：残念
      u16     patternCategorySecret;                     //!< 定型カテゴリ：秘密
      u16     padding1;
      //----------------------------------------------------------------------------------
      u8      isRejectFriend       : 1;                  //!< 友達との通信拒否フラグ
      u8      isRejectVip          : 1;                  //!< VIPとの通信拒否フラグ
      u8      isRejectGuest        : 1;                  //!< ゲストとの通信拒否フラグ
      u8      isAlwaysBattleOk     : 1;                  //!< 通信対戦いつでもOkフラグ
      u8      isAlwaysTradeOk      : 1;                  //!< 通信交換いつでもOkフラグ
      u8      isAlwaysAttractionOk : 1;                  //!< アトラクションいつでもOkフラグ
      u8      dummy                : 2;
      //----------------------------------------------------------------------------------
      u8      theme;                                      //!< 現在のテーマ
      u32     facilitiesLotterySeed;                      //!< 施設抽選用ランダムシード
      //----------------------------------------------------------------------------------
      u8      questionnaireAnswer[16];                    //!< アンケート回答
      //----------------------------------------------------------------------------------
      u8      padding2[3];
      //----------------------------------------------------------------------------------
      //----------------------------------------------------------------------------------
      u8      beforeQuestionnaireId;                      //!< アンケートイベントで前に質問したID
      //----------------------------------------------------------------------------------
      u8      padding3[464];
    };

    /**  UGCガイドラインメッセージフラグを表示したか  */
    struct UGCGuidelineMessageFlags
    {
      u8  bPhotoView_FromScriptTalk:1;              /**<  写真閲覧：会話からの接続時                      */
      u8  bPhotoView_FromTrainerSelect:1;           /**<  写真閲覧：トレーナー選択からの接続時            */
      u8  bPhotoView_FromTrainerSelectSlideShow:1;  /**<  写真閲覧：トレーナー選択からの接続時(slideshow) */
    };

    //----------------------------------------------------------------------------------
    // パーソナルイベント用
    NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::SlotData    m_personalEventSlotData[ NetAppLib::JoinFesta::JoinFestaPersonalEventDefine::ACTUATE_PERSONAL_EVENT_MAX ];
    gfl2::system::DateParameters                                    m_personalEventLotteryTime;

    //----------------------------------------------------------------------------------
    // 施設用  72x7
    NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData            m_facilities[ JoinFestaScript::FACILITIES_INDEX_MAX ];

    //----------------------------------------------------------------------------------
    // ジョインフェスタカスタム用
    u32                                 m_coin;           //!< 獲得コイン
    u32                                 m_totalCoin;      //!< 獲得コイン累計
    gfl2::str::STRCODE                  m_joinFestaName[ NetApp::JoinFesta::JoinFestaDefine::STRLEN_JOIN_FESTA_NAME + ::System::EOM_LEN ];      //! ジョインフェスタの名前（20文字+EOM）
    u16                                 m_rank;           //!< ジョインフェスタのランク
    u8                                  m_rewardState[ JoinFestaScript::REWARD_MAX ];  //! ご褒美イベント状態
    u16                                 m_fesRankMaxTotalCoin;  //!< フェスランクカンストしてからのランクアップ検出用
    u8                                  paddingCustom[3];

    //----------------------------------------------------------------------------------
    // ローカルアトラクション用 ( 8byte × 84 )
    NetAppLib::JoinFesta::JoinFestaAttractionDefine::SaveData       m_attractionData[ JoinFestaScript::ATTRACTION_NUM ];

    //----------------------------------------------------------------------------------
    // ブラックリスト用 88byte x 100
    NetAppLib::JoinFestaUI::JoinFestaBlackPlayerData::CoreData      m_blackList[ NetAppLib::JoinFesta::JoinFestaDefine::BLACK_LIST_MAX ];    

    //----------------------------------------------------------------------------------
    // 定型コメント用
    b8              m_isUseSelectWord[ NetAppLib::JoinFesta::JoinFestaSelectWordTable::OPEN_MESSAGE_NUM ];  //!< 開放フラグ
    u8              paddingSelectWord;

    //----------------------------------------------------------------------------------
    // グローバルアトラクション用( 80byte × 12)
    NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::SaveData m_globalAttractionData[ NetAppLib::JoinFesta::JoinFestaGlobalAttractionDefine::RECORD_MAX ];


    //----------------------------------------------------------------------------------
    // その他
    u32                               m_tradePenaltyCount;      //!< 交換切断ペナルティ用
    NetAppLib::JoinFesta::JoinFestaPersonalData::CoreData  m_lastP2pPersonal;  //!< P2Pで最後に遊んだ人のパーソナルデータ実体
    b8                                m_isFinishLastP2pPersonalEvent; //!< P2Pで最後に遊んだ人のパーソナルイベントが終了したか

    b8                                m_isProfileAppTutorial;       //!< プロフィールアプリチュートリアルフラグ
    b8                                m_isPlayerListShowAppTutorial;//!< プレイヤーリストアプリチュートリアルフラグ
    b8                                m_isSelectWordAppTutorial;    //!< 定型文選択アプリチュートリアルフラグ

    // プリセットNPCの名前男女分(26byte x2 52byte)
    gfl2::str::STRCODE                m_presetNpcName[ 2 ][ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];
    // プリセットNPCの発動パーソナルイベント
    u8                                m_presetNpcEventState[ NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX ];
    u8                                m_presetNpcEventId[ NetApp::JoinFesta::JoinFestaDefine::PRESET_ID_MAX ];

    /*  photoview: ugcガイドラインメッセージ  */
    UGCGuidelineMessageFlags          m_UGCGuidelineMessageFlags;

    // 定義ないのでとりあえずサイズだけ
    u8              m_dummy[ 2243 ];

  }SaveBody;

private:
  SaveBody  m_body;  // セーブデータ

  // 処理用

  JoinFestaScript::FacilitiesIndex m_facilitiesIndex; //! スクリプトで起動しているのがどの施設なのか。

  NetApp::JoinFesta::JoinFestaFacilitiesData::CoreData            m_fakeFacilitiesCoreData;//! 施設を立てた振りをする用
  JoinFestaScript::FacilitiesIndex                                m_fakeFacilitiesIndex; //! スクリプトで起動しているのがどの施設なのか。
  u8                                                              m_personalTalkCameraCount;  //! 人物話しかけのカメラ決め用
  b8                                                              m_isSkipMessageAirToy;    //! エアバルーンの注意喚起メッセージスキップフラグ
  b8                                                              m_isSkipMessageStall;     //! 屋台の注意喚起メッセージスキップフラグ
  b8                                                              m_isSkipMessageHorrorHouse;//! ホラーハウスの注意喚起メッセージスキップフラグ
  u8                                                              m_tempSaveTheme;           //! 現在のテーマを一時的に保存


};  // class JoinFestaDataSave

GFL_NAMESPACE_END(Savedata)

#endif // __JOIN_FESTA_DATA_SAVE_H__
