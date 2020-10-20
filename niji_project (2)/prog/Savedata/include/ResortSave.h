#ifndef __RESORT_SAVE_H__
#define __RESORT_SAVE_H__
//=============================================================================================
/**
 * @file    ResortSave.h
 * @date    2015.08.28
 * @brief   ポケモンリゾートセーブデータ
 * @author  k.ohno
 */
//=============================================================================================
#pragma once
#include "pml/include/pmlib.h"
#include "Savedata/include/SaveDataInterface.h"
#include "debug/include/gfl2_Assert.h"
#include <math/include/gfl2_tinymt32.h>

#include "GameSys/include/NijiTime.h"



GFL_NAMESPACE_BEGIN(Savedata)

class ResortSave : public SaveDataInterface
{
public:
  ResortSave();
  virtual ~ResortSave();

//  static const int STOCKPOKEMON_MAX = 36;   //18*2

  static const int DOWNTIMER_BUFF = 3;   //時、分、秒で管理されている
  static const int DOWNTIMER_FULL_BUFF = 8;   //年、月、日、時、分、秒で管理されている
  
  static const int STOCKPOKEMON_MAX = 18; //ポケモンは各施設１８


//  static const int POKEBEANS_KIND_MAX = 15;  //ポケまめの種類
 // static const int RESORT_KIND_MAX = 5;  //リゾートの種類
  static const int RESORT_FRIENDSHIP_PLAYPOKE_MAX = 3;  //ふれあいリゾート：遊びに来るやせいぽけもん

  enum PokeResortKind{
    // ループで使っているので、値やMAXの変更の際は注意して下さい。
    RESORT_KIND_FRIENDSHIP,  //ふれあい
    RESORT_KIND_FARM,     //きのみ
    RESORT_KIND_ADVENTURE,   //たんけん
    RESORT_KIND_GIM,      //うんどう
    RESORT_KIND_HOTSPA,   //おんせん
    RESORT_KIND_MAX, //リゾートの種類
  };

  enum PokeResortGimKind{
    RESORT_GIM_KIND_LV,
    RESORT_GIM_KIND_HP,
    RESORT_GIM_KIND_AT,
    RESORT_GIM_KIND_DF,
    RESORT_GIM_KIND_SA,
    RESORT_GIM_KIND_SD,
    RESORT_GIM_KIND_SP,
  };

  enum PokeResortGimState{
    RESORT_GIM_STATE_INIT,
    RESORT_GIM_STATE_TRAINING,
    RESORT_GIM_STATE_END,
  };

  // たんけんリゾートのコースID
  enum PokeResortAdvCourse {
    RESORT_ADV_COURSE_KAKERA,   //!< 欠片洞窟(あやしいかけら)
    RESORT_ADV_COURSE_ISHI,     //!< 進化石洞窟(かがやくいし)
    RESORT_ADV_COURSE_DOUGU1,   //!< いろいろ１(きになるおたから１)
    RESORT_ADV_COURSE_OTAKARA,  //!< お宝洞窟(きちょうなおたから)
    RESORT_ADV_COURSE_DOUGU2,   //!< いろいろ２(きになるおたから１)
    RESORT_ADV_COURSE_NONE,     //!< 未選択
    RESORT_ADV_COURSE_MAX,
  };

/*
  enum{
    POKEMONEVENT_NONE,
    POKEMONEVENT_MEMORY,
    POKEMONEVENT_HAPPY,
    POKEMONEVENT_GO_OSEWA1,
    POKEMONEVENT_GO_OSEWA2,
    POKEMONEVENT_GO_PARULE,
    POKEMONEVENT_SKINSHIP,
    POKEMONEVENT_POKEMAME,
    POKEMONEVENT_HOTSPA,
    POKEMONEVENT_GIM,
    POKEMONEVENT_YASEI,
    POKEMONEVENT_DEAI,
    POKEMONEVENT_LEAGION,

    

  };
*/

  enum ResortTutorialState{
    RESORT_TUTORIAL_STATE_OPENING = 0x01,//起動時
    RESORT_TUTORIAL_STATE_TOUCH_BEANS = 0x02, //ポケマメタッチ時
    RESORT_TUTORIAL_STATE_TOUCH_TREE = 0x04,  //ポケマメの木をタッチ時
    RESORT_TUTORIAL_STATE_TOUCH_BOX = 0x08,//ポケマメいれをタッチ時
    RESORT_TUTORIAL_STATE_MENU = 0x10,//管理人画面起動時
    RESORT_TUTORIAL_STATE_DEVELOP = 0x20, // 管理人開発ボタン
    RESORT_TUTORIAL_STATE_CHANGE = 0x40, // ぽけまめ交換
    RESORT_TUTORIAL_STATE_BOTTLE = 0x80, // まめびんながす


  };



  
  enum{  //ボーナス段階
    RESORT_BONUS0,   //  ０～２９					ボーナス０				
    RESORT_BONUS1, //３０～８９					ボーナス１				
    RESORT_BONUS2, //９０～１４９					ボーナス２				
    RESORT_BONUS3, ///１５０以上					ボーナス３				
  };



  
  enum{ //ポケマメいれの数
    RESORT_FRIENDSHIP_MAX_LV1 = 10,
    RESORT_FRIENDSHIP_MAX_LV2 = 30,
    RESORT_FRIENDSHIP_MAX_LV3 = 50,
  };


  enum{
    BOTTLE_STATE_RESORTOK = 0x01,   // リゾートから流した
    BOTTLE_STATE_FESCIRCLE  = 0x02, // フェスサークルからもどってきた
    BOTTLE_STATE_PUTOK = 0x01,   // リゾートにばらまいてＯＫ
    BOTTLE_STATE_GET  = 0x02,   // リゾートでとった
  };

  enum PokeBeansEnum{
    POKEBEANS_NORMAL_01,
    POKEBEANS_NORMAL_02,
    POKEBEANS_NORMAL_03,
    POKEBEANS_NORMAL_04,
    POKEBEANS_NORMAL_05,
    POKEBEANS_NORMAL_06,
    POKEBEANS_NORMAL_07,
    POKEBEANS_HEART_01,
    POKEBEANS_HEART_02,
    POKEBEANS_HEART_03,
    POKEBEANS_HEART_04,
    POKEBEANS_HEART_05,
    POKEBEANS_HEART_06,
    POKEBEANS_HEART_07,
    POKEBEANS_NIJI,
    POKEBEANS_NONE,
    POKEBEANS_KIND_MAX=POKEBEANS_NONE,

    POKEBEANS_NORMAL_RED = 0,   // nijiまめの正式な並び
    POKEBEANS_NORMAL_BLUE,
    POKEBEANS_NORMAL_LIGHTBLUE,
    POKEBEANS_NORMAL_GREEN,
    POKEBEANS_NORMAL_YELLOW,
    POKEBEANS_NORMAL_PURPLE,
    POKEBEANS_NORMAL_ORANGE,
    POKEBEANS_PATTERN_RED,
    POKEBEANS_PATTERN_BLUE,
    POKEBEANS_PATTERN_LIGHTBLUE,
    POKEBEANS_PATTERN_GREEN,
    POKEBEANS_PATTERN_YELLOW,
    POKEBEANS_PATTERN_PURPLE,
    POKEBEANS_PATTERN_ORANGE,

  };

  static const int FRIENDSHIP_GROUND_BEANS_MAX = 20;//地面においてあるまめ

  static const int ADVENTURE_ITEM_MAX = 20;  //探検のアイテム最大
  
  static const int RESORT_FARM_MAX = 18;  //畑は18箇所

  static const int RESORT_GIM_KIND_MAX = 3; //３種類
  static const int RESORT_GIM_KIND_NUM_MAX = 6; //一種類につきポケモン６

  static const int RESORT_HOTSPA_KIND_MAX = 3; //３種類

  static const int POKEBEANS_KIND_NUM_MAX = 255; //1種類の所持数max

  //! ポケマメランク
  enum PokeBeansRank
  {
    POKEBEANS_RANK_NONE = 0,
    POKEBEANS_RANK_NORMAL,
    POKEBEANS_RANK_RARE,
    POKEBEANS_RANK_S_RARE,
  };


public:
  //------------------------------------------------------------------------------------------
  /**
   * @brief	ポケモンデータ
   */
  //------------------------------------------------------------------------------------------
  struct ResortPokemonData {
    u8 pokemonCoreParam[ pml::pokepara::CoreParam::DATASIZE ];	//!< ポケモンデータ
    u8 tray;
    u8 pos;
    u8 bnew;   //新しくsetされたら１
    u8 dmy;
  };

private:
  struct ResortSaveData {
    gfl2::math::Random::State random;  ///< 乱数の内部状態 保存する値
    PokeResortGimKind mGimKind[RESORT_GIM_KIND_MAX]; // うんどう装置の状態
    PokeResortGimState mGimState[RESORT_GIM_KIND_MAX];   //運動の状態
    ResortPokemonData	mFriendShipPokeStock[STOCKPOKEMON_MAX];		//!< ポケモンデータ
    ResortPokemonData wildPoke[RESORT_FRIENDSHIP_PLAYPOKE_MAX];//今いる分３件
    ResortPokemonData	mFarmPokeStock[STOCKPOKEMON_MAX];		//!< ポケモンデータ
    ResortPokemonData	mAdventurePokeStock[STOCKPOKEMON_MAX];		//!< ポケモンデータ
    ResortPokemonData	mGimPokeStock[STOCKPOKEMON_MAX];		//!< ポケモンデータ
    ResortPokemonData	mHotSpaPokeStock[STOCKPOKEMON_MAX];		//!< ポケモンデータ
    u16 updateFlg;//ポケモンデータの書き換えが行われた場合true
    u16 dmy2[DOWNTIMER_BUFF-1];   // !使用禁止
    u16 mFarmItemKind[RESORT_FARM_MAX];  //植えてる木の実番号
    u16 mItemBuff[ADVENTURE_ITEM_MAX];  //得たアイテム

    u32 rtcoffset1;  //rtcoffset1
    u32 rtcoffset2;  //rtcoffset2
    gfl2::str::STRCODE dummy[ pml::MONS_NAME_LENGTH + pml::EOM_LENGTH - 4  ];  ///< 会話イベント用ポケモンニックネーム  !使用禁止

    u32 mEventProgress;    //現在のイベントの段階  ResortTutorialStateでのビット管理

    u8 mLastAccessTime[DOWNTIMER_FULL_BUFF];   //前回のアプリの終了時間   ----基準時間の算出 yyyy/mm/dd/hh/mm/ss

    u8 mPokeBeansNum[POKEBEANS_KIND_MAX];   ///ポケ豆種類数

    u8 mResortLevel[RESORT_KIND_MAX];  //リゾートの種類のレベル

    u8 mBeansBoxNum[RESORT_KIND_MAX];   //今入ってる豆の数

    u8 mBeansBottleState[RESORT_KIND_MAX*2];  //豆びんは各リゾートに２つけい１０本  仕様変更　各島に１つ

    u8 mbUseBeansBottle;  //マメビンがつかえるかどうかの判定用 

    u8 mBounusLevel;   //ボーナスのレベル  ＢＯＸのポケモン数によって決まっている

    //ポケモンのイベント
    u8 mPokemonEventTime[DOWNTIMER_BUFF];   // ポケモンのイベントを再度抽選するタイマー
    u8 mBeansBottleTime[DOWNTIMER_BUFF];   // まめ瓶のイベントタイマー

    //---ふれあい

    u8 mBeansGetCount; //ポケマメ入手カウント	1	"ポケマメを入手できるカウンタ0～255"
    u8 dmy_mAppearCount;    // 野生ポケモンの出現カウント	1	"野生ポケモンの出現の抽選権MAX0～4"   使用しなくなっています 
    u8 mWildPokemonTime[DOWNTIMER_BUFF];   // 野生ポケモン抽選用カウントをためる時間
    u8 mWildPokemonServiceTime[DOWNTIMER_BUFF];    //野生ポケモンの抽選確率が２倍になるカウンター

    
    u8 mPlayPokemonKind[RESORT_FRIENDSHIP_PLAYPOKE_MAX];   // はずれ、今きている、居ついている、かえった1byte"


    ///-------------------------------------------

    u8 mFriendShipTime[DOWNTIMER_BUFF];   // 次に豆やポケモンの抽選を行うまでの時間  ふれあいブースとタイマー
    u8 GroundBeans[FRIENDSHIP_GROUND_BEANS_MAX];  //地面においてあるまめの種類  まめは散らばらせる時に抽選を行い、内容を保存する

    //木のみリゾート
    u8 mFarmState[ RESORT_FARM_MAX ]; // 木の実が出来た個数
    u8 mFarmTime[ RESORT_FARM_MAX ][ DOWNTIMER_BUFF ];  //収穫の時間

    u8 mFarmFertilizerTime[DOWNTIMER_BUFF];   //こやしの効果持続時間

    //  たんけんリゾート

    u8 mAdventureCount[DOWNTIMER_BUFF];   //  アイテム抽選権のタイマーカウント

    u8 mAdventureCoursePattern;  //  選択中のコース	1	"以下の５つのパターンがある
    //・お宝洞窟のみ
    //・進化石洞窟のみ
    //・欠片洞窟のみ
    //・お宝洞窟＆進化石洞窟
    //・３つすべて"
    u8 mAdventureBonusTime[DOWNTIMER_BUFF];   //トロッコの効果持続時間	3	"トロッコ加速の効果の持続時間  ＭＡＸ９９ｈ９９ｍ９９ｓ　：３byte"

    //  うんどうリゾート
    u8 mGimBonusTime[DOWNTIMER_BUFF];  //  うんどう装置の効果持続時間	4	"うんどう装置パワーアップの効果の持続時間
    u8 mTrainingCount[RESORT_GIM_KIND_MAX];   //繰り返し行う運動の回数
    u8 mTrainingCountMax[RESORT_GIM_KIND_MAX];   //繰り返し行う運動の回数 与えたMAX値
    u8 mGimEndTime[RESORT_GIM_KIND_MAX][DOWNTIMER_BUFF]; //  うんどうの残り時間	12	"うんどうの終了時間（ＭＡＸ９９ｈ９９ｍ９９ｓ）：４byte

    //    おんせんリゾート
    u8 mHotSpaBonusTime[DOWNTIMER_BUFF];  // 入浴剤の効果持続時間	3	"入浴剤の効果の持続時間
    u8 mHotSpaPokeTime[RESORT_HOTSPA_KIND_MAX][DOWNTIMER_BUFF];  // タイマーはトレイぶん３つ


    u8 mGimAddNum[STOCKPOKEMON_MAX];  ///GIMで運動直後の値を保存 成長したら差分を取得して会話を変更する
		u8 mHotSpaNoboseTime[RESORT_HOTSPA_KIND_MAX][DOWNTIMER_BUFF];  // このタイマーをみて０ならばのぼせ状態とする
		u8 mSelectResortPos;   ///<< 最後に起動していたリゾートの場所(PokeResortKind)

    //豆の木たたき
    u8 beansIvyHiddenTime[DOWNTIMER_BUFF];  // 豆の木を叩いた回数リセットタイマー
    u8 beansIvyHiddenCount;  ///<< 豆の木を叩いた回数
  };


  ResortSaveData mData;  //リゾートセーブの全部

  u64 _rtcoffset;  //RTCオフセット

public:

  //--------------------------------------------------------------
  /**
   * @brief   乱数を得る
   * @param   
   */
  //--------------------------------------------------------------
  gfl2::math::Random::State GetRandomState(void){ return mData.random; }

  //--------------------------------------------------------------
  /**
   * @brief   乱数を保存する
   * @param   
   */
  //--------------------------------------------------------------
  void SetRandomState(gfl2::math::Random::State rstate){ mData.random = rstate; }

  //--------------------------------------------------------------
  /**
   * @brief 種類に応じた持っている数を得る
   * @param PokeBeansEnum
   */
  //--------------------------------------------------------------
  u8 GetPokeBeansNum(PokeBeansEnum kind);
  //--------------------------------------------------------------
  /**
   * @brief 種類に応じた持っている数を増減させる
   * @param PokeBeansEnum
   * @param add ＋ならふやし－ならへらす
   */
  //--------------------------------------------------------------
  void SetPokeBeansNum(PokeBeansEnum kind,int add);

  //--------------------------------------------------------------
  /**
   * @brief 種類に応じた持っているまめ数をへらす
   * @param normal  ノーマルの数
   * @param rare  レアの数
   * @param niji  にじまめのかず
   */
  //--------------------------------------------------------------
  void SubPokeBeansNum(int normal, int rare, int niji );

  //--------------------------------------------------------------
  /**
   * @brief リゾートのレベルを得る
   * @param PokeResortKind
   * @return レベル 0-3  開いていない時は０  
   */
  //--------------------------------------------------------------
  inline u8 GetResortLevel(PokeResortKind kind){  return mData.mResortLevel[kind];  };
  //--------------------------------------------------------------
  /**
   * @brief リゾートのレベルを設定する
   * @param PokeBeansEnum
   */
  //--------------------------------------------------------------
  void SetResortLevel(PokeResortKind kind,u8 level);

  //--------------------------------------------------------------
  /**
   * @brief リゾートの進行具合を得る チュートリアル終わったかとか
   * @return 終わっていたらtrue 終わってないとfalse
   */
  //--------------------------------------------------------------
   bool GetResortProgress(ResortTutorialState kind);
  //--------------------------------------------------------------
  /**
   * @brief リゾートの進行具合を設定する
   * @param progress  進行度
   */
  //--------------------------------------------------------------
  void SetResortProgress(ResortTutorialState progress);

#if PM_DEBUG
  void DebugSetResortProgress(u32 data);
#endif
  
  //--------------------------------------------------------------
  /**
   * @brief   現在の時間との差分を得る
   * @return  秒単位の時間の差  マイナスは返さない
   */
  //--------------------------------------------------------------
  u64 GetDiffTime(u8* time8);
  u64 GetDiffTimeDate(GameSys::DeviceDate* pdate);

  //--------------------------------------------------------------
  /**
   * @brief   終了時に時間を設定する
   * @param   時間
   */
  //--------------------------------------------------------------
  void SetPlayTime(GameSys::DeviceDate* pdate);



  //--------------------------------------------------------------
  /**
   * @brief   リゾートからサークルに豆ビンが使えるかどうかを得る
   * @return  使える場合TRUE
   */
  //--------------------------------------------------------------
  bool GetUseBeansBottle(void);
  //--------------------------------------------------------------
  /**
   * @brief   リゾートからサークルに豆ビンが使えるかどうか設定
   * @param  使える場合TRUE 使えなくなったらFALSE
   */
  //--------------------------------------------------------------
  void SetUseBeansBottle(bool onoff);
  //--------------------------------------------------------------
  /**
   * @brief   サークルからリゾートに帰ってきたかどうかを得る
   * @param   使える場合TRUE
   */
  //--------------------------------------------------------------
  void SetReturnBeansBottle(bool onoff);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんをリゾートにおいていいかどうか得る
   * @param    各リゾート
   * @return   true ならばおいて良い
   */
  //--------------------------------------------------------------
  bool IsBeansBottleGround(PokeResortKind type);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんをリゾートにおいていいかどうか設定する
   * @param    各リゾート
   * @param    onoff trueならば置いてよい
    */
  //--------------------------------------------------------------
  void SetBeansBottleGround(PokeResortKind type, bool onoff);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんをもう入手したかどうかを得る
   * @return   とられたらtrue とる前はfalse
   */
  //--------------------------------------------------------------
  bool IsBeansBottleGet(PokeResortKind type);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんをもう入手したかどうかを設定する
   * @param    各リゾート
   * @param    onoff trueならば置いてよい
   */
  //--------------------------------------------------------------
  void SetBeansBottleGet(PokeResortKind type, bool onoff);

  //--------------------------------------------------------------
  /**
   * @brief    まめびんの時間を得る
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetBeansBottleServiceTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんの時間設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetBeansBottleServiceTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    まめびんの時間ダウンカウント
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool BeansBottleServiceTimeDownCount(void);

  
  //--------------------------------------------------------------
  /**
   * @brief   ボーナスのレベル
   * @return  ボーナスタイムの場合TRUE
   */
  //--------------------------------------------------------------
  u8 GetBonusLevel(void);
  //--------------------------------------------------------------
  /**
   * @brief   ボーナスのレベル
   * @param   ボーナスタイムの場合TRUE
   */
  //--------------------------------------------------------------
  void SetBonusLevel(u8 level);

  //--------------------------------------------------------------
  /**
   * @brief   ポケマメ入手カウント
   * @return  0-255
   */
  //--------------------------------------------------------------
  inline u8 GetBeansGetCount(void){ return mData.mBeansGetCount; };
  //--------------------------------------------------------------
  /**
   * @brief   ポケマメ入手カウント
   * @param   0-255
   */
  //--------------------------------------------------------------
  inline void SetBeansGetCount(u8 count){ mData.mBeansGetCount = count; };



  //--------------------------------------------------------------
  /**
   * @brief 野生のPokemonを得る３体
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void GetWildPokemon(int index, pml::pokepara::CoreParam * cp);

  //--------------------------------------------------------------
  /**
   * @brief 野生のPokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetWildPokemon(int index, const pml::pokepara::CoreParam * cp);



  //--------------------------------------------------------------
  /**
   * @brief    いつき率の保存
   * @param    0-99
   * @return   いつき率
   */
  //--------------------------------------------------------------
  inline u8 GetPlayPokemonKind(int index){ return mData.mPlayPokemonKind[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   いつき率の保存
   * @param   index   MAX0～3
   * @param   いつき率
   */
  //--------------------------------------------------------------
  inline void SetPlayPokemonKind(int index,u8 data){ mData.mPlayPokemonKind[index] = data; };


  //--------------------------------------------------------------
  /**
   * @brief    今入ってる豆の数
   * @return   今入ってる豆の数
   */
  //--------------------------------------------------------------
  inline u8 GetBeansBoxNum(PokeResortKind kind){ return mData.mBeansBoxNum[kind]; };
  //--------------------------------------------------------------
  /**
   * @brief   今入ってる豆の数
   * @param   data   豆の数
   */
  //--------------------------------------------------------------
  inline void SetBeansBoxNum(PokeResortKind kind,u8 data){ mData.mBeansBoxNum[kind] = data; };

  //--------------------------------------------------------------
  /**
   * @brief    ポケモンイベント抽選の時間
   * @param    時間、分、秒
   @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool PokemonEventTimeDownCount(void);
  //--------------------------------------------------------------
  /**
   * @brief    ポケモンイベント抽選の時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetPokemonEventTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    ポケモンイベント抽選の時間を得る
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetPokemonEventTime(u8* hour,u8* min,u8* sec);


  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモン抽選確率２倍の時間
   * @param    時間、分、秒
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool WildPokemonServiceTimeDownCount(void);
  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモン抽選確率２倍の時間設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetWildPokemonServiceTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモン抽選確率２倍の時間を得る
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetWildPokemonServiceTime(u8* hour,u8* min,u8* sec);


  
  //--------------------------------------------------------------
  /**
   * @brief    植えてる木の実番号
   * @param    index RESORT_FARM_MAX
   * @return   アイテム番号
   */
  //--------------------------------------------------------------
  inline u16 GetFarmItemKind(int index){ return mData.mFarmItemKind[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   植えてる木の実番号
   * @param   index RESORT_FARM_MAX
   * @param   アイテム番号
   */
  //--------------------------------------------------------------
  inline void SetFarmItemKind(int index,u16 data){ mData.mFarmItemKind[index] = data; };


  //--------------------------------------------------------------
  /**
   * @brief    植えてる木の実番号
   * @param    index RESORT_FARM_MAX
   * @return   こすう
   */
  //--------------------------------------------------------------
  inline u8 GetFarmNum(int index){ return mData.mFarmState[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   植えてる木の実個数
   * @param   index RESORT_FARM_MAX
   * @param   data   こすう
   */
  //--------------------------------------------------------------
  inline void SetFarmNum(int index,u8 data){ mData.mFarmState[index] = data; };
  //--------------------------------------------------------------
  /**
   * @brief    収穫の時間
   * @param    index RESORT_FARM_MAX
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetFarmTime(int index,u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   収穫の時間
   * @param   index RESORT_FARM_MAX
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetFarmTime(int index,u8 hour,u8 min,u8 sec);

  //--------------------------------------------------------------
  /**
   * @brief   収穫の時間を減らしていく
   * @param   index RESORT_FARM_MAX
   * @param    時間、分、秒
   * @param   時間になったらtrue
   */
  //--------------------------------------------------------------
  bool FarmTimeDownCount(int index);



  

  //--------------------------------------------------------------
  /**
   * @brief    こやしの効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetFarmFertilizerTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   こやしの効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetFarmFertilizerTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    こやしの効果持続時間ダウンカウント
   * @return   ボーナス時間ならtrue
   */
  //--------------------------------------------------------------
  bool IsFarmFertilizerTime(void);


  //--------------------------------------------------------------
  /**
   * @brief    こやしの効果持続時間ダウンカウント
   * @return   ボーナス時間ならtrue
   */
  //--------------------------------------------------------------
  bool FarmFertilizerTimeDownCount(void);


  //--------------------------------------------------------------
  /**
   * @brief    探検時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetAdventureTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   探検時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetAdventureTime(u8 hour,u8 min,u8 sec);

  //--------------------------------------------------------------
  /**
   * @brief    探検時間ダウンカウント
   * @return   探検終了でTRUE
   */
  //--------------------------------------------------------------
  bool AdventureTimeDownCount(void);


  //--------------------------------------------------------------
  /**
   * @brief    選択中のコース	1	"以下の５つのパターンがある
   * @return    お宝洞窟のみ   進化石洞窟のみ    欠片洞窟のみ    お宝洞窟＆進化石洞窟    ３つすべて"
   */
  //--------------------------------------------------------------
  inline u8 GetAdventureCoursePattern(void){ return mData.mAdventureCoursePattern; };
  //--------------------------------------------------------------
  /**
   * @brief    選択中のコース	1	"以下の５つのパターンがある
   * @param    お宝洞窟のみ   進化石洞窟のみ    欠片洞窟のみ    お宝洞窟＆進化石洞窟    ３つすべて"
   */
  //--------------------------------------------------------------
  inline void SetAdventureCoursePattern(u8 course){  mData.mAdventureCoursePattern = course; };

  //--------------------------------------------------------------
  /**
   * @brief    トロッコの効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetAdventureBonusTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   トロッコの効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetAdventureBonusTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    トロッコの効果持続時間ダウンカウント
   * @return   動いていたらtrue
   */
  //--------------------------------------------------------------
  bool IsAdventureBonusTime(void);
  
  

  
  //--------------------------------------------------------------
  /**
   * @brief    トロッコの効果持続時間ダウンカウント
   * @return   ボーナス時間ならtrue
   */
  //--------------------------------------------------------------
  bool AdventureBonusTimeDownCount(void);


  //--------------------------------------------------------------
  /**
   * @brief    探検で得たアイテム
   * @return   アイテム番号  
   */
  //--------------------------------------------------------------
  u16 GetAdventureItem(int index);
  //--------------------------------------------------------------
  /**
   * @brief   探検で得たアイテム セット
   * @param    格納番号
   * @param    アイテム番号
   */
  //--------------------------------------------------------------
  void SetAdventureItem(int index,u16 item);
  //--------------------------------------------------------------
  /**
   * @brief    探検で得たアイテム 追加  入らないものは入らない
   * @param    アイテム番号
   */
  //--------------------------------------------------------------
  void AddAdventureItem(u16 item, int max);

  //--------------------------------------------------------------
  /**
   * @brief    探検で得たアイテムが何個入っているか得る
   * @return   数
   */
  //--------------------------------------------------------------
  int GetTreasureItemNum(void);
  

  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置の効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetGimBonusTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置の効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetGimBonusTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    GIMのボーナス時間ダウンカウント
   * @return   動いていたらtrue
   */
  //--------------------------------------------------------------
  bool IsGimBonusTime(void);

  //--------------------------------------------------------------
  /**
   * @brief    GIMのボーナス時間ダウンカウント
   * @return   ボーナス時間ならtrue
   */
  //--------------------------------------------------------------
  bool GimBonusTimeDownCount(void);

  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置の種類
   * @param    index ３つのそうち
   * @return   運動種類
   */
  //--------------------------------------------------------------
  inline PokeResortGimKind GetGimKind(int index){ return mData.mGimKind[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置の選んだもの
   * @param   index ３つのそうち
   * @param   運動種類 PokeResortGimKind
   */
  //--------------------------------------------------------------
  inline void SetGimKind(int index,PokeResortGimKind data){ mData.mGimKind[index] = data; };

  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置の状態
   * @param    index ３つのそうち
   * @return   運動状態
   */
  //--------------------------------------------------------------
  inline PokeResortGimState GetGimState(int index){ return mData.mGimState[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置の選んだもの
   * @param   index ３つのそうち
   * @param   運動状態
   */
  //--------------------------------------------------------------
  inline void SetGimState(int index,PokeResortGimState data){ mData.mGimState[index] = data; };
  
  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置の状態
   * @param    index ３つのそうち
   * @return   運動状態
   */
  //--------------------------------------------------------------
  inline u8 GetGimTrainingCount(int index){ return mData.mTrainingCount[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置の選んだもの
   * @param   index ３つのそうち
   * @param   運動状態
   */
  //--------------------------------------------------------------
  inline void SetGimTrainingCount(int index,u8 num){ mData.mTrainingCount[index] = num; };


  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置の状態 MAX値の取得
   * @param    index ３つのそうち
   * @return   運動状態
   */
  //--------------------------------------------------------------
  inline u8 GetGimTrainingCountMax(int index){ return mData.mTrainingCountMax[index]; };
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置の選んだもの 与えたMAX値を入れる
   * @param   index ３つのそうち
   * @param   運動状態
   */
  //--------------------------------------------------------------
  inline void SetGimTrainingCountMax(int index,u8 num){ mData.mTrainingCountMax[index] = num; };

  //--------------------------------------------------------------
  /**
   * @brief    うんどう装置ののこり時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetGimEndTime(int kind,u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   うんどう装置ののこり時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetGimEndTime(int kind,u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    GIMののこり時間ダウンカウント
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool GimEndTimeDownCount(int kind);

  //--------------------------------------------------------------
  /**
   * @brief    入浴剤の効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetHotSpaBonusTime(u8* hour,u8* min,u8* sec);
  //--------------------------------------------------------------
  /**
   * @brief   入浴剤の効果持続時間
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetHotSpaBonusTime(u8 hour,u8 min,u8 sec);
  //--------------------------------------------------------------
  /**
   * @brief    入浴剤の効果持続ダウンカウント
   * @return   ボーナス時間ならtrue
   */
  //--------------------------------------------------------------
  bool HotSpaBonusTimeDownCount(void);

  //--------------------------------------------------------------
  /**
   * @brief    入浴剤の効果持続
   * @return   動いていたらtrue
   */
  //--------------------------------------------------------------
  bool IsHotSpaBonusTime(void);

  //--------------------------------------------------------------
  /**
   * @brief    ふれあいリゾートの時間
   * @param    時間、分、秒
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool FriendShipTimeDownCount(void);

  //--------------------------------------------------------------
  /**
   * @brief    ふれあいリゾートの時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetFriendShipTime(u8 hour,u8 min,u8 sec);

  //--------------------------------------------------------------
  /**
   * @brief    ふれあいリゾートの時間を得る
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void GetFriendShipTime(u8* hour,u8* min,u8* sec);

  //--------------------------------------------------------------
  /**
   * @brief    散らばっている豆を得る
   * @param    index  順番
   * @return   まめのしゅるい
   */
  //--------------------------------------------------------------
  inline u8 GetGroundBeans(int no){ return mData.GroundBeans[no];  }

  //--------------------------------------------------------------
  /**
   * @brief    散らばっている豆を設定する
   * @param    index  順番
   * @param    まめのしゅるい
   */
  //--------------------------------------------------------------
  void SetGroundBeans(int no,u8 beans);

  //--------------------------------------------------------------
  /**
   * @brief   散らばっている豆のランクを取得
   * @param   no  順番
   * @return  ランク
   */
  //--------------------------------------------------------------
  PokeBeansRank GetGroundBeansRank( int no );

  //--------------------------------------------------------------
  /**
   * @brief   豆のランクを取得
   * @param   beans   種類
   * @return  ランク
   */
  //--------------------------------------------------------------
  PokeBeansRank GetBeansRank( PokeBeansEnum beans );

  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモンカウントの時間
   * @param    時間、分、秒
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool WildPokemonTimeDownCount(void);
  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモン抽選確率２倍の時間
   * @return   時間が０ならfalse 動いてたらtrue
   */
  //--------------------------------------------------------------
  bool IsWildPokemonServiceTime(void);

  //--------------------------------------------------------------
  /**
   * @brief    野生ポケモンカウントの時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetWildPokemonTime(u8 hour,u8 min,u8 sec);
#if PM_DEBUG
  void GetWildPokemonTime( u8 * hour, u8 * min, u8 * sec );
#endif




  //--------------------------------------------------------------
  /**
   * @brief    温泉ポケモンカウントの時間
   * @param    index 
   * @return   時間が０ならtrue
   */
//--------------------------------------------------------------
  bool HotSpaPokemonTimeDownCount(int index);

  //--------------------------------------------------------------
  /**
   * @brief    温泉ポケモンカウントの時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetHotSpaPokemonTime(int index, u8 hour,u8 min,u8 sec);
#if PM_DEBUG
  void GetHotSpaPokemonTime( int index, u8 * hour, u8 * min, u8 * sec );
#endif


  //--------------------------------------------------------------
  /**
   * @brief    温泉のぼせカウントの時間
   * @param    index
   * @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool HotSpaNoboseTimeDownCount(int index);

  //--------------------------------------------------------------
  /**
   * @brief    温泉のぼせカウントの時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetSpaNoboseTime(int index, u8 hour,u8 min,u8 sec);
#if PM_DEBUG
  void GetSpaNoboseTime( int index, u8 * hour, u8 * min, u8 * sec );
#endif


  //--------------------------------------------------------------
  /**
   * @brief 温泉のPokemonを得る
   * @param index  0-STOCKPOKEMON_MAXの範囲
   * @param cp     PokemonCoreParam
 */
  //--------------------------------------------------------------
  void GetHotSpaPokemon(int index, pml::pokepara::CoreParam * cp);

  
  //--------------------------------------------------------------
  /**
   * @brief 温泉のポケモンが更新されたかを得る
   */
  //--------------------------------------------------------------
  bool IsNewHotSpaPokemon(int index);
  void ResetNewHotSpaPokemon(int index);

  
  //--------------------------------------------------------------
  /**
   * @brief Pokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetHotSpaPokemon(int index, const pml::pokepara::CoreParam * cp, bool bNew=true);


  //--------------------------------------------------------------
  /**
   * @brief GimのPokemonを得る
   * @param index  0-STOCKPOKEMON_MAXの範囲
   * @param cp     PokemonCoreParam
   */
  //--------------------------------------------------------------
  void GetGimPokemon(int index, pml::pokepara::CoreParam * cp);

  //--------------------------------------------------------------
  /**
   * @brief GimのPokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetGimPokemon(int index, const pml::pokepara::CoreParam * cp);



  //--------------------------------------------------------------
  /**
   * @brief FriendShipのPokemonを得る
   * @param PokemonCoreParam
 */
  //--------------------------------------------------------------
  void GetFriendShipPokemon(int index, pml::pokepara::CoreParam * cp);
  //--------------------------------------------------------------
  /**
   * @brief FriendShipのPokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetFriendShipPokemon(int index, const pml::pokepara::CoreParam * cp, int tray=-1, int pos=-1);
  //--------------------------------------------------------------
  /**
   * @brief FriendShipのPokemonのボックストレイ位置を帰す
   * @return 上位８トレイ 下位８場所
   */
  //--------------------------------------------------------------
  u16 GetFriendShipPokemonBoxPos(int index);
  //--------------------------------------------------------------
  /**
   * @brief AdventureのPokemonを得る
   * @param PokemonCoreParam
 */
  //--------------------------------------------------------------
  void GetAdventurePokemon(int index, pml::pokepara::CoreParam * cp);
  //--------------------------------------------------------------
  /**
   * @brief AdventureのPokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetAdventurePokemon(int index, const pml::pokepara::CoreParam * cp, int tray=-1, int pos=-1);
  //--------------------------------------------------------------
  /**
   * @brief AdventureのPokemonのボックストレイ位置を帰す
   * @return 上位８トレイ 下位８場所
   */
  //--------------------------------------------------------------
  u16 GetAdventurePokemonBoxPos(int index);
  //--------------------------------------------------------------
  /**
   * @brief FarmのPokemonを得る
   * @param PokemonCoreParam
 */
  //--------------------------------------------------------------
  void GetFarmPokemon(int index, pml::pokepara::CoreParam * cp);
  //--------------------------------------------------------------
  /**
   * @brief FarmのPokemonをセットする
   * @param PokemonCoreParam
   */
  //--------------------------------------------------------------
  void SetFarmPokemon(int index, const pml::pokepara::CoreParam * cp, int tray=-1, int pos=-1);
  //--------------------------------------------------------------
  /**
   * @brief FarmのPokemonのボックストレイ位置を帰す
   * @return 上位８トレイ 下位８場所
   */
  //--------------------------------------------------------------
  u16 GetFarmPokemonBoxPos(int index);
  

  //--------------------------------------------------------------
  /**
   * @brief あずけているポケモンの数を数える たまごこみ
   * @param あずけている数
   */
  //--------------------------------------------------------------
  int PokemonCountHotSpaAndGim(pml::pokepara::CoreParam * cp);


  //--------------------------------------------------------------
  /**
   * @brief 更新があったかどうかを確認する
   * @return 更新 =true
   */
  //--------------------------------------------------------------
  bool CheckUpdateFlg(void);

  //--------------------------------------------------------------
  /**
   * @brief 更新状態にする
   */
  //--------------------------------------------------------------
  void SetUpdateFlg(void);

  //--------------------------------------------------------------
  /**
   * @brief 温泉リゾートのPokemonをPokeParty形式でセットする
   * @param groupno  グループ番号0,1,2
   * @param PokeParty
   * @param heap 一時使用heap
   */
  //--------------------------------------------------------------
  void SetPokePartyToHotSpaPokemon( u32 groupno, const pml::PokeParty * pParty,gfl2::heap::HeapBase* heap);
  //--------------------------------------------------------------
  /**
   * @brief 運動リゾートのPokemonをPokeParty形式でセットする
   * @param groupno  グループ番号0,1,2
   * @param PokeParty
   * @param heap 一時使用heap
   */
  //--------------------------------------------------------------
  void SetPokePartyToGimPokemon( u32 groupno, const pml::PokeParty * pParty,gfl2::heap::HeapBase* heap);

  
  //--------------------------------------------------------------
  /**
   * @brief 運動リゾート開始時の値をセット
   * @param index   インデックス
   * @param val     セットする値
   */
  //--------------------------------------------------------------
  void SetGimStartValue( u8 index, u8 val );

  //--------------------------------------------------------------
  /**
   * @brief 運動リゾート開始時の値を取得
   * @param index   インデックス
   * @return  セットされた値
   */
  //--------------------------------------------------------------
  u8 GetGimStartValue( u8 index );
  
  //--------------------------------------------------------------
  /**
   * @brief 運動リゾートで上昇する努力値のIDを取得
   * @param kind  種類
   * @return  努力値ID
   */
  //--------------------------------------------------------------
  static pml::pokepara::PowerID GetGimKindPowerID( PokeResortGimKind kind );



  //--------------------------------------------------------------
  /**
   * @brief 最後に起動していたリゾートの場所(PokeResortKind)を設定
   */
  //--------------------------------------------------------------
	PokeResortKind GetSelectResortPos(void) const { return static_cast<PokeResortKind>(mData.mSelectResortPos); } 
  //--------------------------------------------------------------
  /**
   * @brief 最後に起動していたリゾートの場所(PokeResortKind)を取得
   */
  //--------------------------------------------------------------
	void SetSelectResortPos(PokeResortKind kind) { mData.mSelectResortPos = kind; } 


  //--------------------------------------------------------------
  /**
   * @brief まめのきを叩いた回数を得る
   */
  //--------------------------------------------------------------
  u8 GetBeansIvyHiddenCount(void);
  //--------------------------------------------------------------
  /**
   * @brief まめのきを叩いた回数をふやす
   */
  //--------------------------------------------------------------
  void AddBeansIvyHiddenCount(u8 max);
  //--------------------------------------------------------------
  /**
   * @brief まめのきを叩いた回数を０にする
   */
  //--------------------------------------------------------------
  void ResetBeansIvyHiddenCount(void);
  //--------------------------------------------------------------
  /**
   * @brief    まめのきかくしタイマーの時間を減算する
   * @param    時間、分、秒
   @return   時間が０ならtrue
   */
  //--------------------------------------------------------------
  bool SetBeansIvyHiddenTimeDownCount(void);
  //--------------------------------------------------------------
  /**
   * @brief    まめのきかくしタイマーの時間を設定
   * @param    時間、分、秒
   */
  //--------------------------------------------------------------
  void SetBeansIvyHiddenTime(u8 hour,u8 min,u8 sec);

  //--------------------------------------------------------------
  // 温泉ポケモン設定処理
  //--------------------------------------------------------------
  void ResultBoxHotSpa(gfl2::heap::HeapBase* heap , int groupno );

  //--------------------------------------------------------------
  // 運動ポケモン設定処理
  //--------------------------------------------------------------
  void ResultBoxGim(gfl2::heap::HeapBase* heap , int groupno, bool binbox );
  
  //--------------------------------------------------------------
  /**
   * @brief    RTCオフセットが変わってないか確認し、場合によってはリゾートのタイマーを変更する
   */
  //--------------------------------------------------------------
  void ResortRTCCheckFunc(void);


  
public:
  virtual void SetData( void * data );
  virtual void * GetData( void ) { return &mData; };
  virtual size_t GetDataSize( void ) { return sizeof(ResortSaveData); };
  virtual void Clear( gfl2::heap::HeapBase * heap );


  u32 GetSerializeDataSize(void);
  u32  Serialize( void* dst ) const;
  void Deserialize( const void* data );

  

private:

  void ResetTime(void);
  bool isRTCOffset(void);
  
  int GetBigBeansKind(int beans );
  bool downTimeCountchk(u8* pTimeBuf);
  bool downTimeCount(u8* pTimeBuf);
  bool downTimeCountBonus(u8* pTimeBuf);
  u64 getDateTimeToSecond(u8* time8);

  GameSys::DeviceDate mTempDate;

#if 0
  void Debug10YearTest(void);
#endif
  
};  // ResortSave

GFL_NAMESPACE_END(Savedata)

#endif //__RESORT_SAVE_H__
