#if PM_DEBUG
//=============================================================================
/**
 * @file    d_nakahiro.h
 * @brief   中村用ランチャーリスト処理
 * @author  Hiroyuki Nakamura
 * @date    2015.02.18
 */
//=============================================================================
#if !defined( D_NAKAHIRO_H_INCLUDED )
#define D_NAKAHIRO_H_INCLUDED
#pragma once

#include <Debug/Launcher/include/LauncherProc.h>
#include <Debug/Launcher/include/LauncherListData.h>

#include "App/Shop/include/ShopAppParam.h"


GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(startmenu)

//-----------------------------------------------------------------------------
/**
 * @class   LauncherNakahiro
 * @brief   中村用ランチャークラス
 * @date    2015.02.18
 */
//-----------------------------------------------------------------------------
class LauncherNakahiro
  : public launcher::ListData
{
private:
  static const u32 LIST_MAX = 21;       //!< 項目数
  static const u32 ItemList[LIST_MAX];  //!< 項目文字列データテーブル

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  LauncherNakahiro(void);
  virtual ~LauncherNakahiro();

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
  void InitItem(void);
  void InitParty(void);
  void InitShopParam(
        App::Shop::APP_PARAM * param,
        App::Shop::ShopType type,
        u32 id,
        u32 howmany_msg,
        u32 really_msg,
        u32 really_msg2,
        u32 hereitis_msg,
        u32 youcanttake_msg,
        u32 nomoney_msg,
        u32 selected_msg );
};

GFL_NAMESPACE_END(startmenu)
GFL_NAMESPACE_END(debug)

#endif  // D_NAKAHIRO_H_INCLUDED

#endif  // PM_DEBUG
