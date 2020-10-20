#if PM_DEBUG
//=============================================================================
/**
 * @file    LauncherListData.h
 * @brief   ランチャー処理リストデータ
 * @author  Hiroyuki Nakamura
 * @date    2015.02.19
 */
//=============================================================================
#if !defined( LAUNCHER_LISTDATA_H_INCLUDED )
#define LAUNCHER_LISTDATA_H_INCLUDED

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

#include <GameSys/include/GameProc.h>

namespace gfl2 { namespace str { class StrBuf; } }

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(launcher)

// 前方宣言
class LauncherProc;

//=============================================================================
/**
 * @class   ListData
 * @brief   リストデータクラス
 * @date    2015.02.05
 */
//=============================================================================
class ListData
{
public:

  //! リスナーからランチャーへのリクエストID
  enum RequestID
  {
    REQID_NONE = 0,   //!< リクエストなし
    REQID_CALL,       //!< 他Proc呼び出し
    REQID_RESET,      //!< リストを再設定
    REQID_FINISH      //!< 終了
  };

  virtual ~ListData(){}


  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.05
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual RequestID SelectListener( u32 pos, LauncherProc * launcher ) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   項目文字列のアークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void) = 0;

  //-----------------------------------------------------------------------------
  /**
   * @func    OnPrintString
   * @brief   文字を描画されるときに呼ばれる。strBufの中身を書き換えたい時に使用するとよい。
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   strBuf 描画しようとしている文字(通常ならばListTableのposにあったものが入っている)
   */
  //-----------------------------------------------------------------------------
  virtual void OnPrintString( u32 pos, gfl2::str::StrBuf * strBuf ) {}
};


GFL_NAMESPACE_END(launcher)
GFL_NAMESPACE_END(debug)

#endif  // LAUNCHER_LISTDATA_H_INCLUDED

#endif  // PM_DEBUG
