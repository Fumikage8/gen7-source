#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANTYPE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANTYPE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanType.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.01
 @brief   図鑑のタイプ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// pmlibのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include "PokeTool/include/PokeToolExtendData.h"
#include "AppLib/include/Tool/app_tool_ButtonManager.h"

// 図鑑のインクルード
#include "ZukanDefine.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanType
 @brief     図鑑のタイプ
 */
//==============================================================================
class ZukanType
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanType);


public:
  ZukanType(void) {}
  virtual ~ZukanType() {}


public:
  //! @brief  ボタンの定数情報
  struct ButtonConstInfo
  {
    u32                                    button_id;
    u32                                    parts_pane_index;
    u32                                    main_pane_index;
    u32                                    bound_pane_index;
    u32                                    cursor_pane_index;
    u32                                    touch_anime_index;
    u32                                    release_anime_index;
    u32                                    cancel_anime_index;
    u32                                    key_select_anime_index;
    u32                                    active_anime_index;
    u32                                    passive_anime_index;
    u32                                    other1_anime_index;
    u32                                    other2_anime_index;
    u32                                    other3_anime_index;
    bool                                   bind_key_flag;
    u32                                    bind_key;
    app::tool::ButtonManager::BindKeyMode  bind_key_mode;
    u32                                    se_id;
  };

  //! @brief  ボタンのペインポインタ情報
  class ButtonPaneInfo
  {
  public:
    gfl2::lyt::LytParts* partsPane;
    gfl2::lyt::LytPane*  boundPane;
    gfl2::lyt::LytPane*  cursorPane;
    ButtonPaneInfo(void)
      : partsPane(NULL),
        boundPane(NULL),
        cursorPane(NULL)
    {}
  };

  //! @brief  ペインリストの定数情報
  struct PaneListConstInfo
  {
    u32 listPosMax;
    f32 defPY;
    f32 defSX;
    f32 defSY;
    s32 writeOffsetY;
  };

  //! @brief  ペインリスト要素の定数情報
  struct PaneListElemConstInfo
  {
    u32 partsPaneIndex;
    u32 textPaneIndex;
    u32 boundPaneIndex;
    u32 cursorPaneIndex;
    u32 holdAnime;
    u32 releaseAnime;
    u32 cancelAnime;
    u32 selectAnime;
    u32 other1Anime;
    u32 other2Anime;
    u32 other3Anime;
  };

  //! @brief  スクロールバーの定数情報
  struct ScrollBarConstInfo
  {
    gfl2::lyt::LytPaneIndex parts_index;
    gfl2::lyt::LytPaneIndex mark_index;
    gfl2::lyt::LytPaneIndex top_index;
    gfl2::lyt::LytPaneIndex bottom_index;
  };

public:
  //! @brief  カーソル座標
  class CursorVector
  {
  public:
    s32 x;  // 計算過程で一時的に負にも成り得るのでs32で。
    s32 y;
  public:
    CursorVector(void)
      : x(0),
        y(0)
    {}
    CursorVector(s32 a_x, s32 a_y)
    {
      x = a_x;
      y = a_y;
    }
  };

  //! @brief  ペインリストの位置の情報
  class PaneListPosInfo
  {
  public:
    u32 pos;         // リストの先頭から(画面外も含む)何番目のペインをカーソルが指しているか(0開始)
    u32 listPos;     // 画面内に見えているうちで何番目のペインをカーソルが指しているか(画面内の一番上のペインを0とする)
    f32 listScroll;  // 画面はどれくらいスクロールして今の状態になっているか(0.0f開始)
  public:
    PaneListPosInfo(void)
      : pos(0),
        listPos(0),
        listScroll(0.0f)
    {}
  };


public:
  //! @brief  シーン(メインフローとなるものをシーンと呼ぶことにする)
  enum SceneType
  {
    // なし
    SCENE_TYPE_NONE,  //!< なし
 
    // シーン
    SCENE_TYPE_LAUNCH,              //!< 起動画面
    SCENE_TYPE_ALOLA_TOP,           //!< アローラ図鑑トップ画面
    SCENE_TYPE_ISLAND_TOP,          //!< 島図鑑トップ画面
    SCENE_TYPE_INFO,                //!< 情報画面
    SCENE_TYPE_REGISTER_INTRO,      //!< 図鑑の登録の際の導入画面
    SCENE_TYPE_REGISTER_ALOLA_TOP,  //!< 登録の際のアローラ図鑑トップ画面
    SCENE_TYPE_REGISTER,            //!< 登録画面
    SCENE_TYPE_MAP,                 //!< 分布画面
    //SCENE_TYPE_DISCOVER,            //!< 発見リスト画面
    SCENE_TYPE_SEARCH,              //!< 検索画面
    SCENE_TYPE_POKE_NAME,           //!< 名前入力画面
    SCENE_TYPE_POKE_PUT,            //!< 送り先選択画面
 
    // 制御
    SCENE_TYPE_WAIT_NEXT,               //!< 次のシーンを決めるのを待つ
    SCENE_TYPE_ZUKAN_REGISTER_CHECKER,  //!< 図鑑登録チェッカー
    
    // 図鑑外
    SCENE_TYPE_START_ZUKAN,                             //!< 図鑑開始
    SCENE_TYPE_START_ZUKAN_REGISTER,                    //!< 図鑑登録開始
    SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_LIST,    //!< 図鑑登録開始(ポケモンリストの後に)
    SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_STATUS,  //!< 図鑑登録開始(ポケモンステータスの後に)
    SCENE_TYPE_END,                                     //!< 終了
    SCENE_TYPE_NEXT_POKE_LIST,                          //!< 次にポケモンリストに行って欲しい
    SCENE_TYPE_NEXT_POKE_STATUS,                        //!< 次にポケモンステータスに行って欲しい
  };


  //! @brief  図鑑で扱う言語
  enum ZukanLang
  {
    // この順番を使用している箇所があるので並び替えないように
    ZUKAN_LANG_JAPAN   ,
    ZUKAN_LANG_ENGLISH ,
    ZUKAN_LANG_FRANCE  ,
    ZUKAN_LANG_ITALY   ,
    ZUKAN_LANG_GERMANY ,
    ZUKAN_LANG_SPAIN   ,
    ZUKAN_LANG_KOREA   ,
    ZUKAN_LANG_CHINA   ,
    ZUKAN_LANG_TAIWAN  ,
    ZUKAN_LANG_NUM     ,
    ZUKAN_LANG_DEFAULT = ZUKAN_LANG_NUM   ,
    ZUKAN_LANG_DUMMY   = ZUKAN_LANG_JAPAN ,
  };
  static u8 ConvertZukanLang2PokemonLang(ZukanLang zukanLang);
  static ZukanLang ConvertPokemonLang2ZukanLang(u8 pokemonLang);


  //! @name  検索関連
  //@{
  
  //! @brief  題目
  enum Subject
  {
    // この順番を使用している箇所があるので並び替えないように
    SUBJECT_ORDER,      //!< 並び
    SUBJECT_INITIAL,    //!< 頭文字
    SUBJECT_POKETYPE1,  //!< タイプ１
    SUBJECT_POKETYPE2,  //!< タイプ２
    SUBJECT_COLOR,      //!< 色
    SUBJECT_STYLE,      //!< 形
    SUBJECT_FIGURE,     //!< 容姿
    SUBJECT_GET,        //!< 捕獲状況
    SUBJECT_NUM,
  };

  //! @brief  並び
  enum OrderType
  {
    // この順番を使用している箇所があるので並び替えないように
    ORDER_TYPE_NO,
    ORDER_TYPE_NAME,
    ORDER_TYPE_HEAVY,  //!< 重い→軽い
    ORDER_TYPE_LIGHT,  //!< 軽い→重い
    ORDER_TYPE_TALL,   //!< 高い→低い
    ORDER_TYPE_SHORT,  //!< 低い→高い
    ORDER_TYPE_NUM,  // 並びに未選択はない
    ORDER_TYPE_DEFAULT  = ORDER_TYPE_NO
  };

  //! @brief  頭文字
  enum InitialType
  {
    // この順番を使用している箇所があるので並び替えないように
    INITIAL_TYPE_NUM_MAX       = 50,                    //!< 全言語で最大の頭文字数以上の値
    INITIAL_TYPE_NONE          = INITIAL_TYPE_NUM_MAX,
    INITIAL_TYPE_ELEM_NUM_MAX,
    INITIAL_TYPE_DEFAULT       = INITIAL_TYPE_NONE
  };

  //! @brief  タイプ
  //! @note  pml::PokeType  (poke_lib/pml/include/pml_Type.h)
  //! @note  PokeType_tag  (poke_lib/pml/include/poketype_def.h)
  enum PoketypeType
  {
    // この順番を使用している箇所があるので並び替えないように
    POKETYPE_TYPE_NUM       = POKETYPE_MAX,       //!< ノーマル～フェアリーの18タイプ  // git_program/poke_lib/pml/include/poketype_def.h
    POKETYPE_TYPE_NONE      = POKETYPE_TYPE_NUM,  //!< 未選択
    POKETYPE_TYPE_ELEM_NUM,                       //!< リスト要素数(個数+未選択)
    POKETYPE_TYPE_DEFAULT   = POKETYPE_TYPE_NONE  //!< デフォルト値
  };

  //! @brief  色
  //! @note  pml::personal::Color  (poke_lib/pml/include/personal/pml_PersonalConst.h)
  enum ColorType
  {
    // この順番を使用している箇所があるので並び替えないように
    COLOR_TYPE_NUM       = pml::personal::COLOR_MAX,  // 赤～桃の10タイプ  // git_program/poke_lib/pml/include/personal/pml_PersonalConst.h
    COLOR_TYPE_NONE      = COLOR_TYPE_NUM,
    COLOR_TYPE_ELEM_NUM,
    COLOR_TYPE_DEFAULT   = COLOR_TYPE_NONE
  };
 
  //! @brief  形
  enum StyleType
  {
    // この順番を使用している箇所があるので並び替えないように
    STYLE_TYPE_NUM       = PokeTool::ExtendData::STYLE_TYPE_MAX,  // 丸型～腕型の14タイプ  // git_program/niji_project/prog/PokeTool/include/PokeToolExtendData.h
    STYLE_TYPE_NONE      = STYLE_TYPE_NUM,
    STYLE_TYPE_ELEM_NUM,
    STYLE_TYPE_DEFAULT   = STYLE_TYPE_NONE
  };

  //!< @brief  容姿
  enum FigureType
  {
    // この順番を使用している箇所があるので並び替えないように
    FIGURE_TYPE_ALOLA,                         //!< アローラ
    FIGURE_TYPE_MEGA,                          //!< メガシンカ
    FIGURE_TYPE_RARE,                          //!< いろちがい
    FIGURE_TYPE_NUM,                           //!< 個数
    FIGURE_TYPE_NONE       = FIGURE_TYPE_NUM,  //!< 未選択
    FIGURE_TYPE_ELEM_NUM,                      //!< リスト要素数(個数+未選択)
    FIGURE_TYPE_DEFAULT    = FIGURE_TYPE_NONE  //!< デフォルト値
  };

  //!< @brief  捕獲状況
  enum GetType
  {
    // この順番を使用している箇所があるので並び替えないように
    GET_TYPE_GET,
    GET_TYPE_SEE,
    GET_TYPE_NUM,
    GET_TYPE_NONE      = GET_TYPE_NUM,
    GET_TYPE_ELEM_NUM,
    GET_TYPE_DEFAULT   = GET_TYPE_NONE,
  };

  //!< @brief  頭文字数を得る
  static u32 GetInitialTypeNum(ZukanLang zukanLang);
  //!< @brief  頭文字のリスト要素数(頭文字数+未選択)を得る
  static u32 GetInitialTypeElemNum(ZukanLang zukanLang);

  //!< @brief  位置をリセットする
  //!< @param[out]  paneListPosInfo  paneListPosInfo[SUBJECT_NUM]
  static void ResetSubjectPaneListPosInfo(PaneListPosInfo* paneListPosInfo);
  //!< @brief  位置がリセットされているか
  //!< @param[in]  paneListPosInfo  paneListPosInfo[SUBJECT_NUM]
  static bool IsSubjectPaneListPosInfoReset(const PaneListPosInfo* paneListPosInfo);

  //!< @brief  位置から値を得る
  static ZukanType::Subject      GetSubjectFromPos(u32 pos);
  static ZukanType::OrderType    GetOrderTypeFromPos(u32 pos);
  static ZukanType::InitialType  GetInitialTypeFromPos(u32 pos, ZukanLang zukanLang);
  static ZukanType::PoketypeType GetPoketypeTypeFromPos(u32 pos);
  static ZukanType::ColorType    GetColorTypeFromPos(u32 pos);
  static ZukanType::StyleType    GetStyleTypeFromPos(u32 pos);
  static ZukanType::FigureType   GetFigureTypeFromPos(u32 pos);
  static ZukanType::GetType      GetGetTypeFromPos(u32 pos);

  //@}  // 検索関連


  //! @brief  情報画面の種類(図鑑の種類(アローラ図鑑、島図鑑))
  enum InfoType
  {
    // この順番、この番号を使用している箇所があるので並びや値を替えないように！
    INFO_TYPE_ALOLA    = 0,
    INFO_TYPE_ISLAND1,
    INFO_TYPE_ISLAND2,
    INFO_TYPE_ISLAND3,
    INFO_TYPE_ISLAND4,
    INFO_TYPE_NUM    ,

    INFO_TYPE_ISLAND_START = INFO_TYPE_ISLAND1,  // START<= <=END
    INFO_TYPE_ISLAND_END   = INFO_TYPE_ISLAND4,
    INFO_TYPE_ISLAND_NUM   = INFO_TYPE_ISLAND_END - INFO_TYPE_ISLAND_START +1,
  };


  //! @brief  色
  static const nw::ut::Color8 POKE_GET_COLOR;
  static const nw::ut::Color8 POKE_SEE_COLOR;

  static const gfl2::math::Vector4 CROSS_FADE_COLOR_START;
  static const gfl2::math::Vector4 CROSS_FADE_COLOR_END;


  //! @brief  伝説レベル
  // ZukanPicScrollDataAccessor::GradeVal(GRADE_VAL_...)と同じ値ではない。ZukanPicScrollDataAccessor::GradeValはQUASI_LEGENDとUBとティキを同一にしているので。
  static const u32 REGISTER_LEGEND_LEVEL_COMMON       = 0;  // 0=普通; 1=準伝; 2=伝説; 3=UB; 4=ティキ; 5=幻;
  static const u32 REGISTER_LEGEND_LEVEL_QUASI_LEGEND = 1;
  static const u32 REGISTER_LEGEND_LEVEL_LEGEND       = 2;
  static const u32 REGISTER_LEGEND_LEVEL_UB           = 3;
  static const u32 REGISTER_LEGEND_LEVEL_TIKI         = 4;  // @fix NMCat[1107] タイプ：ヌル、シルヴァディの図鑑表示についての質問  →準伝説とティキを分けたので、ティキがUBのような扱いになった。
  static const u32 REGISTER_LEGEND_LEVEL_MABOROSHI    = 5;  // 幻  // @fix GFNMCat[3983] まぼろしポケモン用の評価コメントが表示されない  →まぼろしを伝説と分ける。


  //! @brief  セットアップ状況
  enum SetupState
  {
    // この順番を使用している箇所があるので並びを変えないように！（これ以上だったらここまでできている、という判定をしている）
    SETUP_STATE_NONE,            //!< 何もセットアップしていない
    SETUP_STATE_ONE_CREATING,    //!< 最初の1個を出せるような生成を行っている最中
    SETUP_STATE_ALL_CREATING,    //!< 全てを出せるような生成を行っている最中
    SETUP_STATE_ALL_CREATING_2,  //!< 全てを出せるような生成を行っている最中その２
    SETUP_STATE_ALL_FINISHED,    //!< 全てセットアップ済み
  };


private:
  static const u8 ZUKAN_LANG_2_POKEMON_LANG[ZUKAN_LANG_NUM];
  enum
  {
    POKEMON_LANG_NUM = 11  // POKEMON_LANG_???の最大値+1  // git_program/niji_project/prog/System/include/PokemonVersion.h
  };
  static const ZukanLang POKEMON_LANG_2_ZUKAN_LANG[POKEMON_LANG_NUM];


  static const u16 INITIAL_TYPE_NUM[ZUKAN_LANG_NUM];


};


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
//! @brief  図鑑デバッグ情報
class ZukanDebugInfo
{
public:
  static const u32 TIME_NUM_MAX = 8;
  enum
  {
    TIME_PHASE_ZUKAN_PROC_INIT_FUNC_START,
    TIME_PHASE_ZUKAN_LAUNCH_FADE_IN_START,
    TIME_PHASE_MAX  = 8,
  };
public:
  s64 time[TIME_NUM_MAX];  // [micro second]
public:
  ZukanDebugInfo(void);
  virtual ~ZukanDebugInfo() {}
  void Init(void);
  void SetCurrTime(u32 index);
  void Print(void);
};
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANTYPE_H_INCLUDED

