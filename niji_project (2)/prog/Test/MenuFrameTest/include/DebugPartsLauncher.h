#ifndef __DEBUG_PARTS_LAUNCHER_H__
#define __DEBUG_PARTS_LAUNCHER_H__
#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugPartsLauncher.h
 * @brief   ランチャーリスト処理
 * @author  k.ohno
 * @date    2015.02.18
 */
//=============================================================================

#pragma once

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )

//-----------------------------------------------------------------------------
/**
 * @class   DebugPartsLauncher
 * @brief   パーツのデバッグをするランチャークラス
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
class DebugPartsLauncher
  : public debug::launcher::ListData
{
private:
  static const u32 ItemList[];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  DebugPartsLauncher(void);
  virtual ~DebugPartsLauncher();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.18
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual debug::launcher::ListData::RequestID SelectListener( u32 pos, debug::launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);
};







//-----------------------------------------------------------------------------
/**
 * @class   DebugLangSelectLauncher
 * @brief   言語選択しながらセーブデータで起動するランチャークラス
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
class DebugLangSelectLauncher
  : public debug::launcher::ListData
{
private:
  static const u32 ItemList[];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  DebugLangSelectLauncher(void);
  virtual ~DebugLangSelectLauncher();

  //-----------------------------------------------------------------------------
  /**
   * @func    SelectListener
   * @brief   決定時のリスナー
   * @date    2015.02.18
   *
   * @param   pos       カーソル位置 ( キャンセル時はCURSOR_POS_CANCEL )
   * @param   launcher  呼び出し元のランチャークラス
   *
   * @return  ランチャーへのリクエストID
   */
  //-----------------------------------------------------------------------------
  virtual debug::launcher::ListData::RequestID SelectListener( u32 pos, debug::launcher::LauncherProc * launcher );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetArcDataID
   * @brief   アークデータID取得
   * @date    2015.02.18
   *
   * @return  アークデータID
   */
  //-----------------------------------------------------------------------------
  virtual gfl2::fs::ArcFile::ARCDATID GetArcDataID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブル取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブル
   */
  //-----------------------------------------------------------------------------
  virtual const u32 * GetListTable(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetListTable
   * @brief   項目文字列データテーブルの要素数取得
   * @date    2015.02.18
   *
   * @return  文字列データテーブルの要素数
   */
  //-----------------------------------------------------------------------------
  virtual u32 GetListTableSize(void);
};



GFL_NAMESPACE_END(MenuFrameTest)
GFL_NAMESPACE_END(Test)

#endif  // PM_DEBUG
#endif //__DEBUG_PARTS_LAUNCHER_H__
