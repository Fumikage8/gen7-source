//=============================================================================
/**
 *
 *  @file   EventWorkDef.h
 *  @brief  フラグ、ワーク領域定義のプログラムへのマッピング
 *  @author hosaka genya(XY)
 *  @data   2011.05.26
 *  @author Satoshi Nohara
 *  @date   2005.11.07
 *
 */
//=============================================================================
#ifndef __EVENTWORKDEF_H__
#define __EVENTWORKDEF_H__

//#include <gflib.h>
#include "niji_conv_header/field/flagwork/FlagDefine.h"
#include "niji_conv_header/field/flagwork/WorkDefine.h"

namespace Field
{

//======================================================================
//  フラグ定義関連
//======================================================================
enum {
  ///  トレーナーフラグ開始ナンバー
  TR_FLAG_START       = TR_FLAG_AREA_START,

  ///  隠しアイテムフラグ開始ナンバー
  FH_FLAG_START       = FLAG_HIDEITEM_AREA_START,

  ///  システムフラグ開始ナンバー
  SYSFLAG_START       = SYS_FLAG_AREA_START,

  ///  時間経過フラグ開始ナンバー
  TIMEFLAG_START      = TMFLG_AREA_START,

  //--------------------------------------------------------------
  //  フラグオフセット
  //--------------------------------------------------------------
  ///セーブフラグスタート　オフセット
  SVFLG_START         = 0x0000,

  ///制御フラグスタート　オフセット
  SCFLG_START         = 0x3000,

  //--------------------------------------------------------------
  //  セーブしないフラグ関連定義 制御フラグ
  //--------------------------------------------------------------
  ///制御フラグ最大数
  CTRLFLAG_MAX        = 64,
  ///制御フラグ占有エリア
  CTRLFLAG_AREA_MAX   = CTRLFLAG_MAX/8 + ( (CTRLFLAG_MAX%8==0) ? 0 : 1 ),
//  CTRLFLAG_AREA_MAX = (gfl::math::RoundUp<int>(CTRLFLAG_MAX,8) / 8),
};


//======================================================================
//  ワーク関連定義
//======================================================================
enum {
  ///セーブワーク定義スタート　オフセット
  SVWK_START          = 0x4000,

  ///データ交換・汎用ワーク定義スタート　オフセット
  SCWK_START          = 0x8000,

  ///データ交換・汎用ワーク定義終了　オフセット
  SCWK_END            = 0xc000,
};


} // namespace Field

#endif // __EVENTWORKDEF_H__
