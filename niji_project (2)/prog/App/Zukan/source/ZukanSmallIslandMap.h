#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANSMALLISLANDMAP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANSMALLISLANDMAP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanSmallIslandMap.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.10
 @brief   図鑑の島図鑑トップ画面の島マップ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanDistributionPokeDataAccessor;
  class ZukanDistributionPositionDataAccessor;
  class ZukanDistributionZoneDataAccessor;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanSmallIslandMap
 @brief     図鑑の島図鑑トップ画面の島マップ
 */
//==============================================================================
class ZukanSmallIslandMap
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanSmallIslandMap);

public:
  //! @brief  生息タイプ
  enum HabitatType
  {
    HABITAT_TYPE_UNKNOWN,   //!< 生息地不明
    HABITAT_TYPE_OTHER,     //!< 他の島に生息している
    HABITAT_TYPE_EXIST,     //!< この島にいる
    HABITAT_TYPE_NOT_SEE,   //!< まだ見ていない
  };

public:
  ZukanSmallIslandMap(
    app::util::G2DUtil*                           pG2DUtil,
    gfl2::lyt::LytMultiResID                      lytRes,
    u32                                           lytWkId,
    gfl2::lyt::LytWk*                             pLytWk,
    ZukanType::InfoType                           infoType,
    const ZukanDistributionPokeDataAccessor*      pDistributionPokeDataAccessor,
    const ZukanDistributionPositionDataAccessor*  pDistributionPositionDataAccessor,
    const ZukanDistributionZoneDataAccessor*      pDistributionZoneDataAccessor
  );
  virtual ~ZukanSmallIslandMap();

  HabitatType SetPokeData(ZukanType::InfoType infoType, MonsNo monsNo, bool isSee);

private:
  void initLayout(void);
  void initHeroIcon(ZukanType::InfoType infoType);

private:
  struct IslandData
  {
    gfl2::lyt::LytPane*  islandTopPane;
    gfl2::lyt::LytPane*  rangePane;
  };
  struct IconData
  {
    gfl2::lyt::LytPane*  iconTopPane;
    u32                  distributionAnimId;
  };

  enum
  {
    ICON_NUM_MAX  = 40
  };

private:
  app::util::G2DUtil*                           m_pG2DUtil;
  gfl2::lyt::LytMultiResID                      m_lytRes;
  u32                                           m_lytWkId;
  gfl2::lyt::LytWk*                             m_pLytWk;
  const ZukanDistributionPokeDataAccessor*      m_pDistributionPokeDataAccessor;
  const ZukanDistributionPositionDataAccessor*  m_pDistributionPositionDataAccessor;
  const ZukanDistributionZoneDataAccessor*      m_pDistributionZoneDataAccessor;

  IslandData           m_islandData[ZukanType::INFO_TYPE_ISLAND_NUM];
  IconData             m_iconData[ICON_NUM_MAX];
  gfl2::lyt::LytPane*  m_allTopPane;
  gfl2::lyt::LytParts* m_playerPane;
  gfl2::lyt::LytPane*  m_cursorPane;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANSMALLISLANDMAP_H_INCLUDED

