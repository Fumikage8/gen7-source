//============================================================================================
/**
 * @file    ConfigSave.cpp
 * @brief   コンフィグセーブデータ
 * @author  Nobuhiko Ariizumi  => k.ohno
 * @date    12/6/14  15/05/02
 */
//============================================================================================
#include "Savedata/include/ConfigSave.h"
#include "Print/include/PrintSystem.h"

GFL_NAMESPACE_BEGIN(Savedata)

  
//--------------------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//--------------------------------------------------------------------------------------------
ConfigSave::ConfigSave(void)
{
    Clear(NULL);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//--------------------------------------------------------------------------------------------
ConfigSave::~ConfigSave(void)
{
}


//--------------------------------------------------------------------------------------------
/**
 * @brief セーブデータクリア処理
 * @param heap
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::Clear(gfl2::heap::HeapBase * heap)
{
  gfl2::std::MemClear(this->GetData(), this->GetDataSize());
  mConfigData.talkingSpeed = MSG_NORMAL;
  mConfigData.battleAnimation = BATTLE_ANIMATION_VISIBLE;
  mConfigData.battleRule = BATTLE_RULE_REPLACEMENT;
  mConfigData.msgLangId = System::Language::ConvertPM2MSG(System::GetLang());
  mConfigData.battleWallpaper = BATTLE_WALLPAPER_1;
  mConfigData.buttonMode = BUTTON_MODE_NORMAL;
  mConfigData.isBox       = BOX_STATUS_MANUAL_SELECT;
  mConfigData.isEnableBoss = false;
  mConfigData.isEnablePss  = true;
  mConfigData.reserved = 0;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージスピードのセット
 * @param   MSG_SPEED  メッセージスピード
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetTalkingSpeed(const ConfigSave::MSG_SPEED spd)
{
    mConfigData.talkingSpeed = spd;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   メッセージスピードの取得
 * @return  MSG_SPEED  メッセージスピード
 */
//--------------------------------------------------------------------------------------------
ConfigSave::MSG_SPEED ConfigSave::GetTalkingSpeed(void)const
{
    return static_cast<ConfigSave::MSG_SPEED> (mConfigData.talkingSpeed);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルアニメーションを見る／見ないをセットする
 * @param   BATTLE_ANIMATION  バトルアニメーションを見る／見ない
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetBattleAnimation(const ConfigSave::BATTLE_ANIMATION battleAnim)
{
    mConfigData.battleAnimation = battleAnim;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルアニメーションを見る／見ないの取得
 * @return  BATTLE_ANIMATION  バトルアニメーションを見る／見ない
 */
//--------------------------------------------------------------------------------------------
ConfigSave::BATTLE_ANIMATION ConfigSave::GetBattleAnimation(void)const
{
    return static_cast<ConfigSave::BATTLE_ANIMATION> (mConfigData.battleAnimation);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルのルールをセットする
 * @param   BATTLE_RULE  バトルのルール
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetBattleRule(const ConfigSave::BATTLE_RULE battleRule)
{
    mConfigData.battleRule = battleRule;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   バトルのルールの取得
 * @return  BATTLE_RULE  バトルのルール
 */
//--------------------------------------------------------------------------------------------
ConfigSave::BATTLE_RULE ConfigSave::GetBattleRule(void)const
{
    return static_cast<ConfigSave::BATTLE_RULE> (mConfigData.battleRule);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   日本語かどうかを取得
 * @param   bool  trueのとき、日本語  falseのとき、それ以外の言語
 */
//--------------------------------------------------------------------------------------------
bool ConfigSave::IsJapanese(void)
{
  return ( System::GetLang() == POKEMON_LANG_JAPAN );
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   漢字モードのセット
 * @param   bool  trueのとき、漢字モード  falseのとき、かなモード
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetKanjiMode(bool is_kanji)
{
  if (IsJapanese() == false ) return;

  if (is_kanji) {
    mConfigData.msgLangId = System::MSGLANGID_KANJI;
  } else {
    mConfigData.msgLangId = System::MSGLANGID_KANA;
  }
  print::SetMessageLangId(static_cast<System::MSGLANGID> (mConfigData.msgLangId));
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   漢字モードの取得
 * @return   bool  trueのとき、漢字モード  falseのとき、かなモードor日本語ではない
 */
//--------------------------------------------------------------------------------------------
bool ConfigSave::IsKanjiMode(void) const
{
    switch (mConfigData.msgLangId) {
    case System::MSGLANGID_KANJI:
        return true;
    case System::MSGLANGID_KANA:
        return false;
    default:
        //日本語モードでないのに問い合わせが来た。絶対に漢字ではない
        return false;
    }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   バトル壁紙をセットする
 * @param   BATTLE_WALLPAPER  バトル壁紙
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetBattleWallpaper(const ConfigSave::BATTLE_WALLPAPER battleWallpaper)
{
    mConfigData.battleWallpaper = battleWallpaper;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   バトル壁紙の取得
 * @return  BATTLE_WALLPAPER  バトル壁紙
 */
//--------------------------------------------------------------------------------------------
ConfigSave::BATTLE_WALLPAPER ConfigSave::GetBattleWallpaper(void)const
{
    return static_cast<ConfigSave::BATTLE_WALLPAPER> (mConfigData.battleWallpaper);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンモードをセットする
 * @param   BUTTON_MODE  ボタンモード
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetButtonMode(const ConfigSave::BUTTON_MODE buttonMode, gfl2::ui::Button* pUiButton, gfl2::ui::Button* pUiButtonOfStickEmu)
{
  mConfigData.buttonMode = buttonMode;

  switch (buttonMode) {
  case BUTTON_MODE_NORMAL:
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButton->AddButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButton->AddButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->AddButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->AddButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    break;
  case BUTTON_MODE_L_EQUAL_A:
    pUiButton->AddButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    pUiButtonOfStickEmu->AddButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    break;
  case BUTTON_MODE_NOT_LR:
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButton->RemoveButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_A, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_L, gfl2::ui::BUTTONID_L);
    pUiButtonOfStickEmu->RemoveButtonAssignment(gfl2::ui::BUTTONID_R, gfl2::ui::BUTTONID_R);
    break;
  default:
    GFL_ASSERT(0);
  }
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   ボタンモードの取得
 * @return  BUTTON_MODE  ボタンモード
 */
//--------------------------------------------------------------------------------------------
ConfigSave::BUTTON_MODE ConfigSave::GetButtonMode(void)const
{
  return static_cast<ConfigSave::BUTTON_MODE> (mConfigData.buttonMode);
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   ボックス状態のセット
  * @param   IR_REPORT
  */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetBoxStatus(const BOX_STATUS isBox)
{
  mConfigData.isBox = isBox;
}

//--------------------------------------------------------------------------------------------
/**
  * @brief   ボックス状態の取得
  * @return  NET_REPORT
  */
//--------------------------------------------------------------------------------------------
ConfigSave::BOX_STATUS ConfigSave::GetBoxStatus(void)const
{
  return static_cast<ConfigSave::BOX_STATUS>(mConfigData.isBox);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   BOSSサーバの有効設定
 *
 * @param   isEnable BOSSサーバを有効にするならtrueを指定
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetBossEnable(bool isEnable)
{
    mConfigData.isEnableBoss = isEnable;
}
//--------------------------------------------------------------------------------------------
/**
 * @brief   BOSSサーバの有効設定
 *
 * @return  BOSSサーバが有効ならtrueを返却
 */
//--------------------------------------------------------------------------------------------
bool ConfigSave::IsEnableBoss(void) const
{
    return mConfigData.isEnableBoss;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   PSS通信の有効設定
 *
 * @param   isEnable PSS通信を有効にするならtrueを指定
 */
//--------------------------------------------------------------------------------------------
void ConfigSave::SetPssEnable(bool isEnable)
{
    mConfigData.isEnablePss = isEnable;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   PSS通信の有効設定
 *
 * @return  PSS通信が有効ならtrueを返却
 */
//--------------------------------------------------------------------------------------------
bool ConfigSave::IsEnablePss(void) const
{
    return mConfigData.isEnablePss;
}

GFL_NAMESPACE_END(Savedata)
