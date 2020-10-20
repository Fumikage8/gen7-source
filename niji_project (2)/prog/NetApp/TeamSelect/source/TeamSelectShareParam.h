//======================================================================
/**
 * @file TeamSelectShareParam.h
 * @date 2015/07/03 19:04:37
 * @author uchida_yuto
 * @brief チーム選択プロジェクト内限定での共有パラメータ宣言
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __TEAM_SELECT_SHARE_PARAM_H_INCLUDED__
#define __TEAM_SELECT_SHARE_PARAM_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <NetApp/TeamSelect/include/TeamSelectAppParam.h>

//  前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeIcon;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

  /**
   * @struct TEAM_VIEW_PARAM
   * @brief  チーム表示に必要なパラメータ
   * @note    上下画面のViewで使うので一つのViewクラスで宣言できないのでここで宣言
   */
  struct TEAM_VIEW_PARAM
  {
    //  チームデータ
    TEAM_DATA*  pTeamData;

    //  チーム内のポケモンのアイコンテクスチャーid
    u32 aPokeIconDataId[ pml::PokeParty::MAX_MEMBERS ];
  };

GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // __TEAM_SELECT_SHARE_PARAM_H_INCLUDED__