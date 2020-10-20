﻿//=============================================================================
/**
 * @file    BagGamedata.h
 * @brief   バッグ画面 ゲーム中に保持し続けるデータ
 * @author  Hiroyuki Nakamura
 * @date    2011.10.20
 */
//=============================================================================
#if !defined( BAG_GAMEDATA_H_INCLUDED )
#define BAG_GAMEDATA_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MyItemSaveDef.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//=============================================================================
/**
 * @class PlayData
 * @brief バッグ画面ゲームデータ
 * @date  2011.10.20
 */
//=============================================================================
class PlayData
{
public:
  static const int POCKET_DATA_MAX = FIELD_BAG_POCKET_MAX+1;  //!< ポケットデータ数 ( フリースペース分 + 1 )
  static const int FREE_SPACE_POCKET = 6;                     //!< フリースペースの仮想ポケット番号

  //! ポケットデータ
  struct POCKET_DATA
  {
    u16 pos;      //!< カーソル位置
    u16 scroll;   //!< スクロール値
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  PlayData(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~PlayData();

  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケットデータ取得
   *
   * @param   pocket  ポケット番号
   *
   * @return  ポケットデータ
   */
  //-----------------------------------------------------------------------------
  const POCKET_DATA * GetPocketData( u32 pocket );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケットデータセット
   *
   * @param   pocket  ポケット番号
   * @param   pos     カーソル位置
   * @param   scroll  スクロール値
   */
  //-----------------------------------------------------------------------------
  void SetPocketData( u32 pocket, u32 pos, u32 scroll );

  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケット位置を取得
   *
   * @return  ポケット位置
   */
  //-----------------------------------------------------------------------------
  u32 GetPocketPos(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケット位置を設定
   *
   * @param   ポケット位置
   */
  //-----------------------------------------------------------------------------
  void SetPocketPos( u32 pos );


private:
  //-----------------------------------------------------------------------------
  /**
   * @brief   ポケットデータ初期化
   */
  //-----------------------------------------------------------------------------
  void InitPocketData(void);


private:
  POCKET_DATA	m_pocketData[POCKET_DATA_MAX];  //!< ポケットデータ
  u32 m_pocketPos;                            //!< ポケット位置
};


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)

#endif  // BAG_GAMEDATA_H_INCLUDED
