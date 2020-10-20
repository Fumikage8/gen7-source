//=============================================================================
/**
 * @file    battle_SetupTrainer.h
 * @brief   バトルに渡すトレーナーデータオブジェクト
 * @date    2012.11.24
 * @author  tamada
 */
//=============================================================================
#if !defined  __BATTLE_SETUPTRAINER_H__
#define  __BATTLE_SETUPTRAINER_H__

#include <str/include/gfl2_Str.h>
#include <debug/include/gfl2_assert.h>

#include <System/include/GameStrLen.h>
#include <Trainer/Trainer/include/TrainerTypeData.h>
#include <niji_conv_header/trainer/trid_def.h>




//=============================================================================
/**
 * @class BSP_TRAINER_DATA
 * @brief バトルに引き渡すトレーナーデータ
 */
//=============================================================================
class BSP_TRAINER_DATA
{
  GFL_FORBID_COPY_AND_ASSIGN(BSP_TRAINER_DATA);

public:

  //-----------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //-----------------------------------------------------------------------------
  BSP_TRAINER_DATA( gfl2::heap::HeapBase* heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~BSP_TRAINER_DATA();

  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナータイプデータ読み込み
   * @param heap         使用するヒープ
   * @param trainerType  読み込むトレーナータイプ
   */
  //-----------------------------------------------------------------------------
  void BSP_TRAINER_DATA::LoadTrTypeData( gfl2::heap::HeapBase* heap, trainer::TrType trainerType );

  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナータイプデータ読み込み
   * @param heap                使用するヒープ
   * @param trainerType         読み込むトレーナータイプ
   * @param trainerTypeNameData トレーナータイプ名のメッセージデータ
   */
  //-----------------------------------------------------------------------------
  void BSP_TRAINER_DATA::LoadTrTypeData( 
    gfl2::heap::HeapBase* heap, 
    trainer::TrType       trainerType, 
    gfl2::str::MsgData*   trainerTypeNameData );

  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナータイプデータの再読み込み
   * @param heap  使用するヒープ
   *
   * 現在保持しているトレーナータイプIDから再読み込みを行う。
   * ※別言語データの場合に自国語に書き戻すために使用する
   */
  //-----------------------------------------------------------------------------
  void ReloadTrTypeData( gfl2::heap::HeapBase* heap );



	//-----------------------------------------------------------------------------
	/**
	 * @brief   トレーナータイプデータ取得
	 * @return  トレーナータイプデータ
	 */
	//-----------------------------------------------------------------------------
	const trainer::TrainerTypeData * GetTrTypeData(void) const; 

  u32 GetTrainerID( void ) const { return mCore.tr_id; }
  u32 GetAIBit( void ) const { return mCore.ai_bit; }
  BattleEffectId GetBattleEffectID( void ) const { return mCore.btl_eff_id; }
  u16 GetTrainerType( void ) const { return mCore.tr_type; }
  u8 GetTrainerGroup( void ) const { return mCore.tr_group; }
  u8 GetTrainerSex( void ) const { return mCore.tr_sex; }
  u8 GetGoldParam( void ) const { return mCore.gold; }

  u16 GetUseItem( int index ) const { return mCore.use_item[ index ]; }
  const gfl2::str::StrBuf * GetName( void ) const { return mCore.name; }
  const gfl2::str::StrBuf * GetTrTypeName( void ) const { return mCore.trtype_name; }
  u8 GetTrTypeGrammarAttr( void ) const { return mCore.trtype_grammar; }
  u8 GetTrTypePlural( void ) const;
  bool HasMsgArcId( void ) const { return mCore.msgArcID != MSGARC_ID_NULL; }
  u32 GetMsgArcId( void ) const { return mCore.msgArcID; }
  u16 GetMsgWinId( void ) const { return mCore.win_strID; }
  u16 GetMsgLoseId( void ) const { return mCore.lose_strID; }

  void SetTrainerID( u32 id ) {  mCore.tr_id = id; }
  void SetAIBit( u32 bit ) { mCore.ai_bit = bit; }
  void SetBattleEffectID( BattleEffectId eff_id ) { mCore.btl_eff_id = eff_id; }
  void SetGoldParam( u8 gold ) { mCore.gold = gold; }
  void SetUseItem( const u16 * items );
  void SetName( gfl2::heap::HeapBase* pTempHeap, trainer::TrainerID trainerId );
	void SetName( gfl2::str::MsgData * msg_data, u16 str_id );
  void SetName( const gfl2::str::StrBuf & name );
  void SetTrTypeName( const gfl2::str::StrBuf & trtype_name, u8 grammar_attr );

  //トレーナータイプの書き換え(SBTS[2078]で見つけた。弟子がkujiraとsangoで違う・・・)
  void SetTrainerType( u16 type ){ mCore.tr_type = type; }

  void SetMsgID( u32 arc_id, u16 win_id, u16 lose_id ) {
    GFL_ASSERT_STOP( arc_id != MSGARC_ID_NULL );
    mCore.msgArcID = arc_id;
    mCore.win_strID = win_id;
    mCore.lose_strID = lose_id;
  }

  void SetMsgArcId( u32 arc_id ){mCore.msgArcID = arc_id;}

  enum {
    USE_ITEM_MAX = 4,
    BSP_TRAINERID_SUBWAY = 0xFFFF,
    MSGARC_ID_NULL = 0xffff,
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief シリアライズ後データ
   * @note  セーブする・通信で送信する等の場合は下記フォーマットに変換する
   * @caution 外部で直接メンバーを操作しないでください。
   */
  //-----------------------------------------------------------------------------
  struct  SERIALIZE_DATA
  {
    u64     local_friend_code;
    b32     is_exist;

    u32     tr_id;
    u32     ai_bit;
    u32     btl_eff_id;
    u16     tr_type;           //トレーナータイプ（たんぱんこぞう♂、など）
    u8      tr_group;          //トレーナーグループ（リーダー、チャンピオン、四天王、など）
    u8      tr_sex:4;            //性別（PM_MALE or PM_FEMALE）
    u8      tr_plural:4;      ///< 複数トレーナーかどうかの判定
    u8      gold;             ///< お小遣い計算係数
    u8      trtype_grammar;   ///<文法特性（性、単複、最初の音、加算性）

    u16     use_item[ BSP_TRAINER_DATA::USE_ITEM_MAX ];      //使用道具

    // トライアルハウスなどの特殊トレーナーは以下のメッセージを参照します
    u32   msgArcID;     ///< メッセージの含まれるアーカイブデータID（ARCID_MESSAGE_xxx内）
    u16   win_strID;    ///< プレイヤーに勝った時のメッセージID
    u16   lose_strID;   ///< プレイヤーに負けた時のメッセージID

    gfl2::str::STRCODE name[ System::STRLEN_PERSON_NAME+1 ]; //トレーナー名
    gfl2::str::STRCODE trtype_name[ System::STRLEN_TRAINER_TYPE+1 ];  //肩書き名

  };

  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナーデータをシリアライズする
   * @param[out]  serializedData  シリアライズしたデータの格納先
   */
  //-----------------------------------------------------------------------------
  void Serialize( BSP_TRAINER_DATA::SERIALIZE_DATA* serializedData ) const;
  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナーデータをデシリアライズする
   * @param[in]  serializedData   元となるシリアライズ済みデータのアドレス
   */
  //-----------------------------------------------------------------------------
  void Deserialize( const BSP_TRAINER_DATA::SERIALIZE_DATA* serializedData );

  //-----------------------------------------------------------------------------
  /**
   * @brief   シリアライズデータのクリア
   * @param[out]  serializedData
   */
  //-----------------------------------------------------------------------------
  static void ClearSerializeData( SERIALIZE_DATA* serializedData );





private:

  /**
   * @brief トレーナーデータ本体
   */
  struct CORE_DATA
  {
    u32               tr_id;
    u32               ai_bit;
    BattleEffectId    btl_eff_id;
    u16               tr_type;           //トレーナータイプ（たんぱんこぞう♂、など）
    u8                tr_group;          //トレーナーグループ（リーダー、チャンピオン、四天王、など）
    u8                tr_sex:4;         ///< 性別（PM_MALE or PM_FEMALE）
    u8                tr_plural:4;      ///< 複数トレーナーかどうかの判定
    u8                trtype_grammar;   ///< 文法特性（性、単複、最初の音、加算性）
    u8                gold;             ///< お小遣い計算係数
    u16               use_item[USE_ITEM_MAX];      //使用道具

    // トライアルハウスなどの特殊トレーナーは以下のメッセージを参照します
    u32   msgArcID;     ///< メッセージの含まれるアーカイブデータID（ARCID_MESSAGE_xxx内）
    u16   win_strID;    ///< プレイヤーに勝った時のメッセージID
    u16   lose_strID;   ///< プレイヤーに負けた時のメッセージID

    gfl2::str::StrBuf*   name;         ///<トレーナー名
    gfl2::str::StrBuf*   trtype_name; ///<肩書き名
    trainer::TrainerTypeData* tr_type_data;

  };

  CORE_DATA mCore;


	void GetTrainerTypeData( trainer::TrType tr_type );

};








#endif  //__BATTLE_SETUPTRAINER_H__
