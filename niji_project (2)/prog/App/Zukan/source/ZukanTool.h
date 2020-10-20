#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANTOOL_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANTOOL_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanTool.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.25
 @brief   図鑑で使うツール
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

// pmlのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeModel.h>

#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Tool/app_tool_MessageMenuView.h>

#include <AppLib/include/Tool/app_tool_2ButtonsView.h>
#include <AppLib/include/Tool/GeneralView/GeneralLowerView.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanViewBg.h"
#include "ZukanViewRegisterFade.h"

#include "ZukanDebugWin.h"




// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(instance)
  class DrawEnvNode;
GFL_NAMESPACE_END(instance)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
  class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
  class BaseCamera;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
  class ExtendData;
  class PersonalSort;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)
  class TalkWindow;
GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class PokeSimpleModelInFrame;
  class AppCommonGrpIconData;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class G2DUtil;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)

class WordSetLoader;

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanTool
 @brief     図鑑で使うツール
 */
//==============================================================================
class ZukanTool
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanTool);

public:
  ZukanTool(
      GameSys::GameManager*           gameManager,
      app::util::AppRenderingManager* appRenderingManager
  );
  virtual ~ZukanTool();

  void Update(void);


public:
  //! @name  会話用ウィンドウ
  //@{
  //! @brief  会話用ウィンドウ生成
  //! @param[in]  appHeap  ヒープ。
  //! @param[in]  buf      リソースを読み込んだバッファ。
  //! @return  生成できたらtrueを返す
  bool CreateTalkWindow(app::util::Heap* appHeap, void* buf);
  //! @brief  会話用ウィンドウ破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyTalkWindow(void);

  //! @brief  [ウィンドウが表示されていないとき] 会話用ウィンドウをオープンしメッセージを流す
  //          [ウィンドウが表示されているとき]   会話用ウィンドウのメッセージを変更し流す
  void OpenTalkWindow(
      app::ui::UIView*   parentView,       // [ウィンドウが表示されていないとき]だけ使用する
      gfl2::str::StrBuf* strBuf,
      bool               msgCursorVisible  // 最後にカーソルを表示するときはtrue
  );
  //! @brief  会話用ウィンドウをクローズする
  void CloseTalkWindow(void);
  //! @brief  会話用ウィンドウが待ち状態にあるか
  //! @note  次のメッセージに変更されるのを待っているのかもしれないし、
  //         クローズされるのを待っているのかもしれないし、、、
  //         何待ちかはユーザ次第。
  bool IsTalkWindowWaiting(void) const;

  //! @brief  会話用ウィンドウがオープン中なら取得する
  App::Tool::TalkWindow* GetTalkWindowIfOpen(void) const;
  //@}


  //! @name  メニューカーソル
  //@{
  //! @brief  メニューカーソル生成
  //! @param[in]  appHeap  ヒープ。このヒープapp::tool::MenuCursorクラス内で保持され、CreateLayoutWorkの際に使われるので注意すること。
  //! @return  生成できたらtrueを返す
  bool CreateMenuCursor(app::util::Heap* appHeap);
  //! @brief  メニューカーソル破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyMenuCursor(void);

  //! @brief  メニューカーソルレイアウトワーク生成。アニメは開始するが、表示は非表示。
  //! @attention  メモリはCreateMenuCursorで渡してもらっていたメモリを使う
  void CreateMenuCursorLayoutWork(app::util::G2DUtil* pG2DUtil, gfl2::lyt::DisplayType displayType, app::tool::MenuCursor::CursorSize cursorSize);
  //! @brief  メニューカーソルレイアウトワーク破棄。trueで削除完了。
  bool DestroyMenuCursorLayoutWork(void);

  //! @brief  メニューカーソルを取得する
  //! @return  生成できていないときはNULLを返す
  app::tool::MenuCursor* GetMenuCursor(void) const;
  //@}


  //! @name  メッセージウィンドウ＆リストメニュー
  //@{
  //! @brief  メッセージウィンドウ＆リストメニュー生成
  //! @param[in]  appHeap  ヒープ。
  //! @param[in]  buf      リソースを読み込んだバッファ。
  //! @return  生成できたらtrueを返す
  //! @note  メニューカーソルのリソースバッファを使用しているので、先にCreateMenuCursorを済ませておくこと。
  bool CreateMessageMenuView(app::util::Heap* appHeap, void* buf);
  //! @brief  メッセージウィンドウ＆リストメニュー破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyMessageMenuView(void);

  //! @brief  メッセージウィンドウ＆リストメニューを取得する
  //! @return  生成できていないときはNULLを返す
  App::Tool::MessageMenuView* GetMessageMenuView(void) const;
  //@}


  //! @name  背景
  //@{
  //! @brief  背景生成
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void CreateBg(const ZukanViewBg::Param& param);
  
  void DestroyBg(void);
 
  //! @brief  背景を取得する
  //! @return  生成できていないときはNULLを返す
  ZukanViewBg* GetViewBg(void) const;
  //@}


  //! @name  図鑑の登録画面に出すフェード層のビュー
  //@{
  //! @brief  図鑑の登録画面に出すフェード層のビューの生成
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  void CreateRegisterFade(const ZukanViewRegisterFade::Param& param);
  
  void DestroyRegisterFade(void);
 
  //! @brief  図鑑の登録画面に出すフェード層のビューを取得する
  //! @return  生成できていないときはNULLを返す
  ZukanViewRegisterFade* GetViewRegisterFade(void) const;
  //@}
 

  //! @name  ポケモンモデル
  //@{
  //! @brief  ポケモンモデルの配置場所
  enum PokeModelPosType
  {
    POKE_MODEL_POS_TYPE_CENTER,
    POKE_MODEL_POS_TYPE_LEFT,
    POKE_MODEL_POS_TYPE_RIGHT,
    POKE_MODEL_POS_TYPE_SMALL_CENTER,
    POKE_MODEL_POS_TYPE_NUM,
  };
  //! @brief  ポケモンモデルの配置サイズ
  enum PokeModelSizeType
  {
    POKE_MODEL_SIZE_TYPE_NORMAL,
    POKE_MODEL_SIZE_TYPE_SMALL,
    POKE_MODEL_SIZE_TYPE_SMALL_RIGHT,
    POKE_MODEL_SIZE_TYPE_NUM,
  };
  //! @brief  ポケモンモデルの配置スケール
  enum PokeModelScaleType
  {
    POKE_MODEL_SCALE_TYPE_NORMAL,
    POKE_MODEL_SCALE_TYPE_REKKUUZA_DELTA,
    POKE_MODEL_SCALE_TYPE_NUM,
  };

  //! @brief  ポケモンモデルシステム生成
  //! @param[in]  appHeap     ヒープ。
  //! @param[in]  heapForComp 圧縮ファイルを読み込むのに使うヒープ。
  //! @return  生成できたらtrueを返す
  bool CreatePokeModelSys(app::util::Heap* appHeap, gfl2::heap::HeapBase* heapForComp);
  //! @brief  ポケモンモデルシステム破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyPokeModelSys(void);

  //! @brief  ポケモンモデルシステムを取得する
  //! @return  生成できていないときはNULLを返す
  PokeTool::PokeModelSystem* GetPokeModelSys(void) const;
  app::tool::PokeSimpleModelInFrame* GetPokeSimpleModelInFrame(void) const;

  //! @brief  ポケモンモデルを変更する。
  //!         既にそのポケモンモデルになっているときは何もしない。
  void ChangePokeModel(const PokeTool::SimpleParam& sp);
  //! @brief  表示ON/OFF
  void SetPokeModelVisible(bool visible);
  //! @brief  表示状態を取得する
  //! @note  m_pokeSimpleModelInFrameが生成されていないときは、falseを返す。これがtrueを返しても生成できていないときは表示されていないので注意！
  //!        m_pokeSimpleModelInFrameが生成されているときは、ポケモンモデルの生成状態に関わらずm_pokeSimpleModelInFrameが保持している値を返す。
  //!        即ち、trueを返してもポケモンモデルが表示されていないこともあり得る(Updateを回していれば生成完了後に表示される)
  bool IsPokeModelVisible(void) const;

  //! @brief  ポケモンモデルを表示したときに鳴き声を再生するか
  void SetPokeModelVoiceFlag(bool voiceFlag) { m_pokeModelVoiceFlag = voiceFlag; }

  //! @brief  ポケモンモデルのアニメ、ポーズ、回転、拡大縮小をリセットする
  void ResetPokeModel(void);

  //! @brief  ポーズ
private:
  void pausePokeModelAnime(bool isPause);  // 止めるときtrue
public:
  void SwitchPokeModelAnimePause(void);
  //! @brief  アニメ変更
private:
  void changePokeModelAnime(PokeTool::MODEL_ANIME animeType);
public:
  void SetNextPokeModelAnime(void);
private:
  void resetPokeModelAnime(void);
public:
  void SetPokeModelPosType(PokeModelPosType posType, bool posAnimFlag=false);
  void UpdatePokeModelController(bool onlyRotate = false);

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  void UpdatePokeModelControllerForPokeModelSRTModifyMode(void);
#endif

private:
  void setPokeModelInFrameSRT(void);  // 現在の値で調整すべきインフレームに関わるパラメータを設定する
  void setPokeModelInFrameFrame(void);  // 現在の値でインフレームのフレームを設定する
  void updatePokeModelPosAnim(void);

  void setPokeModelInFrameUserValue(void);  // 現在の値でインフレームのUserPosOffsetとUserScaleを設定する

private:
  static bool checkUsingKawaigariInAppForZukan( MonsNo monsNo , pml::FormNo formNo );
  //@}


  //! @name  パーソナル外ポケモンデータ
  //@{
public:
  //! @brief  パーソナル外ポケモンデータ生成
  //! @param[in]  appHeap  ヒープ。
  //! @return  生成できたらtrueを返す
  //bool CreatePokeExtendData(app::util::Heap* appHeap);
  //! @brief  パーソナル外ポケモンデータ破棄
  //! @return  破棄できたらtrueを返す
  //bool DestroyPokeExtendData(void);

  //! @brief  パーソナル外ポケモンデータを取得する
  //! @return  生成できていないときはNULLを返す
  const PokeTool::ExtendData* GetPokeExtendData(void) const;
  //@}
  //! @name  パーソナルデータソートクラス
  //@{
  //! @brief  パーソナルデータソートクラスのインスタンス生成
  //! @param[in]  appHeap  ヒープ。
  //! @return  生成できたらtrueを返す
  bool CreatePokePersonalSort(app::util::Heap* appHeap);
  //! @brief  パーソナルデータソートクラスのインスタンス破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyPokePersonalSort(void);

  //! @brief  パーソナルデータソートクラスのインスタンスを取得する
  //! @return  生成できていないときはNULLを返す
  PokeTool::PersonalSort* GetPokePersonalSort(void) const;
  //@}


  //! @name  メッセージ
  //@{
  //! @brief  メッセージタイプ
  enum MsgType
  {
    MSG_TYPE_ZUKAN       ,
    MSG_TYPE_POKE_NAME   ,
    MSG_TYPE_POKE_SPECIES,
    MSG_TYPE_POKE_FORM   ,
    MSG_TYPE_POKE_HEIGHT ,
    MSG_TYPE_POKE_WEIGHT ,
    MSG_TYPE_POKE_COMMENT,
    MSG_TYPE_INITIAL     ,
    MSG_TYPE_ZUKAN_HYOUKA,
    MSG_TYPE_NUM,
    MSG_TYPE_NUM_FOR_INFO = MSG_TYPE_INITIAL,  // 情報画面ではINITIALとZUKAN_HYOUKAは使わない
  };
  //! @brief  登録バッファIDのタイプ
  enum RegBufType
  {
    REG_BUF_TYPE_NONE,            //!< なし
    REG_BUF_TYPE_POKE_NICK_NAME,  //!< ポケモンにユーザが付けたニックネーム
    REG_BUF_TYPE_POKE_MONS_NAME,  //!< ポケモンの種族名(ピカチュウとか)
    REG_BUF_TYPE_PLACE_NAME,      //!< 地名
    REG_BUF_TYPE_PLAYER_NAME,    //!< プレイヤー名
    REG_BUF_TYPE_PLAYER_NICKNAME,//!< プレイヤーニックネーム
    REG_BUF_TYPE_ITEM_NAME,       //!< 道具名
  };

  //! @brief  メッセージシステム生成
  //! @param[in]  appHeap  ヒープ。
  //! @return  生成できたらtrueを返す
  bool CreateMsgSys(app::util::Heap* appHeap);
  //! @brief  メッセージシステム破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyMsgSys(void);

  print::WordSet*    GetWordSet(void) const;
  //gfl2::str::StrBuf* GetTempStrBuf(u32 index) const;  // 内部で使うので、外部からGetさせてはならない。

  void CreateMsgData(MsgType msgType, const void* buf, gfl2::heap::HeapBase* heap, ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT);
  void DestroyMsgData(MsgType msgType, ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT);
  gfl2::str::MsgData* GetMsgData(MsgType msgType, ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT) const;

  //! @brief  ExpandStrBufの引数
  class ExpandStrBufParam
  {
  public:
    gfl2::str::StrBuf*               dstStrBuf;          //!< [out]  生成した文字列を入れて返す文字列バッファ
    MsgType                          srcMsgType;         //!< [in]   MsgType
    u32                              srcMsgStrId;        //!< [in]   メッセージスタジオに書いたラベル
    RegBufType                       srcRegBufType[3];   //!< [in]   登録バッファID=iのタイプ
    const pml::pokepara::CoreParam*  srcCP[3];           //!< [in]   pml::pokepara。REG_BUF_TYPE_POKE_NICK_NAME, REG_BUF_TYPE_POKE_MONS_NAMEのときに使う。
    u32                              srcPlaceNameID[3];  //!< [in]   REG_BUF_TYPE_PLACE_NAMEのときに使う。
    u32                              srcItemID[3];       //!< [in]   REG_BUF_TYPE_ITEM_NAMEのときに使う。
    ZukanType::ZukanLang             zukanLang;          //!< [in]   言語                                                                                 
  public:
    ExpandStrBufParam(void)
      : dstStrBuf(NULL),
        srcMsgType(MSG_TYPE_ZUKAN),
        srcMsgStrId(0),
        zukanLang(ZukanType::ZUKAN_LANG_DEFAULT)
    {
      for(u32 regBufId=0; regBufId<3; ++regBufId)
      {
        srcRegBufType[regBufId]  = REG_BUF_TYPE_NONE;
        srcCP[regBufId]          = NULL;
        srcPlaceNameID[regBufId] = 0;
        srcItemID[regBufId]      = 0;
      }
    }
  };

  //! @brief  文字列を生成する
  void ExpandStrBuf(const ExpandStrBufParam& param);
  //@}
  

  //! @name  2ボタンビュー
  //@{
  //! @brief  2ボタンビュー生成
  //! @param[in]  appHeap         ヒープ。
  //! @param[in]  menuWindow2Buf  ARCID_MENU_WINDOW2のGARC_MenuWindow2_MenuWindow2_applyt_COMPと同等のものを読み込み済みのバッファ。
  //! @return  生成できたらtrueを返す
  bool CreateTwoButtonsView(app::util::Heap* appHeap, void* menuWindow2Buf);
  //! @brief  2ボタンビュー破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyTwoButtonsView(void);

  //! @brief  2ボタンビューをオープンする
  void OpenTwoButtonsView(
      app::ui::UIView*                          parentView,
      gfl2::str::StrBuf*                        strBuf1,
      gfl2::str::StrBuf*                        strBuf2,
      bool                                      button2UseBButton,
      app::tool::TwoButtonsView::IEventHandler* eventHandler
  );
  //! @brief  2ボタンビューをクローズする
  void CloseTwoButtonsView(void);
  //@}


  //! @name  マルチボタンビュー(GeneralLowerView, 共通下画面View)
  //@{
  //! @brief  マルチボタンビュー生成
  //! @param[in]  appHeap         ヒープ。
  //! @param[in]  menuWindow2Buf  ARCID_MENU_WINDOW2のGARC_MenuWindow2_MenuWindow2_applyt_COMPと同等のものを読み込み済みのバッファ。
  //! @return  生成できたらtrueを返す
  bool CreateMultiButtonsView(app::util::Heap* appHeap, void* menuWindow2Buf);
  //! @brief  マルチボタンビュー破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyMultiButtonsView(void);

  //! @brief  マルチボタンビューをオープンする
  void OpenMultiButtonsView(
      app::ui::UIView*                            parentView,
      gfl2::str::StrBuf*                          strBuf0,
      gfl2::str::StrBuf*                          strBuf1,
      gfl2::str::StrBuf*                          strBuf2,
      gfl2::str::StrBuf*                          strBuf3,
      bool                                        button3UseBButton,
      bool                                        button1UseBButtonDecideSE,
      app::tool::GeneralLowerView::IEventHandler* eventHandler,
      s32                                         cursorPos=0
  );
  //! @brief  マルチボタンビューをクローズする
  void CloseMultiButtonsView(void);
  //@}


  //! @name  フォント
  //@{
  enum FontType
  {
    FONT_TYPE_NUM_FONT,
    FONT_TYPE_MAX,
  };
  bool CreateFont(app::util::Heap* appHeap, FontType fontType);
  bool DestroyFont(FontType fontType);
  //@}


  //! @name  共通グラフィックアイコン
  //@{
  //! @brief  共通グラフィックアイコンシステム生成
  //! @param[in]  appHeap  ヒープ。
  //! @return  生成できたらtrueを返す
  bool CreateCommonGrpIcon(app::util::Heap* appHeap);
  //! @brief  共通グラフィックアイコンシステム破棄
  //! @return  破棄できたらtrueを返す
  bool DestroyCommonGrpIcon(void);

  //! @brief  タイプアイコンのテクスチャを貼り替える(タイプアイコンとはくさ、ほのお、みずとかのこと)
  //! @caution  ペインの表示ON/OFFも変更しているので注意！
  //! @note  pml::PokeType  (poke_lib/pml/include/pml_Type.h)
  //! @note  PokeType_tag  (poke_lib/pml/include/poketype_def.h)
  void ReplaceTypeIconTextureByCommonGrpIcon(
      gfl2::lyt::LytPicture* pane, PokeType_tag type,
      ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT
  ) const;
  //! @note  type1とtype2が同じ場合はpane2を非表示にする
  //! @note  u32  (pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type1))  (poke_lib/pml/include/personal/pml_PersonalSystem.h)
  void ReplaceTypeIconTextureByCommonGrpIcon(
      gfl2::lyt::LytPicture* pane1, PokeType_tag type1, 
      gfl2::lyt::LytPicture* pane2, PokeType_tag type2,
      ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT
  ) const;

  //! @brief  言語アイコンのテクスチャを貼り替える(言語アイコンとはJPN、ENG、FRAとかのこと)
  //! @param[in]  pane         テクスチャを貼り替えるピクチャペイン
  //! @param[in]  pokemonLang  u32  (#define POKEMON_LANG_NULL  #define POKEMON_LANG_JAPAN  #define POKEMON_LANG_ENGLISH)  (niji_project/prog/System/include/PokemonVersion.h)
  //!                          これはポケモンに埋め込んでいる言語コードである。
  //!
  //! @note  nijiで言語コードは次のようになりました。
  //!        PokemonVersionの言語コード                    (ポケモンに埋め込む言語コード)    R:/home/momiji/git_program/niji_project/prog/System/include/PokemonVersion.h   #define POKEMON_LANG_NULL  #define POKEMON_LANG_JAPAN  ...
  //!        PokemonLanguageのMSGLANGIDの言語コード        (メッセージスタジオの言語コード)  R:/home/momiji/git_program/niji_project/prog/System/include/PokemonLanguage.h  enum MSGLANGID{ MSGLANGID_KANA, MSGLANGID_KANJI, MSGLANGID_ENGLISH, ... };
  //!        PokemonLanguageのCfgLanguageCodeの言語コード  (CTR(3DS)本体の言語コード)        R:/home/momiji/git_program/niji_project/prog/System/include/PokemonLanguage.h  enum CfgLanguageCode{ LANGUAGE_JAPAN = nn::cfg::CTR::CFG_LANGUAGE_JAPANESE, ... };
  void ReplaceLangIconTextureByCommonGrpIcon(
      gfl2::lyt::LytPicture* pane, u32 pokemonLang,
      ZukanType::ZukanLang zukanLang = ZukanType::ZUKAN_LANG_DEFAULT
  ) const;
  //@}


private:
  GameSys::GameManager*           m_gameManager;
  app::util::AppRenderingManager* m_appRenderingManager;
  ZukanType::ZukanLang            m_gameLang;  // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。

  //! @name  会話用ウィンドウ
  //@{
  class TalkWindowState
  {
  public:
    enum Tag
    {
      NONE,
      WORKING,
    };
  };
  TalkWindowState::Tag m_talkWindowState;
  App::Tool::TalkWindow* m_talkWindow;
  bool m_talkWindowIsOpen;
  //@}

  //! @name  メニューカーソル
  //@{
  class MenuCursorState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
      DESTROYING,
    };
  };
  MenuCursorState::Tag m_menuCursorState;
  app::tool::MenuCursor* m_menuCursor;
  void* m_menuCursorBuf;
  //@}
  
  //! @name  メッセージウィンドウ＆リストメニュー
  //@{
  class MessageMenuViewState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
      DESTROYING,
    };
  };
  MessageMenuViewState::Tag          m_messageMenuViewState;
  App::Tool::MessageMenuView*        m_messageMenuView;
  App::Tool::MessageMenuView::PARAM* m_messageMenuViewParam;
  //@}

  //! @name  背景
  //@{
  ZukanViewBg* m_viewBg;
  //@}

  //! @name  図鑑の登録画面に出すフェード層のビュー
  //@{
  ZukanViewRegisterFade* m_viewRegisterFade;
  //@}

  //! @name  ポケモンモデル
  //@{
  class PokeModelSysState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
      DESTROYING_MODEL,
      DESTROYING_SYS,
    };
  };

  static const s32 POKE_MODEL_INFRAME_CENTER[POKE_MODEL_POS_TYPE_NUM][2];  // [0]=x; [1]=y;
  static const s32 POKE_MODEL_INFRAME_LENGTH[POKE_MODEL_SIZE_TYPE_NUM];
  static const f32 POKE_MODEL_INFRAME_SCALE_DEFAULT[POKE_MODEL_SCALE_TYPE_NUM];
  static const f32 POKE_MODEL_INFRAME_SCALE_SMALL_DEFAULT[POKE_MODEL_SCALE_TYPE_NUM];
  static const f32 POKE_MODEL_INFRAME_SCALE_MAX[POKE_MODEL_SCALE_TYPE_NUM];
  static const f32 POKE_MODEL_INFRAME_SCALE_MIN[POKE_MODEL_SCALE_TYPE_NUM];
  static const f32 POKE_MODEL_INFRAME_PITCH_MIN;

  PokeModelSysState::Tag m_pokeModelSysState;
  PokeTool::PokeModelSystem*  m_pokeModelSys;
  app::tool::PokeSimpleModelInFrame* m_pokeSimpleModelInFrame;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*     m_drawEnvNodeOfCamera;
  poke_3d::model::BaseCamera*                                   m_camera;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;
  PokeTool::SimpleParam  m_pokeModelSp;
  bool  m_pokeModelVoiceFlag;
  bool  m_currPokeModelIsReady;
  bool  m_currPokeModelIsVisible;
  bool  m_pokeModelPauseFlag;
  u32   m_pokeModelAnimeIndex;
  s32                m_pokeModelCurrInFrameCenterX;   // 現在の位置x
  s32                m_pokeModelCurrInFrameCenterY;   // 現在の位置y
  s32                m_pokeModelCurrInFrameLength;    // 1辺の長さ
  PokeModelSizeType  m_pokeModelCurrInFrameSizeType;  // サイズタイプ
  f32                m_pokeModelCurrInFrameScale;     // 現在のスケール
  s32                m_pokeModelCurrInFrameAddSize;   // 基本の大きさに加算する値
  f32  m_pokeModelInFrameScaleDefault;
  f32  m_pokeModelInFrameScaleMax;
  f32  m_pokeModelInFrameScaleMin;
  PokeModelPosType       m_pokeModelGoalPosType;       // 目的の位置
  bool                   m_pokeModelPosAnimFlag;       // 位置アニメ中(falseのときm_pokeModelGoalPosTypeの位置にいる)
  //@}


  //! @name  パーソナル外ポケモンデータ
  //@{
  //PokeTool::ExtendData* m_pokeExtendData;
  //@}
  //! @name  パーソナルデータソートクラス
  //@{
  PokeTool::PersonalSort* m_pokePersonalSort;
  //@}


  //! @name  メッセージ
  //@{
  class MsgSysState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
    };
  };
  enum
  {
    TEMP_STR_BUF_NUM          = 2,
    TEMP_STR_BUF_CHAR_NUM_MAX = 512,
    STR_BUF_FOR_TEXT_BOX_PANE_NUM          = 2,
    STR_BUF_FOR_TEXT_BOX_PANE_CHAR_NUM_MAX = 512,
  };
  MsgSysState::Tag    m_msgSysState;
  WordSetLoader*      m_wordSetLoader;
  print::WordSet*     m_wordSet;
  gfl2::str::StrBuf*  m_tempStrBuf[TEMP_STR_BUF_NUM];  // 内部で使うので、外部からGetさせてはならない。
  gfl2::str::MsgData* m_msgData[MSG_TYPE_NUM][ZukanType::ZUKAN_LANG_NUM];
  //@}

  //! @name  2ボタンビュー
  //@{
  class TwoButtonsViewState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
    };
  };
  TwoButtonsViewState::Tag    m_twoButtonsViewState;
  app::tool::TwoButtonsView*  m_twoButtonsView;
  //@}

  //! @name  マルチボタンビュー(GeneralLowerView, 共通下画面View)
  //@{
  class MultiButtonsViewState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
    };
  };
  class MultiButtonsViewResourceProvider : public app::tool::IResourceProvider
  {
  GFL_FORBID_COPY_AND_ASSIGN(MultiButtonsViewResourceProvider);
  public:
    MultiButtonsViewResourceProvider(void);
    virtual ~MultiButtonsViewResourceProvider() {}
  public:
    virtual void* IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID); 
  public:
    void* m_menuWindow2Buf;
    void* m_menuCursorBuf;
  };
  MultiButtonsViewState::Tag        m_multiButtonsViewState;
  app::tool::GeneralLowerView*      m_multiButtonsView;
  MultiButtonsViewResourceProvider* m_multiButtonsViewResourceProvider;
  //@}

  //! @name  フォント
  //@{
  class FontState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
      ALREADY_EXISTING,  // 他のところで生成されて稼働中
    };
  };
  FontState::Tag    m_fontState[FONT_TYPE_MAX];
  //@}

  //! @name  共通グラフィックアイコン
  //@{
  class CommonGrpIconState
  {
  public:
    enum Tag
    {
      NONE,
      CREATING,
      WORKING,
    };
  };
  CommonGrpIconState::Tag           m_commonGrpIconState;
  ZukanType::ZukanLang              m_commonGrpIconLang;  // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // 今処理中の言語。全ての言語の処理が終わったらZUKAN_LANG_NUMを入れておく。
  app::tool::AppCommonGrpIconData*  m_commonGrpIcon[ZukanType::ZUKAN_LANG_NUM];
  //@}
 

public:
  //! @name  言語別MsgData
  //@{
  class LangMsgDataManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(LangMsgDataManager);
  public:
    LangMsgDataManager(gfl2::heap::HeapBase* heap, ZukanType::ZukanLang gameLang, ZukanTool* tool);
    virtual ~LangMsgDataManager();
    void UpdateCreate(void);  // 1フレームに1回ではなく、1フレームに何回も呼んでもいい。
    bool IsCreated(void) const;
  private:
    gfl2::heap::HeapBase* m_heap;
    ZukanType::ZukanLang  m_gameLang;
    ZukanTool*            m_tool;
    s32                   m_createStep;
  };
  //@}
  
  //! @name  言語別Font
  //@{
  class LangFontManager
  {
    GFL_FORBID_COPY_AND_ASSIGN(LangFontManager);
  public:
    enum LangFontType
    {
      LANG_FONT_TYPE_JAPAN,
      LANG_FONT_TYPE_CHINA,
      LANG_FONT_TYPE_TAIWAN,
      LANG_FONT_TYPE_NUM,
    };
  public:
    LangFontManager(gfl2::heap::HeapBase* heap, ZukanType::ZukanLang gameLang, ZukanDataManager* dataManager);
    virtual ~LangFontManager();
    void UpdateCreate(void);  // 1フレームに1回ではなく、1フレームに何回も呼んでもいい。
    bool IsCreated(void) const;
    gfl2::str::Font* GetLangFont(LangFontType langFontType) const;
  private:
    gfl2::heap::HeapBase* m_heap;
    ZukanType::ZukanLang  m_gameLang;
    ZukanDataManager*     m_dataManager;
    s32                   m_createStep;
    gfl2::str::Font*      m_langFont[LANG_FONT_TYPE_NUM];
  };
  //@}


  //! @name  言語別MsgData
  //@{
  LangMsgDataManager*  m_langMsgDataManager;
  void CreateLangMsgDataManager(app::util::Heap* appHeapForInstance, gfl2::heap::HeapBase* heapForManager);
  void DestroyLangMsgDataManager(void);
  LangMsgDataManager* GetLangMsgDataManager(void) const { return m_langMsgDataManager; }
  //@}

  //! @name  言語別Font
  //@{
  LangFontManager*  m_langFontManager;
  void CreateLangFontManager(app::util::Heap* appHeapForInstance, gfl2::heap::HeapBase* heapForManager, ZukanDataManager* dataManager);
  void DestroyLangFontManager(void);
  LangFontManager* GetLangFontManager(void) const { return m_langFontManager; }
  //@}


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  //! @name  デバッグ用インスタンス
  //@{
public:
  void CreateDebugInstance(gfl2::heap::HeapBase* heap);
  void DestroyDebugInstance(void);
private:
  gfl2::str::StrBuf* m_monsNameStrBuf;
  //@}
#endif
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANTOOL_H_INCLUDED

