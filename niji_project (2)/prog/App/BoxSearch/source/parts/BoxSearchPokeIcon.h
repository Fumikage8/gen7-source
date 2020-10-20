#ifndef NIJI_PROJECT_APP_BOX_SEARCH_POKE_ICON_H_INCLUDED
#define NIJI_PROJECT_APP_BOX_SEARCH_POKE_ICON_H_INCLUDED
#pragma once
//======================================================================
/**
 * @file    BoxSearchPokeIcon.h
 * @date    2015/08/06 12:38:08
 * @author  fukushima_yuya
 * @brief   ポケモンの条件検索で表示するポケモンアイコン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/BoxSearch/source/BoxSearchConst.h"

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )
class DrawerBase;   //!< 描画共通処理
class SortData;     //!< ソートデータ
GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class PokeIcon;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( PokeTool )
struct SimpleParam;
GFL_NAMESPACE_END( PokeTool )

GFL_NAMESPACE_BEGIN( Savedata )
class ZukanData;   //!< 描画共通処理
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   ポケモンアイコン
//------------------------------------------------------------------------------
class PokeIcon
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeIcon );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   * @param   pBase   描画共通処理
   */
  //------------------------------------------------------------------
  PokeIcon( DrawerBase* pBase, SortData* pSortData, Savedata::ZukanData* pZukanData, u32 btnMax );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeIcon( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   */
  //------------------------------------------------------------------
  void SetPokeIcon( u32 pane_index, MonsNo monsNo );

  //------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替え
   * @param   index   アイコンインデックス
   */
  //------------------------------------------------------------------
  void ReplaceIcon( u32 index );

private:
  DrawerBase*     m_pBase;

  SortData*       m_pSortData;

  Savedata::ZukanData*    m_pZukanData;

  app::tool::PokeIcon*    m_pPokeIcon;

  u32     m_ButtonMax;

  gfl2::lyt::LytPicture** m_pIconPane;

  PokeTool::SimpleParam*  m_pSimpleParam;

  bool*     m_pLoadingIcon;
};

GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // NIJI_PROJECT_APP_BOX_SEARCH_POKE_ICON_H_INCLUDED