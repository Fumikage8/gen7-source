//============================================================================================
/**
 * @file    ConfigSave.h
 * @brief   コンフィグセーブデータ
 * @author  Nobuhiko Ariizumi  ->k.ohno
 * @date    12/6/14  15/05/02
 */
//============================================================================================
#pragma once

#ifndef __CONFIG_SAVE_H__
#define __CONFIG_SAVE_H__

#include "util/include/gfl2_std_string.h"
#include "ui/include/gfl2_UI.h"

#include "System/include/PokemonLanguage.h"
#include "System/include/PokemonVersion.h"
#include "Savedata/include/SaveDataInterface.h"

GFL_NAMESPACE_BEGIN(Savedata)

class ConfigSave : public SaveDataInterface {
  GFL_FORBID_COPY_AND_ASSIGN(ConfigSave); //コピーコンストラクタ＋代入禁止

public:

  typedef enum {
    MSG_SLOW = 0, //1文字につき2フレーム
    MSG_NORMAL, //1文字につき1フレーム
    MSG_FAST, //1文字につき0.5フレーム

    MSG_NO_WAIT, //255フレーム(表示処理用)

    MSG_SPEED_MAX,
  } MSG_SPEED;

  typedef enum {
    BATTLE_ANIMATION_VISIBLE, // 見る
    BATTLE_ANIMATION_UNVISIBLE, // 見ない

    BATTLE_ANIMATION_MAX,
  } BATTLE_ANIMATION;

  typedef enum {
    BATTLE_RULE_REPLACEMENT, // いれかえ
    BATTLE_RULE_TOURNAMENT, // トーナメント

    BATTLE_RULE_MAX,
  } BATTLE_RULE;

  typedef enum {
    BATTLE_WALLPAPER_1,
    BATTLE_WALLPAPER_2,
    BATTLE_WALLPAPER_3,
    BATTLE_WALLPAPER_4,
    BATTLE_WALLPAPER_5,
    BATTLE_WALLPAPER_6,
    BATTLE_WALLPAPER_7,
    BATTLE_WALLPAPER_8,
    BATTLE_WALLPAPER_9,
    BATTLE_WALLPAPER_10,
    BATTLE_WALLPAPER_11,
    BATTLE_WALLPAPER_12,
    BATTLE_WALLPAPER_13,
    BATTLE_WALLPAPER_14,
    BATTLE_WALLPAPER_15,

    BATTLE_WALLPAPER_MAX,
  } BATTLE_WALLPAPER;

  typedef enum {
    BUTTON_MODE_NORMAL, // 通常
    BUTTON_MODE_L_EQUAL_A, // L=A
    BUTTON_MODE_NOT_LR, // L,R禁止

    BUTTON_MODE_MAX,
  } BUTTON_MODE;

  /**
   * @brief ボックス状態
   */
  typedef enum {
    BOX_STATUS_MANUAL_SELECT,    //!< じぶんで選ぶ
    BOX_STATUS_AUTO_SELECT,     //!< 自動でおくる

    BOX_STATUS_MAX,
  }BOX_STATUS;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ConfigSave(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ~ConfigSave(void);


private:

  struct ConfigData {
    u32 talkingSpeed : 2; //0~2 ///< 話の速さ
    u32 battleAnimation : 1; ///< 戦闘アニメを観る・観ない
    u32 battleRule : 1; ///< 試合のルール
    u32 msgLangId : 4; //0~7
    u32 battleWallpaper : 5; //0~14 ///< 戦闘壁紙
    u32 buttonMode : 2; //0~2 ///< 通常、L=A、LR禁止
   // u32 isReport : 1;     ///< 通信を行う前にセーブするか？
    u32 isBox : 1;        ///< ボックス送信するか？
    u32 isEnableBoss : 1; ///< いつの間にか通信を利用するか？
    u32 isEnablePss  : 1; ///< PSS通信が有効か？
    u32 reserved : 14;
  };

  ConfigData mConfigData; //データ本体

public:

  /**
   *  シリアライズ用データサイズ
   */
  static const int SerialDataSize = sizeof (ConfigData);

  //-----------------------------------------------------------------------------
  /**
   * @brief    読み込んだデータをセットする関数
   * @param    pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void SetData(void* pData)
  {
    gfl2::std::MemCopy(pData, reinterpret_cast<void*> (&mConfigData), sizeof (ConfigData));
  };
  //-----------------------------------------------------------------------------
  /**
   * @brief    データの先頭ポインタを得る関数
   * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void* GetData(void)
  {
    return reinterpret_cast<void*> (&mConfigData);
  };
  //-----------------------------------------------------------------------------
  /**
   * @brief    データサイズ
   * @retval   バイト
   */
  //-----------------------------------------------------------------------------
  virtual size_t GetDataSize(void)
  {
    return sizeof (ConfigData);
  };

  //--------------------------------------------------------------------------------------------
  /**
   * @brief セーブデータクリア処理
   * @param heap
   */
  //--------------------------------------------------------------------------------------------
  void Clear(gfl2::heap::HeapBase * heap);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージスピードのセット
   * @param   MSG_SPEED  メッセージスピード
   */
  //--------------------------------------------------------------------------------------------
  void SetTalkingSpeed(const MSG_SPEED spd);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   メッセージスピードの取得
   * @return  MSG_SPEED  メッセージスピード
   */
  //--------------------------------------------------------------------------------------------
  MSG_SPEED GetTalkingSpeed(void)const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルアニメーションを見る／見ないをセットする
   * @param   BATTLE_ANIMATION  バトルアニメーションを見る／見ない
   */
  //--------------------------------------------------------------------------------------------
  void SetBattleAnimation(const BATTLE_ANIMATION battleAnim);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルアニメーションを見る／見ないの取得
   * @return  BATTLE_ANIMATION  バトルアニメーションを見る／見ない
   */
  //--------------------------------------------------------------------------------------------
  BATTLE_ANIMATION GetBattleAnimation(void)const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルのルールをセットする
   * @param   BATTLE_RULE  バトルのルール
   */
  //--------------------------------------------------------------------------------------------
  void SetBattleRule(const BATTLE_RULE battleRule);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルのルールの取得
   * @return  BATTLE_RULE  バトルのルール
   */
  //--------------------------------------------------------------------------------------------
  BATTLE_RULE GetBattleRule(void)const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   日本語かどうかを取得
   * @param   bool  trueのとき、日本語  falseのとき、それ以外の言語
   */
  //--------------------------------------------------------------------------------------------
  bool IsJapanese(void);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   漢字モードのセット
   * @param   bool  trueのとき、漢字モード  falseのとき、かなモード
   */
  //--------------------------------------------------------------------------------------------
  void SetKanjiMode(bool is_kanji);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   漢字モードの取得
   * @return   bool  trueのとき、漢字モード  falseのとき、かなモードor日本語ではない
   */
  //--------------------------------------------------------------------------------------------
  bool IsKanjiMode(void) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトル壁紙をセットする
   * @param   BATTLE_WALLPAPER  バトル壁紙
   */
  //--------------------------------------------------------------------------------------------
  void SetBattleWallpaper(const BATTLE_WALLPAPER battleWallpaper);
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトル壁紙の取得
   * @return  BATTLE_WALLPAPER  バトル壁紙
   */
  //--------------------------------------------------------------------------------------------
  BATTLE_WALLPAPER GetBattleWallpaper(void)const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンモードをセットする
   * @param   BUTTON_MODE  ボタンモード
   */
  //--------------------------------------------------------------------------------------------
  void SetButtonMode(const BUTTON_MODE buttonMode, gfl2::ui::Button* pUiButton, gfl2::ui::Button* pUiButtonOfStickEmu);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボタンモードの取得
   * @return  BUTTON_MODE  ボタンモード
   */
  //--------------------------------------------------------------------------------------------
  BUTTON_MODE GetButtonMode(void)const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボックス状態のセット
   * @param   IR_REPORT
   */
  //--------------------------------------------------------------------------------------------
  void SetBoxStatus(const BOX_STATUS isBox);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ボックス状態の取得
   * @return  NET_REPORT
   */
  //--------------------------------------------------------------------------------------------
  BOX_STATUS GetBoxStatus(void)const;
  
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   BOSSサーバのセット
   * @param   isEnable BOSSサーバを有効にするならtrueを指定
   */
  //--------------------------------------------------------------------------------------------
  void SetBossEnable(bool isEnable);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   BOSSサーバの取得
   * @return  BOSSサーバが有効ならtrueを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsEnableBoss(void) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   PSS通信の有効設定
   *
   * @param   isEnable PSS通信を有効にするならtrueを指定
   */
  //--------------------------------------------------------------------------------------------
  void SetPssEnable(bool isEnable);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   PSS通信の有効設定
   *
   * @return  PSS通信が有効ならtrueを返却
   */
  //--------------------------------------------------------------------------------------------
  bool IsEnablePss(void) const;

};

GFL_NAMESPACE_END(Savedata)
#endif // __CONFIG_SAVE_H__
