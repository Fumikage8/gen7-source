#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFO_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFO_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewInfo.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <str/include/gfl2_Font.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanViewInfoFunc.h"

#include "ZukanDebugWin.h"




// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class ScrollBar;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanPokeIconTextureManager;
  class ZukanFrameInfoCell;
  class ZukanViewInfoListener;
  class ZukanQRDisplay;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewInfo
 @brief     図鑑の情報画面のビュー
 */
//==============================================================================
class ZukanViewInfo : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewInfo);

  friend class ZukanViewInfoListener;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewInfo(const ZukanSceneParam& param);
  
  virtual ~ZukanViewInfo();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //! @name  friendクラスZukanViewLaunchListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_KEY_TRIGGER_L ,
    REQ_ID_KEY_TRIGGER_R ,
    REQ_ID_KEY_REPEAT_L  ,
    REQ_ID_KEY_REPEAT_R  ,
    
    REQ_ID_KEY_TRIGGER_LEFT  ,
    REQ_ID_KEY_TRIGGER_RIGHT ,
    REQ_ID_KEY_REPEAT_LEFT   ,
    REQ_ID_KEY_REPEAT_RIGHT  ,

    REQ_ID_KEY_TRIGGER_X ,

    REQ_ID_BUTTON_DECIDE     ,  // reqParamはBUTTON_ID_
    REQ_ID_BUTTON_TOUCH      ,  // reqParamはBUTTON_ID_
  };

  //! @brief  ボタン
  enum
  {
    ///////////////////////////
    // 情報画面の説明ページ
    ///////////////////////////
    // Tab
    BUTTON_ID_EXPLAIN_LANG00,
    BUTTON_ID_EXPLAIN_LANG01,
    BUTTON_ID_EXPLAIN_LANG02,
    BUTTON_ID_EXPLAIN_LANG03,
    BUTTON_ID_EXPLAIN_LANG04,
    BUTTON_ID_EXPLAIN_LANG05,
    BUTTON_ID_EXPLAIN_LANG06,
    BUTTON_ID_EXPLAIN_LANG07,
    BUTTON_ID_EXPLAIN_LANG08,
    // Header -> Info_button
    BUTTON_ID_EXPLAIN_PREV_MONS,
    BUTTON_ID_EXPLAIN_NEXT_MONS,
    // Btn_Info
    BUTTON_ID_EXPLAIN_MAP,
    BUTTON_ID_EXPLAIN_FORMVOICE,
    BUTTON_ID_EXPLAIN_BACK,
    // 上画面
    //BUTTON_ID_EXPLAIN_QR,

    // ↓制御用の値
    BUTTON_ID_EXPLAIN_START = BUTTON_ID_EXPLAIN_LANG00,  // START<= <=END
    BUTTON_ID_EXPLAIN_END   = BUTTON_ID_EXPLAIN_BACK,//BUTTON_ID_EXPLAIN_QR,
    BUTTON_ID_EXPLAIN_NUM   = BUTTON_ID_EXPLAIN_END - BUTTON_ID_EXPLAIN_START +1,
    
    BUTTON_ID_EXPLAIN_LOWER_START = BUTTON_ID_EXPLAIN_LANG00,  // START<= <=END
    BUTTON_ID_EXPLAIN_LOWER_END   = BUTTON_ID_EXPLAIN_BACK,
    BUTTON_ID_EXPLAIN_LOWER_NUM   = BUTTON_ID_EXPLAIN_LOWER_END - BUTTON_ID_EXPLAIN_LOWER_START +1,

    BUTTON_ID_EXPLAIN_LANG_START = BUTTON_ID_EXPLAIN_LANG00,  // START<= <=END
    BUTTON_ID_EXPLAIN_LANG_END   = BUTTON_ID_EXPLAIN_LANG08,
    BUTTON_ID_EXPLAIN_LANG_NUM   = BUTTON_ID_EXPLAIN_LANG_END - BUTTON_ID_EXPLAIN_LANG_START +1,
  };

  enum
  {
    ///////////////////////////////
    // 情報画面の姿＆鳴き声ページ
    ///////////////////////////////
    // Bg_V
    BUTTON_ID_FORMVOICE_PREV_MONS,
    BUTTON_ID_FORMVOICE_NEXT_MONS,
    BUTTON_ID_FORMVOICE_PREV_FORM,
    BUTTON_ID_FORMVOICE_NEXT_FORM,
    // Btn_Cries
    BUTTON_ID_FORMVOICE_BACK,
    BUTTON_ID_FORMVOICE_VOICE,
    BUTTON_ID_FORMVOICE_MOTION,
    BUTTON_ID_FORMVOICE_PAUSE,

    // ↓制御用の値
    BUTTON_ID_FORMVOICE_START = BUTTON_ID_FORMVOICE_PREV_MONS,  // START<= <=END
    BUTTON_ID_FORMVOICE_END   = BUTTON_ID_FORMVOICE_PAUSE,
    BUTTON_ID_FORMVOICE_NUM   = BUTTON_ID_FORMVOICE_END - BUTTON_ID_FORMVOICE_START +1,
  };

  enum
  {
    ///////////////////////////////
    // 登録画面
    ///////////////////////////////
    // Btn_Entry
    BUTTON_ID_REGISTER_OK,
    BUTTON_ID_REGISTER_VOICE,
    BUTTON_ID_REGISTER_MOTION,
    BUTTON_ID_REGISTER_PAUSE,

    // ↓制御用の値
    BUTTON_ID_REGISTER_START = BUTTON_ID_REGISTER_OK,     // START<= <=END
    BUTTON_ID_REGISTER_END   = BUTTON_ID_REGISTER_PAUSE,
    BUTTON_ID_REGISTER_NUM   = BUTTON_ID_REGISTER_END - BUTTON_ID_REGISTER_START +1,
  };

  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  static const u32 QR_MEM_SIZE = 0x480000;  // 4.5MB

  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,       // 表示していない
    DISP_STATE_ON_IN,     // 表示している(イン中)
    DISP_STATE_ON_WORK,   // 表示している(稼働中)
    DISP_STATE_ON_OUT,    // 表示している(アウト中)
    DISP_STATE_ON_MODE_IN,   // 表示している(新モードがイン中)
    DISP_STATE_ON_MODE_OUT,  // 表示している(新モードがアウト中)
    DISP_STATE_ON_FADE_OUT,  // 表示している(フェードアウト中)
    DISP_STATE_WAIT_SAFETY,  // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,  // 表示したまま終了するのを待っている
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_LOWER,
    LYT_WK_ID_UPPER,
    LYT_WK_ID_NUM,
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void createScrollBar(void);
  void destroyScrollBar(void);

  void startInSeq(void);
  void startOutSeq(void);
  void startModeInSeq(void);
  void startModeOutSeq(void);

  void changeDispState(DispState dispState);

  void createPageOnce(void);
  void createPageForMode(void);
  void setCurrDispPoke(void);
  void setCurrDispForm(void);

  bool increaseCurrLangIndex(bool isTrigger);  // インデックスが変わったらtrueを返す
  bool decreaseCurrLangIndex(bool isTrigger);  // インデックスが変わったらtrueを返す
  void setCurrLangIndex(s32 buttonId);
  void setCurrLang(void);

  void setCurrButtonStateForPage(void);
  void setCurrButtonStateForDispPoke(void);
  void setCurrButtonStateForDispForm(void);
  void setCurrButtonStateForLang(void);

  void setQRCodeDisp(bool isDisp);
  void setPokeModelPosByQRCodeDisp(bool qrCodeDispChange);  // ポケモンモデルが変わったとき、QRコード表示モードが変わったとき、に呼ぶ

  void playVoice(void);
  void changeMotion(void);
  void switchPause(void);

  void createLangFont(void);
  void destroyLangFont(void);
  gfl2::str::Font* getLangFont(ZukanType::ZukanLang zukanLang) const;

  void replaceLangIconTexture(gfl2::lyt::LytPicture* pane, u32 pokemonLang);

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  void unbindKeyForPokeModelSRTModifyMode(void);
#endif

private:
  static const u8 ZUKAN_LANG_ORDER_TABLE[ZukanType::ZUKAN_LANG_NUM][ZukanType::ZUKAN_LANG_NUM];  // git_program/niji_project/prog/System/include/PokemonVersion.h  POKEMON_LANG_JAPANなど
  
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_NUM];
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_FORMVOICE[BUTTON_ID_FORMVOICE_NUM];
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO_FOR_REGISTER[BUTTON_ID_REGISTER_NUM];
  
  static const ZukanType::ScrollBarConstInfo SCROLL_BAR_CONST_INFO;

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  gfl2::heap::HeapBase*  m_qrDevMem;

  ZukanViewInfoListener* m_viewListener;

  ZukanType::ButtonPaneInfo* m_buttonPaneInfo;  // m_buttonPaneInfo[BUTTON_ID_???_NUM];

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];

  DispState m_dispState;
  u32       m_dispStateCount;

  bool m_modeIsFormVoice;
  bool m_modeChanging;
  bool m_isQRCodeDisp;

  ZukanType::ZukanLang m_gameLang;       // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。
  s32                  m_currLangIndex;  // 0<= <ZUKAN_LANG_NUM  ただのインデックスなのでZUKAN_LANG_ではなく数字を入れる  // 図鑑の情報画面で変更できる言語はこのインデックスで指す。0のときm_gameLangの言語が表示される。
                                         // ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]
  s32                  m_userLangIndex;  // 0<= <ZUKAN_LANG_NUM  ただのインデックスなのでZUKAN_LANG_ではなく数字を入れる  // ユーザが選択した言語を指すインデックス。
                                         // m_currLangIndexは実際に表示に使う言語。ポケモンごとの取得済み言語によって0かm_userLangIndexになる。
                                         // m_userLangIndexはユーザがどこかのポケモンで選択した言語。他のポケモンでは取得済みでないかもしれない。

  ZukanPokeIconTextureManager*  m_pokeIconTexManager;

  // スクロールバー
  app::tool::ScrollBar*  m_scrollBar;

  // ペインのポインタを覚えておく
  gfl2::lyt::LytTextBox*  m_pokeNoTextPane;       // 025
  gfl2::lyt::LytTextBox*  m_pokeNameTextPane;     // ピカチュウ
  gfl2::lyt::LytTextBox*  m_pokeSpeciesTextPane;  // ねずみポケモン
  gfl2::lyt::LytTextBox*  m_formNameTextPane;
  gfl2::lyt::LytTextBox*  m_heightLabelTextPane;
  gfl2::lyt::LytTextBox*  m_weightLabelTextPane;
  gfl2::lyt::LytTextBox*  m_heightValTextPane;
  gfl2::lyt::LytTextBox*  m_weightValTextPane;
  gfl2::lyt::LytTextBox*  m_commentTextPane;
  gfl2::lyt::LytPicture*  m_type1PicPane;
  gfl2::lyt::LytPicture*  m_type2PicPane;
  gfl2::lyt::LytPane*     m_QRButtonPane;
  gfl2::lyt::LytPane*     m_QRLytTopPane;
  gfl2::lyt::LytPane*     m_QRDispTopPane;
  gfl2::lyt::LytPane*     m_NoQRTopPane;
  gfl2::lyt::LytTextBox*  m_NoQRTextPane;
  gfl2::lyt::LytPane*     m_QRTopPane;
  gfl2::lyt::LytPicture*  m_QRPicPane;

  // ポケアイコンのペインのポインタを覚えておく(情報画面の説明ページ)
  gfl2::lyt::LytPicture*  m_pokeIconPicPaneForExplain;
  gfl2::lyt::LytPane*     m_rarePaneForExplain;
  //gfl2::lyt::LytPane*     m_gender1PaneForExplain;  // アニメでできるので使わない
  //gfl2::lyt::LytPane*     m_gender2PaneForExplain;  // アニメでできるので使わない

  // ポケアイコンのペインのポインタを覚えておく(情報画面の姿＆鳴き声ページ)
  gfl2::lyt::LytPicture*  m_pokeIconPicPaneForFormVoice;
  gfl2::lyt::LytPane*     m_rarePaneForFormVoice;
  //gfl2::lyt::LytPane*     m_gender1PaneForFormVoice;  // アニメでできるので使わない
  //gfl2::lyt::LytPane*     m_gender2PaneForFormVoice;  // アニメでできるので使わない

  ZukanViewInfoFunc  m_infoFunc;

  ZukanQRDisplay*  m_QRDisplay;
  s32              m_QRDisplayDummyVal;

  gfl2::str::Font*  m_japanFont;
  gfl2::str::Font*  m_chinaFont;
  gfl2::str::Font*  m_taiwanFont;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFO_H_INCLUDED

