//=============================================================================
/**
 * @file PokeMakeCore.cpp
 * @brief デバッグポケモン作成
 * @author obata_toshihiro
 * @date 2011.07.11
 */
//=============================================================================
#if PM_DEBUG

// gflib2
#include <str/include/gfl2_Str.h>
#include <Fade/include/gfl2_FadeManager.h>

#include <System/include/GflUse.h>
#include <System/include/PokemonVersion.h>
#include <System/include/PokemonLanguage.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/MyStatus.h>
#include <GameSys/include/DllProc.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>
#include <PokeTool/include/PokeToolPokePlace.h>

#include <Debug/DebugKeyboard/include/DebugKeyboard.h>
#include <Debug/DebugNumInput/include/DebugNumInput.h>
#include <Debug/DebugPokeMake/include/DebugPokeMake.h>
#include "./DebugPokeMakeCore.h"

#include <arc_def_index/arc_def.h>

#include <arc_index/message.gaix>
#include <arc_index/debug_message.gaix>
#include <niji_conv_header/message/debug/msg_debug_poke_make.h>

#include <arc_index/debug_poke_make.gaix>
#include <niji_conv_header/app/debug/debug_poke_make/res2d/debug_poke_make.h>
#include <niji_conv_header/app/debug/debug_poke_make/res2d/debug_poke_make_anim_list.h>
#include <niji_conv_header/app/debug/debug_poke_make/res2d/debug_poke_make_pane.h>

#include <niji_conv_header/poke_lib/seikaku.h>


namespace debug {
namespace PokeMake {

  static const gfl2::str::STRCODE* SEIKAKU_NAME[ pml::pokepara::SEIKAKU_NUM ] =
  {
    L"がんばりや",
    L"さみしがり",
    L"ゆうかん",
    L"いじっぱり",
    L"やんちゃ",
    L"ずぶとい",
    L"すなお",
    L"のんき",
    L"わんぱく",
    L"のうてんき",
    L"おくびょう",
    L"せっかち",
    L"まじめ",
    L"ようき",
    L"むじゃき",
    L"ひかえめ",
    L"おっとり",
    L"れいせい",
    L"てれや",
    L"うっかりや",
    L"おだやか",
    L"おとなしい",
    L"なまいき",
    L"しんちょう",
    L"きまぐれ",
  };

  const gfl2::lyt::LytWkSetUp PokeMakeCore::LAYOUT_WORK_SETUP_DATA_UPPER =
  {
    gfl2::math::Vector3( 0, 0, 0 ),  // pos
    gfl2::math::Vector2( 1, 1 ),        // scale
    gfl2::math::Vector3( 0, 0, 0 ),     // rotate
    1,                              // priority
  };

  const gfl2::lyt::LytWkSetUp PokeMakeCore::LAYOUT_WORK_SETUP_DATA =
  {
    gfl2::math::Vector3( 0, -290, 0 ),  // pos
    gfl2::math::Vector2( 1, 1 ),        // scale
    gfl2::math::Vector3( 0, 0, 0 ),     // rotate
    1,                              // priority
  };

#if 0

  const gfl2::grp::ProjectionParam PokeMakeCore::PROJECTION_PARAM[2] =
  {
    // 上画面
    {
      gfl2::grp::PROJECTION_ORTHO,
      { -200.0f, 200.0f, -120.0f, 120.0f },
      0.10f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    },
    // 下画面
    {
      gfl2::grp::PROJECTION_ORTHO,
      { -160.0f, 160.0f, -120.0f, 120.0f },
      0.10f,
      300.0f,
      gfl2::math::PIVOT_UPSIDE_TO_TOP,
    },
  };


  const gfl2::grp::ViewParam PokeMakeCore::VIEW_PARAM[2] =
  {
    // 上画面
    {
      gfl2::math::VEC3( 0, 0, 100 ),  // カメラ座標
      gfl2::math::VEC3( 0, 1, 0 ),    // 上方向
      gfl2::math::VEC3( 0, 0, 0 ),    // ターゲット座標
    },
    // 下画面
    {
      gfl2::math::VEC3( 0, -290, 100 ),  // カメラ座標
      gfl2::math::VEC3( 0, 1, 0 ),       // 上方向
      gfl2::math::VEC3( 0, -290, 0 ),    // ターゲット座標
    },
  };


  const gfl2::grp::FragmentOperationParam PokeMakeCore::OPERATION_PARAM =
  {
    gfl2::grp::CULL_NONE,  // カリングモード（ENV_CULL〜）
    false,                // シザーテスト（true/false）
    {0},                  //
    false,                // アーリーデプステスト(true/false)
    false,                // ステンシルテスト(true/false)
    {0},                  //
    0.0f,                 // Wバッファ設定（0.0fで無効）
    0.0f,                 // デプスレンジ Near
    1.0f,                 // デプスレンジ Far
    0,                    // デプス値 分解最小値に対するスケール値
    false,                // デプステスト(true/false)
    0,                    // デプステスト比較関数(ENV_TEST_FUNC_〜)
    true,                 // デプスバッファへの書き込みが可能か(true/false)
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファへ赤を書き込むか（true/false）
    true,                 // カラーバッファの読み込みが可能か（true/false）
    true,                 // カラーバッファの書き込みが可能か（true/false）
    true,                 // デプスバッファの読み込みが可能か（true/false）
    true,                 // デプスバッファの書き込みが可能か（true/false）
    false,                // ステンシルバッファの読み込みが可能か（true/false）
    false,                // ステンシルバッファの書き込みが可能か（true/false）
  };

#endif

  // アニメーションデータ
  const gfl2::lyt::LytArcIndex PokeMakeCore::ANIME_DATA_TABLE[ ANIME_DATA_NUM ] =
  {
    /* ANIME_DATA_ARROW_HOLD */ LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_HOLD_BFLAN,
    /* ANIME_DATA_ARROW_RELEASE */ LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_RELEASE_BFLAN,
    /* ANIME_DATA_ARROW_SELECT */ LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_SELECT_BFLAN,
    /* ANIME_DATA_SWITCH_HOLD */ LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_HOLD_BFLAN,
    /* ANIME_DATA_SWITCH_RELEASE */ LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_RELEASE_BFLAN,
    /* ANIME_DATA_SWITCH_SELECT */ LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_SELECT_BFLAN,
    /* ANIME_DATA_STRBOX_HOLD */ LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_HOLD_BFLAN,
    /* ANIME_DATA_STRBOX_RELEASE */ LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_RELEASE_BFLAN,
    /* ANIME_DATA_STRBOX_SELECT */ LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_SELECT_BFLAN,
    /* ANIME_DATA_NUMBOX_HOLD */ LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_HOLD_BFLAN,
    /* ANIME_DATA_NUMBOX_RELEASE */ LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_RELEASE_BFLAN,
    /* ANIME_DATA_NUMBOX_SELECT */ LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_SELECT_BFLAN,
  };

  const gfl2::lyt::LytArcIndex PokeMakeCore::ANIME_DATA_TABLE_UPPER[ UPPER_ANIME_DATA_NUM ] =
  {
    /* ANIME_DATA_START_NUM_DISPLAY */ LYTRES_DEBUG_POKE_MAKE_SUB_START_NUM_DISPLAY_BFLAN,
    /* ANIME_DATA_INFO_SCROLL */ LYTRES_DEBUG_POKE_MAKE_SUB_INFO_SCROLL_BFLAN,
    /* ANIME_DATA_PAGE_INDEX */ LYTRES_DEBUG_POKE_MAKE_SUB_PAGE_INDEX_BFLAN,
  };

#if 0
  const app::util::LayoutContainer::ShareAnimeDesc PokeMakeCore::SHARE_ANIME_TABLE[ SHARE_ANIME_NUM ] =
  {
    /* SHARE_ANIME_ARROW_HOLD */
    {
      ANIME_ARROW_HOLD,
      ARROW_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_HOLD_BCLAN,
      PANENAME_PAGE1_GROUP_ARROW,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_ARROW_RELEASE */
    {
      ANIME_ARROW_RELEASE,
      ARROW_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_RELEASE_BCLAN,
      PANENAME_PAGE1_GROUP_ARROW,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_ARROW_SELECT */
    {
      ANIME_ARROW_SELECT,
      ARROW_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_ARROW_SELECT_BCLAN,
      PANENAME_PAGE1_GROUP_ARROW,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_SWITCH_HOLD */
    {
      ANIME_SWITCH_HOLD,
      SWITCH_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_HOLD_BCLAN,
      PANENAME_PAGE1_GROUP_BUTTON,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_SWITCH_RELEASE */
    {
      ANIME_SWITCH_RELEASE,
      SWITCH_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_RELEASE_BCLAN,
      PANENAME_PAGE1_GROUP_BUTTON,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_SWITCH_SELECT */
    {
      ANIME_SWITCH_SELECT,
      SWITCH_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_BUTTON_SELECT_BCLAN,
      PANENAME_PAGE1_GROUP_BUTTON,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_STRBOX_HOLD */
    {
      ANIME_STRBOX_HOLD,
      STRBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_HOLD_BCLAN,
      PANENAME_PAGE1_GROUP_STRING,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_STRBOX_RELEASE */
    {
      ANIME_STRBOX_RELEASE,
      STRBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_RELEASE_BCLAN,
      PANENAME_PAGE1_GROUP_STRING,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_STRBOX_SELECT */
    {
      ANIME_STRBOX_SELECT,
      STRBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_STRING_SELECT_BCLAN,
      PANENAME_PAGE1_GROUP_STRING,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_NUMBOX_HOLD */
    {
      ANIME_NUMBOX_HOLD,
      NUMBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_HOLD_BCLAN,
      PANENAME_PAGE1_GROUP_NUMBER,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_NUMBOX_RELEASE */
    {
      ANIME_NUMBOX_RELEASE,
      NUMBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_RELEASE_BCLAN,
      PANENAME_PAGE1_GROUP_NUMBER,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
    /* SHARE_ANIME_NUMBOX_SELECT */
    {
      ANIME_NUMBOX_SELECT,
      NUMBOX_NUM,
      LYTRES_DEBUG_POKE_MAKE_PAGE1_NUMBER_SELECT_BCLAN,
      PANENAME_PAGE1_GROUP_NUMBER,
      gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
    },
  };

#endif

  // 技名のペイン
  const gfl2::lyt::LytPaneIndex
    PokeMakeCore::PANE_INDEX_OF_WAZA_NAME[ pml::MAX_WAZA_NUM ] =
    {
      PANENAME_PAGE1_PANE_WAZA1_NAME,
      PANENAME_PAGE1_PANE_WAZA2_NAME,
      PANENAME_PAGE1_PANE_WAZA3_NAME,
      PANENAME_PAGE1_PANE_WAZA4_NAME,
    };

  //「ポイントアップ」使用回数のペイン
  const gfl2::lyt::LytPaneIndex
    PokeMakeCore::PANE_INDEX_OF_WAZA_UP_VAL[ pml::MAX_WAZA_NUM ] =
    {
      PANENAME_PAGE1_PANE_WAZA1_UP_VAL,
      PANENAME_PAGE1_PANE_WAZA2_UP_VAL,
      PANENAME_PAGE1_PANE_WAZA3_UP_VAL,
      PANENAME_PAGE1_PANE_WAZA4_UP_VAL,
    };

  // PP現在値のペイン
  const gfl2::lyt::LytPaneIndex
    PokeMakeCore::PANE_INDEX_OF_WAZA_PP_VAL[ pml::MAX_WAZA_NUM ] =
    {
      PANENAME_PAGE1_PANE_WAZA1_PP_VAL,
      PANENAME_PAGE1_PANE_WAZA2_PP_VAL,
      PANENAME_PAGE1_PANE_WAZA3_PP_VAL,
      PANENAME_PAGE1_PANE_WAZA4_PP_VAL,
    };

  // PP最大値のペイン
  const gfl2::lyt::LytPaneIndex
    PokeMakeCore::PANE_INDEX_OF_WAZA_PP_MAX[ pml::MAX_WAZA_NUM ] =
    {
      PANENAME_PAGE1_PANE_WAZA1_PP_MAX,
      PANENAME_PAGE1_PANE_WAZA2_PP_MAX,
      PANENAME_PAGE1_PANE_WAZA3_PP_MAX,
      PANENAME_PAGE1_PANE_WAZA4_PP_MAX,
    };

  // たまご技名のペイン
  const gfl2::lyt::LytPaneIndex
    PokeMakeCore::PANE_INDEX_OF_EGG_WAZA_NAME[ pml::MAX_WAZA_NUM ] =
    {
      PANENAME_PAGE1_PANE_WAZA_EGG1_NAME,
      PANENAME_PAGE1_PANE_WAZA_EGG2_NAME,
      PANENAME_PAGE1_PANE_WAZA_EGG3_NAME,
      PANENAME_PAGE1_PANE_WAZA_EGG4_NAME,
    };


  // ボタンごとの設定
  const PokeMakeCore::ButtonDesc PokeMakeCore::BUTTON_DESC[ BUTTON_NUM ] =
  {
    /* ARROW_MONSNO_L */
    {
      PANENAME_PAGE1_PANE_MONS_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_MONSNO_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_MONSNO_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_MONSNO_L ),
    },
    /* ARROW_MONSNO_R */
    {
      PANENAME_PAGE1_PANE_MONS_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_MONSNO_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_MONSNO_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_MONSNO_R ),
    },
    /* ARROW_FORMNO_L */
    {
      PANENAME_PAGE1_PANE_FORM_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_FORMNO_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_FORMNO_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_FORMNO_L ),
    },
    /* ARROW_FORMNO_R */
    {
      PANENAME_PAGE1_PANE_FORM_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_FORMNO_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_FORMNO_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_FORMNO_R ),
    },
    /* ARROW_SEIKAKU_L */
    {
      PANENAME_PAGE1_PANE_SEIKAKU_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_SEIKAKU_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_SEIKAKU_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_SEIKAKU_L ),
    },
    /* ARROW_SEIKAKU_R */
    {
      PANENAME_PAGE1_PANE_SEIKAKU_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_SEIKAKU_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_SEIKAKU_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_SEIKAKU_R ),
    },
    /* ARROW_ITEM_L */
    {
      PANENAME_PAGE1_PANE_ITEM_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_ITEM_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_ITEM_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_ITEM_L ),
    },
    /* ARROW_ITEM_R */
    {
      PANENAME_PAGE1_PANE_ITEM_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_ITEM_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_ITEM_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_ITEM_R ),
    },
    /* ARROW_TOKUSEI_L */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_TOKUSEI_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_TOKUSEI_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_TOKUSEI_L ),
    },
    /* ARROW_TOKUSEI_R */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_TOKUSEI_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_TOKUSEI_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_TOKUSEI_R ),
    },
    /* SWITCH_SEX */
    {
      PANENAME_PAGE1_PANE_SEX_BACK,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_SEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_SEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_SEX ),
    },
    /* SWITCH_SICK */
    {
      PANENAME_PAGE1_PANE_SICK_BUTTON,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_SICK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_SICK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_SICK ),
    },
    /* SWITCH_RND_MAX */
    {
      PANENAME_PAGE1_PANE_RND_MAX_BUTTON,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_RND_MAX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_RND_MAX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_RND_MAX ),
    },
    /* SWITCH_TOKUSEI1 */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_BTN1,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TOKUSEI1 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TOKUSEI1 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TOKUSEI1 ),
    },
    /* SWITCH_TOKUSEI2 */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_BTN2,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TOKUSEI2 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TOKUSEI2 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TOKUSEI2 ),
    },
    /* SWITCH_TOKUSEI3 */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_BTN3,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TOKUSEI3 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TOKUSEI3 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TOKUSEI3 ),
    },
    /* SWITCH_EGG */
    {
      PANENAME_PAGE1_PANE_EGG_BACK,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_EGG ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_EGG ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_EGG ),
    },
    /* SWITCH_RARE */
    {
      PANENAME_PAGE1_PANE_RARE_BACK,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_RARE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_RARE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_RARE ),
    },

    /* STRBOX_MONSNAME */
    {
      PANENAME_PAGE1_PANE_MONS_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_MONSNAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_MONSNAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_MONSNAME ),
    },
    /* STRBOX_SEIKAKU */
    {
      PANENAME_PAGE1_PANE_SEIKAKU_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_SEIKAKU ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_SEIKAKU ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_SEIKAKU ),
    },
    /* STRBOX_ITEM */
    {
      PANENAME_PAGE1_PANE_ITEM_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_ITEM ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_ITEM ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_ITEM ),
    },
    /* STRBOX_TOKUSEI */
    {
      PANENAME_PAGE1_PANE_TOKUSEI_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_TOKUSEI ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_TOKUSEI ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_TOKUSEI ),
    },
    /* NUMBOX_FORMNO */
    {
      PANENAME_PAGE1_PANE_FORM_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_FORMNO ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_FORMNO ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_FORMNO ),
    },
    /* NUMBOX_LEVEL */
    {
      PANENAME_PAGE1_PANE_LEVEL_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_LEVEL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_LEVEL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_LEVEL ),
    },
    /* NUMBOX_EXP */
    {
      PANENAME_PAGE1_PANE_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_EXP ),
    },
    /* ARROW_WAZA1_L */
    {
      PANENAME_PAGE1_PANE_WAZA1_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA1_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA1_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA1_L ),
    },
    /* ARROW_WAZA1_R */
    {
      PANENAME_PAGE1_PANE_WAZA1_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA1_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA1_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA1_R ),
    },
    /* ARROW_WAZA2_L */
    {
      PANENAME_PAGE1_PANE_WAZA2_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA2_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA2_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA2_L ),
    },
    /* ARROW_WAZA2_R */
    {
      PANENAME_PAGE1_PANE_WAZA2_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA2_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA2_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA2_R ),
    },
    /* ARROW_WAZA3_L */
    {
      PANENAME_PAGE1_PANE_WAZA3_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA3_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA3_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA3_L ),
    },
    /* ARROW_WAZA3_R */
    {
      PANENAME_PAGE1_PANE_WAZA3_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA3_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA3_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA3_R ),
    },
    /* ARROW_WAZA4_L */
    {
      PANENAME_PAGE1_PANE_WAZA4_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA4_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA4_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA4_L ),
    },
    /* ARROW_WAZA4_R */
    {
      PANENAME_PAGE1_PANE_WAZA4_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_WAZA4_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_WAZA4_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_WAZA4_R ),
    },
    /* SWITCH_DEL_WAZA2 */
    {
      PANENAME_PAGE1_PANE_WAZA2_DEL_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_DEL_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_DEL_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_DEL_WAZA2 ),
    },
    /* SWITCH_DEL_WAZA3 */
    {
      PANENAME_PAGE1_PANE_WAZA3_DEL_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_DEL_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_DEL_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_DEL_WAZA3 ),
    },
    /* SWITCH_DEL_WAZA4 */
    {
      PANENAME_PAGE1_PANE_WAZA4_DEL_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_DEL_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_DEL_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_DEL_WAZA4 ),
    },
    /* SWITCH_DEFAULT_WAZA */
    {
      PANENAME_PAGE1_PANE_WAZA_DEF_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_DEFAULT_WAZA ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_DEFAULT_WAZA ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_DEFAULT_WAZA ),
    },
    /* SWITCH_HATAKU */
    {
      PANENAME_PAGE1_PANE_WAZA_HATAKU_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_HATAKU ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_HATAKU ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_HATAKU ),
    },
    /* SWITCH_HANERU */
    {
      PANENAME_PAGE1_PANE_WAZA_HANERU_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_HANERU ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_HANERU ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_HANERU ),
    },
    /* STRBOX_WAZA1 */
    {
      PANENAME_PAGE1_PANE_WAZA1_NAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_WAZA1 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_WAZA1 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_WAZA1 ),
    },
    /* STRBOX_WAZA2 */
    {
      PANENAME_PAGE1_PANE_WAZA2_NAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_WAZA2 ),
    },
    /* STRBOX_WAZA3 */
    {
      PANENAME_PAGE1_PANE_WAZA3_NAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_WAZA3 ),
    },
    /* STRBOX_WAZA4 */
    {
      PANENAME_PAGE1_PANE_WAZA4_NAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_WAZA4 ),
    },
    /* STRBOX_NICKNAME */
    {
      PANENAME_PAGE1_PANE_NICKNAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_NICKNAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_NICKNAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_NICKNAME ),
    },
    /* NUMBOX_HP_RND */
    {
      PANENAME_PAGE1_PANE_HP_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_HP_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_HP_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_HP_RND ),
    },
    /* NUMBOX_HP_EXP */
    {
      PANENAME_PAGE1_PANE_HP_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_HP_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_HP_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_HP_EXP ),
    },
    /* NUMBOX_HP_VAL */
    {
      PANENAME_PAGE1_PANE_HP_VAL_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_HP_VAL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_HP_VAL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_HP_VAL ),
    },
    /* NUMBOX_ATK_RND */
    {
      PANENAME_PAGE1_PANE_ATK_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_ATK_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_ATK_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_ATK_RND ),
    },
    /* NUMBOX_ATK_EXP */
    {
      PANENAME_PAGE1_PANE_ATK_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_ATK_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_ATK_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_ATK_EXP ),
    },
    /* NUMBOX_DEF_RND */
    {
      PANENAME_PAGE1_PANE_DEF_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_DEF_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_DEF_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_DEF_RND ),
    },
    /* NUMBOX_DEF_EXP */
    {
      PANENAME_PAGE1_PANE_DEF_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_DEF_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_DEF_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_DEF_EXP ),
    },
    /* NUMBOX_AGI_RND */
    {
      PANENAME_PAGE1_PANE_AGI_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_AGI_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_AGI_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_AGI_RND ),
    },
    /* NUMBOX_AGI_EXP */
    {
      PANENAME_PAGE1_PANE_AGI_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_AGI_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_AGI_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_AGI_EXP ),
    },
    /* NUMBOX_SPATK_RND */
    {
      PANENAME_PAGE1_PANE_SPATK_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_SPATK_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_SPATK_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_SPATK_RND ),
    },
    /* NUMBOX_SPATK_EXP */
    {
      PANENAME_PAGE1_PANE_SPATK_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_SPATK_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_SPATK_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_SPATK_EXP ),
    },
    /* NUMBOX_SPDEF_RND */
    {
      PANENAME_PAGE1_PANE_SPDEF_RND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_SPDEF_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_SPDEF_RND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_SPDEF_RND ),
    },
    /* NUMBOX_SPDEF_EXP */
    {
      PANENAME_PAGE1_PANE_SPDEF_EXP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_SPDEF_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_SPDEF_EXP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_SPDEF_EXP ),
    },
    /* NUMBOX_WAZA1_UP */
    {
      PANENAME_PAGE1_PANE_WAZA1_UP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA1_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA1_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA1_UP ),
    },
    /* NUMBOX_WAZA1_PP */
    {
      PANENAME_PAGE1_PANE_WAZA1_PP_VAL_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA1_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA1_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA1_PP ),
    },
    /* NUMBOX_WAZA2_UP */
    {
      PANENAME_PAGE1_PANE_WAZA2_UP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA2_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA2_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA2_UP ),
    },
    /* NUMBOX_WAZA2_PP */
    {
      PANENAME_PAGE1_PANE_WAZA2_PP_VAL_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA2_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA2_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA2_PP ),
    },
    /* NUMBOX_WAZA3_UP */
    {
      PANENAME_PAGE1_PANE_WAZA3_UP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA3_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA3_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA3_UP ),
    },
    /* NUMBOX_WAZA3_PP */
    {
      PANENAME_PAGE1_PANE_WAZA3_PP_VAL_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA3_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA3_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA3_PP ),
    },
    /* NUMBOX_WAZA4_UP */
    {
      PANENAME_PAGE1_PANE_WAZA4_UP_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA4_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA4_UP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA4_UP ),
    },
    /* NUMBOX_WAZA4_PP */
    {
      PANENAME_PAGE1_PANE_WAZA4_PP_VAL_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_WAZA4_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_WAZA4_PP ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_WAZA4_PP ),
    },
    /* NUMBOX_FAMILY */
    {
      PANENAME_PAGE1_PANE_FAMILY_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_FAMILY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_FAMILY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_FAMILY ),
    },
    /* NUMBOX_FRIEND */
    {
      PANENAME_PAGE1_PANE_FRIEND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_FRIEND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_FRIEND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_FRIEND ),
    },
    /* NUMBOX_FEED */
    {
      PANENAME_PAGE1_PANE_FEED_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_FEED ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_FEED ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_FEED ),
    },
    /* NUMBOX_PLAY */
    {
      PANENAME_PAGE1_PANE_PLAY_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PLAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PLAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PLAY ),
    },
    /* ARROW_VERSION_L */
    {
      PANENAME_PAGE1_PANE_VERSION_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_VERSION_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_VERSION_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_VERSION_L ),
    },
    /* ARROW_VERSION_R */
    {
      PANENAME_PAGE1_PANE_VERSION_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_VERSION_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_VERSION_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_VERSION_R ),
    },
    /* ARROW_LANGUAGE_L */
    {
      PANENAME_PAGE1_PANE_LANGUAGE_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_LANGUAGE_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_LANGUAGE_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_LANGUAGE_L ),
    },
    /* ARROW_LANGUAGE_R */
    {
      PANENAME_PAGE1_PANE_LANGUAGE_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_LANGUAGE_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_LANGUAGE_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_LANGUAGE_R ),
    },
    /* ARROW_CAPTUREBALL_L */
    {
      PANENAME_PAGE1_PANE_CAP_BALL_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_CAPTUREBALL_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_CAPTUREBALL_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_CAPTUREBALL_L ),
    },
    /* ARROW_CAPTUREBALL_R */
    {
      PANENAME_PAGE1_PANE_CAP_BALL_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_CAPTUREBALL_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_CAPTUREBALL_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_CAPTUREBALL_R ),
    },
    /* SWITCH_OYA_SET */
    {
      PANENAME_PAGE1_PANE_OYA_SET_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_OYA_SET ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_OYA_SET ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_OYA_SET ),
    },
    /* SWITCH_OYA_SEX */
    {
      PANENAME_PAGE1_PANE_OYASEX_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_OYA_SEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_OYA_SEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_OYA_SEX ),
    },
    /* SWITCH_EVENT */
    {
      PANENAME_PAGE1_PANE_EVENT_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_EVENT ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_EVENT ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_EVENT ),
    },
    /* STRBOX_VERSION */
    {
      PANENAME_PAGE1_PANE_VERSION_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_VERSION ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_VERSION ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_VERSION ),
    },
    /* STRBOX_LANGUAGE */
    {
      PANENAME_PAGE1_PANE_LANGUAGE_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_LANGUAGE ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_LANGUAGE ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_LANGUAGE ),
    },
    /* STRBOX_OYANAME */
    {
      PANENAME_PAGE1_PANE_OYANAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_OYANAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_OYANAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_OYANAME ),
    },
    /* STRBOX_CAPTURE_BALL */
    {
      PANENAME_PAGE1_PANE_CAP_BALL_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_CAPTURE_BALL ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_CAPTURE_BALL ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_CAPTURE_BALL ),
    },
    /* NUMBOX_POKERUS */
    {
      PANENAME_PAGE1_PANE_POKERUS_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_POKERUS ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_POKERUS ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_POKERUS ),
    },
    /* NUMBOX_CAPTURE_LEVEL */
    {
      PANENAME_PAGE1_PANE_CAP_LEVEL_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_CAPTURE_LEVEL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_CAPTURE_LEVEL ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_CAPTURE_LEVEL ),
    },
    /* NUMBOX_CONTACT_PLACE */
    {
      PANENAME_PAGE1_PANE_CONTACT_PLC_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_CONTACT_PLACE ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_CONTACT_PLACE ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_CONTACT_PLACE ),
    },
    /* NUMBOX_EGG_TAKEN_PLACE */
    {
      PANENAME_PAGE1_PANE_EGG_TAKEN_PLC_BC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_EGG_TAKEN_PLACE ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_EGG_TAKEN_PLACE ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_EGG_TAKEN_PLACE ),
    },
    /* NUMBOX_CONTACT_DAY */
    {
      PANENAME_PAGE1_PANE_CONTACT_D_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_CONTACT_DAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_CONTACT_DAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_CONTACT_DAY ),
    },
    /* NUMBOX_CONTACT_MONTH */
    {
      PANENAME_PAGE1_PANE_CONTACT_M_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_CONTACT_MONTH ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_CONTACT_MONTH ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_CONTACT_MONTH ),
    },
    /* NUMBOX_CONTACT_YEAR */
    {
      PANENAME_PAGE1_PANE_CONTACT_Y_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_CONTACT_YEAR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_CONTACT_YEAR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_CONTACT_YEAR ),
    },
    /* NUMBOX_EGG_TAKEN_DAY */
    {
      PANENAME_PAGE1_PANE_EGG_TAKEN_D_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_EGG_TAKEN_DAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_EGG_TAKEN_DAY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_EGG_TAKEN_DAY ),
    },
    /* NUMBOX_EGG_TAKEN_MONTH */
    {
      PANENAME_PAGE1_PANE_EGG_TAKEN_M_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_EGG_TAKEN_MONTH ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_EGG_TAKEN_MONTH ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_EGG_TAKEN_MONTH ),
    },
    /* NUMBOX_EGG_TAKEN_YEAR */
    {
      PANENAME_PAGE1_PANE_EGG_TAKEN_Y_BCK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_EGG_TAKEN_YEAR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_EGG_TAKEN_YEAR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_EGG_TAKEN_YEAR ),
    },
    /* NUMBOX_MEMORY_CODE_OYA */
    {
      PANENAME_PAGE1_PANE_OYA_MEM_CODE_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_CODE_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_CODE_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_CODE_OYA ),
    },
    /* NUMBOX_MEMORY_LEVEL_OYA */
    {
      PANENAME_PAGE1_PANE_OYA_MEM_LV_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_LEVEL_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_LEVEL_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_LEVEL_OYA ),
    },
    /* NUMBOX_MEMORY_DATA_OYA */
    {
      PANENAME_PAGE1_PANE_OYA_MEM_DAT_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_DATA_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_DATA_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_DATA_OYA ),
    },
    /* NUMBOX_MEMORY_FEEL_OYA */
    {
      PANENAME_PAGE1_PANE_OYA_MEM_FEEL_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_FEEL_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_FEEL_OYA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_FEEL_OYA ),
    },
    /* NUMBOX_MEMORY_CODE_OTHER */
    {
      PANENAME_PAGE1_PANE_OTH_MEM_CODE_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_CODE_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_CODE_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_CODE_OTHER ),
    },
    /* NUMBOX_MEMORY_LEVEL_OTHER */
    {
      PANENAME_PAGE1_PANE_OTH_MEM_LV_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_LEVEL_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_LEVEL_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_LEVEL_OTHER ),
    },
    /* NUMBOX_MEMORY_DATA_OTHER */
    {
      PANENAME_PAGE1_PANE_OTH_MEM_DAT_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_DATA_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_DATA_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_DATA_OTHER ),
    },
    /* NUMBOX_MEMORY_FEEL_OTHER */
    {
      PANENAME_PAGE1_PANE_OTH_MEM_FEEL_BAC,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_MEMORY_FEEL_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_MEMORY_FEEL_OTHER ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_MEMORY_FEEL_OTHER ),
    },

    /* ARROW_REGION_L */
    {
      PANENAME_PAGE1_PANE_REGION_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_REGION_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_REGION_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_REGION_L ),
    },
    /* ARROW_REGION_R */
    {
      PANENAME_PAGE1_PANE_REGION_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_REGION_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_REGION_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_REGION_R ),
    },

    /* STRBOX_REGION */
    {
      PANENAME_PAGE1_PANE_REGION_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_REGION ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_REGION ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_REGION ),
    },
    /* NUMBOX_COUNTRY */
    {
      PANENAME_PAGE1_PANE_COUNTRY_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_COUNTRY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_COUNTRY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_COUNTRY ),
    },
    /* NUMBOX_COUNTRY_AREA */
    {
      PANENAME_PAGE1_PANE_AREA_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_COUNTRY_AREA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_COUNTRY_AREA ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_COUNTRY_AREA ),
    },
    /* NUMBOX_OTHER_FAMILY */
    {
      PANENAME_PAGE1_PANE_OTH_FAMILY_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_OTHER_FAMILY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_OTHER_FAMILY ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_OTHER_FAMILY ),
    },
    /* NUMBOX_OTHER_FRIEND */
    {
      PANENAME_PAGE1_PANE_OTH_FRIEND_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_OTHER_FRIEND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_OTHER_FRIEND ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_OTHER_FRIEND ),
    },
    /* STRBOX_OTHER_OYANAME */
    {
      PANENAME_PAGE1_PANE_OTH_OYANAME_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_OTHER_OYANAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_OTHER_OYANAME ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_OTHER_OYANAME ),
    },
    /* SWITCH_OTHER_OYASEX */
    {
      PANENAME_PAGE1_PANE_OTH_OYASEX_BTN,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_OTHER_OYASEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_OTHER_OYASEX ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_OTHER_OYASEX ),
    },
    /* NUMBOX_PAST_COUNTRY0 */
    {
      PANENAME_PAGE1_PANE_OTH_CTRY_BACK_00,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_COUNTRY_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_COUNTRY_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_COUNTRY_00 ),
    },
    /* NUMBOX_PAST_AREA0 */
    {
      PANENAME_PAGE1_PANE_OTH_AREA_BACK_00,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_AREA_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_AREA_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_AREA_00 ),
    },
    /* NUMBOX_PAST_COUNTRY1 */
    {
      PANENAME_PAGE1_PANE_OTH_CTRY_BACK_01,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_COUNTRY_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_COUNTRY_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_COUNTRY_01 ),
    },
    /* NUMBOX_PAST_AREA1 */
    {
      PANENAME_PAGE1_PANE_OTH_AREA_BACK_01,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_AREA_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_AREA_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_AREA_01 ),
    },
    /* NUMBOX_PAST_COUNTRY2 */
    {
      PANENAME_PAGE1_PANE_OTH_CTRY_BACK_02,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_COUNTRY_02 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_COUNTRY_02 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_COUNTRY_02 ),
    },
    /* NUMBOX_PAST_AREA2 */
    {
      PANENAME_PAGE1_PANE_OTH_AREA_BACK_02,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_AREA_02 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_AREA_02 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_AREA_02 ),
    },
    /* NUMBOX_PAST_COUNTRY3 */
    {
      PANENAME_PAGE1_PANE_OTH_CTRY_BACK_03,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_COUNTRY_03 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_COUNTRY_03 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_COUNTRY_03 ),
    },
    /* NUMBOX_PAST_AREA3 */
    {
      PANENAME_PAGE1_PANE_OTH_AREA_BACK_03,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_AREA_03 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_AREA_03 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_AREA_03 ),
    },
    /* NUMBOX_PAST_COUNTRY4 */
    {
      PANENAME_PAGE1_PANE_OTH_CTRY_BACK_04,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_COUNTRY_04 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_COUNTRY_04 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_COUNTRY_04 ),
    },
    /* NUMBOX_PAST_AREA4 */
    {
      PANENAME_PAGE1_PANE_OTH_AREA_BACK_04,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_PAST_AREA_04 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_PAST_AREA_04 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_PAST_AREA_04 ),
    },
    /* NUMBOX_TRAINER_ID_00 */
    {
      PANENAME_PAGE1_PANE_TRAINER_ID_B_00,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRAINER_ID_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRAINER_ID_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRAINER_ID_00 ),
    },
    /* NUMBOX_TRAINER_ID_01 */
    {
      PANENAME_PAGE1_PANE_TRAINER_ID_B_01,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRAINER_ID_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRAINER_ID_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRAINER_ID_01 ),
    },
    /* NUMBOX_RARE_RND_00 */
    {
      PANENAME_PAGE1_PANE_RARE_RND_B_00,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_RARE_RND_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_RARE_RND_00 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_RARE_RND_00 ),
    },
    /* NUMBOX_RARE_RND_01 */
    {
      PANENAME_PAGE1_PANE_RARE_RND_B_01,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_RARE_RND_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_RARE_RND_01 ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_RARE_RND_01 ),
    },
    /* NUMBOX_TRIMIAN_HAIR */
    {
      PANENAME_PAGE1_PANE_TRI_HAIR_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_HAIR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_HAIR ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_HAIR ),
    },
    /* NUMBOX_TRIMIAN_COUNT */
    {
      PANENAME_PAGE1_PANE_TRI_HAIRCNT_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT ),
    },
    /* NUMBOX_TRIMIAN_COUNT_MAX */
    {
      PANENAME_PAGE1_PANE_TRI_HAIRMAX_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },

    //ここからsango追加分
    /* コンテスト */
    {
      PANENAME_PAGE1_PANE_CON_VAL_BACK_01,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    {
      PANENAME_PAGE1_PANE_CON_VAL_BACK_02,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    {
      PANENAME_PAGE1_PANE_CON_VAL_BACK_03,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    {
      PANENAME_PAGE1_PANE_CON_VAL_BACK_04,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    {
      PANENAME_PAGE1_PANE_CON_VAL_BACK_05,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT + NUMBOX_TRIMIAN_COUNT_MAX ),
    },

    //タマゴ技
    /* ARROW_EGG_WAZA1_L */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG1_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA1_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA1_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA1_L ),
    },
    /* ARROW_EGG_WAZA1_R */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG1_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA1_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA1_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA1_R ),
    },
    /* STRBOX_EGG_WAZA1 */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG1_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_EGG_WAZA1 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_EGG_WAZA1 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_EGG_WAZA1 ),
    },
    /* ARROW_EGG_WAZA2_L */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG2_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA2_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA2_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA2_L ),
    },
    /* ARROW_EGG_WAZA2_R */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG2_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA2_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA2_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA2_R ),
    },
    /* STRBOX_EGG_WAZA2 */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG2_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_EGG_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_EGG_WAZA2 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_EGG_WAZA2 ),
    },
    /* ARROW_EGG_WAZA3_L */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG3_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA3_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA3_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA3_L ),
    },
    /* ARROW_EGG_WAZA3_R */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG3_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA3_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA3_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA3_R ),
    },
    /* STRBOX_EGG_WAZA3 */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG3_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_EGG_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_EGG_WAZA3 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_EGG_WAZA3 ),
    },
    /* ARROW_EGG_WAZA4_L */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG4_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA4_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA4_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA4_L ),
    },
    /* ARROW_EGG_WAZA4_R */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG4_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD + ARROW_EGG_WAZA4_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_EGG_WAZA4_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT + ARROW_EGG_WAZA4_R ),
    },
    /* STRBOX_EGG_WAZA4 */
    {
      PANENAME_PAGE1_PANE_WAZA_EGG4_BACK,
      static_cast<AnimeIndex>( ANIME_STRBOX_HOLD + STRBOX_EGG_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_RELEASE + STRBOX_EGG_WAZA4 ),
      static_cast<AnimeIndex>( ANIME_STRBOX_SELECT + STRBOX_EGG_WAZA4 ),
    },

    // デフォルト能力アップ
    /* BUTTON_DEFAULT_POWERUP_REASON_L */
    {
      PANENAME_PAGE1_PANE_POWERUP_REASON_L,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD    + ARROW_DEFAULT_POWERUP_REASON_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_DEFAULT_POWERUP_REASON_L ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT  + ARROW_DEFAULT_POWERUP_REASON_L ),
    },
    /* BUTTON_DEFAULT_POWERUP_REASON_R */
    {
      PANENAME_PAGE1_PANE_POWERUP_REASON_R,
      static_cast<AnimeIndex>( ANIME_ARROW_HOLD    + ARROW_DEFAULT_POWERUP_REASON_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_RELEASE + ARROW_DEFAULT_POWERUP_REASON_R ),
      static_cast<AnimeIndex>( ANIME_ARROW_SELECT  + ARROW_DEFAULT_POWERUP_REASON_R ),
    },
    /* BUTTON_DEFAULT_POWERUP_RANK_ATK */
    {
      PANENAME_PAGE1_PANE_POWERUP_ATK_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD    + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT  + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    /* BUTTON_DEFAULT_POWERUP_RANK_DEF */
    {
      PANENAME_PAGE1_PANE_POWERUP_DEF_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD    + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT  + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    /* BUTTON_DEFAULT_POWERUP_RANK_SPATK */
    {
      PANENAME_PAGE1_PANE_POWERUP_SPATK_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD    + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT  + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    /* BUTTON_DEFAULT_POWERUP_RANK_SPDEF */
    {
      PANENAME_PAGE1_PANE_POWERUP_SPDEF_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD    + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT  + NUMBOX_TRIMIAN_COUNT_MAX ),
    },
    /* BUTTON_DEFAULT_POWERUP_RANK_AGI */
    {
      PANENAME_PAGE1_PANE_POWERUP_AGI_BACK,
      static_cast<AnimeIndex>( ANIME_NUMBOX_HOLD    + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_RELEASE + NUMBOX_TRIMIAN_COUNT_MAX ),
      static_cast<AnimeIndex>( ANIME_NUMBOX_SELECT  + NUMBOX_TRIMIAN_COUNT_MAX ),
    },

    /* BUTTON_SWITCH_TRAINING_FLAG_HP */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_00,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_HP ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_HP ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_HP ),
    },
    /* BUTTON_SWITCH_TRAINING_FLAG_ATTACK */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_01,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_ATTACK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_ATTACK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_ATTACK ),
    },
    /* BUTTON_SWITCH_TRAINING_FLAG_DEFENSE */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_02,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_DEFENSE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_DEFENSE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_DEFENSE ),
    },
    /* BUTTON_SWITCH_TRAINING_FLAG_AGILITY */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_03,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_AGILITY ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_AGILITY ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_AGILITY ),
    },
    /* BUTTON_SWITCH_TRAINING_FLAG_SP_ATTACK */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_04,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_SP_ATTACK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_SP_ATTACK ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_SP_ATTACK ),
    },
    /* BUTTON_SWITCH_TRAINING_FLAG_SP_DEFENSE */
    {
      PANENAME_PAGE1_PANE_TRAINING_BUTTON_05,
      static_cast<AnimeIndex>( ANIME_SWITCH_HOLD + SWITCH_TRAINING_FLAG_SP_DEFENSE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_RELEASE + SWITCH_TRAINING_FLAG_SP_DEFENSE ),
      static_cast<AnimeIndex>( ANIME_SWITCH_SELECT + SWITCH_TRAINING_FLAG_SP_DEFENSE ),
    },
  };



  const gfl2::str::STRCODE* PokeMakeCore::TYPE_NAME[ POKETYPE_MAX ] =
  {
    L"ノーマル",
    L"かくとう",
    L"ひこう",
    L"どく",
    L"じめん",
    L"いわ",
    L"むし",
    L"ゴースト",
    L"はがね",
    L"ほのお",
    L"みず",
    L"くさ",
    L"でんき",
    L"エスパー",
    L"こおり",
    L"ドラゴン",
    L"あく",
    L"ようせい",
  };


  const gfl2::str::STRCODE* PokeMakeCore::SEX_NAME[ pml::SEX_NUM ] =
  {
    L"♂",
    L"♀",
    L"？",
  };


  const gfl2::str::STRCODE* PokeMakeCore::SICK_NAME[ pml::pokepara::SICK_MAX ] =
  {
    L"−",
    L"まひ",
    L"ねむり",
    L"こおり",
    L"やけど",
    L"どく",
  };

  const gfl2::str::STRCODE* PokeMakeCore::LANGUAGE_NAME[12]  =
  {
    L"−",          ///LANG_NULL            0
    L"にほん",     ///LANG_JAPAN           1
    L"アメリカ",   ///LANG_ENGLISH         2
    L"フランス",   ///LANG_FRANCE          3
    L"イタリア",   ///LANG_ITALY           4
    L"ドイツ",     ///LANG_GERMAY          5
    L"−",          ///欠番                 6
    L"スペイン",   ///LANG_SPAIN           7
    L"かんこく",   ///LANG_KOREA           8
    L"簡体字",     ///POKEMON_LANG_CHINA   9 簡体字（中国）
    L"繁体字",     ///POKEMON_LANG_TAIWAN 10 繁体字（台湾、香港）
    L"未知国",     ///LANG_?????          11 チェック用のありえない国
  };

  const gfl2::str::STRCODE* PokeMakeCore::REGION_NAME[ REGION_MAX ]  =
  {
    L"にほん",//CFG_REGION_JAPAN     = 0,   //!< 日本リージョン
    L"ほくべい",//CFG_REGION_AMERICA   = 1,   //!< 北米リージョン
    L"おうしゅう",//CFG_REGION_EUROPE    = 2,   //!< 欧州および豪州リージョン
    L"−",//CFG_REGION_AUSTRALIA = 3,   //!< 豪州リージョンは欧州リージョンと統合されました。この値は使用されません。
    L"ちゅうごく",//CFG_REGION_CHINA     = 4,   //!< 中国リージョン
    L"かんこく",//CFG_REGION_KOREA     = 5,   //!< 韓国リージョン
    L"たいわん",//CFG_REGION_TAIWAN    = 6,   //!< 台湾リージョン
  };

  const gfl2::str::STRCODE* PokeMakeCore::VERSION_NAME[ 42 ] =
  {
    L"−",
    L"サファイア",         //	VERSION_SAPPHIRE	1	  ///<	バージョン：AGBサファイア
    L"ルビー",             //	VERSION_RUBY		  2	  ///<	バージョン：AGBルビー
    L"エメラルド",         //	VERSION_EMERALD		3	  ///<	バージョン：AGBエメラルド
    L"Ｆレッド",           //	VERSION_RED			  4	  ///<	バージョン：AGBファイアーレッド
    L"Ｒグリーン",         //	VERSION_GREEN		  5	  ///<	バージョン：AGBリーフグリーン
    L"−",                  //                       
    L"Ｈゴールド",         //	VERSION_GOLD		  7	  ///<	バージョン：ゴールド用予約定義
    L"Ｓシルバー",         //	VERSION_SILVER		8	  ///<	バージョン：シルバー用予約定義
    L"−",                  //
    L"ダイヤモンド",       //	VERSION_DIAMOND		10	///<	バージョン：DSダイヤモンド
    L"パール",             //	VERSION_PEARL		  11	///<	バージョン：DSパール
    L"プラチナ",           //	VERSION_PLATINUM	12	///<	バージョン：DSプラチナ
    L"−",                  //
    L"−",                  //
    L"コロシアム",         //	VERSION_COLOSSEUM	15	///<	バージョン：GCコロシアム
    L"−",                  //
    L"−",                  //
    L"−",                  //
    L"−",                  //
    L"ホワイト",           //	VERSION_WHITE		          20	///<	バージョン：DSホワイト
    L"ブラック",           //	VERSION_BLACK		          21	///<	バージョン：DSブラック
    L"ホワイト２",         //	VERSION_W2			          22	///<	バージョン：DSホワイト２
    L"ブラック２",         //	VERSION_B2			          23	///<	バージョン：DSブラック２
    L"Ｘ",                 //	VERSION_X			            24	///<	バージョン：3DS X
    L"Ｙ",                 //	VERSION_Y			            25	///<	バージョン：3DS Y
    L"３ＤＳサファイア",   //	VERSION_CTR_SAPPHIRE			26	///<	3DS アルファサファイア
    L"３ＤＳルビー",       //	VERSION_CTR_RUBY			    27	///<	オメガルビー
    L"Ｘ３",               //	VERSION_CALOS_RESERVE3		28	///<	※カロス地方予約領域
    L"Ｙ３",               //	VERSION_CALOS_RESERVE4		29	///<	※カロス地方予約領域
    L"ＳＵＮ",             //	VERSION_SUN			          30	///<	3DS SUN
    L"ＭＯＯＮ",           //	VERSION_MOON			        31	///<	3DS MOON
    L"ＳＵＮ２",           // VERSION_SUN2              32  ///<  3DS momiji SUN
    L"ＭＯＯＮ２",         // VERSION_MOON2             33  ///<  3DS momiji MOON
    L"HoloHolo",           // VERSION_HOLOHOLO          34  ///< Pokemon HOLOHOLO
    L"VC Red",             // VERSION_VC_RED            35  ///< VC 赤
    L"VC Green",           // VERSION_VC_GREEN          36  ///< VC 緑
    L"VC Blue",            // VERSION_VC_BLUE           37  ///< VC 青
    L"VC Pika",            // VERSION_VC_PIKACHU        38  ///< VC ぴかちゅう
    L"VC Gold",            // VERSION_VC_GOLD           39  ///< VC 金
    L"VC Slilver",         // VERSION_VC_SILVER         40  ///< VC 銀
    L"VC Cristal",         // VERSION_VC_CRISTAL        41  ///< VC クリスタル
  };

  const gfl2::str::STRCODE* PokeMakeCore::DEFAULT_POWERUP_REASON_NAME[ DEFAULT_POWERUP_REASON_NUM ] =
  {
    L"−",
    L"ぬし",
    L"ウルトラビースト",
    L"代表ビースト",
  };



  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  PokeMakeCore::PokeMakeCore( void ) :
    PokeMakeTemp::LytUtil( DebugPokeMake::DeviceHeap() ),
#if 0
    app::util::LayoutContainer(
        DebugPokeMake::DeviceHeap(),
        DebugPokeMake::DeviceHeap(),
        DebugPokeMake::LayoutSystem(),
        ARCID_DEBUG_POKEMAKE,
        GARC_debug_poke_make_debug_poke_make_LYTDAT,
        LYTRES_DEBUG_POKE_MAKE_PAGE1_BCLYT,
        ANIME_DATA_TABLE,
        ANIME_DATA_NUM,
        ANIME_NUM,
        SHARE_ANIME_TABLE,
        SHARE_ANIME_NUM,
        LAYOUT_WORK_SETUP_DATA,
        PROJECTION_PARAM[0],
        PROJECTION_PARAM[1],
        VIEW_PARAM[0],
        VIEW_PARAM[0],
        VIEW_PARAM[1],
        OPERATION_PARAM ),
#endif
    app::util::Controller(),
    app::tool::ButtonManager::IButtonCallback(),
    m_msgData( NULL ),
    m_wordset( NULL ),
    m_button_manager( NULL ),
    m_seq( SEQ_FADEIN ),
    m_number_input_target( BUTTON_NUM ),
    m_keyboad_input_target( BUTTON_NUM ),
    m_keyboard_buf( NULL ),
    m_keyboard_choice( 0 ),
    m_page( 0 ),
    m_page_old( 0 )
  {
    gfl2::heap::HeapBase* heap = DebugPokeMake::DeviceHeap();
    gfl2::heap::HeapBase* device_heap = DebugPokeMake::DeviceHeap();

    this->CreateRenderingPipeLine( heap, device_heap );

    {
      // リソース読み込み
      app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_POKE_MAKE, heap );
      app::util::FileAccessor::FileLoadSync( ARCID_DEBUG_POKE_MAKE, GARC_debug_poke_make_debug_poke_make_APPLYT, &m_lytDataBuffer, device_heap, false, 128 );
      app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_POKE_MAKE );

      app::util::AppLytAccessor* accessor = GFL_NEW_LOW( heap ) app::util::AppLytAccessor();
      accessor->SetAppLytBuf( m_lytDataBuffer );

      gfl2::lyt::LytSys* layout_system = DebugPokeMake::LayoutSystem();
      m_lytResID = layout_system->LoadLytRes( NULL, DebugPokeMake::NwDeviceAllocator(), accessor->GetLytDat() );

      m_lytWk  = layout_system->CreateLytWk( &m_lytResID, LYTRES_DEBUG_POKE_MAKE_PAGE1_BFLYT, ANIME_DATA_TABLE, ANIME_DATA_NUM, ANIME_NUM, &LAYOUT_WORK_SETUP_DATA );
      m_lytWk->SetAutoDrawFlag( false );

      m_lytWk_Upper  = layout_system->CreateLytWk( &m_lytResID, LYTRES_DEBUG_POKE_MAKE_SUB_BFLYT, ANIME_DATA_TABLE_UPPER, UPPER_ANIME_DATA_NUM, UPPER_ANIME_NUM, &LAYOUT_WORK_SETUP_DATA_UPPER );
      m_lytWk_Upper->SetAutoDrawFlag( false );

      m_isDrawEnable = true;

      GFL_SAFE_DELETE( accessor );
    }

    m_msgData = GFL_NEW( heap ) gfl2::str::MsgData( print::GetMessageArcId_Debug( System::MSGLANGID_MAX ), GARC_debug_message_debug_poke_make_DAT, heap );
    m_wordset = GFL_NEW( heap ) print::WordSet( heap );

    m_button_manager = GFL_NEW( heap ) app::tool::ButtonManager( heap, this->GetDeviceManager(), BUTTON_NUM );
    m_button_manager->SetSelectType( app::tool::ButtonManager::SELECT_TYPE_TRIGGER );

    for( u32 i=0; i<BUTTON_NUM; i++ ) {
      m_button_manager->AddButton(
          heap, i, m_lytWk,
          BUTTON_DESC[i].pane_index,
          BUTTON_DESC[i].hold_anime,
          BUTTON_DESC[i].release_anime,
          app::tool::ButtonManager::ANIMATION_NULL,
          BUTTON_DESC[i].select_anime,
          app::tool::ButtonManager::ANIMATION_NULL,
          app::tool::ButtonManager::ANIMATION_NULL,
          this );
    }
    m_button_manager->SetUseVisibleOfRoot(true);

    m_keyboard_buf = GFL_NEW( heap ) gfl2::str::StrBuf( 256, heap );

#if 0
    gfl2::grp::GraphicsSystem* gsys = DebugPokeMake::GraphicsSystem();
#endif
    gfl2::lyt::LytSys* layout_system = DebugPokeMake::LayoutSystem();
    m_num_input = GFL_NEW( heap ) NumInput( heap, device_heap, layout_system );

    pml::pokepara::PokemonParam* clone_param = DebugPokeMake::ClonePokemonParam();

    //必ず設定しなければならない値が入っていなければ入れておく

    // 捕獲バージョン
    if(clone_param->GetCassetteVersion()==0){
      clone_param->SetCassetteVersion( System::GetVersion() );
    }
    // 言語コード
    if(clone_param->GetLangId()==System::LANGUAGE_MAX){
      clone_param->SetLangId( System::GetLang() );
    }
    // 捕獲ボール
    if(clone_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL)==item::BALLID_NULL ||
       clone_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL)>item::BALLID_MAX ){
      clone_param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, item::BALLID_MONSUTAABOORU );
    }
    //思い出
    if( clone_param->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT)==0 )
    {
      clone_param->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT, 1 );
      clone_param->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT, 1 );
    }

    //親名
    {
      clone_param->GetParentName( DebugPokeMake::WorkStrBuf() );
      if( DebugPokeMake::WorkStrBuf()->GetLength() == 0  )
      {
        GameSys::GameData *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
        mystatus->SetPokemonParent( clone_param, DebugPokeMake::DeviceHeap() );
      }
    }

    this->StartAnimation( m_lytWk_Upper, UPPER_ANIME_INFO_SCROLL );
    this->UpdateAllOnDisplay( clone_param );

    PrintDebugMakeFlag( clone_param );
  }

  /**
   * @brief レンダリングパイプラインを生成する
   */
  void PokeMakeCore::CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap )
  {
    m_renderingPipeLine = GFL_NEW( systemHeap ) AppRenderingPipeLine( deviceHeap, 512, 256 );
  }



  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  PokeMakeCore::~PokeMakeCore()
  {
    GFL_SAFE_DELETE( m_wordset );
    GFL_SAFE_DELETE( m_msgData );
    GFL_DELETE m_num_input;
    GFL_DELETE m_keyboard_buf;
    GFL_DELETE m_button_manager;

    this->DeleteRenderingPipeLine();

    gfl2::lyt::LytSys* layout_system = DebugPokeMake::LayoutSystem();
    {
      if( m_lytWk )
      {
        layout_system->RemoveLytWk( m_lytWk );
        m_lytWk = NULL;
      }

      if( m_lytWk_Upper )
      {
        layout_system->RemoveLytWk( m_lytWk_Upper );
        m_lytWk_Upper = NULL;
      }

      layout_system->ReleaseLytRes( &m_lytResID );
    }
    GflHeapSafeFreeMemory( m_lytDataBuffer );
  }

  /**
   * @brief レンダリングパイプラインを削除する
   */
  void PokeMakeCore::DeleteRenderingPipeLine( void )
  {
    GFL_SAFE_DELETE( m_renderingPipeLine );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief プロセス更新関数
   * @retval true  終了
   * @retval false 継続中
   */
  //---------------------------------------------------------------------------
  bool PokeMakeCore::Update( void )
  {
    const gfl2::ui::Button* button = this->GetButton();

    switch( m_seq )
    {
    case SEQ_FADEIN:
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
      m_seq = SEQ_FADEIN_WAIT;
      break;

    case SEQ_FADEIN_WAIT:
      if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) == false ){
        break;
      }
      m_seq = SEQ_WAIT;
      break;

    case SEQ_WAIT:  // 入力待ち
      m_button_manager->Update();
      if( button->IsTrigger( gfl2::ui::BUTTON_L ) ||
          button->IsTrigger( gfl2::ui::BUTTON_LEFT ) ) {
        if( 0 < m_page ) {
          m_page_old = m_page;
          m_page--;
        }
      }
      else if(
          button->IsTrigger( gfl2::ui::BUTTON_R ) ||
          button->IsTrigger( gfl2::ui::BUTTON_RIGHT ) ) {
        m_page_old = m_page;
        m_page++;
      }
      else if( button->IsTrigger( gfl2::ui::BUTTON_START ) ) {
        m_makeResult = true;           // 作成したポケモンを適用
        m_seq = SEQ_EXIT;
      }
      else if( button->IsTrigger( gfl2::ui::BUTTON_B ) ) {
        m_makeResult = false;          // 作成したポケモンをキャンセル
        m_seq = SEQ_EXIT;
      }
      break;


    case SEQ_NUM_INPUT:  // 数値入力中
      {
        NumInput::Result input_result = m_num_input->Update();
        switch( input_result ) {
        case NumInput::RES_DECIDE:
          this->OnNumberInputFinished( m_number_input_target, DebugPokeMake::ClonePokemonParam(), DebugPokeMake::CloneDefaultPowerUpDesc() );
          m_seq = SEQ_WAIT;
          break;
        case NumInput::RES_CANCEL:
          m_seq = SEQ_WAIT;
          break;
        }
      }
      break;


    case SEQ_KEYBOARD:  // デバッグキーボードから復帰
      this->OnStringInputFinished(
          m_keyboad_input_target, m_keyboard_buf, DebugPokeMake::ClonePokemonParam() );
      m_seq = SEQ_WAIT;
      break;


    case SEQ_EXIT:
      m_isDrawEnable = false;
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestDefaultOut( gfl2::Fade::DISP_DOUBLE );
      m_seq = SEQ_EXIT_WAIT;
      break;

    case SEQ_EXIT_WAIT:
      if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE) == false ||
          this->IsDrawing() ) {
        break;
      }
      return true;

    default:
      GFL_ASSERT(0);
      break;
    }

    this->UpdatePage();
    m_lytWk->Update();
    m_lytWk_Upper->Update();

    if( m_renderingPipeLine ) {
      m_renderingPipeLine->Update();
    }
    return false;
  }


  //---------------------------------------------------------------------------
  /**
   * @brief 編集結果を反映させるか？
   * @retval true  反映させる
   * @retval false させない
   */
  //---------------------------------------------------------------------------
  bool PokeMakeCore::GetResult( void )
  {
    return m_makeResult;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 描画中か？
   * @retval true   描画中
   * @retval false  描画中でない
   */
  //---------------------------------------------------------------------------
  bool PokeMakeCore::IsDrawing( void ) const
  {
    if( ( m_lytWk != NULL ) &&
        ( m_lytWk->IsDrawing() ) ) {
      return true;
    }

    if( ( m_lytWk_Upper != NULL ) &&
        ( m_lytWk_Upper->IsDrawing() ) ) {
      return true;
    }

    if( ( m_num_input->GetLayoutWork() != NULL ) &&
        ( m_num_input->GetLayoutWork()->IsDrawing() ) ) {
      return true;
    }

    return false;
  }


  /**
   * @brief ページを更新する
   */
  void PokeMakeCore::UpdatePage( void )
  {
    static const u32 page_table[]={
      PANENAME_PAGE1_PANE_PAGE0,
      PANENAME_PAGE1_PANE_PAGE1,
      PANENAME_PAGE1_PANE_PAGE2,
      PANENAME_PAGE1_PANE_PAGE3,
      PANENAME_PAGE1_PANE_BACK_04,
      PANENAME_PAGE1_PANE_PAGE5,
      PANENAME_PAGE1_PANE_PAGE6,
      // ページが増えたらここに足す事。page8まではいけるはず
    };

    this->StartAnimation( m_lytWk_Upper, UPPER_ANIME_PAGE_INDEX );
    this->PauseAnimation( m_lytWk_Upper, UPPER_ANIME_PAGE_INDEX, false );
    this->SetAnimationFrame( m_lytWk_Upper, UPPER_ANIME_PAGE_INDEX, m_page );

    gfl2::math::VEC3 now_pos;
    gfl2::math::VEC3 fix_pos( -320 * m_page, 0, 0 );
    this->GetLocalPos( m_lytWk, PANENAME_PAGE1_PANE_PAGE_ROOT, &now_pos );

    gfl2::math::VEC3 move =  fix_pos - now_pos;
    gfl2::math::VEC3 next_pos = now_pos + ( move * 0.5f );
    this->SetLocalPos( m_lytWk, PANENAME_PAGE1_PANE_PAGE_ROOT, next_pos );

    // 今のページ以外を描画OFFにする
    for(int i=0;i<GFL_NELEMS(page_table);i++){
      if(i==m_page || i==m_page_old){
        SetPaneVisibility( m_lytWk, page_table[i], true );
      }else{
        SetPaneVisibility( m_lytWk, page_table[i], false );
      }
    }
  }










  /**
   * @brief ボタンが選択された際のコールバック関数
   * @param selected_button  選択されたボタン
   */
  void PokeMakeCore::CallbackFuncOnButtonSelected(
      app::tool::ButtonManager::ButtonId selected_button )
  {
    pml::pokepara::PokemonParam* clone_param = DebugPokeMake::ClonePokemonParam();
    DefaultPowerUpDesc* clone_defaultPowerUpDesc = DebugPokeMake::CloneDefaultPowerUpDesc();
    bool keyboard_call = false;
    NuminputParam num_param;
    s32 messageID = -1;
    int wordMax   = 0;

    GFL_PRINT("select = %d\n", selected_button);

    switch( selected_button )
    {
    case BUTTON_ARROW_MONSNO_L:
      {
        MonsNo old_monsno = clone_param->GetMonsNo();
        MonsNo new_monsno = static_cast<MonsNo>( ( old_monsno + MONSNO_END ) % ( MONSNO_END + 1 ) );
        u8 level = clone_param->GetLevel();          // 変更前のレベルを保存
        clone_param->ChangeMonsNo( new_monsno, 0 );  // フォルムは0にする
        clone_param->SetExp(0);                      // 変更後に一度レベル1に戻して変更前のレベルに引き上げる
        clone_param->LevelUp( level-1 );
        if( old_monsno == MONSNO_NULL ) {  // MONSNO_NULLでなくなった際にレベル1にする
          clone_param->SetExp(0);
          clone_param->RecoverHpFull();
          clone_param->SetDefaultWaza();
          clone_param->SetNotRare();
        }
        this->UpdateAllOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_MONSNAME, new_monsno );
      }
      break;

    case BUTTON_ARROW_MONSNO_R:
      {
        MonsNo old_monsno = clone_param->GetMonsNo();
        MonsNo new_monsno = static_cast<MonsNo>( ( old_monsno + 1 ) % ( MONSNO_END + 1 ) );
        u8 level = clone_param->GetLevel();          // 変更前のレベルを保存
        clone_param->ChangeMonsNo( new_monsno, 0 );  // フォルムは0にする
        clone_param->SetExp(0);                      // 変更後に一度レベル1に戻して変更前のレベルに引き上げる
        clone_param->LevelUp( level-1 );
        if( old_monsno == MONSNO_NULL ) {  // MONSNO_NULLでなくなった際にレベル1にする
          clone_param->SetExp(0);
          clone_param->RecoverHpFull();
          clone_param->SetDefaultWaza();
          clone_param->SetNotRare();
          pml::personal::LoadGrowTable( clone_param->GetMonsNo(), clone_param->GetFormNo() );
        }
        this->UpdateAllOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_MONSNAME, new_monsno );
      }
      break;

    case BUTTON_ARROW_FORMNO_L:
      {
        MonsNo monsno = clone_param->GetMonsNo();
        pml::FormNo old_formno = clone_param->GetFormNo();
        pml::personal::LoadPersonalData( monsno, old_formno );
        u32 form_num = GetFormMax( clone_param );
        if( 1 < form_num ) {
          pml::FormNo new_formno = ( old_formno + form_num - 1 ) % form_num;
          clone_param->ChangeFormNo( new_formno );
          this->UpdateMonsNameOnDisplay( clone_param );
          this->UpdateFormNoOnDisplay( clone_param );
        }
      }
      break;

    case BUTTON_ARROW_FORMNO_R:
      {
        MonsNo monsno = clone_param->GetMonsNo();
        pml::FormNo old_formno = clone_param->GetFormNo();
        pml::personal::LoadPersonalData( monsno, old_formno );
        u32 form_num = GetFormMax( clone_param );
        if( 1 < form_num ) {
          pml::FormNo new_formno = ( old_formno + 1 ) % form_num;
          clone_param->ChangeFormNo( new_formno );
          this->UpdateMonsNameOnDisplay( clone_param );
          this->UpdateFormNoOnDisplay( clone_param );
        }
      }
      break;

    case BUTTON_ARROW_SEIKAKU_L:
      {
        pml::pokepara::Seikaku old_seikaku = clone_param->GetSeikaku();
        u32 new_seikaku_value = ( old_seikaku + pml::pokepara::SEIKAKU_NUM - 1 ) % pml::pokepara::SEIKAKU_NUM;
        pml::pokepara::Seikaku new_seikaku = static_cast<pml::pokepara::Seikaku>( new_seikaku_value );
        clone_param->ChangeSeikaku( new_seikaku );
        this->UpdateSeikakuOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_SEIKAKU, new_seikaku );
      }
      break;

    case BUTTON_ARROW_SEIKAKU_R:
      {
        pml::pokepara::Seikaku old_seikaku = clone_param->GetSeikaku();
        u32 new_seikaku_value = ( old_seikaku + 1 ) % pml::pokepara::SEIKAKU_NUM;
        pml::pokepara::Seikaku new_seikaku = static_cast<pml::pokepara::Seikaku>( new_seikaku_value );
        clone_param->ChangeSeikaku( new_seikaku );
        this->UpdateSeikakuOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_SEIKAKU, new_seikaku );
      }
      break;

    case BUTTON_ARROW_ITEM_L:
      {
        u32 old_itemno = clone_param->GetItem();
        u32 new_itemno = ( old_itemno + ITEM_DATA_MAX ) % ( ITEM_DATA_MAX + 1 );
        clone_param->SetItem( new_itemno );
        this->UpdateItemOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_ITEM, new_itemno );
      }
      break;

    case BUTTON_ARROW_ITEM_R:
      {
        u32 old_itemno = clone_param->GetItem();
        u32 new_itemno = ( old_itemno + 1 ) % ( ITEM_DATA_MAX + 1 );
        clone_param->SetItem( new_itemno );
        this->UpdateItemOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_ITEM, new_itemno );
      }
      break;

    case BUTTON_ARROW_TOKUSEI_L:
      {
        TokuseiNo old_tokusei_no = clone_param->GetTokuseiNo();
        TokuseiNo new_tokusei_no = static_cast<TokuseiNo>( ( old_tokusei_no + TOKUSEI_NUM - 1 ) % TOKUSEI_NUM );
        clone_param->OverwriteTokuseiNo( new_tokusei_no );
        CheckTokuseiNo( clone_param );
        this->UpdateTokuseiOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_TOKUSEI, new_tokusei_no );
      }
      break;

    case BUTTON_ARROW_TOKUSEI_R:
      {
        TokuseiNo old_tokusei_no = clone_param->GetTokuseiNo();
        TokuseiNo new_tokusei_no = static_cast<TokuseiNo>( ( old_tokusei_no + 1 ) % TOKUSEI_NUM );
        clone_param->OverwriteTokuseiNo( new_tokusei_no );
        this->UpdateTokuseiOnDisplay( clone_param );
        CheckTokuseiNo( clone_param );
        this->StartNumberDisplayAnime( STRBOX_TOKUSEI, new_tokusei_no );
      }
      break;

    case BUTTON_SWITCH_SEX:
      {
        pml::personal::SexType sex_type = clone_param->GetSexType();
        if( sex_type == pml::personal::SEX_TYPE_RANDOM ) {
          pml::Sex old_sex = clone_param->GetSex();
          pml::Sex new_sex = ( old_sex == pml::SEX_MALE ) ? pml::SEX_FEMALE : pml::SEX_MALE;
          clone_param->ChangeSex( new_sex );
          this->UpdateSexOnDisplay( clone_param );
        }
      }
      break;

    case BUTTON_SWITCH_SICK:
      {
        pml::pokepara::Sick old_sick = clone_param->GetSick();
        pml::pokepara::Sick new_sick = static_cast<pml::pokepara::Sick>( ( old_sick + 1 ) % pml::pokepara::SICK_MAX );
        clone_param->SetSick( new_sick );
        this->UpdateSickOnDisplay( clone_param );
      }
      break;

    case BUTTON_SWITCH_RND_MAX:
      {
        for(int i=0;i<pml::pokepara::POWER_NUM;i++){
          clone_param->ChangeTalentPower( static_cast<pml::pokepara::PowerID>(i), 31 );
        }
        this->UpdateHpOnDisplay( clone_param );
        this->UpdateAtkOnDisplay( clone_param );
        this->UpdateDefOnDisplay( clone_param );
        this->UpdateAgiOnDisplay( clone_param );
        this->UpdateSpatkOnDisplay( clone_param );
        this->UpdateSpdefOnDisplay( clone_param );
      }
      break;

    case BUTTON_SWITCH_TOKUSEI1:
      clone_param->SetTokuseiIndex( 0 );
      this->UpdateTokuseiOnDisplay( clone_param );
      this->UpdateTokuseiIndexSwitchTrans( clone_param );
      break;

    case BUTTON_SWITCH_TOKUSEI2:
      clone_param->SetTokuseiIndex( 1 );
      this->UpdateTokuseiOnDisplay( clone_param );
      this->UpdateTokuseiIndexSwitchTrans( clone_param );
      break;

    case BUTTON_SWITCH_TOKUSEI3:
      clone_param->SetTokuseiIndex( 2);
      this->UpdateTokuseiOnDisplay( clone_param );
      this->UpdateTokuseiIndexSwitchTrans( clone_param );
      break;

    case BUTTON_SWITCH_EGG:
      if( clone_param->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG ) ) {
        GFL_PRINT("ダメタマゴだから何もしない");
      }
      else if( clone_param->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) ) {
        clone_param->Birth();
      }
      else {
        clone_param->SetEggFlag();
      }
      this->UpdateEggOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_RARE:
      if( clone_param->IsRare() ) {
        clone_param->SetNotRare();
      }
      else {
        clone_param->SetRare();
      }
      this->UpdateRareOnDisplay( clone_param );
      this->UpdateRareRandomDisplay( clone_param );
      break;

    case BUTTON_ARROW_WAZA1_L:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(0);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 0, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 0 );
        this->StartNumberDisplayAnime( STRBOX_WAZA1, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA1_R:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(0);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 0, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 0 );
        this->StartNumberDisplayAnime( STRBOX_WAZA1, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA2_L:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(1);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 1, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 1 );
        this->StartNumberDisplayAnime( STRBOX_WAZA2, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA2_R:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(1);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 1, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 1 );
        this->StartNumberDisplayAnime( STRBOX_WAZA2, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA3_L:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(2);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 2, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 2 );
        this->StartNumberDisplayAnime( STRBOX_WAZA3, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA3_R:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(2);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 2, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 2 );
        this->StartNumberDisplayAnime( STRBOX_WAZA3, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA4_L:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(3);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 3, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 3 );
        this->StartNumberDisplayAnime( STRBOX_WAZA4, new_wazano );
      }
      break;

    case BUTTON_ARROW_WAZA4_R:
      {
        WazaNo old_wazano = clone_param->GetWazaNo(3);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetWaza( 3, new_wazano );
        this->UpdateWazaOnDisplay( clone_param, 3 );
        this->StartNumberDisplayAnime( STRBOX_WAZA4, new_wazano );
      }
      break;

    case BUTTON_SWITCH_DEL_WAZA2:
      clone_param->RemoveWaza(1);
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_DEL_WAZA3:
      clone_param->RemoveWaza(2);
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_DEL_WAZA4:
      clone_param->RemoveWaza(3);
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_DEFAULT_WAZA:
      clone_param->SetDefaultWaza();
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_HATAKU:
      for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ ) {
        clone_param->RemoveWaza(i);
      }
      clone_param->SetWaza( 0, WAZANO_HATAKU );
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_HANERU:
      for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ ) {
        clone_param->RemoveWaza(i);
      }
      clone_param->SetWaza( 0, WAZANO_HANERU );
      this->UpdateAllWazaOnDisplay( clone_param );
      break;

    case BUTTON_NUMBOX_FORMNO:
      {
        MonsNo monsno = clone_param->GetMonsNo();
        pml::FormNo formno = clone_param->GetFormNo();
        pml::personal::LoadPersonalData( monsno, formno );
        u32 form_num = GetFormMax( clone_param );
        num_param.input = true;
        num_param.init = formno;
        num_param.min = 0;
        num_param.max = form_num - 1;
        num_param.title = L"フォルムNo.";
      }
      break;

    case BUTTON_NUMBOX_LEVEL:
      {
        u32 level = clone_param->GetLevel();
        num_param.input = true;
        num_param.init = level;
        num_param.min = 1;
        num_param.max = pml::MAX_POKE_LEVEL;
        num_param.title = L"レベル";
      }
      break;

    case BUTTON_NUMBOX_EXP:
      {
        u32 exp = clone_param->GetExp();
        num_param.input = true;
        num_param.init = exp;
        num_param.min = 0;
        num_param.max = 9999999;
        num_param.title = L"経験値";
      }
      break;

    case BUTTON_NUMBOX_HP_RND:
      {
        u32 hp_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_HP );
        num_param.input = true;
        num_param.init = hp_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"ＨＰ　個体値";
      }
      break;

    case BUTTON_NUMBOX_ATK_RND:
      {
        u32 atk_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_ATK );
        num_param.input = true;
        num_param.init = atk_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"こうげき　個体値";
      }
      break;

    case BUTTON_NUMBOX_DEF_RND:
      {
        u32 def_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_DEF );
        num_param.input = true;
        num_param.init = def_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"ぼうぎょ　個体値";
      }
      break;

    case BUTTON_NUMBOX_AGI_RND:
      {
        u32 agi_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_AGI );
        num_param.input = true;
        num_param.init = agi_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"すばやさ　個体値";
      }
      break;

    case BUTTON_NUMBOX_SPATK_RND:
      {
        u32 spatk_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_SPATK );
        num_param.input = true;
        num_param.init = spatk_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"とくこう　個体値";
      }
      break;

    case BUTTON_NUMBOX_SPDEF_RND:
      {
        u32 spdef_rnd = clone_param->GetNativeTalentPower( pml::pokepara::POWER_SPDEF );
        num_param.input = true;
        num_param.init = spdef_rnd;
        num_param.min = 0;
        num_param.max = pml::MAX_TALENT_POWER;
        num_param.title = L"とくぼう　個体値";
      }
      break;

    case BUTTON_NUMBOX_HP_EXP:
      {
        u32 hp_exp = clone_param->GetEffortPower( pml::pokepara::POWER_HP );
        num_param.input = true;
        num_param.init = hp_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"ＨＰ　努力値";
      }
      break;

    case BUTTON_NUMBOX_HP_VAL:
      {
        u32 max_hp = clone_param->GetPower( pml::pokepara::POWER_HP );
        u32 current_hp = clone_param->GetHp();
        num_param.input = true;
        num_param.init = current_hp;
        num_param.min = 0;
        num_param.max = max_hp;
        num_param.title = L"ＨＰ　現在値";
      }
      break;

    case BUTTON_NUMBOX_ATK_EXP:
      {
        u32 atk_exp = clone_param->GetEffortPower( pml::pokepara::POWER_ATK );
        num_param.input = true;
        num_param.init = atk_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"こうげき　努力値";
      }
      break;

    case BUTTON_NUMBOX_DEF_EXP:
      {
        u32 def_exp = clone_param->GetEffortPower( pml::pokepara::POWER_DEF );
        num_param.input = true;
        num_param.init = def_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"ぼうぎょ　努力値";
      }
      break;

    case BUTTON_NUMBOX_AGI_EXP:
      {
        u32 agi_exp = clone_param->GetEffortPower( pml::pokepara::POWER_AGI );
        num_param.input = true;
        num_param.init = agi_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"すばやさ　努力値";
      }
      break;


    case BUTTON_NUMBOX_SPATK_EXP:
      {
        u32 spatk_exp = clone_param->GetEffortPower( pml::pokepara::POWER_SPATK );
        num_param.input = true;
        num_param.init = spatk_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"とくこう　努力値";
      }
      break;

    case BUTTON_NUMBOX_SPDEF_EXP:
      {
        u32 spdef_exp = clone_param->GetEffortPower( pml::pokepara::POWER_SPDEF );
        num_param.input = true;
        num_param.init = spdef_exp;
        num_param.min = 0;
        num_param.max = pml::MAX_EFFORT_POWER;
        num_param.title = L"とくぼう　努力値";
      }
      break;

    case BUTTON_NUMBOX_WAZA1_UP:
      {
        u32 up_count = clone_param->GetWazaPPUpCount(0);
        num_param.input = true;
        num_param.init = up_count;
        num_param.min = 0;
        num_param.max = pml::MAX_WAZAPP_UPCOUNT;
        num_param.title = L"ポイントアップ使用回数";
      }
      break;

    case BUTTON_NUMBOX_WAZA2_UP:
      {
        u32 up_count = clone_param->GetWazaPPUpCount(1);
        num_param.input = true;
        num_param.init = up_count;
        num_param.min = 0;
        num_param.max = pml::MAX_WAZAPP_UPCOUNT;
        num_param.title = L"ポイントアップ使用回数";
      }
      break;

    case BUTTON_NUMBOX_WAZA3_UP:
      {
        u32 up_count = clone_param->GetWazaPPUpCount(2);
        num_param.input = true;
        num_param.init = up_count;
        num_param.min = 0;
        num_param.max = pml::MAX_WAZAPP_UPCOUNT;
        num_param.title = L"ポイントアップ使用回数";
      }
      break;

    case BUTTON_NUMBOX_WAZA4_UP:
      {
        u32 up_count = clone_param->GetWazaPPUpCount(3);
        num_param.input = true;
        num_param.init = up_count;
        num_param.min = 0;
        num_param.max = pml::MAX_WAZAPP_UPCOUNT;
        num_param.title = L"ポイントアップ使用回数";
      }
      break;

    case BUTTON_NUMBOX_WAZA1_PP:
      {
        WazaNo wazano = clone_param->GetWazaNo(0);
        u32 pp_count = clone_param->GetWazaPP(0);
        u32 up_count = clone_param->GetWazaPPUpCount(0);
        u32 max_pp_count = pml::wazadata::GetMaxPP( wazano, up_count );
        num_param.input = true;
        num_param.init = pp_count;
        num_param.min = 0;
        num_param.max = max_pp_count;
        num_param.title = L"のこりＰＰ";
      }
      break;

    case BUTTON_NUMBOX_WAZA2_PP:
      {
        WazaNo wazano = clone_param->GetWazaNo(1);
        u32 pp_count = clone_param->GetWazaPP(1);
        u32 up_count = clone_param->GetWazaPPUpCount(1);
        u32 max_pp_count = pml::wazadata::GetMaxPP( wazano, up_count );
        num_param.input = true;
        num_param.init = pp_count;
        num_param.min = 0;
        num_param.max = max_pp_count;
        num_param.title = L"のこりＰＰ";
      }
      break;

    case BUTTON_NUMBOX_WAZA3_PP:
      {
        WazaNo wazano = clone_param->GetWazaNo(2);
        u32 pp_count = clone_param->GetWazaPP(2);
        u32 up_count = clone_param->GetWazaPPUpCount(2);
        u32 max_pp_count = pml::wazadata::GetMaxPP( wazano, up_count );
        num_param.input = true;
        num_param.init = pp_count;
        num_param.min = 0;
        num_param.max = max_pp_count;
        num_param.title = L"のこりＰＰ";
      }
      break;

    case BUTTON_NUMBOX_WAZA4_PP:
      {
        WazaNo wazano = clone_param->GetWazaNo(3);
        u32 pp_count = clone_param->GetWazaPP(3);
        u32 up_count = clone_param->GetWazaPPUpCount(3);
        u32 max_pp_count = pml::wazadata::GetMaxPP( wazano, up_count );
        num_param.input = true;
        num_param.init = pp_count;
        num_param.min = 0;
        num_param.max = max_pp_count;
        num_param.title = L"のこりＰＰ";
      }
      break;

    case BUTTON_NUMBOX_FAMILY:
      num_param.input = true;
      num_param.init = clone_param->GetOriginalFamiliarity();
      num_param.min = 0;
      num_param.max = pml::MAX_FAMILIARITY;
      num_param.title = L"なつき度";
      break;

    case BUTTON_NUMBOX_FRIEND:
      num_param.input = true;
      num_param.init = clone_param->GetFriendship();
      num_param.min = 0;
      num_param.max = pml::MAX_FRIENDSHIP;
      num_param.title = L"なかよし度";
      break;

    case BUTTON_NUMBOX_FEED:
      num_param.input = true;
      num_param.init = clone_param->GetFeed();
      num_param.min = 0;
      num_param.max = pml::MAX_FEED;
      num_param.title = L"まんぷく度";
      break;

    case BUTTON_NUMBOX_NADENADE:
      num_param.input = true;
      num_param.init = clone_param->GetNadenadeValue();
      num_param.min = 0;
      num_param.max = pml::MAX_NADENADE_VALUE;
      num_param.title = L"なでなで値";
      break;

    case BUTTON_STRBOX_MONSNAME:
      keyboard_call = true;
      messageID = GARC_message_monsname_DAT;
      wordMax   = MONSNO_END+1;
      break;

    case BUTTON_STRBOX_NICKNAME:
      keyboard_call = true;
      wordMax   = 0;
      break;

    case BUTTON_STRBOX_SEIKAKU:
      keyboard_call = true;
      messageID = GARC_message_seikaku_DAT;
      wordMax   = pml::pokepara::SEIKAKU_NUM;
      break;

    case BUTTON_STRBOX_ITEM:
      keyboard_call = true;
      messageID = GARC_message_itemname_DAT;
      wordMax   = ITEM_DATA_MAX+1;
      break;

    case BUTTON_STRBOX_TOKUSEI:
      keyboard_call = true;
      messageID = GARC_message_tokusei_DAT;
      wordMax   = TOKUSEI_MAX+1;
      break;

    case BUTTON_STRBOX_WAZA1:
    case BUTTON_STRBOX_WAZA2:
    case BUTTON_STRBOX_WAZA3:
    case BUTTON_STRBOX_WAZA4:
      keyboard_call = true;
      messageID = GARC_message_wazaname_DAT;
      wordMax   = WAZANO_MAX;
      break;

    case BUTTON_ARROW_VERSION_L:
      {
        u32 old_version = clone_param->GetCassetteVersion();
        u32 new_version = ( old_version + GFL_NELEMS(VERSION_NAME) - 1 ) % GFL_NELEMS(VERSION_NAME);
        clone_param->SetCassetteVersion( new_version );
        this->UpdateVersionOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_VERSION, new_version );
      }
      break;
    case BUTTON_ARROW_VERSION_R:
      {
        u32 old_version = clone_param->GetCassetteVersion();
        u32 new_version = ( old_version + GFL_NELEMS(VERSION_NAME) + 1 ) % GFL_NELEMS(VERSION_NAME);
        clone_param->SetCassetteVersion( new_version );
        this->UpdateVersionOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_VERSION, new_version );
      }
      break;
    case BUTTON_ARROW_LANGUAGE_L:
      {
        u32 old_lang = clone_param->GetLangId();
        u32 new_lang = ( old_lang + GFL_NELEMS(LANGUAGE_NAME) - 1 ) % GFL_NELEMS(LANGUAGE_NAME);
        clone_param->SetLangId( new_lang );
        this->UpdateLanguageOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_LANGUAGE, new_lang );
      }
      break;
    case BUTTON_ARROW_LANGUAGE_R:
      {
        u32 old_lang = clone_param->GetLangId();
        u32 new_lang = ( old_lang + GFL_NELEMS(LANGUAGE_NAME) + 1 ) % GFL_NELEMS(LANGUAGE_NAME);
        clone_param->SetLangId( new_lang );
        this->UpdateLanguageOnDisplay( clone_param );
        this->StartNumberDisplayAnime( STRBOX_LANGUAGE, new_lang );
      }
      break;

    case BUTTON_ARROW_CAPTUREBALL_L:
      {
        int new_ball_id = clone_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL ) ;
        if(++new_ball_id > item::BALLID_MAX){ new_ball_id = 1; }
        clone_param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, new_ball_id );
        this->UpdateCaptureBallOnDisplay( clone_param );
      }
      break;
    case BUTTON_ARROW_CAPTUREBALL_R:
      {
        int new_ball_id = static_cast<item::BALL_ID>(clone_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL ) );
        if(--new_ball_id<1){ new_ball_id = item::BALLID_MAX; }
        clone_param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, new_ball_id );
        this->UpdateCaptureBallOnDisplay( clone_param );
      }
      break;
    case BUTTON_SWITCH_OYA_SET:
      {
        GameSys::GameData *game_data = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData();
        Savedata::MyStatus *mystatus = game_data->GetPlayerStatus();
        mystatus->SetPokemonParent( clone_param, DebugPokeMake::DeviceHeap() );
        this->UpdateOyaNameOnDisplay( clone_param );
        this->UpdateOyaSexOnDisplay( clone_param );
      }
      break;
    case BUTTON_SWITCH_OYA_SEX:
      {
        pml::Sex old_sex = clone_param->GetParentSex();
        pml::Sex new_sex = ( old_sex == pml::SEX_MALE ) ? pml::SEX_FEMALE : pml::SEX_MALE;
        clone_param->SetParentSex( new_sex );
        this->UpdateOyaSexOnDisplay( clone_param );
      }
      break;
    case BUTTON_SWITCH_EVENT:
      {
        bool is_event = clone_param->GetEventPokeFlag();
        clone_param->SetEventPokeFlag( !is_event );
        this->UpdateEventOnDisplay( clone_param );
      }
      break;
    case BUTTON_STRBOX_VERSION:
      keyboard_call = true;
      wordMax   = 0;
      break;
    case BUTTON_STRBOX_LANGUAGE:
      keyboard_call = true;
      wordMax   = 0;
      break;
    case BUTTON_STRBOX_OYANAME:
      keyboard_call = true;
      wordMax   = 0;
      break;
    case BUTTON_STRBOX_CAPTURE_BALL:
      keyboard_call = true;
      wordMax   = 0;
      break;
    case BUTTON_NUMBOX_POKERUS:
      num_param.input = true;
      num_param.init = clone_param->GetPokerus();
      num_param.min = 0;
      num_param.max = 0x100;
      num_param.title = L"ポケルス";
      break;
    case BUTTON_NUMBOX_CAPTURE_LEVEL:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL );
      num_param.min = 0;
      num_param.max = pml::MAX_POKE_LEVEL;
      num_param.title = L"捕獲レベル";
      break;
    case BUTTON_NUMBOX_CONTACT_PLACE:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE );
      num_param.min = 0;
      num_param.max = PokeTool::PokePlace::PLACE_NO_END;
      num_param.title = L"出会った／タマゴがふかした場所";
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_PLACE:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE );
      num_param.min = 0;
      num_param.max = PokeTool::PokePlace::PLACE_NO_END;
      num_param.title = L"タマゴを貰った場所";
      break;
    case BUTTON_NUMBOX_CONTACT_DAY:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY );
      num_param.min = 0;
      num_param.max = 31;
      num_param.title = L"出会った／タマゴをふかした日";
      break;
    case BUTTON_NUMBOX_CONTACT_MONTH:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH );
      num_param.min = 0;
      num_param.max = 12;
      num_param.title = L"出会った／タマゴをふかした月";
      break;
    case BUTTON_NUMBOX_CONTACT_YEAR:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR );
      num_param.min = 0;
      num_param.max = 99;
      num_param.title = L"出会った／タマゴをふかした年";
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_DAY:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY );
      num_param.min = 0;
      num_param.max = 31;
      num_param.title = L"タマゴをもらった日";
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_MONTH:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH );
      num_param.min = 0;
      num_param.max = 12;
      num_param.title = L"タマゴをもらった月";
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_YEAR:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR );
      num_param.min = 0;
      num_param.max = 99;
      num_param.title = L"タマゴをもらった年";
      break;
    case BUTTON_NUMBOX_MEMORY_CODE_OYA:
      //@
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親との思い出事象コード";
      break;
    case BUTTON_NUMBOX_MEMORY_LEVEL_OYA:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT );
      num_param.min = 0;
      num_param.max = 7;
      num_param.title = L"親との思い出レベル";
      break;
    case BUTTON_NUMBOX_MEMORY_DATA_OYA:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_PARENT );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親との思い出データ";
      break;
    case BUTTON_NUMBOX_MEMORY_FEEL_OYA:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_PARENT );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親との思い出気持ち";
      break;
    case BUTTON_NUMBOX_MEMORY_CODE_OTHER:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_OTHERS );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親以外との思い出事象コード";
      break;
    case BUTTON_NUMBOX_MEMORY_LEVEL_OTHER:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_OTHERS );
      num_param.min = 0;
      num_param.max = 7;
      num_param.title = L"親以外との思い出レベル";
      break;
    case BUTTON_NUMBOX_MEMORY_DATA_OTHER:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_OTHERS );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親以外との思い出データ";
      break;
    case BUTTON_NUMBOX_MEMORY_FEEL_OTHER:
      num_param.input = true;
      num_param.init = clone_param->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_OTHERS );
      num_param.min = 0;
      num_param.max = 999;
      num_param.title = L"親以外との思い出気持ち";
      break;

    case BUTTON_ARROW_REGION_L:
      {
        u32 old_id = clone_param->GetHWRegionCode();
        u32 new_id = ( old_id + REGION_MAX - 1 ) % REGION_MAX;
        clone_param->SetHWRegionCode( new_id );
        this->UpdateRegion( clone_param );
        this->StartNumberDisplayAnime( STRBOX_REGION, new_id );
      }
      break;
    case BUTTON_ARROW_REGION_R:
      {
        u32 old_id = clone_param->GetHWRegionCode();
        u32 new_id = ( old_id + REGION_MAX + 1 ) % REGION_MAX;
        clone_param->SetHWRegionCode( new_id );
        this->UpdateRegion( clone_param );
        this->StartNumberDisplayAnime( STRBOX_REGION, new_id );
      }
      break;
    case BUTTON_STRBOX_REGION:
      keyboard_call = true;
      break;

    case BUTTON_NUMBOX_COUNTRY:
      num_param.input = true;
      num_param.init = clone_param->GetCountryCode();
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"国コード(CountryCode)";
      break;
    case BUTTON_NUMBOX_COUNTRY_AREA:
      num_param.input = true;
      num_param.init = clone_param->GetCountryRegionCode();
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"地域コード(CoungryRegionCode)";
      break;

    case BUTTON_NUMBOX_OTHER_FAMILY:
      num_param.input = true;
      num_param.init = clone_param->GetOthersFamiliarity();
      num_param.min = 0;
      num_param.max = pml::MAX_FAMILIARITY;
      num_param.title = L"他の親とのなつき度";
      break;

    case BUTTON_NUMBOX_OTHER_FRIEND:
      num_param.input = true;
      num_param.init = clone_param->GetOthersFriendship();
      num_param.min = 0;
      num_param.max = pml::MAX_FRIENDSHIP;
      num_param.title = L"他の親とのなかよし度";
      break;

    case BUTTON_STRBOX_OTHER_OYANAME:
      keyboard_call = true;
      wordMax   = 0;
      break;

    case BUTTON_SWITCH_OTHER_OYASEX:
      {
        pml::Sex old_sex = clone_param->GetPastParentsSex();
        pml::Sex new_sex = ( old_sex == pml::SEX_MALE ) ? pml::SEX_FEMALE : pml::SEX_MALE;
        clone_param->SetPastParentsSex( new_sex );
        this->UpdatePastOyaSexOnDisplay( clone_param );
      }
      break;

    case BUTTON_NUMBOX_PAST_COUNTRY_00:   CallNumInputPastCountry( clone_param, 0, &num_param );     break;
    case BUTTON_NUMBOX_PAST_COUNTRY_01:   CallNumInputPastCountry( clone_param, 1, &num_param );     break;
    case BUTTON_NUMBOX_PAST_COUNTRY_02:   CallNumInputPastCountry( clone_param, 2, &num_param );     break;
    case BUTTON_NUMBOX_PAST_COUNTRY_03:   CallNumInputPastCountry( clone_param, 3, &num_param );     break;
    case BUTTON_NUMBOX_PAST_COUNTRY_04:   CallNumInputPastCountry( clone_param, 4, &num_param );     break;
    case BUTTON_NUMBOX_PAST_AREA_00:      CallNumInputPastArea( clone_param, 0, &num_param );      break;
    case BUTTON_NUMBOX_PAST_AREA_01:      CallNumInputPastArea( clone_param, 1, &num_param );      break;
    case BUTTON_NUMBOX_PAST_AREA_02:      CallNumInputPastArea( clone_param, 2, &num_param );      break;
    case BUTTON_NUMBOX_PAST_AREA_03:      CallNumInputPastArea( clone_param, 3, &num_param );      break;
    case BUTTON_NUMBOX_PAST_AREA_04:      CallNumInputPastArea( clone_param, 4, &num_param );      break;

    case BUTTON_NUMBOX_TRAINER_ID_00:
      {
        u32 id = clone_param->GetID()>>16;
        num_param.input = true;
        num_param.init = id;
        num_param.min = 0;
        num_param.max = 65535;
        num_param.title = L"トレーナーID（上位）";
      }
      break;
    case BUTTON_NUMBOX_TRAINER_ID_01:
        num_param.input = true;
        num_param.init = clone_param->GetID()&0xffff;
        num_param.min = 0;
        num_param.max = 65535;
        num_param.title = L"トレーナーID（下位）";
      break;

    case BUTTON_NUMBOX_TRIMIAN_HAIR:
        num_param.input = true;
        num_param.init = clone_param->TORIMIAN_GetBodyHairCount();
        num_param.min = 0;
        num_param.max = 5;
        num_param.title = L"体毛カウント";
      break;

    case BUTTON_NUMBOX_TRIMIAN_COUNT:
        num_param.input = true;
        num_param.init = clone_param->TORIMIAN_GetKeepDayCount();
        num_param.min = 0;
        num_param.max = 255;
        num_param.title = L"経過日数カウント";
      break;

    case BUTTON_NUMBOX_TRIMIAN_COUNT_MAX:
        num_param.input = true;
        num_param.init = clone_param->TORIMIAN_GetKeepMaxDayCount();
        num_param.min = 0;
        num_param.max = 255;
        num_param.title = L"経過日数MAX(トリミアンのみ代入可）";
      break;


    //ここからsango追加分
    case BUTTON_NUMBOX_CONDITION_01:
      num_param.input = true;
      num_param.init = clone_param->GetCondition(pml::pokepara::CONDITION_STYLE);
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"かっこよさ";
      break;
    case BUTTON_NUMBOX_CONDITION_02:
      num_param.input = true;
      num_param.init = clone_param->GetCondition(pml::pokepara::CONDITION_BEAUTIFUL);
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"うつくしさ";
      break;
    case BUTTON_NUMBOX_CONDITION_03:
      num_param.input = true;
      num_param.init = clone_param->GetCondition(pml::pokepara::CONDITION_CUTE);
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"かわいさ";
      break;
    case BUTTON_NUMBOX_CONDITION_04:
      num_param.input = true;
      num_param.init = clone_param->GetCondition(pml::pokepara::CONDITION_CLEVER);
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"かしこさ";
      break;
    case BUTTON_NUMBOX_CONDITION_05:
      num_param.input = true;
      num_param.init = clone_param->GetCondition(pml::pokepara::CONDITION_STRONG);
      num_param.min = 0;
      num_param.max = 255;
      num_param.title = L"たくましさ";
      break;

    case BUTTON_NUMBOX_EGG_WAZA1_L:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(0);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 0,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 0 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA1, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA1_R:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(0);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 0,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 0 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA1, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA1_NAME:
      keyboard_call = true;
      messageID = GARC_message_wazaname_DAT;
      wordMax   = WAZANO_MAX;
      break;

    case BUTTON_NUMBOX_EGG_WAZA2_L:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(1);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 1,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 1 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA2, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA2_R:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(1);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 1,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 1 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA2, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA2_NAME:
      keyboard_call = true;
      messageID = GARC_message_wazaname_DAT;
      wordMax   = WAZANO_MAX;
      break;

    case BUTTON_NUMBOX_EGG_WAZA3_L:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(2);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 2,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 2 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA3, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA3_R:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(2);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 2,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 2 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA3, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA3_NAME:
      keyboard_call = true;
      messageID = GARC_message_wazaname_DAT;
      wordMax   = WAZANO_MAX;
      break;

    case BUTTON_NUMBOX_EGG_WAZA4_L:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(3);
        u32 new_wazano_value = ( old_wazano + WAZANO_MAX - 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 3,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 3 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA4, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA4_R:
      {
        WazaNo old_wazano = clone_param->GetTamagoWazaNo(3);
        u32 new_wazano_value = ( old_wazano + 1 ) % WAZANO_MAX;
        WazaNo new_wazano = static_cast<WazaNo>( new_wazano_value );
        clone_param->SetTamagoWazaNo( 3,new_wazano );
        this->UpdateEggWazaOnDisplay( clone_param, 3 );
        this->StartNumberDisplayAnime( STRBOX_EGG_WAZA4, new_wazano );
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA4_NAME:
      keyboard_call = true;
      messageID = GARC_message_wazaname_DAT;
      wordMax   = WAZANO_MAX;
      break;

    case BUTTON_DEFAULT_POWERUP_REASON_L:
      {
        DefaultPowerUpReason old_reason = clone_defaultPowerUpDesc->reason;
        clone_defaultPowerUpDesc->reason =static_cast<DefaultPowerUpReason>( ( old_reason + DEFAULT_POWERUP_REASON_NUM - 1 ) % DEFAULT_POWERUP_REASON_NUM ); 
        UpdateDefaultPowerUpDisplay( *clone_defaultPowerUpDesc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_REASON_R:
      {
        DefaultPowerUpReason old_reason = clone_defaultPowerUpDesc->reason;
        clone_defaultPowerUpDesc->reason =static_cast<DefaultPowerUpReason>( ( old_reason + 1 ) % DEFAULT_POWERUP_REASON_NUM ); 
        UpdateDefaultPowerUpDisplay( *clone_defaultPowerUpDesc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_ATK:
      num_param.input = true;
      num_param.init  = clone_defaultPowerUpDesc->rankUp_Attack;
      num_param.min   = 0;
      num_param.max   = 3;
      num_param.title = L"ランク上昇値( こうげき )";
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_DEF:
      num_param.input = true;
      num_param.init  = clone_defaultPowerUpDesc->rankUp_Defense;
      num_param.min   = 0;
      num_param.max   = 3;
      num_param.title = L"ランク上昇値( ぼうぎょ )";
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_SPATK:
      num_param.input = true;
      num_param.init  = clone_defaultPowerUpDesc->rankUp_SpAttack;
      num_param.min   = 0;
      num_param.max   = 3;
      num_param.title = L"ランク上昇値( とくこう )";
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_SPDEF:
      num_param.input = true;
      num_param.init  = clone_defaultPowerUpDesc->rankUp_SpDefense;
      num_param.min   = 0;
      num_param.max   = 3;
      num_param.title = L"ランク上昇値( とくぼう )";
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_AGI:
      num_param.input = true;
      num_param.init  = clone_defaultPowerUpDesc->rankUp_Agility;
      num_param.min   = 0;
      num_param.max   = 3;
      num_param.title = L"ランク上昇値( すばやさ )";
      break;


    case BUTTON_SWITCH_TRAINING_FLAG_HP:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_HP );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateHpOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_TRAINING_FLAG_ATTACK:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_ATK );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateAtkOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_TRAINING_FLAG_DEFENSE:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_DEF );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateDefOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_TRAINING_FLAG_AGILITY:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_AGI );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateAgiOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_TRAINING_FLAG_SP_ATTACK:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_SPATK );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateSpatkOnDisplay( clone_param );
      break;

    case BUTTON_SWITCH_TRAINING_FLAG_SP_DEFENSE:
      this->SwitchTrainingFlag( clone_param, pml::pokepara::POWER_SPDEF );
      this->UpdateTrainingFlagDisplay( clone_param );
      this->UpdateSpdefOnDisplay( clone_param );
      break;


    default:
      GFL_ASSERT(0);
      break;
    }


    if( keyboard_call )
    {
      m_seq = SEQ_KEYBOARD;
      m_keyboad_input_target = static_cast<ButtonID>( selected_button );
      m_keyboard_buf->Clear();
      debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
      proc->SetupParam( DebugPokeMake::DeviceHeap(), DebugPokeMake::DeviceHeap(), m_keyboard_buf, 256, wordMax );
      proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_HIRAGANA );
      proc->SetLayoutSystem( DebugPokeMake::LayoutSystem() );
      if( 0 <= messageID ) {
        proc->SetSearchWords( messageID, &m_keyboard_choice );
      }
    }
    else if( num_param.input )
    {
      m_seq = SEQ_NUM_INPUT;
      m_number_input_target = static_cast<ButtonID>( selected_button );
      m_num_input->Start( num_param.title, num_param.init, num_param.min, num_param.max );
    }
  }


  /**
   * @brief 数値表示アニメーションを開始する
   * @param strbox_index  数値を表示する文字列ボックス
   * @param value         表示する数値
   */
  void PokeMakeCore::StartNumberDisplayAnime( StringBoxIndex strbox_index, u32 value )
  {
    print::StrNumber( DebugPokeMake::WorkStrBuf(), value, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->StartAnimation( m_lytWk_Upper, UPPER_ANIME_START_NUM_DISPLAY );
    this->SetTextboxPaneString( m_lytWk_Upper, PANENAME_SUB_PANE_NUM_VAL, DebugPokeMake::WorkStrBuf() );
  }










  /**
   * @brief 文字列入力が完了した際のコールバック関数
   * @param selected_button 入力した数値ボックス
   * @param input_string    入力された文字列
   * @param pokemon_param   操作対象のポケモン
   */
  void PokeMakeCore::OnStringInputFinished(
      ButtonID selected_button,
      const gfl2::str::StrBuf* input_string,
      pml::pokepara::PokemonParam* pokemon_param )
  {
    if( m_keyboard_choice < 0 ) {
      return;  // 入力なし
    }

    switch( selected_button )
    {
    case BUTTON_STRBOX_MONSNAME:
      {
        MonsNo old_monsno = pokemon_param->GetMonsNo();
        MonsNo new_monsno = static_cast<MonsNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        if( new_monsno != old_monsno ) {
          u8 level = pokemon_param->GetLevel();            // 変更前のレベルを保存
          pokemon_param->ChangeMonsNo( new_monsno, 0 );  // フォルムは0にする
          pokemon_param->SetExp(0);                        // 変更後に一度レベル1に戻して変更前のレベルに引き上げる
          pokemon_param->LevelUp( level-1 );
          if( ( old_monsno == MONSNO_NULL ) && ( new_monsno != MONSNO_NULL ) ) {  // MONSNO_NULLでなくなった際にレベル1にする
            pokemon_param->SetExp(0);
            pokemon_param->RecoverHpFull();
            pokemon_param->SetDefaultWaza();
            pokemon_param->SetNotRare();
          }
          this->UpdateAllOnDisplay( pokemon_param );
        }
      }
      break;

    case BUTTON_STRBOX_NICKNAME:
      if( input_string->GetLength() == 0 ) {
        pokemon_param->SetDefaultNickName();
      }
      else {
        pokemon_param->SetNickName( input_string );
      }
      this->UpdateNickNameOnDisplay( pokemon_param );
      break;

    case BUTTON_STRBOX_SEIKAKU:
      {
        pml::pokepara::Seikaku new_seikaku =
          static_cast<pml::pokepara::Seikaku>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->ChangeSeikaku( new_seikaku );
        this->UpdateSeikakuOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_STRBOX_ITEM:
      {
        u32 new_itemno = m_keyboard_choice;  // 選択した文字列のインデックスと一致
        new_itemno = this->CorrectBeadsItemNo( new_itemno );
        pokemon_param->SetItem( new_itemno );
        this->UpdateItemOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_STRBOX_TOKUSEI:
      {
        TokuseiNo new_tokusei = static_cast<TokuseiNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->OverwriteTokuseiNo( new_tokusei );
        CheckTokuseiNo( pokemon_param );
        this->UpdateTokuseiOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_STRBOX_WAZA1:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetWaza( 0, new_wazano );
        this->UpdateWazaOnDisplay( pokemon_param, 0 );
      }
      break;

    case BUTTON_STRBOX_WAZA2:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetWaza( 1, new_wazano );
        this->UpdateWazaOnDisplay( pokemon_param, 1 );
      }
      break;

    case BUTTON_STRBOX_WAZA3:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetWaza( 2, new_wazano );
        this->UpdateWazaOnDisplay( pokemon_param, 2 );
      }
      break;

    case BUTTON_STRBOX_WAZA4:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetWaza( 3, new_wazano );
        this->UpdateWazaOnDisplay( pokemon_param, 3 );
      }
      break;
    case BUTTON_STRBOX_VERSION:
      if( input_string->GetLength() != 0 ) {
        for(int i=0; i<GFL_NELEMS(VERSION_NAME);++i) {
          gfl2::str::StrBuf version_str( VERSION_NAME[i], DebugPokeMake::DeviceHeap() );
          if( input_string->Compare( version_str ) )
          {
            pokemon_param->SetCassetteVersion( i );
            break;
          }
        }
      }
      this->UpdateVersionOnDisplay( pokemon_param );
      break;
    case BUTTON_STRBOX_LANGUAGE:
      if( input_string->GetLength() != 0 ) {
        for(int i=0; i<GFL_NELEMS(LANGUAGE_NAME);++i) {
          gfl2::str::StrBuf version_str( LANGUAGE_NAME[i], DebugPokeMake::DeviceHeap() );
          if( input_string->Compare( version_str ) )
          {
            pokemon_param->SetCountryCode( i );
            break;
          }
        }
      }
      this->UpdateLanguageOnDisplay( pokemon_param );
      break;
    case BUTTON_STRBOX_OYANAME:
      if( input_string->GetLength() != 0 ) {
        pokemon_param->SetParentName( input_string );
      }
      this->UpdateOyaNameOnDisplay( pokemon_param );
      break;
    case BUTTON_STRBOX_CAPTURE_BALL:
      if( input_string->GetLength() != 0 ) {
        for(int i=0; i<item::BALLID_MAX;++i) {
          item::BALL_ID ball_id = static_cast<item::BALL_ID>(i);
          u16 item_id = item::ITEM_BallID2ItemID( ball_id );
          m_msgData->GetString( label_item_name, *(DebugPokeMake::WorkStrBuf(1)) );
          m_wordset->RegisterItemName( 0, item_id, 1 );
          m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );
          if( input_string->Compare( *DebugPokeMake::WorkStrBuf(0) ) )
          {
            pokemon_param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL, i );
            break;
          }
        }
      }
      this->UpdateCaptureBallOnDisplay( pokemon_param );
      break;
    case BUTTON_STRBOX_REGION:
      if( input_string->GetLength() != 0 ) {
        for(int i=0; i<GFL_NELEMS(REGION_NAME);++i) {
          gfl2::str::StrBuf region_str( REGION_NAME[i], DebugPokeMake::DeviceHeap() );
          if( input_string->Compare( region_str ) )
          {
            pokemon_param->SetHWRegionCode( i );
            break;
          }
        }
      }
      this->UpdateRegion( pokemon_param );
      break;
    case BUTTON_STRBOX_OTHER_OYANAME:
      pokemon_param->SetPastParentsName( input_string );
      this->UpdateOtherOyaNameOnDisplay( pokemon_param );
      break;

    case BUTTON_NUMBOX_EGG_WAZA1_NAME:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetTamagoWazaNo( 0,new_wazano );
        this->UpdateEggWazaOnDisplay( pokemon_param , 0);
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA2_NAME:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetTamagoWazaNo( 1,new_wazano );
        this->UpdateEggWazaOnDisplay( pokemon_param , 1);
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA3_NAME:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetTamagoWazaNo( 2,new_wazano );
        this->UpdateEggWazaOnDisplay( pokemon_param , 2);
      }
      break;

    case BUTTON_NUMBOX_EGG_WAZA4_NAME:
      {
        WazaNo new_wazano = static_cast<WazaNo>( m_keyboard_choice );  // 選択した文字列のインデックスと一致
        pokemon_param->SetTamagoWazaNo( 3,new_wazano );
        this->UpdateEggWazaOnDisplay( pokemon_param , 3);
      }
      break;

    }
  }

  /**
   * @brief ビーズの道具No.を修正する
   * @param inputItemNo デバッグキーボードで入力された道具No.
   * @return 修正後の道具No.
   *
   * @fix GFNMCat[991] 固有全力技が使えない
   * ポケモンに持たせるビーズと
   * 大切な道具のビーズが同じ名前で定義されているため、
   * デバッグキーボードでは、どちらが選択されたか判断でず、先に定義されている方の道具No.になってしまいます。
   * 大切な道具をポケモンに持たせられる必要がありませんので、ここで、ポケモンに持たせるビーズに変換します。
   */
  u32 PokeMakeCore::CorrectBeadsItemNo( u32 inputItemNo ) const
  {
    static const struct 
    {
      u16 changeItem;
      u16 correctedItem;
    } 
    CORRECT_LIST[] = 
    {
      { ITEM_NOOMARUBIIZU,  ITEM_NOOMARUBIIZU_1  },
      { ITEM_HONOOBIIZU,    ITEM_HONOOBIIZU_1    },
      { ITEM_MIZUBIIZU,     ITEM_MIZUBIIZU_1     },
      { ITEM_DENKIBIIZU,    ITEM_DENKIBIIZU_1    },
      { ITEM_KUSABIIZU,     ITEM_KUSABIIZU_1     },
      { ITEM_KOORIBIIZU,    ITEM_KOORIBIIZU_1    },
      { ITEM_KAKUTOUBIIZU,  ITEM_KAKUTOUBIIZU_1  },
      { ITEM_DOKUBIIZU,     ITEM_DOKUBIIZU_1     },
      { ITEM_ZIMENBIIZU,    ITEM_ZIMENBIIZU_1    },
      { ITEM_HIKOUBIIZU,    ITEM_HIKOUBIIZU_1    },
      { ITEM_ESUPAABIIZU,   ITEM_ESUPAABIIZU_1   },
      { ITEM_MUSIBIIZU,     ITEM_MUSIBIIZU_1     },
      { ITEM_IWABIIZU,      ITEM_IWABIIZU_1      },
      { ITEM_GOOSUTOBIIZU,  ITEM_GOOSUTOBIIZU_1  },
      { ITEM_DORAGONBIIZU,  ITEM_DORAGONBIIZU_1  },
      { ITEM_AKUBIIZU,      ITEM_AKUBIIZU_1      },
      { ITEM_HAGANEBIIZU,   ITEM_HAGANEBIIZU_1   },
      { ITEM_FEARIIBIIZU,   ITEM_FEARIIBIIZU_1   },
      { ITEM_PIKATYUUBIIZU, ITEM_PIKATYUUBIIZU_1 },
      { ITEM_HUKUROU3BIIZU, ITEM_HUKUROU3BIIZU_1 },
      { ITEM_TORA3BIIZU,    ITEM_TORA3BIIZU_1    },
      { ITEM_ASIKA3BIIZU,   ITEM_ASIKA3BIIZU_1   },
      { ITEM_THIKIBIIZU,    ITEM_THIKIBIIZU_1    },
      { ITEM_HAISINBIIZU,   ITEM_HAISINBIIZU_1   },
      { ITEM_ARORAIBIIZU,   ITEM_ARORAIBIIZU_1   },
      { ITEM_KABIGONBIIZU,  ITEM_KABIGONBIIZU_1  },
      { ITEM_IIBUIBIIZU,    ITEM_IIBUIBIIZU_1    },
      { ITEM_MYUUBIIZU,     ITEM_MYUUBIIZU_1     },
      { ITEM_SATOPIKABIIZU, ITEM_SATOPIKABIIZU_1 },
      { ITEM_SORUGAREOz,    ITEM_SORUGAREOz_1    },
      { ITEM_RUNAAARAz,     ITEM_RUNAAARAz_1     },
      { ITEM_NEKUROZUMAz,   ITEM_NEKUROZUMAz_1   },
      { ITEM_MIMIKKYUz,     ITEM_MIMIKKYUz_1     },
      { ITEM_RUGARUGANz,    ITEM_RUGARUGANz_1    },
      { ITEM_ZYARARANGAz,   ITEM_ZYARARANGAz_1   },
    };

    for( u32 i=0; i<GFL_NELEMS(CORRECT_LIST); ++i )
    {
      if( CORRECT_LIST[i].changeItem == inputItemNo )
      {
        return CORRECT_LIST[i].correctedItem;
      }
    }

    return inputItemNo;
  }







  /**
   * @brief 数値入力が完了した際のコールバック関数
   * @param selected_button      入力した数値ボックス
   * @param pokemon_param  操作対象のポケモン
   */
  void PokeMakeCore::OnNumberInputFinished(
    ButtonID selected_button, pml::pokepara::PokemonParam* pokemon_param, DefaultPowerUpDesc* default_powerup_desc )
  {

    switch( selected_button )
    {
    case BUTTON_NUMBOX_FORMNO:
      {
        u32 new_formno = m_num_input->GetValue();
        pokemon_param->ChangeFormNo( new_formno );
        this->UpdateFormNoOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_LEVEL:
      {
        MonsNo monsno = pokemon_param->GetMonsNo();
        pml::FormNo formno = pokemon_param->GetFormNo();
        pml::personal::LoadGrowTable( monsno, formno );
        u32 level = m_num_input->GetValue();
        u32 min_exp = pml::personal::GetMinExp( level );
        pokemon_param->SetExp( min_exp );
        pokemon_param->RecoverHpFull();
        this->UpdateAllOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_EXP:
      {
        u32 exp = m_num_input->GetValue();
        pokemon_param->SetExp( exp );
        pokemon_param->RecoverHpFull();
        this->UpdateExpOnDisplay( pokemon_param );
        this->UpdateLevelOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_HP_RND:
      {
        u32 hp_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_HP, hp_rnd );
        this->UpdateHpOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_ATK_RND:
      {
        u32 atk_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_ATK, atk_rnd );
        this->UpdateAtkOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_DEF_RND:
      {
        u32 def_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_DEF, def_rnd );
        this->UpdateDefOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_AGI_RND:
      {
        u32 agi_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_AGI, agi_rnd );
        this->UpdateAgiOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_SPATK_RND:
      {
        u32 spatk_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_SPATK, spatk_rnd );
        this->UpdateSpatkOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_SPDEF_RND:
      {
        u32 spdef_rnd = m_num_input->GetValue();
        pokemon_param->ChangeTalentPower( pml::pokepara::POWER_SPDEF, spdef_rnd );
        this->UpdateSpdefOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_HP_EXP:
      {
        u32 hp_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_HP, hp_exp );
        this->UpdateHpOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_HP_VAL:
      {
        u32 val = m_num_input->GetValue();
        pokemon_param->SetHp( val );
        this->UpdateHpOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_ATK_EXP:
      {
        u32 atk_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_ATK, atk_exp );
        this->UpdateAtkOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_DEF_EXP:
      {
        u32 def_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_DEF, def_exp );
        this->UpdateDefOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_AGI_EXP:
      {
        u32 agi_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_AGI, agi_exp );
        this->UpdateAgiOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_SPATK_EXP:
      {
        u32 spatk_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_SPATK, spatk_exp );
        this->UpdateSpatkOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_SPDEF_EXP:
      {
        u32 spdef_exp = m_num_input->GetValue();
        pokemon_param->ChangeEffortPower( pml::pokepara::POWER_SPDEF, spdef_exp );
        this->UpdateSpdefOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_WAZA1_UP:
      {
        u32 up_count = m_num_input->GetValue();
        pokemon_param->SetWazaPPUpCount( 0, up_count );
        this->UpdateWazaOnDisplay( pokemon_param, 0 );
      }
      break;

    case BUTTON_NUMBOX_WAZA2_UP:
      {
        u32 up_count = m_num_input->GetValue();
        pokemon_param->SetWazaPPUpCount( 1, up_count );
        this->UpdateWazaOnDisplay( pokemon_param, 1 );
      }
      break;

    case BUTTON_NUMBOX_WAZA3_UP:
      {
        u32 up_count = m_num_input->GetValue();
        pokemon_param->SetWazaPPUpCount( 2, up_count );
        this->UpdateWazaOnDisplay( pokemon_param, 2 );
      }
      break;

    case BUTTON_NUMBOX_WAZA4_UP:
      {
        u32 up_count = m_num_input->GetValue();
        pokemon_param->SetWazaPPUpCount( 3, up_count );
        this->UpdateWazaOnDisplay( pokemon_param, 3 );
      }
      break;

    case BUTTON_NUMBOX_WAZA1_PP:
      {
        u32 pp_count = m_num_input->GetValue();
        pokemon_param->SetWazaPP( 0, pp_count );
        this->UpdateWazaOnDisplay( pokemon_param, 0 );
      }
      break;

    case BUTTON_NUMBOX_WAZA2_PP:
      {
        u32 pp_count = m_num_input->GetValue();
        pokemon_param->SetWazaPP( 1, pp_count );
        this->UpdateWazaOnDisplay( pokemon_param, 1 );
      }
      break;

    case BUTTON_NUMBOX_WAZA3_PP:
      {
        u32 pp_count = m_num_input->GetValue();
        pokemon_param->SetWazaPP( 2, pp_count );
        this->UpdateWazaOnDisplay( pokemon_param, 2 );
      }
      break;

    case BUTTON_NUMBOX_WAZA4_PP:
      {
        u32 pp_count = m_num_input->GetValue();
        pokemon_param->SetWazaPP( 3, pp_count );
        this->UpdateWazaOnDisplay( pokemon_param, 3 );
      }
      break;

    case BUTTON_NUMBOX_FAMILY:
      {
        u32 family = m_num_input->GetValue();
        pokemon_param->SetOriginalFamiliarity( family );
        this->UpdateFamiliarityOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_FRIEND:
      {
        u32 friendship = m_num_input->GetValue();
        pokemon_param->SetFriendship( friendship );
        this->UpdateFriendshipOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_FEED:
      {
        u32 feed = m_num_input->GetValue();
        pokemon_param->SetFeed( feed );
        this->UpdateFeedOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_NADENADE:
      {
        u32 play = m_num_input->GetValue();
        pokemon_param->SetNadenadeValue( play );
        this->UpdatePlayOnDisplay( pokemon_param );
      }
      break;

    case BUTTON_NUMBOX_POKERUS:
      {
        pokemon_param->SetPokerus( m_num_input->GetValue() );
        this->UpdatePokerusOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CAPTURE_LEVEL:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL, m_num_input->GetValue() );
        this->UpdateCaptureLevelOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONTACT_PLACE:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, m_num_input->GetValue() );
        this->UpdateContactPlaceOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_PLACE:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE, m_num_input->GetValue() );
        this->UpdateEggTakenPlaceOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONTACT_DAY:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY, m_num_input->GetValue() );
        this->UpdateContactDayOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONTACT_MONTH:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH, m_num_input->GetValue() );
        this->UpdateContactMonthOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONTACT_YEAR:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR, m_num_input->GetValue() );
        this->UpdateContactYearOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_DAY:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY, m_num_input->GetValue() );
        this->UpdateEggTakenDayOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_MONTH:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH, m_num_input->GetValue() );
        this->UpdateEggTakenMonthOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_EGG_TAKEN_YEAR:
      {
        pokemon_param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR, m_num_input->GetValue() );
        this->UpdateEggTakenYearOnDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_MEMORY_CODE_OYA:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT, m_num_input->GetValue() );
      this->UpdateMemoryCodeWithParent( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_LEVEL_OYA:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT, m_num_input->GetValue() );
      this->UpdateMemoryLevelWithParent( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_DATA_OYA:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_DATA_WITH_PARENT, m_num_input->GetValue() );
      this->UpdateMemoryDataWithParent( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_FEEL_OYA:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_FEEL_WITH_PARENT, m_num_input->GetValue() );
      this->UpdateMemoryFeelWithParent( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_CODE_OTHER:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_CODE_WITH_OTHERS, m_num_input->GetValue() );
      this->UpdateMemoryCodeWithOther( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_LEVEL_OTHER:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_OTHERS, m_num_input->GetValue() );
      this->UpdateMemoryLevelWithOther( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_DATA_OTHER:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_DATA_WITH_OTHERS, m_num_input->GetValue() );
      this->UpdateMemoryDataWithOther( pokemon_param );
      break;
    case BUTTON_NUMBOX_MEMORY_FEEL_OTHER:
      pokemon_param->SetMemories( pml::pokepara::MEMORIES_FEEL_WITH_OTHERS, m_num_input->GetValue() );
      this->UpdateMemoryFeelWithOther( pokemon_param );
      break;

    case BUTTON_NUMBOX_COUNTRY:
      pokemon_param->SetCountryCode( m_num_input->GetValue() );
      this->UpdateCountryCode( pokemon_param );
      break;
    case BUTTON_NUMBOX_COUNTRY_AREA:
      pokemon_param->SetCountryRegionCode( m_num_input->GetValue() );
      this->UpdateCountryRegionCode( pokemon_param );
      break;
    case BUTTON_NUMBOX_OTHER_FAMILY:
      pokemon_param->SetOthersFamiliarity( m_num_input->GetValue() );
      this->UpdateOtherFamiliarityOnDisplay( pokemon_param );
      break;
    case BUTTON_NUMBOX_OTHER_FRIEND:
      pokemon_param->SetOthersFriendship( m_num_input->GetValue() );
      this->UpdateOtherFriendshipOnDisplay( pokemon_param );
      break;

    case BUTTON_NUMBOX_PAST_COUNTRY_00: SetPastCountry( pokemon_param, 0, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_COUNTRY_01: SetPastCountry( pokemon_param, 1, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_COUNTRY_02: SetPastCountry( pokemon_param, 2, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_COUNTRY_03: SetPastCountry( pokemon_param, 3, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_COUNTRY_04: SetPastCountry( pokemon_param, 4, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_AREA_00: SetPastArea( pokemon_param, 0, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_AREA_01: SetPastArea( pokemon_param, 1, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_AREA_02: SetPastArea( pokemon_param, 2, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_AREA_03: SetPastArea( pokemon_param, 3, m_num_input->GetValue() ); break;
    case BUTTON_NUMBOX_PAST_AREA_04: SetPastArea( pokemon_param, 4, m_num_input->GetValue() ); break;

    case BUTTON_NUMBOX_TRAINER_ID_00:
      {
        u32 id = m_num_input->GetValue();
        id = id <<16;
        id += pokemon_param->GetID()&0xffff;
        pokemon_param->SetID(id);
      }
      this->UpdateTrainerIdDisplay( pokemon_param );
      break;
    case BUTTON_NUMBOX_TRAINER_ID_01:
      {
        u32 id = m_num_input->GetValue();
        id += pokemon_param->GetID()&0xffff0000;
        pokemon_param->SetID(id);
      }
      this->UpdateTrainerIdDisplay( pokemon_param );
      break;

    case BUTTON_NUMBOX_TRIMIAN_HAIR:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->TORIMIAN_SetBodyHairCount(num);
      }
      this->UpdateTrimianHairDisplay( pokemon_param );
      break;
    case BUTTON_NUMBOX_TRIMIAN_COUNT:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->TORIMIAN_SetKeepDayCount(num);
      }
      this->UpdateTrimianCountDisplay( pokemon_param );
      break;
    case BUTTON_NUMBOX_TRIMIAN_COUNT_MAX:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->TORIMIAN_SetKeepMaxDayCount(num);
      }
      this->UpdateTrimianCountMaxDisplay( pokemon_param );
      break;

    case BUTTON_NUMBOX_CONDITION_01:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->SetCondition(pml::pokepara::CONDITION_STYLE,num);
        this->UpdateConditionDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONDITION_02:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->SetCondition(pml::pokepara::CONDITION_BEAUTIFUL,num);
        this->UpdateConditionDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONDITION_03:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->SetCondition(pml::pokepara::CONDITION_CUTE,num);
        this->UpdateConditionDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONDITION_04:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->SetCondition(pml::pokepara::CONDITION_CLEVER,num);
        this->UpdateConditionDisplay( pokemon_param );
      }
      break;
    case BUTTON_NUMBOX_CONDITION_05:
      {
        u8 num = m_num_input->GetValue();
        pokemon_param->SetCondition(pml::pokepara::CONDITION_STRONG,num);
        this->UpdateConditionDisplay( pokemon_param );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_ATK:
      {
        u8 num = m_num_input->GetValue();
        default_powerup_desc->rankUp_Attack = num;
        this->UpdateDefaultPowerUpDisplay( *default_powerup_desc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_DEF:
      {
        u8 num = m_num_input->GetValue();
        default_powerup_desc->rankUp_Defense = num;
        this->UpdateDefaultPowerUpDisplay( *default_powerup_desc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_SPATK:
      {
        u8 num = m_num_input->GetValue();
        default_powerup_desc->rankUp_SpAttack = num;
        this->UpdateDefaultPowerUpDisplay( *default_powerup_desc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_SPDEF:
      {
        u8 num = m_num_input->GetValue();
        default_powerup_desc->rankUp_SpDefense = num;
        this->UpdateDefaultPowerUpDisplay( *default_powerup_desc );
      }
      break;

    case BUTTON_DEFAULT_POWERUP_RANK_AGI:
      {
        u8 num = m_num_input->GetValue();
        default_powerup_desc->rankUp_Agility = num;
        this->UpdateDefaultPowerUpDisplay( *default_powerup_desc );
      }
      break;


    default:
      GFL_ASSERT(0);
      break;
    }

  }


  /**
   * @brief 数値入力呼び出し：過去の親の国コード（５件分)
   * @param index 過去の何番目か
   */
  void PokeMakeCore::CallNumInputPastCountry( pml::pokepara::PokemonParam* param, const int index, NuminputParam *num_param )
  {
    num_param->input = true;
    num_param->init = 0;
    num_param->min = 0;
    num_param->max = 255;
    num_param->title = L"過去の親の国コード";

    GFL_PRINT("numinput init=%d\n", num_param->init);
  }

  /**
   * @brief 数値入力呼び出し：過去の親の地域コード（５件分)
   * @param index 過去の何番目か
   */
  void PokeMakeCore::CallNumInputPastArea( pml::pokepara::PokemonParam* param, int index, NuminputParam *num_param )
  {
    num_param->input = true;
    num_param->init = 0;
    num_param->min = 0;
    num_param->max = 255;
    num_param->title = L"過去の親の地域コード";
  }

  /**
   * @brief 過去の親の国コードを設定
   * @param pokemon_param 書き込むポケモン
   * @param index         過去の何番目か
   * @param coutnry       国コード
   */
  void PokeMakeCore::SetPastCountry( pml::pokepara::PokemonParam* pokemon_param, int index, u8 country )
  {
    u8 area = 0;
    this->UpdatePastParentCountryOnDisplay( pokemon_param, index );

    GFL_PRINT("numinput set country=%d,area=%d\n", country, area);

  }

  /**
   * @brief 過去の親の地域コードを設定
   * @param pokemon_param 書き込むポケモン
   * @param index         過去の何番目か
   * @param coutnry       地域コード
   */
  void PokeMakeCore::SetPastArea( pml::pokepara::PokemonParam* pokemon_param, int index, u8 area )
  {
    u16 country = 0;
    this->UpdatePastParentAreaOnDisplay( pokemon_param, index );

    GFL_PRINT("numinput set country=%d,area=%d\n", country, area);
  }




  //---------------------------------------------------------------------------
  /**
   * @brief プロセス描画関数
   * @param displayNo  描画対象画面
   */
  //---------------------------------------------------------------------------
  void PokeMakeCore::Draw( gfl2::gfx::CtrDisplayNo displayNo )
  {
    if( ( m_renderingPipeLine == NULL ) ||
       !( m_isDrawEnable ) ) {
      return;
    }

    m_renderingPipeLine->GetLayoutRenderPath()->ClearDrawableLytWk();

    if( displayNo == gfl2::gfx::CtrDisplayNo::DOWN ) {
      m_renderingPipeLine->GetLayoutRenderPath()->AddDrawableLytWk( m_lytWk, gfl2::lyt::DISPLAY_LOWER );
    }
    else {
      m_renderingPipeLine->GetLayoutRenderPath()->AddDrawableLytWk( m_lytWk_Upper, gfl2::lyt::DISPLAY_UPPER );
    }

    gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[ displayNo ];
    gfl2::gfx::Surface* pBackBuffer   = gfl2::gfx::GFGL::GetBackBuffer( handle );
    gfl2::gfx::Surface* pDepthBuffer  = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
    u32				          displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( handle );
    u32			          	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
    m_renderingPipeLine->SetRenderTarget( pBackBuffer );
    m_renderingPipeLine->SetDepthStencil( pDepthBuffer );
    m_renderingPipeLine->SetDisplaySize( displayWidth, displayHeight );
    m_renderingPipeLine->Execute();

    if( ( displayNo == gfl2::gfx::CtrDisplayNo::DOWN ) &&
        ( m_seq == SEQ_NUM_INPUT ) ) {
      m_num_input->Draw();
    }
  }

  /**
   * @brief 全ての表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateAllOnDisplay(
      const pml::pokepara::PokemonParam* pokemon_param )
  {
    this->UpdateMonsNameOnDisplay( pokemon_param );
    this->UpdateNickNameOnDisplay( pokemon_param );
    this->UpdateFormNoOnDisplay( pokemon_param );
    this->UpdateTypeOnDisplay( pokemon_param );
    this->UpdateSexOnDisplay( pokemon_param );
    this->UpdateSickOnDisplay( pokemon_param );
    this->UpdateEggOnDisplay( pokemon_param );
    this->UpdateRareOnDisplay( pokemon_param );
    this->UpdateSeikakuOnDisplay( pokemon_param );
    this->UpdateFamiliarityOnDisplay( pokemon_param );
    this->UpdateFriendshipOnDisplay( pokemon_param );
    this->UpdateFeedOnDisplay( pokemon_param );
    this->UpdatePlayOnDisplay( pokemon_param );
    this->UpdateItemOnDisplay( pokemon_param );
    this->UpdateTokuseiOnDisplay( pokemon_param );
    this->UpdateLevelOnDisplay( pokemon_param );
    this->UpdateExpOnDisplay( pokemon_param );
    this->UpdateHpOnDisplay( pokemon_param );
    this->UpdateAtkOnDisplay( pokemon_param );
    this->UpdateDefOnDisplay( pokemon_param );
    this->UpdateAgiOnDisplay( pokemon_param );
    this->UpdateSpatkOnDisplay( pokemon_param );
    this->UpdateSpdefOnDisplay( pokemon_param );
    this->UpdateAllWazaOnDisplay( pokemon_param );
    this->UpdatePokerusOnDisplay(  pokemon_param );
    this->UpdateEventOnDisplay(  pokemon_param );
    this->UpdateVersionOnDisplay(  pokemon_param );
    this->UpdateLanguageOnDisplay(  pokemon_param );
    this->UpdateOyaNameOnDisplay(  pokemon_param );
    this->UpdateOyaSexOnDisplay(  pokemon_param );
    this->UpdateCaptureBallOnDisplay(  pokemon_param );
    this->UpdateCaptureLevelOnDisplay(  pokemon_param );
    this->UpdateEggTakenPlaceOnDisplay(  pokemon_param );
    this->UpdateEggTakenYearOnDisplay(  pokemon_param );
    this->UpdateEggTakenMonthOnDisplay(  pokemon_param );
    this->UpdateEggTakenDayOnDisplay(  pokemon_param );
    this->UpdateContactPlaceOnDisplay(  pokemon_param );
    this->UpdateContactYearOnDisplay(  pokemon_param );
    this->UpdateContactMonthOnDisplay(  pokemon_param );
    this->UpdateContactDayOnDisplay(  pokemon_param );
    this->UpdateMemoryCodeWithParent( pokemon_param );
    this->UpdateMemoryLevelWithParent( pokemon_param );
    this->UpdateMemoryDataWithParent( pokemon_param );
    this->UpdateMemoryFeelWithParent( pokemon_param );
    this->UpdateMemoryCodeWithOther( pokemon_param );
    this->UpdateMemoryLevelWithOther( pokemon_param );
    this->UpdateMemoryDataWithOther( pokemon_param );
    this->UpdateMemoryFeelWithOther( pokemon_param );
    this->UpdateRegion( pokemon_param );
    this->UpdateCountryCode( pokemon_param );
    this->UpdateCountryRegionCode( pokemon_param );
    this->UpdateOtherFamiliarityOnDisplay( pokemon_param );
    this->UpdateOtherFriendshipOnDisplay( pokemon_param );
    this->UpdateOtherOyaNameOnDisplay( pokemon_param );
    this->UpdatePastOyaSexOnDisplay(  pokemon_param );
    for(int i=0;i<5;i++){
      this->UpdatePastParentAreaOnDisplay( pokemon_param, i );
      this->UpdatePastParentCountryOnDisplay( pokemon_param, i );
    }
    this->UpdatePersonalRandDisplay( pokemon_param );
    this->UpdateTrainerIdDisplay( pokemon_param );
    this->UpdateRareRandomDisplay( pokemon_param );
    this->UpdateTokuseiIndexSwitchTrans( pokemon_param );
    this->UpdateTrimianCountMaxDisplay( pokemon_param );
    this->UpdateTrimianCountDisplay( pokemon_param );
    this->UpdateTrimianHairDisplay( pokemon_param );
    this->UpdateConditionDisplay( pokemon_param );
    this->UpdateTrainingFlagDisplay( pokemon_param );

    for(int i=0;i<4;i++){
      this->UpdateEggWazaOnDisplay( pokemon_param ,i);
    }

    DefaultPowerUpDesc* defaultPowerUpDesc = DebugPokeMake::CloneDefaultPowerUpDesc();
    this->UpdateDefaultPowerUpDisplay( *defaultPowerUpDesc );

  }

  /**
   * @brief モンスター名の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMonsNameOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::personal::GetMonsName( DebugPokeMake::WorkStrBuf(), monsno );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_MONS_NAME, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief ニックネームの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateNickNameOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    if( pokemon_param->HaveNickName() ) {
      pokemon_param->GetNickName( DebugPokeMake::WorkStrBuf() );
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_NICKNAME_VAL, DebugPokeMake::WorkStrBuf() );
    }
    else {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_NICKNAME_VAL, L"−−−−−−" );
    }
  }

  /**
   * @brief フォルム番号の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateFormNoOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    pml::FormNo max_formno = GetFormMax( pokemon_param );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), formno, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_FORM_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), max_formno, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_FORM_MAX_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief タイプの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateTypeOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pml::PokeType type1 = pokemon_param->GetType1();
    pml::PokeType type2 = pokemon_param->GetType2();
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TYPE1, TYPE_NAME[ type1 ] );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TYPE2, TYPE_NAME[ type2 ] );
  }

  /**
   * @brief 性別の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateSexOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pml::Sex sex = pokemon_param->GetSex();
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SEX_VAL, SEX_NAME[ sex ] );
  }

  /**
   * @brief 状態異常の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateSickOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pml::pokepara::Sick sick = pokemon_param->GetSick();
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SICK_VAL, SICK_NAME[ sick ] );
  }

  /**
   * @brief タマゴの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEggOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    if( pokemon_param->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG ) ) {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_VAL, L"ダメ" );
    }
    else if( pokemon_param->IsEgg( pml::pokepara::CHECK_ONLY_LEGAL_EGG ) ) {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_VAL, L"○" );
    }
    else {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_VAL, L"−" );
    }
  }

  /**
   * @brief レアの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateRareOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    if( pokemon_param->IsRare() ) {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_RARE_VAL, L"○" );
    }
    else {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_RARE_VAL, L"−" );
    }
  }

  /**
   * @brief 性格の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateSeikakuOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pml::pokepara::Seikaku seikaku = pokemon_param->GetSeikaku();
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SEIKAKU_NAME, SEIKAKU_NAME[ seikaku ] );
  }

  /**
   * @brief なつき度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateFamiliarityOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 family = pokemon_param->GetOriginalFamiliarity();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), family, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_FAMILY_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief なかよし度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateFriendshipOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 friendship = pokemon_param->GetFriendship();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), friendship, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_FRIEND_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief まんぷく度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateFeedOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 feed = pokemon_param->GetFeed();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), feed, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_FEED_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief あそび度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdatePlayOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 value = pokemon_param->GetNadenadeValue();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), value, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_PLAY_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief ポケルス表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdatePokerusOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 pokerus = pokemon_param->GetPokerus();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), pokerus, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POKERUS_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief イベントの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEventOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    if( pokemon_param->GetEventPokeFlag()) {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EVENT_VAL, L"○" );
    }
    else {
      this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EVENT_VAL, L"−" );
    }
  }
  /**
   * @brief バージョンの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateVersionOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 version = pokemon_param->GetCassetteVersion();

    if( version >= GFL_NELEMS( VERSION_NAME ) )
    {
      GFL_PRINT( "設定されているバージョンが間違っています！ version=%d", version );
      version = 0;
    }
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_VERSION_NAME, VERSION_NAME[ version ] );
  }
  /**
   * @brief 言語の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateLanguageOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 lang = pokemon_param->GetLangId();

    if( lang >= GFL_NELEMS( LANGUAGE_NAME ) )
    {
      GFL_PRINT( "設定されている国コードが間違っています！ lang=%d", lang );
      lang = System::LANGUAGE_JAPAN;
    }
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_LANGUAGE_NAME, LANGUAGE_NAME[ lang ] );
  }
  /**
   * @brief 親名の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateOyaNameOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pokemon_param->GetParentName( DebugPokeMake::WorkStrBuf() );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYANAME_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 親の性別表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateOyaSexOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    pml::Sex sex = pokemon_param->GetParentSex();
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYASEX_VAL, SEX_NAME[ sex ] );
  }
  /**
   * @brief 捕獲ボールの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateCaptureBallOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    item::BALL_ID ball_id = static_cast<item::BALL_ID>(pokemon_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_BALL ) );
    if( ball_id == item::BALLID_NULL || ball_id > item::BALLID_MAX )
    {
      GFL_PRINT( "設定されている捕獲ボールが間違っています！ ball_id=%d", ball_id );
      ball_id = item::BALLID_MONSUTAABOORU;
    }
    u16 itemno = item::ITEM_BallID2ItemID( ball_id );
    m_msgData->GetString( label_item_name, *(DebugPokeMake::WorkStrBuf(1)) );
    m_wordset->RegisterItemName( 0, itemno, 1 );
    m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CAP_BALL_NAME, DebugPokeMake::WorkStrBuf(0) );
  }
  /**
   * @brief 捕獲レベルの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateCaptureLevelOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 cap_level = pokemon_param->GetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), cap_level, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CAP_LEVEL_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief タマゴを貰った場所を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEggTakenPlaceOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 place = pokemon_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE );

    print::StrNumber( DebugPokeMake::WorkStrBuf(1), place, 6, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_TAKEN_PLC_VA, DebugPokeMake::WorkStrBuf(1) );

    gfl2::str::StrBuf *place_str = PokeTool::PokePlace::CreatePlaceName( place, DebugPokeMake::DeviceHeap() );
    DebugPokeMake::WorkStrBuf()->SetStr( *place_str );
    GFL_SAFE_DELETE( place_str );

    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_TAKEN_NAME_V, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief タマゴを貰った年の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEggTakenYearOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 year = pokemon_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), year, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_TAKEN_Y_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief タマゴを貰った月の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEggTakenMonthOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 month = pokemon_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), month, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_TAKEN_M_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief タマゴを貰った日の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateEggTakenDayOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 day= pokemon_param->GetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), day, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EGG_TAKEN_D_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 出会った場所の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateContactPlaceOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 place = pokemon_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE );

    print::StrNumber( DebugPokeMake::WorkStrBuf(1), place, 6, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CONTACT_PLC_VAL, DebugPokeMake::WorkStrBuf(1) );

    gfl2::str::StrBuf *place_str = PokeTool::PokePlace::CreatePlaceName( place, DebugPokeMake::DeviceHeap() );
    DebugPokeMake::WorkStrBuf()->SetStr( *place_str );
    GFL_SAFE_DELETE( place_str );

    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CONTACT_NAME_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 出会った年の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateContactYearOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 year = pokemon_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), year, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CONTACT_Y_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 出会った月の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateContactMonthOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 month = pokemon_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), month, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CONTACT_M_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 出会った日の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateContactDayOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 day= pokemon_param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), day, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CONTACT_D_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 持ち物の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateItemOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 itemno = pokemon_param->GetItem();
    m_msgData->GetString( label_item_name, *(DebugPokeMake::WorkStrBuf(1)) );
    m_wordset->RegisterItemName( 0, itemno, 1 );
    m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_ITEM_NAME, DebugPokeMake::WorkStrBuf(0) );
  }

  /**
   * @brief 特性の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateTokuseiOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    TokuseiNo tokusei_no = pokemon_param->GetTokuseiNo();
    m_msgData->GetString( label_tokusei_name, *(DebugPokeMake::WorkStrBuf(1)) );
    m_wordset->RegisterTokuseiName( 0, tokusei_no );
    m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TOKUSEI_NAME, DebugPokeMake::WorkStrBuf(0) );
  }

  /**
   * @brief レベルの表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateLevelOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 level = pokemon_param->GetLevel();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), level, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_LEVEL_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 経験値の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateExpOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 exp = pokemon_param->GetExp();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), exp, 7, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_EXP_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief HPの表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateHpOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    u32 hp = pokemon_param->GetHp();
    u32 max_hp = pokemon_param->GetPower( pml::pokepara::POWER_HP );
    u32 hp_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_HP );
    u32 hp_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_HP );
    u32 hp_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_hp );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), hp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_HP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), max_hp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_HP_MAX, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), hp_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_HP_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), hp_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_HP_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), hp_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_HP_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「こうげき」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateAtkOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    u32 atk = pokemon_param->GetPower( pml::pokepara::POWER_ATK );
    u32 atk_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_ATK );
    u32 atk_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_ATK );
    u32 atk_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_atk );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), atk, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_ATK_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), atk_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_ATK_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), atk_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_ATK_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), atk_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_ATK_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「ぼうぎょ」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateDefOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    u32 def = pokemon_param->GetPower( pml::pokepara::POWER_DEF );
    u32 def_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_DEF );
    u32 def_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_DEF );
    u32 def_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_def );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), def, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_DEF_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), def_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_DEF_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), def_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_DEF_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), def_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_DEF_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「すばやさ」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateAgiOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    u32 agi = pokemon_param->GetPower( pml::pokepara::POWER_AGI );
    u32 agi_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_AGI );
    u32 agi_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_AGI );
    u32 agi_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_agi );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), agi, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_AGI_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), agi_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_AGI_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), agi_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_AGI_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), agi_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_AGI_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「とくこう」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateSpatkOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 spatk = pokemon_param->GetPower( pml::pokepara::POWER_SPATK );
    u32 spatk_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_SPATK );
    u32 spatk_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_SPATK );
    u32 spatk_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_spatk );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spatk, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPATK_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spatk_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPATK_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spatk_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPATK_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spatk_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPATK_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「とくぼう」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateSpdefOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    MonsNo monsno = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    u32 spdef = pokemon_param->GetPower( pml::pokepara::POWER_SPDEF );
    u32 spdef_rnd = pokemon_param->GetNativeTalentPower( pml::pokepara::POWER_SPDEF );
    u32 spdef_exp = pokemon_param->GetEffortPower( pml::pokepara::POWER_SPDEF );
    u32 spdef_base = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_basic_spdef );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spdef, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPDEF_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spdef_rnd, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPDEF_RND_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spdef_exp, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPDEF_EXP_VAL, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), spdef_base, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_SPDEF_BASE_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 全ての技の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateAllWazaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ ) {
      this->UpdateWazaOnDisplay( pokemon_param, i );
    }
  }

  /**
   * @brief 指定した技の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   * @param waza_index     更新する技のインデックス
   */
  void PokeMakeCore::UpdateWazaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param, u32 waza_index )
  {
    WazaNo wazano = pokemon_param->GetWazaNo( waza_index );
    u32 up_count = pokemon_param->GetWazaPPUpCount( waza_index );
    u32 pp = pokemon_param->GetWazaPP( waza_index );
    u32 max_pp = pokemon_param->GetWazaMaxPP( waza_index );

    m_msgData->GetString( label_waza_name, *(DebugPokeMake::WorkStrBuf(1)) );
    m_wordset->RegisterWazaName( 0, wazano );
    m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );

    this->SetTextboxPaneString( m_lytWk, PANE_INDEX_OF_WAZA_NAME[ waza_index ], DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), up_count, 1, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANE_INDEX_OF_WAZA_UP_VAL[ waza_index ], DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), pp, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANE_INDEX_OF_WAZA_PP_VAL[ waza_index ], DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), max_pp, 2, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANE_INDEX_OF_WAZA_PP_MAX[ waza_index ], DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親との記憶コード」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryCodeWithParent( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_PARENT );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYA_MEM_CODE_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親との記憶レベル」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryLevelWithParent( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_PARENT );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYA_MEM_LV_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親との記憶データ」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryDataWithParent( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_PARENT );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYA_MEM_DAT_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親との記憶気持ち」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryFeelWithParent( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_PARENT );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OYA_MEM_FEEL_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親以外との記憶コード」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryCodeWithOther( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_CODE_WITH_OTHERS );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_MEM_CODE_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親以外との記憶レベル」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryLevelWithOther( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_LEVEL_WITH_OTHERS );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_MEM_LV_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親以外との記憶データ」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryDataWithOther( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_DATA_WITH_OTHERS );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_MEM_DAT_VAL, DebugPokeMake::WorkStrBuf() );
  }
  /**
   * @brief 「親以外との記憶気持ち」の表示を更新する
   * @param pokemon_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateMemoryFeelWithOther( const pml::pokepara::PokemonParam* pokemon_param  )
  {
    u32 val = pokemon_param->GetMemories( pml::pokepara::MEMORIES_FEEL_WITH_OTHERS );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_MEM_FEEL_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 「リージョン」の表示を更新する
   *
   * @param  pokemon_param　表示対象のポケモン
   */
  void PokeMakeCore::UpdateRegion( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 region = pokemon_param->GetHWRegionCode();

    if( region >= GFL_NELEMS( REGION_NAME ) )
    {
      GFL_PRINT( "設定されているリージョンが間違っています！ region=%d", region );
      region = 0;
    }
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_REGION_NAME, REGION_NAME[ region ] );
  }
  /**
   * @brief 「国コード」の表示を更新する
   *
   * @param  pokemon_param　表示対象のポケモン
   */
  void PokeMakeCore::UpdateCountryCode( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 val = pokemon_param->GetCountryCode();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_COUNTRY_NAME, DebugPokeMake::WorkStrBuf() );
  }
  void PokeMakeCore::UpdateCountryRegionCode( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 val = pokemon_param->GetCountryRegionCode();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_AREA_NAME, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief メガ進化抜きで最大いくつのフォルムを持っているか返す
   *
   * @param  core 表示対象のポケモン
   *
   * メガ進化するポケモンにはフォルム違いがいないという条件の関数です。
   * 直前で指定のポケモンのLoadPersonalDataが行われているという前提で動作しています
   */
  pml::FormNo PokeMakeCore::GetFormMax( const pml::pokepara::CoreParam *core )
  {
#if 0
    if(core->GetMegaFormCount()){
      return static_cast<pml::FormNo>(1);
    }
#endif

    int formmax = 1;
    MonsNo monsno = core->GetMonsNo();
    // コフキムシ・コフーライ・ガーメイルはフォルム最大数を設定する
    if(monsno==MONSNO_KOHUKIMUSI || monsno==MONSNO_KOHUURAI){
      formmax = FORMNO_BIBIYON_MAX;
    }else if(monsno==MONSNO_GAAMEIRU){
      formmax = FORMNO_MINOMUTTI_MAX;
    }else{
     formmax = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_form_max );
    }
    return formmax;
  }

  /**
   * @brief なつき度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateOtherFamiliarityOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 family = pokemon_param->GetOthersFamiliarity();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), family, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_FAMILY_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief なかよし度の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateOtherFriendshipOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 friendship = pokemon_param->GetOthersFriendship();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), friendship, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_FRIEND_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 他の親の名前の表示を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateOtherOyaNameOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
   pokemon_param->GetPastParentsName( DebugPokeMake::WorkStrBuf() );
   this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTH_OYANAME_VAL, DebugPokeMake::WorkStrBuf() );
  }

  /**
   * @brief 他の親の性別を更新する
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdatePastOyaSexOnDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
   pml::Sex sex = pokemon_param->GetPastParentsSex();
   this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_OTHER_OYASEX_VAL, SEX_NAME[ sex ] );
  }

  /**
   * @brief 過去の親の国コードを更新する
   * @param pokeomn_param  表示対象のポケモン
   * @param index          過去の指定（0-4)
   */
  void PokeMakeCore::UpdatePastParentCountryOnDisplay( const pml::pokepara::PokemonParam* pokemon_param, int index )
  {
    static const u32 pane_table[]={
      PANENAME_PAGE1_PANE_OTH_CTRY_VAL_00,
      PANENAME_PAGE1_PANE_OTH_CTRY_VAL_01,
      PANENAME_PAGE1_PANE_OTH_CTRY_VAL_02,
      PANENAME_PAGE1_PANE_OTH_CTRY_VAL_03,
      PANENAME_PAGE1_PANE_OTH_CTRY_VAL_04,
    };
    u8 country = 0;
    print::StrNumber( DebugPokeMake::WorkStrBuf(), country, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, pane_table[index], DebugPokeMake::WorkStrBuf() );

    GFL_PRINT("numinput update country=%d\n", country );
  }
  /**
   * @brief 過去の親の地域コードを更新する
   * @param pokeomn_param  表示対象のポケモン
   * @param index          過去の指定（0-4)
   */
  void PokeMakeCore::UpdatePastParentAreaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param, int index )
  {
    static const u32 pane_table[]={
      PANENAME_PAGE1_PANE_OTH_AREA_VAL_00,
      PANENAME_PAGE1_PANE_OTH_AREA_VAL_01,
      PANENAME_PAGE1_PANE_OTH_AREA_VAL_02,
      PANENAME_PAGE1_PANE_OTH_AREA_VAL_03,
      PANENAME_PAGE1_PANE_OTH_AREA_VAL_04,
    };
    u8 area = 0;
    print::StrNumber( DebugPokeMake::WorkStrBuf(), area, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, pane_table[index], DebugPokeMake::WorkStrBuf() );

    GFL_PRINT("numinput update area=%d\n",  area);
  }

  /**
   * @brief 個性乱数の表示更新
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdatePersonalRandDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    PrintHighLowNumString( pokemon_param->GetPersonalRnd(), PANENAME_PAGE1_PANE_RANDH_VAL,
                                                             PANENAME_PAGE1_PANE_RANDL_VAL );
  }

  /**
   * @brief トレーナーIDの表示更新
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateTrainerIdDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    PrintHighLowNumString( pokemon_param->GetID(), PANENAME_PAGE1_PANE_TRAINER_ID_00,
                                                    PANENAME_PAGE1_PANE_TRAINER_ID_01 );
  }

  /**
   * @brief レア乱数の表示更新
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::UpdateRareRandomDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    PrintHighLowNumString( pokemon_param->GetRareRnd(), PANENAME_PAGE1_PANE_RARE_RND_VAL_00,
                                                        PANENAME_PAGE1_PANE_RARE_RND_VAL_01 );
  }

  void PokeMakeCore::UpdateTrimianHairDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u8 count = pokemon_param->TORIMIAN_GetBodyHairCount();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), count, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRI_HAIR_VAL, DebugPokeMake::WorkStrBuf() );
  }
  void PokeMakeCore::UpdateTrimianCountDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u8 count = pokemon_param->TORIMIAN_GetKeepDayCount();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), count, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRI_HAIRCNT_VAL, DebugPokeMake::WorkStrBuf() );
  }
  void PokeMakeCore::UpdateTrimianCountMaxDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u8 maxcount = pokemon_param->TORIMIAN_GetKeepMaxDayCount();
    print::StrNumber( DebugPokeMake::WorkStrBuf(), maxcount, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRI_HAIRMAX_VAL, DebugPokeMake::WorkStrBuf() );
  }

  void PokeMakeCore::UpdateConditionDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u8 val;
    val = pokemon_param->GetCondition(pml::pokepara::CONDITION_STYLE);
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CON_VAL_01, DebugPokeMake::WorkStrBuf() );

    val = pokemon_param->GetCondition(pml::pokepara::CONDITION_BEAUTIFUL);
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CON_VAL_02, DebugPokeMake::WorkStrBuf() );

    val = pokemon_param->GetCondition(pml::pokepara::CONDITION_CUTE);
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CON_VAL_03, DebugPokeMake::WorkStrBuf() );

    val = pokemon_param->GetCondition(pml::pokepara::CONDITION_CLEVER);
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CON_VAL_04, DebugPokeMake::WorkStrBuf() );

    val = pokemon_param->GetCondition(pml::pokepara::CONDITION_STRONG);
    print::StrNumber( DebugPokeMake::WorkStrBuf(), val, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_CON_VAL_05, DebugPokeMake::WorkStrBuf() );

  }

  void PokeMakeCore::UpdateEggWazaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param , int idx )
  {
    WazaNo wazano = pokemon_param->GetTamagoWazaNo(idx);
    m_msgData->GetString( label_waza_name, *(DebugPokeMake::WorkStrBuf(1)) );
    m_wordset->RegisterWazaName( 0, wazano );
    m_wordset->Expand( DebugPokeMake::WorkStrBuf(0), DebugPokeMake::WorkStrBuf(1) );
    this->SetTextboxPaneString( m_lytWk, PANE_INDEX_OF_EGG_WAZA_NAME[idx], DebugPokeMake::WorkStrBuf() );
  }

  void PokeMakeCore::UpdateDefaultPowerUpDisplay( const DefaultPowerUpDesc& desc )
  {
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_REASON_NAME, DEFAULT_POWERUP_REASON_NAME[ desc.reason ] );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), desc.rankUp_Attack, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_ATK_VAL, DebugPokeMake::WorkStrBuf() );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), desc.rankUp_Defense, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_DEF_VAL, DebugPokeMake::WorkStrBuf() );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), desc.rankUp_SpAttack, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_SPATK_VAL, DebugPokeMake::WorkStrBuf() );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), desc.rankUp_SpDefense, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_SPDEF_VAL, DebugPokeMake::WorkStrBuf() );

    print::StrNumber( DebugPokeMake::WorkStrBuf(), desc.rankUp_Agility, 3, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_POWERUP_AGI_VAL, DebugPokeMake::WorkStrBuf() );
  }

  void PokeMakeCore::SwitchTrainingFlag( pml::pokepara::PokemonParam* pokemon_param, pml::pokepara::PowerID power_id ) const
  {
    bool isDone = pokemon_param->IsTrainingDone( power_id );

    if( isDone )
    {
      pokemon_param->ResetTrainingDone( power_id );
    }
    else
    {
      pokemon_param->SetTrainingDone( power_id );
    }

    pokemon_param->RecalculateCalcData();
  }

  void PokeMakeCore::UpdateTrainingFlagDisplay( const pml::pokepara::PokemonParam* pokemon_param )
  {
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_00, pokemon_param->IsTrainingDone( pml::pokepara::POWER_HP )    ? ( L"○" ) : ( L"-" ) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_01, pokemon_param->IsTrainingDone( pml::pokepara::POWER_ATK )   ? ( L"○" ) : ( L"-" ) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_02, pokemon_param->IsTrainingDone( pml::pokepara::POWER_DEF )   ? ( L"○" ) : ( L"-" ) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_03, pokemon_param->IsTrainingDone( pml::pokepara::POWER_AGI )   ? ( L"○" ) : ( L"-" ) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_04, pokemon_param->IsTrainingDone( pml::pokepara::POWER_SPATK ) ? ( L"○" ) : ( L"-" ) );
    this->SetTextboxPaneString( m_lytWk, PANENAME_PAGE1_PANE_TRAINING_VALUE_05, pokemon_param->IsTrainingDone( pml::pokepara::POWER_SPDEF ) ? ( L"○" ) : ( L"-" ) );
  }

  /**
   * @brief 現在の特性INDEXの番号だけボタン色を濃くする
   * @param pokeomn_param  表示対象のポケモン
   *
   */
  void PokeMakeCore::UpdateTokuseiIndexSwitchTrans( const pml::pokepara::PokemonParam* pokemon_param )
  {
    u32 index = pokemon_param->GetTokuseiIndex();
    gfl2::lyt::ColorU8 half_trans( 255,255,255,100 );
    gfl2::lyt::ColorU8 none_trans( 255,255,255,255 );
    gfl2::lyt::LytMaterial * material[3];

    gfl2::lyt::LytWk* layout_work = m_lytWk;
    material[0] = layout_work->GetMaterial( PANENAME_PAGE1_MATERIAL_TOKUSEI_BTN1_OF_TOKUSEI_BTN1 );
    material[1] = layout_work->GetMaterial( PANENAME_PAGE1_MATERIAL_TOKUSEI_BTN2_OF_TOKUSEI_BTN2 );
    material[2] = layout_work->GetMaterial( PANENAME_PAGE1_MATERIAL_TOKUSEI_BTN3_OF_TOKUSEI_BTN3 );
    for(u32 i=0;i<3;i++){
      if(i==index){
        material[i]->SetColor( app::util::G2DUtil::COLOR_INDEX_BLACK, none_trans );  // マテリアルカラー白の変更
      }else{
        material[i]->SetColor( app::util::G2DUtil::COLOR_INDEX_BLACK, half_trans );  // マテリアルカラー白の変更
      }
    }
  }



  /**
   * @brief 現在設定されている特性が何番目の特性と一致しているかチェックして特性INDEXを書き換える
   * @param pokeomn_param  表示対象のポケモン
   *
   * デバッグで書き換えると特性が一致しない事もあるのでその時は特性INDEXを書き換えない
   */
  void PokeMakeCore::CheckTokuseiNo( pml::pokepara::PokemonParam* pokemon_param )
  {
    TokuseiNo tokusei = pokemon_param->GetTokuseiNo();
    MonsNo monsno      = pokemon_param->GetMonsNo();
    pml::FormNo formno = pokemon_param->GetFormNo();
    pml::personal::LoadPersonalData( monsno, formno );
    TokuseiNo p_tokusei[3];
    p_tokusei[0] = static_cast<TokuseiNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei1 ) );
    p_tokusei[1] = static_cast<TokuseiNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei2 ) );
    p_tokusei[2] = static_cast<TokuseiNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_tokusei3 ) );
    for(int i=0;i<3;i++){
      if(p_tokusei[i]==tokusei){
        pokemon_param->SetTokuseiIndex(i);
        break;
      }
    }
    UpdateTokuseiIndexSwitchTrans( pokemon_param );
  }


  /**
   * @brief 32bit数値を16bitずつで表示する時の共通表示関数
   * @param value     表示する32bitの数値
   * @param high_pane 上位のペイン
   * @param low_pane  下位のペイン
   *
   */
  void PokeMakeCore::PrintHighLowNumString( u32 value, int high_pane, int low_pane )
  {
    u16 high = value>>16;
    u16 low  = value&0xffff;
    print::StrNumber( DebugPokeMake::WorkStrBuf(), high, 5, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, high_pane, DebugPokeMake::WorkStrBuf() );
    print::StrNumber( DebugPokeMake::WorkStrBuf(), low, 5, print::NUM_DISP_SPACE, print::NUM_CODE_HANKAKU );
    this->SetTextboxPaneString( m_lytWk, low_pane, DebugPokeMake::WorkStrBuf() );

  }

  /**
   * @brief デバッグ生成フラグを表示
   * @param pokeomn_param  表示対象のポケモン
   */
  void PokeMakeCore::PrintDebugMakeFlag( const pml::pokepara::PokemonParam * pp )
  {
    if( pp->GetDebugEditFlag() == false )
    {
      this->SetTextboxPaneString( m_lytWk_Upper, PANENAME_SUB_PANE_DEBUG_MAKE_01, L"０" );
    }
    else
    {
      this->SetTextboxPaneString( m_lytWk_Upper, PANENAME_SUB_PANE_DEBUG_MAKE_01, L"１" );
    }
  }


} // namespace PokeMake
} // namespace debug

#endif // PM_DEBUG
