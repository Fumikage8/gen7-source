//======================================================================
/**
 * @file    LayoutDefaultFont.cpp
 * @brief   レイアウトエディタで指定されるデフォルトフォント
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    12/07/20
 *
 * @li  sangoのlayout_default_font.cppを移植
 */
//======================================================================
#include <Print/include/SystemFont.h>
#include <Print/include/LayoutDefaultFont.h>

GFL_NAMESPACE_BEGIN(print)

const char *const LAYOUT_DEFAULT_FONT = "cbf_std_kujira.bffnt";
//const char *const LAYOUT_DEFAULT_FONT = "jphanall.bffnt";
const char *const LAYOUT_NUM_FONT = "num_font.bffnt";
#if KANTAIHANTAI_TEST
const char *const LAYOUT_BATTLE_FONT = "kujira_battle.bffnt";  //これはレイアウト修正に呼応する必要があるため
#else
const char *const LAYOUT_BATTLE_FONT = "kujira_battle.bffnt";
#endif
const char *const LAYOUT_TWHK_FONT = "fontres_twhk.bffnt";

GFL_NAMESPACE_END(print)
