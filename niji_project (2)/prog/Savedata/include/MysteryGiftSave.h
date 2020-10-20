//===================================================================
/**
 * @file    MysteryGiftSave.h
 * @brief   不思議な贈り物セーブデータ
 * @author  Shin Kosaka
 * @date    2012.09.13
 */
//===================================================================
#ifndef __MYSTERY_GIFT_SAVE_DATA_H__
#define __MYSTERY_GIFT_SAVE_DATA_H__

#include <pml/include/pmlib.h>
#include <system/include/Date/gfl2_Date.h>

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include "heap/include/gfl2_heap_manager.h"
#include "Savedata/include/SaveDataInterface.h"


GFL_NAMESPACE_BEGIN(Savedata)

/**
*  不思議な贈り物セーブデータ管理クラス
*/

class MysteryGiftSave : public SaveDataInterface
{
GFL_FORBID_COPY_AND_ASSIGN(MysteryGiftSave); //コピーコンストラクタ＋代入禁止

public:

  enum TalentVPattern{
    TALENT_V_1 = 252,       ///<ランダムV1(一つでもこの値が見つかったら他の値は無視 優先度高V1→低V3)
    TALENT_V_2 = 253,       ///<ランダムV2(一つでもこの値が見つかったら他の値は無視 優先度高V1→低V3)
    TALENT_V_3 = 254,       ///<ランダムV3(一つでもこの値が見つかったら他の値は無視 優先度高V1→低V3)
    TALENT_V_RANDOM = 255,  ///<ランダム
  };

  enum PokemonGiftType{
    POKEMON_GIFT_TYPE_MINE,   //<自分が親のポケモンとして受け取る
    POKEMON_GIFT_TYPE_OTHER,  //<他人が親のポケモンとして受け取る
    POKEMON_GIFT_TYPE_EGG,    //<タマゴとして受け取る
  };
public:

  /*
MysteryGiftSave::CORE SIZE=6896
MysteryGiftSave::MYSTERY_GIFT_POKE_DATA SIZE=128
MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA SIZE=12
MysteryGiftSave::MYSTERY_GIFT_POWER_DATA SIZE=4
MysteryGiftSave::MYSTERY_GIFT_DATA SIZE=268
MysteryGiftSave::RANDOM_DELIVERY_FLAG SIZE=4
MysteryGiftSave::SERIAL_DATA SIZE=8
  */

  typedef struct
  {
    u32 parentID;///<親ID 										0ならばプレイヤーのIDを入れる
    u32 version;///<ROMバージョン 								0ならば自分のROMバージョンを入れる 0.自動	24.3DSkujira X	25.3DSkujira Y		1.AGBサファイア	2.AGBルビー	3.AGBエメラルド	4.AGBファイヤーレッド	5.AGBリーフグリーン	6.定義なし	7.DSハートゴールド	8.DSソウルシルバー	9.定義なし	10.DSダイヤモンド	11.DSパール	12.DSプラチナ	13.定義なし	14.定義なし	15.GCコロシアム	16.定義なし	17.定義なし	18.定義なし	19.定義なし	20.DSホワイト	21.DSブラック	22.DSホワイト２	23.DSブラック２	24.3DSkujiraX	25.3DSkujiraY
    u32 rnd;///<個体乱数 											0＝性別、レアかどうか、特性の値を使いランダムを生成。１以上の場合は性別、レアかどうか、特性の値を無視して、この値の固体乱数で生成する。
    u16 ribbonID;///<リボンID 									ビットで16個分扱う。１つ余り
    u16 ballID;///<ボールID 										0ならばモンスターボールにする。	1.マスターボール	2.ハイパーボール	3.スーパーボール	4.モンスターボール	5.サファリボール	6.ネットボール	7.ダイブボール	8.ネストボール	9.リピートボール	10.タイマーボール	11.ゴージャスボール	12.プレミアボール	13.ダークボール	14.ヒールボール	15.クィックボール	16.プレシャスボール	17.スピードボール	18.レベルボール	19.ルアーボール	20.ベビーボール	21.ラブラブボール	22.フレンドボール	23.ムーンボール	24.コンペボール	25.ドリームボール
    u16 itemID;///<持ち物
    u16 waza1;///<技１
    u16 waza2;///<技２
    u16 waza3;///<技３
    u16 waza4;///<技４
    u16 monsNo;///<モンスターNo
    u8 formNo;///<フォルムNo
    u8 countryCode;///<国コード 									0ならばROMの国コードを入れる。	0.定義なし	1.日本	2.英語	3.フランス語	4.イタリア語	5.ドイツ語	6.未使用	7.スペイン語	8.韓国語
    wchar_t nickName[13];///<ニックネーム 						最初がEOMならデフォルト
    u8 seikaku;///<性格 											255＝ランダム
    u8 sex;///<性別 												255=ランダム（雄雌比率は実際のパーソナルデータから算出）	0.♂	1.♀	2.性別なし	255.ランダム
    u8 speabino;///<特性										0.特性1	１.特性2	2.特性3	3.特性1-2のランダム	4.特性1-3のランダム
    u8 isRare;///<レアかどうか 									0=レアにしない	1=ランダム	2=レアにする
    u16 getPlace;///<卵を手に入れた場所
    u16 bornPlace;///<卵が孵化した場所or捕獲した場所
    u8 getLevel;///<捕まえたLevel
    u8 stype;///<かっこよさ
    u8 beautiful;///<うつくしさ
    u8	cute;///<かわいさ
    u8	clever;///<かしこさ
    u8	strong;///<たくました
    u8	fur;///<毛艶
    u8	hp_rnd;///<HP乱数                                        255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	pow_rnd;///<攻撃力乱数                                   255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	def_rnd;///<防御力乱数                                   255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	agi_rnd;///<素早さ乱数                                   255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	spepow_rnd;///<特攻乱数                                  255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	spedef_rnd;///<特防乱数                                  255＝ランダム、254＝3Ｖ設定。254に設定したデータの中で抽選で３つまで31にする。それ以外はランダム。
    u8	oyasex;///<親の性別                                      	0：男	1：女	2：使用禁止	3：プレイヤーの性別
    wchar_t oyanam[13];///<親の名前                              1文字目がEOMならばプレイヤーの名前をいれる
    u8	level;///<ポケモンレベル                                 0ならばランダム
    u8	egg;///<卵かどうか                                       TRUE=タマゴ
    u16 setItem;///<同時配布アイテム
    u32	color_rnd;///<色違い乱数                                 個性乱数から分離した
    //u64	tamago_waza;///<卵技ストック                             タマゴ技を記憶しておく。
    u16 tamago_waza1;///<卵技１
    u16 tamago_waza2;///<卵技２
    u16 tamago_waza3;///<卵技３
    u16 tamago_waza4;///<卵技４
    u8	memories_level; ///<親との思い出レベル
    u8	memories_code;  ///<親との思い出事象コード
    u16	memories_data;  ///<親との思い出内容データ
    u8	memories_feel;  ///<親との思い出気持ちデータ
    u8  hp_effort;
    u8  atk_effort;
    u8  def_effort;
    u8  agi_effort;
    u8  satk_effort;
    u8  sdef_effort;
  }MYSTERY_GIFT_POKE_DATA;

  /*
  外部配布リボン15bit	ribbon_no	u16	2		ビットで16個分扱う。１つ余り。
  バトルチャンプリボン	ID_PARA_marine_ribbon		25bit目	0-1	旧マリンリボン
  エリアチャンプリボン	ID_PARA_land_ribbon		26bit目	0-1	旧ランドリボン
  ナショナルチャンプリボン	ID_PARA_sky_ribbon		27bit目	0-1	旧スカイリボン
  カントリーリボン	ID_PARA_country_ribbon		28bit目	0-1	カントリーリボン
  ナショナルリボン	ID_PARA_national_ribbon		29bit目	0-1	ナショナルリボン
  アースリボン	ID_PARA_earth_ribbon		30bit目	0-1	アースリボン
  ワールドリボン	ID_PARA_world_ribbon		31bit目	0-1	ワールドリボン
  イベントリボン	ID_PARA_sinou_history_ribbon		71bit目	0-1	旧ヒストリーリボン。syachiより外部配布リボン扱い。
  ワールドチャンプリボン	ID_PARA_sinou_red_ribbon		73bit目	0-1	旧レッドリボン
  バースデーリボン	ID_PARA_sinou_green_ribbon		74bit目	0-1	旧グリーンリボン
  スペシャルリボン	ID_PARA_sinou_blue_ribbon		75bit目	0-1	旧ブルーリボン
  メモリアルリボン	ID_PARA_sinou_festival_ribbon		76bit目	0-1	旧フェスティバルリボン
  ウィッシュリボン	ID_PARA_sinou_carnival_ribbon		77bit目	0-1	旧カーニバルリボン
  クラシックリボン	ID_PARA_sinou_classic_ribbon		78bit目	0-1	クラシックリボン
  プレミアリボン	ID_PARA_sinou_premiere_ribbon		79bit目	0-1	プレミアリボン

  */

  typedef struct
  {
    u16 itemNo1;///<道具No
    u16 itemNum1;///<アイテム個数
    u16 itemNo2;///<道具No
    u16 itemNum2;///<アイテム個数
    u16 itemNo3;///<道具No
    u16 itemNum3;///<アイテム個数
    u16 itemNo4;///<道具No
    u16 itemNum4;///<アイテム個数
    u16 itemNo5;///<道具No
    u16 itemNum5;///<アイテム個数
    u16 itemNo6;///<道具No
    u16 itemNum6;///<アイテム個数
  }MYSTERY_GIFT_ITEM_DATA;

  typedef struct
  {
    u32 value;//マメ　or　BP
  }MYSTERY_GIFT_NUM_DATA;

  //ふしぎなおくりものデータ
  typedef struct
  {
    u16     eventID;          ///<イベントID   ※データごとに一意であり、ゲーム中に同時に同一のIDを持ったデータが複数存在しないことが前提
    wchar_t eventName[37];    ///<イベント名
    u32     recvDate;         ///<受信した日付 ※8ケタの整数。2012/10/02だったら20121002となる
    u8      cardMessageID;    ///<カードメッセージID
    u8      dataType;         ///<データタイプ    MYSTERY_GIFT_TYPE:  ポケモン / アイテム / BattlePoint / 豆 / ポケモン+アイテム
    u8      recvFlag;         ///<受け取りフラグ  bit0: 一度きりしか受け取れないか  bit1: 配達員から受け取り済みか  bit2: 一日一回しか受け取れないか
    u8      rarity;           ///<レアリティ： 0:普通（カードの色：銅） 1:少し貴重（銀） 2:貴重（金）
    u8      padding1[16];     ///<パディング
    u16     crc;              ///<CRC
    u16     key;              ///<暗号キー
    u8      body[160];        ///<データボディ(MYSTERY_GIFT_POKE_DATA か MYSTERY_GIFT_ITEM_DATA か MYSTERY_GIFT_NUM_DATA のいずれかが入る)
    
    /*  funcs  */
    u16   GetYear(void) const     {return (recvDate / 10000);}
    u16   GetMonth(void) const    {return ((recvDate % 10000) / 100);}
    u16   GetDay(void) const      {return (recvDate % 100);}
    bool  GetRecvFlag(void) const {return ((recvFlag & 0x02) != 0);}    /*  受け取り済みであるか  */
    u32   GetBPData(void) const   {return ((dataType  == MYSTERY_GIFT_TYPE_bp)    ? *(reinterpret_cast<const u32*>(body)) : 0);}
    u32   GetMameData(void) const {return ((dataType  == MYSTERY_GIFT_TYPE_mame)  ? *(reinterpret_cast<const u32*>(body)) : 0);}
    /*  recvFlag  */
    bool  IsReceiveAtOnce(void) const   {return (recvFlag & (1 << 0));}   /**<  一度しか受け取れないデータか      */
    bool  IsReceived(void) const        {return (recvFlag & (1 << 1));}   /**<  配達員から受け取り済みか          */
    bool  IsReceiveOnceADay(void) const {return (recvFlag & (1 << 2));}   /**<  一日一度しか受け取れないデータか  */
  }MYSTERY_GIFT_DATA;

  //ランダム配信フラグ
  typedef struct
  {
    u16	eventID;///<イベントID
    u8 randNo;///<ランダム配信No
    u8 padding;///<パディング
  }RANDOM_DELIVERY_FLAG;


  //シリアル用セーブ
  static const s64  SERIAL_AUTH_TIMEOUT_PENALTY_MINUTES = (60*24);   /*  24h  */
  static const s64  SERIAL_AUTH_TIMEOUT_PENALTY_SECONDS = (SERIAL_AUTH_TIMEOUT_PENALTY_MINUTES * 60);
  static const u8   SERIAL_AUTH_FAILED_COUNT_TO_PENALTY = (10);      /**<  何回の認証失敗でTimeoutPenaltyを発動するか  */

  typedef struct
  {
    u16   failedYear;       ///<失敗したサーバ時間(UTC)：年
    u8    failedMonth;      ///<失敗したサーバ時間(UTC)：月
    u8    failedDay;        ///<失敗したサーバ時間(UTC)：日
    u8    failedHour;       ///<失敗したサーバ時間(UTC)：時
    u8    failedMinute;     ///<失敗したサーバ時間(UTC)：分
    bool  NGflag;           ///<シリアルＮＧフラグ
    u8    NGCounter;        ///<NGカウンター

    /*  funcs  */
    void  Clear(void);
    void  ClearPenaltyStatus(void)  {NGflag = false; NGCounter = 0;}
    bool  UpdateSerialAuthStatus(const bool isSerialOK, const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute);   /**<  シリアル認証結果を渡し、内部状態を更新する  */
    bool  QueryTimeoutPenalty(const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute);                             /**<    */
    bool  IsTimeoutPenalty(void) const    {return NGflag;}
  }SERIAL_DATA;


  //受け取るおくりものの種類
  typedef enum
  {
    MYSTERY_GIFT_TYPE_poke = 0,   ///<  ポケモン
    MYSTERY_GIFT_TYPE_item,       ///<  アイテム
    MYSTERY_GIFT_TYPE_mame,       ///<  まめ
    MYSTERY_GIFT_TYPE_bp,         ///<  BP
//    MYSTERY_GIFT_TYPE_poke_item,  ///<  ポケモン+アイテム

  }MYSTERY_GIFT_TYPE;

  enum
  {
    MYSTERY_GIFT_DATA_NUM = 48, ///< 48件保存する
    RANDOM_DELIVERY_FLAG_NUM = 50,///<ランダム配信用フラグ50件
    RECV_FLAG_NUM = 64,
    RECV_FLAG_BIT = 32,
    ONE_DAY_FLAG_NUM = 10,
//    PGL_ITEM_EVENT_ID = 0xFFFF
    PGL_ITEM_EVENT_ID = 2047    /*  下山田さん、幸坂さん要望：一度だけ受信可能フラグ（recvFlag）が2048[bit]しかないため、PGL配信データのIDも有効な値域の最大と定義する  */
  };

  ///<1日1回受信管理
  struct ONE_DAY_FLAG
  {
    u16 eventID;
    u16 year : 7;
    u16 month : 4;
    u16 day : 5;
  };

  //セーブデータ本体
  typedef struct
  {
    u32                   recvFlag[RECV_FLAG_NUM];                      ///<受信フラグ256byte
    MYSTERY_GIFT_DATA     wonderGiftData[MYSTERY_GIFT_DATA_NUM];        ///<配信データ本体 
    RANDOM_DELIVERY_FLAG  randomDeliveryFlag[RANDOM_DELIVERY_FLAG_NUM]; ///<ランダム配信用フラグ 200byte
    SERIAL_DATA           serialData;                                   ///<シリアル用セーブ 8byte
    ONE_DAY_FLAG          oneDayFlag[ONE_DAY_FLAG_NUM];                 ///<1日1回受信管理
    u8                    reserve[(64*MYSTERY_GIFT_DATA_NUM) - 40];
  }CORE_DATA;




  /**
    通信データ
  */
  typedef struct MYSTERY_GIFT_RECV_DATA
  { 
    u32                 version; 
    wchar_t             detailText[253]; 
    bool                isMovie; 
    u8                  rang; 
    bool                isRandom; 
    bool                randomNo; 
    u16                 crc; 
    u8                  randomNoMax; 
    u8                  randomParam;
    u8                  dummy[2]; 
    MYSTERY_GIFT_DATA   dataBody; 
    //1 対象バージョン 4 bitでバージョン管理。0の場合は、バージョン制限なしで配付。 ○ 
    //2 説明テキスト  505 最大252文字＋EOM1（日本語版は126文字）  ○ 
    //3 映画配信フラグ 1 特別なデモをするかどうかフラグ ○ 
    //4 対象言語バージョン 1 言語コードのナンバー  ○ 
    //5 ダミー 2   ○ 
    //6 CRC 2   ○ 
    //7 実際のデータ  264 KujiraのＰフラッシュで保持するデータ ○ 
    //8 ランダム配信フラグ 1 Kujira新規追加パラメータ。ランダム配信する／しない  ○ 
    //9 ランダム配信No. 1 Kujira新規追加パラメータ。ランダム配信の中で何番目か。最大同時は10件まで  ○ 
   
    MYSTERY_GIFT_DATA&        GetGiftData(void)       {return dataBody;}
    const MYSTERY_GIFT_DATA&  GetGiftData(void) const {return dataBody;}
    bool                      CRCCheck(u16* pCRCOut = NULL);
  }MYSTERY_GIFT_RECV_DATA;



public:
  //=============================================================================================
  /**
  *  コンストラクタ
  */
  //=============================================================================================
  MysteryGiftSave( void );
  //=============================================================================================
  /**
  *  デストラクタ
  */
  //=============================================================================================
  virtual ~MysteryGiftSave();


  //=============================================================================================
  /**
  *  ゲームシンクアプリ専用関数 ふしぎなおくりものを受け取れる状態か調べる関数
  *  @retval true : 受け取れる
  *  @retval false : 受け取れない
  */
  //=============================================================================================
  bool IsMysteryGiftItemReceived()
  { 
    for( u32 i = 0 ; i < MYSTERY_GIFT_DATA_NUM ; ++i )
    {
      if( GetGiftData( i )->eventID == PGL_ITEM_EVENT_ID )
      {
        if( GetGiftData( i )->GetRecvFlag() == false )
        {
          return true;
        }
      }
    }

    return false; 
  }


  //=============================================================================================
  /**
  *  配達員用インターフェース
  */
  //=============================================================================================


  //=============================================================================================
  /**
  *  受け取れるポケモン数取得
  */
  //=============================================================================================
  u32 GetPokeGiftNum( void );

  //=============================================================================================
  /**
  *  受け取れるアイテム数取得
  */
  //=============================================================================================
  u32 GetItemGiftNum( void );

  //=============================================================================================
  /**
  *  受け取れるBP数取得
  */
  //=============================================================================================
  u32 GetBPGiftNum( void );

  //=============================================================================================
  /**
  *  受け取れるポケマメ数取得
  */
  //=============================================================================================
  u32 GetMameGiftNum( void );

  //=============================================================================================
  /**
  *  先頭のポケモンデータ取得（受け取り済みフラグが立ちます）
  *  @param[out]   pOutput  出力先
  *  @param[in]    isReceive  trueなら受け取り済みにする
  */
  //=============================================================================================
  void GetTopPokeGiftData( pml::pokepara::PokemonParam* pOutput , gfl2::heap::HeapBase * heap , bool isReceive = true );

  //=============================================================================================
  /**
  *  先頭のポケモンデータのタイプを取得（受け取り済みフラグが立ちます）
  *  @param[out]   type  受け取りデータタイプ
  */
  //=============================================================================================
  MysteryGiftSave::PokemonGiftType GetTopPokeGiftDataType( bool isReceive = true );

  //=============================================================================================
  /**
  *  先頭のポケモンデータの同時配布アイテム取得（受け取り済みフラグが立ちます）
  *  @param[out]   itemno
  */
  //=============================================================================================
  u16 GetTopPokeGiftDataItem( bool isReceive = true );

  //=============================================================================================
  /**
  *  先頭のポケモンデータの場所ID取得（受け取り済みフラグが立ちます）
  *  @param[out]   itemno
  */
  //=============================================================================================
  u16 GetTopPokeGiftDataPlaceID( bool isReceive = true );


  //=============================================================================================
  /**
  *  先頭のアイテムデータ取得（受け取り済みフラグが立ちます）
  *  @param[out]   pOutput  出力先
  *  @param[in]    isReceive  trueなら受け取り済みにする
  */
  //=============================================================================================
  void GetTopItemGiftData( MYSTERY_GIFT_ITEM_DATA* pOutput , bool isReceive = true );

  //=============================================================================================
  /**
  *  先頭のBPデータ取得（受け取り済みフラグが立ちます）
  *  @param[in]    isReceive  trueなら受け取り済みにする
  */
  //=============================================================================================
  u32 GetTopBPGiftData( bool isReceive = true  );

  //=============================================================================================
  /**
  *  先頭のポケマメデータ取得（受け取り済みフラグが立ちます）
  *  @param[in]    isReceive  trueなら受け取り済みにする
  */
  //=============================================================================================
  u32 GetTopMameGiftData( bool isReceive = true  );




  //=============================================================================================
  /**
  *  旧インターフェース
  */
  //=============================================================================================

  //=============================================================================================
  /**
  *  不思議なおくりものデータポインタ取得(通信アプリ用)
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  const MYSTERY_GIFT_DATA* GetGiftData( u32 index ) const;
  const MYSTERY_GIFT_DATA* GetLastGiftData(void) const      {const u32 dataNum = GetGiftDataNum(); return (dataNum ? GetGiftData(dataNum-1) : NULL);}

  //=============================================================================================
  /**
  *  不思議なおくりものデータ設定(通信アプリ用)
  * @param[in]   index   インデックス
  */
  //=============================================================================================
private:
#if PM_DEBUG
public:
#endif
  bool SetGiftData( u32 index , const MYSTERY_GIFT_DATA* pData );

public:
#if PM_DEBUG
  bool PushBackGiftData(const MYSTERY_GIFT_DATA* pData) {return SetGiftData(GetGiftDataNum(), pData);}
#endif
  bool PushBackGiftData(const MYSTERY_GIFT_RECV_DATA* pData);

  //=============================================================================================
  /**
  *  不思議なおくりものデータ順番入れ替え(通信アプリ用)
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  void SwapGiftData( u32 index1 , u32 index2 );

  //=============================================================================================
  /**
  *  不思議なおくりものデータ削除(通信アプリ用)
  * @param[in]   index        インデックス
  * @param[in]   bDoDelete    可能な場合、実際に削除する
  */
  //=============================================================================================
  bool DeleteGiftData( u32 index1, const bool bDoDelete = true );


  //=============================================================================================
  /**
  *  不思議なおくりものデータ数取得
  */
  //=============================================================================================
  u32 GetGiftDataNum( void ) const;


  //=============================================================================================
  /**
  *  不思議なおくりものデータ空き数取得
  */
  //=============================================================================================
  u32 GetGiftDataVacancies(void) const {return (MYSTERY_GIFT_DATA_NUM - GetGiftDataNum());}


  //=============================================================================================
  /**
  *  不思議なおくりものデータ空き数取得（削除可能な数も合算）
  */
  //=============================================================================================
  u32 GetGiftDataVacanciesWithDeletables(void);


  //=============================================================================================
  /**
  *  不思議なおくりものデータタイプ取得
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  MYSTERY_GIFT_TYPE GetGiftDataType( u32 index ) const;


  //=============================================================================================
  /**
  *  不思議なおくりものポケモンデータ取得
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  const MYSTERY_GIFT_POKE_DATA* GetPokeGiftData( u32 index ) const;

  //=============================================================================================
  /**
  *  不思議なおくりものアイテムデータ取得
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  const MYSTERY_GIFT_ITEM_DATA* GetItemGiftData( u32 index ) const;

  //=============================================================================================
  /**
  *  不思議なおくりものBP取得
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  u32 GetBPGiftData( u32 index ) const;

  //=============================================================================================
  /**
  *  不思議なおくりものまめ取得
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  u32 GetMameGiftData( u32 index ) const;

  //=============================================================================================
  /**
  *  不思議なおくりものデータを「配達員から受け取り済み」に設定
  * @param[in]   index   インデックス
  */
  //=============================================================================================
  void SetRecvFlag( u32 index );

  //=============================================================================================
  /**
  *  不思議なおくりものデータをゲーム内（配達員）で受け取り済みかどうかのフラグ取得
  * @param[in]   index   インデックス
  * @retval       true : 配達員から受け取り済み , false : まだ配達員から受け取ってない
  *
  *             falseが返ったら配達員からデータを受け取り、その後SetRecvFlagを呼んでください。
  *             存在しないindexを指定した場合はtrueが返ります。
  */
  //=============================================================================================
  bool GetRecvFlag( u32 index );


  u32 GetRandomNo( u32 index )
  {
    if( index >= RANDOM_DELIVERY_FLAG_NUM )
    {
      return 0;
    }

    return m_coreData.randomDeliveryFlag[index].randNo;
  }

  u32 GetRandomEventID( u32 index )
  {
    if( index >= RANDOM_DELIVERY_FLAG_NUM )
    {
      return 0;
    }

    return m_coreData.randomDeliveryFlag[index].eventID;
  }

/*
  おくりものアプリ向け
*/

  //=============================================================================================
  /**
  *  
  *  
  */
  //=============================================================================================

  /** 受信後、Save可能かチェック結果  */
  enum  ReceiveCheckResult
  {
    RECEIVE_CHECK_RESULT_OK,   /**<  Save可能  */
    
    RECEIVE_CHECK_RESULT_NG,  /**<  NG  受け取り済み*/
    RECEIVE_CHECK_RESULT_NG_TODAY,  /**<  NG   一日1回*/
    REDEIVE_CHECK_RESULT_NG_TODAY_FULL,/**<  NG   一日1回の履歴が一杯*/
    RECEIVE_CHECK_RESULT_NG_FULL,  /**<  NG カードアルバム一杯*/

    RECEIVE_CHECK_RESULT_
  };

  static ReceiveCheckResult   ReceiveCheck(const MYSTERY_GIFT_RECV_DATA* pTargetData); 
  static ReceiveCheckResult   ReceiveCheck(const MYSTERY_GIFT_DATA* pTargetData); 

  static const u16  EVENTID_MAX = 2047;
  static bool                 IsValidEventID(const u16 eventID) {return ((0 < eventID) && (eventID <= EVENTID_MAX));}


  CORE_DATA*  CreateClone(gfl2::heap::HeapBase* pHeapBase);    /**<  commitSave巻き戻し用Cloneの作成      */
  void        SetupClone(CORE_DATA& rCoreData);                /**<  commitSave巻き戻し用Cloneのセット    */
  void        WriteBackClone(const CORE_DATA* pCoreData);      /**<  commitSave巻き戻し用Cloneを書き戻す  */


  /*  PGL serial authentication  */
  /**  シリアル認証結果を渡し、内部状態を更新する  */
  bool  UpdateSerialAuthStatus(const bool isSerialOK, const gfl2::system::Date &rServerDate)
        {
          return  UpdateSerialAuthStatus(
                    isSerialOK,
                    static_cast<u16>(rServerDate.GetYear()),
                    static_cast< u8>(rServerDate.GetMonth()),
                    static_cast< u8>(rServerDate.GetDay()),
                    static_cast< u8>(rServerDate.GetHour()),
                    static_cast< u8>(rServerDate.GetMinute())
                  );
        }
  bool  UpdateSerialAuthStatus(const bool isSerialOK, const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute)
        {
          return  m_coreData.serialData.UpdateSerialAuthStatus(isSerialOK, serverTime_Year, serverTime_Month, serverTime_Day, serverTime_Hour, serverTime_Minute);
        }


  bool  QueryTimeoutPenalty(const gfl2::system::Date& rServerDate)
        {
          return  QueryTimeoutPenalty(
                    static_cast<u16>(rServerDate.GetYear()),
                    static_cast< u8>(rServerDate.GetMonth()),
                    static_cast< u8>(rServerDate.GetDay()),
                    static_cast< u8>(rServerDate.GetHour()),
                    static_cast< u8>(rServerDate.GetMinute())
                  );
        }
  bool  QueryTimeoutPenalty(const u16 serverTime_Year, const u8 serverTime_Month, const u8 serverTime_Day, const u8 serverTime_Hour, const u8 serverTime_Minute)
        {
          return  m_coreData.serialData.QueryTimeoutPenalty(serverTime_Year, serverTime_Month, serverTime_Day, serverTime_Hour, serverTime_Minute);
        }




  /*
    for debug
  */
#if PM_DEBUG
  void  Debug_SetSerialNGDate_Year(const u16 value)   {m_coreData.serialData.failedYear   = value;}
  void  Debug_SetSerialNGDate_Month(const u8 value)   {m_coreData.serialData.failedMonth  = value;}
  void  Debug_SetSerialNGDate_Day(const u8 value)     {m_coreData.serialData.failedDay    = value;}
  void  Debug_SetSerialNGDate_Hour(const u8 value)    {m_coreData.serialData.failedHour   = value;}
  void  Debug_SetSerialNGDate_Minute(const u8 value)  {m_coreData.serialData.failedMinute = value;}
  void  Debug_SetSerialNGCount(const u8 value)        {m_coreData.serialData.NGCounter    = value;}

  void  Debug_SetSerialNGDate(const gfl2::system::Date &rDate)
        {
          Debug_SetSerialNGDate_Year(rDate.GetYear());
          Debug_SetSerialNGDate_Month(rDate.GetMonth());
          Debug_SetSerialNGDate_Day(rDate.GetDay());
          Debug_SetSerialNGDate_Hour(rDate.GetHour());
          Debug_SetSerialNGDate_Minute(rDate.GetMinute());
        }
  const SERIAL_DATA*  Debug_GetSerialData(void) const {return &(m_coreData.serialData);}

  void  Debug_ClearSerialNGPenalty(void);

  /*  一度のみ受信可能フラグ関連  */
  void  Debug_SetRecieveOnceFlag(const u16 targetEventID /* < 2047 */, const bool isReceived);
  bool  Debug_IsRecieveOnceFlag(const u16 targetEventID /* < 2047 */) const;
  void  Debug_ClearRecieveOnceFlag(void)
        {
          memset(&(m_coreData.recvFlag), 0, sizeof(m_coreData.recvFlag));
        }
  
  /*  ランダム配信関連  */
  const RANDOM_DELIVERY_FLAG& Debug_GetRandomDeliveryFlag(const u32 randomDeliveryFlagIndex /* < 50 */);
  void  Debug_DeleteRandomDeliveryFlag(const u32 randomDeliveryFlagIndex);
  void  Debug_ClearAllRandomDeliveryFlag(void)
        {
          memset(&(m_coreData.randomDeliveryFlag), 0, sizeof(m_coreData.randomDeliveryFlag));
        }


  /*  一日一回フラグ関連  */
  const ONE_DAY_FLAG&   Debug_GetOneDayFlag(const u32 oneDayFlagIndex /* < 10 */) const;
  void                  Debug_ClearOneDayFlag(const u32 oneDayFlagIndex);
  void                  Debug_ClearAllOneDayFlag(void);
#endif







public:

  //Create PokemonParam from WonderGiftData
  static pml::pokepara::PokemonParam* CreatePokemonParamFromGiftData( const MYSTERY_GIFT_POKE_DATA* pPokeData , gfl2::heap::HeapBase * heap , s32 level );
  
public:
  //======================================================================================================================
  // for SaveData
  //======================================================================================================================

  //=============================================================================================
  /**
  * [for SaveData]  保存するデータバイナリへのポインタを返す
  *
  * @retval        void*
  */
  //=============================================================================================
  virtual void*  GetData( void );

  //=============================================================================================
  /**
  * [for SaveData]  保存するデータバイナリのサイズを返す
  *
  * @retval        void*
  */
  //=============================================================================================
  virtual size_t    GetDataSize( void );


  //=============================================================================================
  /**
  * [for SaveData]  セーブデータを初期化する
  *
  * @param heap
  */
  //=============================================================================================
  virtual void    Clear( gfl2::heap::HeapBase * heap );

private:
  CORE_DATA           m_coreData;           ///<セーブデータ本体(6800byte)

};  // class MysteryGiftSave

GFL_NAMESPACE_END(Savedata)




#endif //__MYSTATUS_H__
