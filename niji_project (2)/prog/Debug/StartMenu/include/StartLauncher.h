#if PM_DEBUG
//=============================================================================
/**
 * @file    StartLauncher.h
 * @brief   ゲーム開始時のランチャー用リスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.18
 */
//=============================================================================
#if !defined( START_LAUNCHER_H_INCLUDED )
#define START_LAUNCHER_H_INCLUDED
#pragma once

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>
#pragma once
GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//=============================================================================
/**
 * @class   StartLauncher
 * @brief   ゲーム開始時のランチャー処理クラス
 * @date    2015.02.18
 */
//=============================================================================
class StartLauncher
  : public launcher::ListData
{
private:
  static const u32 LIST_MAX = 43;       //!< 項目数
  static const u32 ItemList[LIST_MAX];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  StartLauncher(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~StartLauncher();

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
  virtual launcher::ListData::RequestID SelectListener( u32 pos, launcher::LauncherProc * launcher );

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




private:
  void CallDebugBattleProc();
  void CallSoundTestProc();

};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // START_LAUNCHER_H_INCLUDED

#endif  // PM_DEBUG
