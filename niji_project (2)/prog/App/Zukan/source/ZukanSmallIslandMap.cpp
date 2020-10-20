//==============================================================================
/**
 @file    ZukanSmallIslandMap.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.10
 @brief   図鑑の島図鑑トップ画面の島マップ
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <Savedata/include/MyStatus.h>
#include <Field/FieldStatic/include/Zone/ZoneDataAccessor.h>
#include <Field/FieldStatic/include/Zone/ZoneDataLoader.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

#include <App/TownMapParts/include/TownMapPartsDraw.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/island_top.h>
#include <niji_conv_header/app/zukan/island_top_pane.h>
#include <niji_conv_header/app/zukan/island_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include "ZukanDefine.h"
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanContents.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanDistributionPokeDataAccessor.h"
#include "ZukanDistributionPositionDataAccessor.h"
#include "ZukanDistributionZoneDataAccessor.h"

#include "ZukanSmallIslandMap.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanSmallIslandMap
//
//==============================================================================
ZukanSmallIslandMap::ZukanSmallIslandMap(
    app::util::G2DUtil*                           pG2DUtil,
    gfl2::lyt::LytMultiResID                      lytRes,
    u32                                           lytWkId,
    gfl2::lyt::LytWk*                             pLytWk,
    ZukanType::InfoType                           infoType,
    const ZukanDistributionPokeDataAccessor*      pDistributionPokeDataAccessor,
    const ZukanDistributionPositionDataAccessor*  pDistributionPositionDataAccessor,
    const ZukanDistributionZoneDataAccessor*      pDistributionZoneDataAccessor
)
  : m_pG2DUtil(pG2DUtil),
    m_lytRes(lytRes),
    m_lytWkId(lytWkId),
    m_pLytWk(pLytWk),
    m_pDistributionPokeDataAccessor(pDistributionPokeDataAccessor),
    m_pDistributionPositionDataAccessor(pDistributionPositionDataAccessor),
    m_pDistributionZoneDataAccessor(pDistributionZoneDataAccessor)
{
  this->initLayout();
  this->initHeroIcon(infoType);
}

ZukanSmallIslandMap::~ZukanSmallIslandMap()
{}

ZukanSmallIslandMap::HabitatType ZukanSmallIslandMap::SetPokeData(ZukanType::InfoType infoType, MonsNo monsNo, bool isSee)
{
  APP_ZUKAN_DISTRIBUTION_ZONE_DATA_BIT_FLAG_DECLARATION(zoneDataBitFlag);
  
  HabitatType habitatType = HABITAT_TYPE_UNKNOWN;

  s32 l_islandDataIndex = infoType - ZukanType::INFO_TYPE_ISLAND_START;     // m_islandDataのインデックスは0始まり
  s32 l_islandId        = infoType - ZukanType::INFO_TYPE_ISLAND_START +1;  // 島番号は1始まり

  // ポケモンごとの分布データ
  ZukanDistributionPokeDataAccessor::PokeDistributionData pokeDistData;
  m_pDistributionPokeDataAccessor->GetPokeDistributionData(monsNo, &pokeDistData);

  // 今の島に生息しているか
  bool currIslandFlag  = false;  // 今の島に生息しているときはtrue  // 今の島にも他の島にも生息しているときはどちらもtrueになる
  bool otherIslandFlag = false;  // 他の島に生息しているときはtrue  // 今の島に生息しているときは使用しないのでそのときは間違った値が入っているかもしれない
  if(pokeDistData.placeIdDataNum > 0)
  {
    for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_pDistributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
      if( ((static_cast<s32>(placeIdData.islandNo)) == l_islandId) || ((static_cast<s32>(placeIdData.islandNo))==0&&l_islandId==1) )  // 島番号は1始まり、l_islandIdは1始まり  // 開発中はデータにまだ正しい島番号が入っていないので。正しいデータになったら下行に変える。
#else
      if((static_cast<s32>(placeIdData.islandNo)) == l_islandId)  // 島番号は1始まり、l_islandIdは1始まり
#endif
      {
        currIslandFlag  = true;
        break;
      }
      else
      {
        if((static_cast<s32>(placeIdData.islandNo)) != 0)  // 島番号は1始まり、0は無効なデータ  // @fix GFNMCat[4769] 図鑑：無効な分布データしかないときに「他の島にいます」と出る可能性がある
        {
          otherIslandFlag = true;
        }
      }
    }
  }

  // 戻り値
  if(!isSee)
  {
    habitatType = HABITAT_TYPE_NOT_SEE;
  }
  else if(currIslandFlag)
  {
    habitatType = HABITAT_TYPE_EXIST;
  }
  else if(otherIslandFlag)
  {
    habitatType = HABITAT_TYPE_OTHER;
  }

  ////////////////////////
  // 表示
  ////////////////////////
  // 島切り替え
  {
    for(s32 i=0; i<ZukanType::INFO_TYPE_ISLAND_NUM; ++i)
    {
      m_islandData[i].islandTopPane->SetVisible( i == l_islandDataIndex );
    }
  }

  // 今の島に生息しているとき
  if(isSee && currIslandFlag)  // 対象のポケモンを見ていなければならない
  {
    /*
    小さいマップ上に配置ID全部を表示すると見辛くなるので、コメントアウト。
    // 表示するアイコン
    s32 iconNum = 0;
    for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_pDistributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);

      for(u32 j=0; j<placeIdData.posDataNum; ++j)
      {
        if( iconNum < ICON_NUM_MAX)
        {
          f32 realX = placeIdData.posData[j].intX /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
          f32 realZ = placeIdData.posData[j].intZ /ZukanDistributionPositionDataAccessor::POS_SCALL_VAL;
          f32 paneX = realX;
          f32 paneY = realZ;
          App::TownMap::TownMapPartsDraw::GetMapPos(m_islandData[l_islandDataIndex].rangePane, &paneX, &paneY);
          
          m_iconData[i].iconTopPane->SetTranslate(gfl2::math::VEC2(paneX, paneY));
          m_iconData[i].iconTopPane->SetVisible(true);
          ++iconNum;
        }
        else
        {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT==0
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。分布用のアイコンの個数が最大数を越えた。  // 開発中は最大数を越えることがあるのでdefine分岐。仮データならいいが、そうでないなら分布をまとめるか。
#endif
          break;
        }
      }
    }
    */

    // 配置ID→ゾーンデータ
    for(u32 i=0; i<pokeDistData.placeIdDataNum; ++i)
    {
      ZukanDistributionPositionDataAccessor::PlaceIdData placeIdData;
      m_pDistributionPositionDataAccessor->GetPlaceIdData(pokeDistData.placeIdData[i].placeId, &placeIdData);

      // ゾーンデータ
      if( placeIdData.zoneDataNo != ZukanDistributionPositionDataAccessor::POSITION_PLACE_ID_ZONE_DATA_NO_INVALID )
      {
        if((static_cast<s32>(placeIdData.islandNo)) == l_islandId)  // 島番号は1始まり、l_islandIdは1始まり  // @fix NMCat[1586] 島図鑑画面と分布画面でエンカウントマークの表示位置が異なる  ←ゾーン単位でアイコン表示をする際にどの島のデータか判定するように修正。
        {
          ZukanDistributionZoneDataAccessor::ZoneDataBitFlagSetOn(zoneDataBitFlag, placeIdData.zoneDataNo);
        }
      }
      else
      {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT==0
        GFL_RELEASE_PRINT("########################################################################\n");
        GFL_RELEASE_PRINT("WARNING: zoneDataNo INVALID: placeId=%d: islandNo=%d, zoneDataNo=INVALID\n", pokeDistData.placeIdData[i].placeId, placeIdData.islandNo);
        GFL_RELEASE_PRINT("########################################################################\n");
        if((static_cast<s32>(placeIdData.islandNo))!=0)  // 島番号は1始まり。
        {
          // 島番号に値が入っているときにゾーンデータ番号が不明だったらASSERTにする。島番号に値が入っていなかったら(0だったら)テスト用の場所だろうから無視していい。
          GFL_ASSERT(0);  // 開発中に気付かせるASSERT。ゾーンデータ番号が不明。  // 開発中はデータがまだ正しくないので今だけdefine分岐しておく。
        }
#endif
      }
    }
    // 表示するアイコン
    s32 iconNum = 0;
    for(u32 zoneDataNo=0; zoneDataNo<ZukanDistributionZoneDataAccessor::ZONE_DATA_NUM_MAX; ++zoneDataNo)
    {
      if( ZukanDistributionZoneDataAccessor::ZoneDataBitFlagIsOn(zoneDataBitFlag, zoneDataNo) )
      {
        if( iconNum < ICON_NUM_MAX)
        {
          ZukanDistributionZoneDataAccessor::ZoneData zoneData;
          m_pDistributionZoneDataAccessor->GetZoneData(zoneDataNo, &zoneData);

          if((static_cast<s32>(zoneData.islandNo)) == l_islandId)  // 島番号は1始まり、l_islandIdは1始まり  // zoneDataBitFlagを立てるときに島番号をチェックしているが、念のためここでもチェックしておく。  // @fix NMCat[1586] 島図鑑画面と分布画面でエンカウントマークの表示位置が異なる  ←ゾーン単位でアイコン表示をする際にどの島のデータか判定するように修正。
          {
            f32 realX = zoneData.intX /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
            f32 realZ = zoneData.intZ /ZukanDistributionZoneDataAccessor::POS_SCALL_VAL;
            f32 paneX = realX;
            f32 paneY = realZ;
            App::TownMap::TownMapPartsDraw::GetMapPos(m_islandData[l_islandDataIndex].rangePane, &paneX, &paneY);
          
            s32 i = iconNum;
            m_iconData[i].iconTopPane->SetTranslate(gfl2::math::VEC2(paneX, paneY));
            m_iconData[i].iconTopPane->SetVisible(true);
            ++iconNum;
          }
          else
          {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
            GFL_PRINT("WARNING(ZukanSmallIslandMap.cpp): place's islandNo=%d, zone's islandNo=%d\n", l_islandId, zoneData.islandNo);
#else
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。配置IDの島番号とゾーンデータの島番号が一致していない。  // 開発中は一致していなくても通す。
#endif
          }
        }
        else
        {
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT
          GFL_PRINT("WARNING(ZukanSmallIslandMap.cpp): iconNum=%d\n", iconNum);
#else
          GFL_RELEASE_PRINT("#################################\n");
          GFL_RELEASE_PRINT("WARNING: iconNum Over: iconNum=%d\n", iconNum);
          GFL_RELEASE_PRINT("#################################\n");
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。分布用のアイコンの個数が最大数を越えた。  // 開発中は最大数を越えることがあるのでdefine分岐。仮データならいいが、そうでないなら分布をまとめるか。
#endif
          break;
        }
      }
    }

    // 使わないアイコンは非表示
    for(s32 i=iconNum; i<ICON_NUM_MAX; ++i)
    {
      m_iconData[i].iconTopPane->SetVisible(false);
    } 
  }
  else
  {
    // 全アイコン非表示にしておく
    for(s32 i=0; i<ICON_NUM_MAX; ++i)
    {
      m_iconData[i].iconTopPane->SetVisible(false);
    }
  }

  return habitatType;
}

void ZukanSmallIslandMap::initLayout(void)
{
  static const u32 ISLAND_TOP_PANE_ID[] =
  {
    PANENAME_ZUKAN_ISD_UPP_000_PANE_LAND_01,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_LAND_02,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_LAND_03,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_LAND_04,
  };
  static const u32 ISLAND_RANGE_PANE_ID[] =
  {
    PANENAME_ZUKAN_ISD_UPP_000_PANE_MAP_RANGE_01,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_MAP_RANGE_02,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_MAP_RANGE_03,
    PANENAME_ZUKAN_ISD_UPP_000_PANE_MAP_RANGE_04,
  };

  for(s32 i=0; i<ZukanType::INFO_TYPE_ISLAND_NUM; ++i)
  {
    m_islandData[i].islandTopPane = m_pLytWk->GetPane(ISLAND_TOP_PANE_ID[i]);
    m_islandData[i].rangePane     = m_pLytWk->GetPane(ISLAND_RANGE_PANE_ID[i]);
  }


#define PANE_ID(iconName) PANENAME_ZUKAN_ISD_UPP_000_PANE_##iconName,
#define ANIM_ID(iconName) LA_ISLAND_TOP_ZUKAN_ISD_UPP_000__##iconName##_ICON_04,
  static const u32 ICON_TOP_PANE_ID[] =
  {
    PANE_ID(ICON_00)
    PANE_ID(ICON_01)
    PANE_ID(ICON_02)
    PANE_ID(ICON_03)
    PANE_ID(ICON_04)
    PANE_ID(ICON_05)
    PANE_ID(ICON_06)
    PANE_ID(ICON_07)
    PANE_ID(ICON_08)
    PANE_ID(ICON_09)
    PANE_ID(ICON_10)
    PANE_ID(ICON_11)
    PANE_ID(ICON_12)
    PANE_ID(ICON_13)
    PANE_ID(ICON_14)
    PANE_ID(ICON_15)
    PANE_ID(ICON_16)
    PANE_ID(ICON_17)
    PANE_ID(ICON_18)
    PANE_ID(ICON_19)
    PANE_ID(ICON_20)
    PANE_ID(ICON_21)
    PANE_ID(ICON_22)
    PANE_ID(ICON_23)
    PANE_ID(ICON_24)
    PANE_ID(ICON_25)
    PANE_ID(ICON_26)
    PANE_ID(ICON_27)
    PANE_ID(ICON_28)
    PANE_ID(ICON_29)
    PANE_ID(ICON_30)
    PANE_ID(ICON_31)
    PANE_ID(ICON_32)
    PANE_ID(ICON_33)
    PANE_ID(ICON_34)
    PANE_ID(ICON_35)
    PANE_ID(ICON_36)
    PANE_ID(ICON_37)
    PANE_ID(ICON_38)
    PANE_ID(ICON_39)
  };
  static const u32 ICON_DISTRIBUTION_ANIM_ID[] =
  {
    ANIM_ID(ICON_00)
    ANIM_ID(ICON_01)
    ANIM_ID(ICON_02)
    ANIM_ID(ICON_03)
    ANIM_ID(ICON_04)
    ANIM_ID(ICON_05)
    ANIM_ID(ICON_06)
    ANIM_ID(ICON_07)
    ANIM_ID(ICON_08)
    ANIM_ID(ICON_09)
    ANIM_ID(ICON_10)
    ANIM_ID(ICON_11)
    ANIM_ID(ICON_12)
    ANIM_ID(ICON_13)
    ANIM_ID(ICON_14)
    ANIM_ID(ICON_15)
    ANIM_ID(ICON_16)
    ANIM_ID(ICON_17)
    ANIM_ID(ICON_18)
    ANIM_ID(ICON_19)
    ANIM_ID(ICON_20)
    ANIM_ID(ICON_21)
    ANIM_ID(ICON_22)
    ANIM_ID(ICON_23)
    ANIM_ID(ICON_24)
    ANIM_ID(ICON_25)
    ANIM_ID(ICON_26)
    ANIM_ID(ICON_27)
    ANIM_ID(ICON_28)
    ANIM_ID(ICON_29)
    ANIM_ID(ICON_30)
    ANIM_ID(ICON_31)
    ANIM_ID(ICON_32)
    ANIM_ID(ICON_33)
    ANIM_ID(ICON_34)
    ANIM_ID(ICON_35)
    ANIM_ID(ICON_36)
    ANIM_ID(ICON_37)
    ANIM_ID(ICON_38)
    ANIM_ID(ICON_39)
  };

  for(s32 i=0; i<ICON_NUM_MAX; ++i)
  {
    m_iconData[i].iconTopPane        = m_pLytWk->GetPane(ICON_TOP_PANE_ID[i]);
    m_iconData[i].distributionAnimId = ICON_DISTRIBUTION_ANIM_ID[i];

    m_pG2DUtil->StartAnime(
        m_lytWkId,  // iはwk_?
        m_iconData[i].distributionAnimId
    );
  }
#undef ANIM_ID
#undef PANE_ID


  m_allTopPane = m_pLytWk->GetPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_ALL);
  m_playerPane = m_pLytWk->GetPartsPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_PLAYERICON_00);
      // LA_ISLAND_TOP_ZUKAN_ISD_UPP_000__PLAYERICON_00_PLAYER_00  // 男の子アイコンを表示するアニメ
      // LA_ISLAND_TOP_ZUKAN_ISD_UPP_000__PLAYERICON_00_PLAYER_01  // 女の子アイコンを表示するアニメ
  m_cursorPane = m_pLytWk->GetPane(PANENAME_ZUKAN_ISD_UPP_000_PANE_CURSOR_00);

  // 不要なものは非表示にしておく
  m_cursorPane->SetVisible(false);

  // 位置を初期化する
  // 島全体
  m_allTopPane->SetTranslate(gfl2::math::VEC2(-70.0f,0.0f));
  // プレーヤーアイコンの位置設定はinitHeroIconで行っている
}

void ZukanSmallIslandMap::initHeroIcon(ZukanType::InfoType infoType)
{
  // niji_project/prog/App/TownMapParts/source/TownMapPartsDraw.cpp
  // を参考にした。

  GameSys::GameData*           gameData       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  const Savedata::MyStatus*    myStatus       = gameData->GetPlayerStatus();         // 自機セーブデータ
  const Field::Location*       fieldLocation  = gameData->GetStartLocation();        // フィールドロケーション
  const Field::ZoneDataLoader* zoneDataLoader = gameData->GetFieldZoneDataLoader();  // ゾーンデータローダー

  Field::ZoneID zoneID = fieldLocation->GetZoneID();
  Field::ZoneDataAccessor zoneDataAccessor( zoneDataLoader->GetZoneData(zoneID) );

  s32 heroLandNo = zoneDataAccessor.GetIslandIndex() - 1;   // 0オリジンに変更
  if( heroLandNo == static_cast<s32>(infoType - ZukanType::INFO_TYPE_ISLAND_START) )  // 島番号を0オリジンにして比較
  {
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 r = 0.0f;
    bool is_arrow = false;
    App::TownMap::TownMapPartsDraw::GetHeroPosition( &zoneDataAccessor, &x, &y, &r, &is_arrow );
 
    f32 paneX = x;
    f32 paneY = y;
    s32 l_islandDataIndex = infoType - ZukanType::INFO_TYPE_ISLAND_START;     // m_islandDataのインデックスは0始まり
    App::TownMap::TownMapPartsDraw::GetMapPos(m_islandData[l_islandDataIndex].rangePane, &paneX, &paneY);

    m_playerPane->SetVisible(true);

    gfl2::lyt::LytPane* playerBasePane  = m_pLytWk->GetPane(m_playerPane, PANENAME_FIELDMENU_BTN_LOW_013_PANE_PLAYERBASE_00, &m_lytRes);
    gfl2::lyt::LytPane* playerArrowPane = m_pLytWk->GetPane(m_playerPane, PANENAME_FIELDMENU_BTN_LOW_013_PANE_PLAYER_ARR, &m_lytRes);
 
    // 位置
    gfl2::math::VEC3 pos = m_playerPane->GetTranslate();
    pos.x = paneX;
    pos.y = paneY;
    m_playerPane->SetTranslate(pos);

    // 矢印
    if( is_arrow != false )
    {
      playerArrowPane->SetVisible(true);
      
      gfl2::math::VEC3 rotate = playerBasePane->GetRotate();
      rotate.z = r;
      playerBasePane->SetRotate(rotate);
    }
    else
    {
      playerArrowPane->SetVisible(false);
    }
   
    // 性別ごとの表示変更
    if( myStatus->GetSex() == PM_MALE )
    {
      m_pG2DUtil->StartAnime( m_lytWkId, LA_ISLAND_TOP_ZUKAN_ISD_UPP_000__PLAYERICON_00_PLAYER_00 );
    }
    else
    {
      m_pG2DUtil->StartAnime( m_lytWkId, LA_ISLAND_TOP_ZUKAN_ISD_UPP_000__PLAYERICON_00_PLAYER_01 );
    }
  }
  else
  {
    m_playerPane->SetVisible(false);
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

