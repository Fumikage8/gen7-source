//======================================================================
/**
 * @file WeatherControl.cpp
 * @date 2015/12/03 16:36:31
 * @author saito_nozomu
 * @brief 天候管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include "System/include/nijiAllocator.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Field/FieldRo/include/Weather/WeatherManager.h"
#include "Field/FieldRo/include/FieldEnvManager.h"
#include "System/include/Skybox/Skybox.h"

#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// arc
#include <arc_def_index/arc_def.h>

// ゾーン
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"

#include "GameSys/include/GameData.h"
#include "Savedata/include/WeatherSave.h"

#include <niji_conv_header/field/zone/zone_id.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

//@fix MMCat[307]:天候「暗黒」の際に屋内の窓に外から光が差し込んでいる >>　32だったゾーン数を8足して40に変更
//@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　40だったゾーン数を2足して42に変更
#define REPLACE_ZONE_NUM (42)

#define SKY_CHANGE_FRAME (10)

/**
 * @brief 天候置き換えができるゾーンを取得
 * @param  zone_id ゾーンＩＤ
 * @return ReplaceGroupグループ
 */
 ReplaceGroup WeatherControl::GetReplaceGroup( Field::ZoneID zone_id )
{
  //ここにハードコーディングしてセーブデータをずらさないようにする
  const ReplacableZoneData data[REPLACE_ZONE_NUM] = {
    { Z_F0201H0401, Group1 }, //シェードジャングル１
    { Z_F0201H0402, Group1 }, //シェードジャングル２
    { Z_F0201H0403, Group1 }, //シェードジャングル３
    { Z_F0201H0404, Group1 }, //シェードジャングル４

    { Z_F0201H0301, Group2 }, //せせらぎの丘
    { Z_F0201H0302, Group2 }, //せせらぎの丘
    { Z_F0201H0303, Group2 }, //せせらぎの丘：深部

    { Z_F0301_S0501, Group3 }, //１４番道路
    { Z_F0301_S0601, Group3 }, //ブラックビーチ

    { Z_F0401D0801, Group4 }, //ナッシーアイランド

    { Z_F0401D0201, Group5 }, //ポニの大峡谷：外マップ１
    { Z_F0401D0202, Group5 }, //ポニの大峡谷：外マップ２
    { Z_F0401D0203, Group5 }, //ポニの大峡谷：洞窟１    momiji追加
    { Z_F0401D0204, Group5 }, //ポニの大峡谷：洞窟２    momiji追加
    { Z_F0401D0205, Group5 }, //ポニの大峡谷：洞窟３    momiji追加
    { Z_F0401H0701, Group5 }, //ポニの大峡谷：試練（外マップ３）
    { Z_F0401_S0101, Group5 }, //ポニのげんや           momiji追加
    { Z_F0401_S0201, Group5 }, //ポニのこどう           momiji追加
    { Z_F0401_S0301, Group5 }, //ポニのあらいそ         momiji追加
    { Z_F0401_T0401, Group5 }, //海の民の村             momiji追加
    { Z_F0401D0501, Group5 }, //ひがんのいせき：入り口  momiji追加

    //@fix MMCat[307]:天候「暗黒」の際に屋内の窓に外から光が差し込んでいる >>　暗黒天候グループに以下のゾーンを追加
    { Z_F0401PC0101, Group5 }, //ポケモンセンター：海の民の村      momiji追加
    { Z_F0401I0201_01, Group5 }, //船上レストラン                  momiji追加
    { Z_F0401I0201_02, Group5}, //船上レストラン：イベントスペース momiji追加
    { Z_F0401R0201, Group5 }, //船団団長の家                       momiji追加
    { Z_F0401R0101, Group5 }, //マツリカの家                       momiji追加
    { Z_F0401I0101, Group5 }, //船上ショップ                       momiji追加
    { Z_F0401R0301_01, Group5 }, //ハプウの家                      momiji追加
    { Z_F0401R0301_02, Group5 }, //ハプウの家：寝室                momiji追加
    //↑ここまで　@fix MMCat[307]
    //@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　暗黒天候グループに以下のゾーンを追加
    { Z_F0401_S0401, Group5 }, //ポニの樹林                        momiji追加
    { Z_F0401_S0801, Group5 }, //ポニのビーチ                      momiji追加
    //↑ここまで　@fix MMCat[379]

    { Z_F0401D0301, Group6 }, //にちりんの祭壇：階段
    { Z_F0401D0302, Group6 }, //にちりんの祭壇：祭壇
    { Z_F0401D0401, Group6 }, //がちりんの祭壇：階段
    { Z_F0401D0402, Group6 }, //がちりんの祭壇：祭壇

    { Z_F0301D1001, Group7 }, //ラナキラマウンテン：山麓
    { Z_F0301D1002, Group7 }, //ラナキラマウンテン：山腹
    { Z_F0301D1003, Group7 }, //ラナキラマウンテン：山間
    { Z_F0301D1004, Group7 }, //ラナキラマウンテン：山頂
    { Z_F0301D1005, Group7 }, //チャンピオンロード外マップ　momiji追加
    { Z_F0301D1203, Group7 }, //こおりの洞窟：洞窟マップ２  momiji追加

    { Z_F0102_S0101, Group8 }, //１ばんどうろはずれ  momiji追加
  };

  for(int i=0;i<REPLACE_ZONE_NUM;i++)
  {
    if(zone_id == data[i].zone_id)
    {
      return data[i].group;
    }
  }

  return GroupNone; 
}

/**
 * @brief 初期化
 * @param param 初期化パラメータ
 */
void WeatherControl::Initialize(InitParam &param)
{
  m_pSkybox = param.pSkybox;
  m_pWeatherManager = param.pWeatherManager;
  m_pEnvManager = param.pEnvManager;
  m_pZoneDataLoader = param.pZoneDataLoader;
  m_EnvIndex = param.EnvIndex;
  m_NowWeather = FORCE_WEATHER_NONE;
  m_ForceWeather = FORCE_WEATHER_NONE;
  m_pGameData = param.pGameData;

  size_t size;

  //指定環境データのモーションをすべてセットアップする
  {
    //ロード直後にクローズするのでヒープの指定はすべてLowからとる
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_ENV_ANIME;
    openReq.heapForFile = param.pHeap->GetLowerHandle();
    openReq.heapForArcSrc = param.pHeap->GetLowerHandle();
    openReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_ENV_ANIME;
    loadReq.datId = m_EnvIndex;
    loadReq.ppBuf = reinterpret_cast<void**>(&m_pEnvAnimeDatas);
    loadReq.heapForBuf = param.pHeap;
    loadReq.pBufSize = &size;
    loadReq.align = 4;
    param.pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_ENV_ANIME;
    closeReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  //天候データをロード
  {
    //ロード直後にクローズするのでヒープの指定はすべてLowからとる
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_WEATHER;
    openReq.heapForFile = param.pHeap->GetLowerHandle();
    openReq.heapForArcSrc = param.pHeap->GetLowerHandle();
    openReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_WEATHER;
    loadReq.datId = 0;
    loadReq.ppBuf = reinterpret_cast<void**>(&m_pWeatherDatas);
    loadReq.heapForBuf = param.pHeap;
    loadReq.pBufSize = &size;
    loadReq.align = 4;
    param.pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_WEATHER;
    closeReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  //環境データをロード
  {
    //ロード直後にクローズするのでヒープの指定はすべてLowからとる
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = ARCID_WEATHER_ENV;
    openReq.heapForFile = param.pHeap->GetLowerHandle();
    openReq.heapForArcSrc = param.pHeap->GetLowerHandle();
    openReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileOpen( openReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARCID_WEATHER_ENV;
    loadReq.datId = 0;
    loadReq.ppBuf = reinterpret_cast<void**>(&m_pMotionTblDatas);
    loadReq.heapForBuf = param.pHeap;
    loadReq.pBufSize = &size;
    loadReq.align = 4;
    param.pAsyncFileReadManager->SyncArcFileLoadData( loadReq );
  }
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_WEATHER_ENV;
    closeReq.heapForReq = param.pHeap->GetLowerHandle();
    param.pAsyncFileReadManager->SyncArcFileClose( closeReq );
  }

  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize(m_pEnvAnimeDatas);

  // Envモーションノード作成
  if( binLinkerAccessor.GetDataSize(PACK_INDEX1) != 0 )
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX1));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData);

    m_pDrawEnvMotionResourceRootNode[MOTION_INDEX1] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( param.pGLAllocator, &resourceData);
  }
  if( binLinkerAccessor.GetDataSize(PACK_INDEX2) != 0 )
  {
    c8* pBinaryData = reinterpret_cast<c8*>( binLinkerAccessor.GetData( PACK_INDEX2));
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( pBinaryData);

    m_pDrawEnvMotionResourceRootNode[MOTION_INDEX2] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( param.pGLAllocator, &resourceData);
  }

  //タイムゾーンを取得
  m_TimeZone = GameSys::TimeZone::GetNowTimeZone();

  u32 weather_idx;
  //セーブデータによる置き換えを調べてセッティングする
  WeatherKind weather = GetReplaceableZoneWeather( param.ZoneId );
  if (weather == Field::weather::FORCE_WEATHER_NONE)
  {
    //現在の時間帯の天候を取得
    weather_idx = GetWeatherByZone(param.ZoneId);
  }
  else
  {
    //セーブデータに従い、置き換え設定
    m_ForceWeather = weather;
    weather_idx = (u32)weather;
  }

  //現在のEnvの天候べつモーションインデックスを取得
  int mot_idx = m_pMotionTblDatas[m_EnvIndex].mMotion[weather_idx];
  NOZOMU_PRINT("env_idx = %d, motion_idx = %d, weather = %d\n",m_EnvIndex, mot_idx, weather_idx);
  if (mot_idx<2)
  {
    m_pEnvManager->SetAnime(m_pDrawEnvMotionResourceRootNode[mot_idx]);
  }

}

/**
 * @brief 終了
 */
void WeatherControl::Terminate(void)
{
  for( u32 i = 0; i < MOTION_INDEX_MAX; ++i)
  {
    GFL_SAFE_DELETE( m_pDrawEnvMotionResourceRootNode[i]);
  }
  GflHeapSafeFreeMemory(m_pWeatherDatas); //GFCHECK[8454]
  GflHeapSafeFreeMemory(m_pMotionTblDatas); //GFCHECK[8454]
  GflHeapSafeFreeMemory(m_pEnvAnimeDatas); //GFCHECK[8454],cov_ctr[10971]
}

/**
 * @brief 天候の変更リクエスト
 */
void WeatherControl::ChangeWeatherRequest(void)
{
  if( !m_Request ) m_Request = true;
  else m_Reserve = true;
}

/**
 * @brief 天候の変更リクエスト
 * @param zoneId ゾーンＩＤ
 */
void WeatherControl::ChangeWeatherFast(Field::ZoneID zoneId)
{
  u32 weather = GetWeatherByZone(zoneId);

  if(m_ForceWeather != FORCE_WEATHER_NONE)
  {
    weather = m_ForceWeather;
  }

#if PM_DEBUG
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::weather::WeatherKind debug_weather;
    debug_weather = pGameManager->GetGameData()->GetDebugWeather();
    if(debug_weather != FORCE_WEATHER_NONE)
    {
      weather = debug_weather;
    }
  }
#endif //PM_DEBUG

  if(m_NowWeather == (WeatherKind)(weather)) return;

  m_NowWeather = (WeatherKind)(weather);

  u32 type = m_pWeatherDatas[weather].mManager;
  u32 box = m_pWeatherDatas[weather].mSkybox;

  m_pWeatherManager->Execute(Field::weather::WEATHER_TYPE(type));
  int mot_idx = m_pMotionTblDatas[m_EnvIndex].mMotion[weather];
  m_pEnvManager->SetAnime(m_pDrawEnvMotionResourceRootNode[mot_idx]);
  m_pSkybox->ChangeSky( System::Skybox::Skybox::SkyType(box) );
}

/**
 * @brief 天候の変更リクエスト
 * @param zoneId ゾーンＩＤ
 */
void WeatherControl::ChangeWeather(Field::ZoneID zoneId)
{
  u32 weather = GetWeatherByZone(zoneId);

  if(m_ForceWeather != FORCE_WEATHER_NONE)
  {
    weather = m_ForceWeather;
  }

#if PM_DEBUG
  {
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Field::weather::WeatherKind debug_weather;
    debug_weather = pGameManager->GetGameData()->GetDebugWeather();
    if(debug_weather != FORCE_WEATHER_NONE)
    {
      weather = debug_weather;
    }
  }
#endif //PM_DEBUG

  if(m_NowWeather == (WeatherKind)(weather)) return;

  m_NowWeather = (WeatherKind)(weather);

  u32 type = m_pWeatherDatas[weather].mManager;
  u32 box = m_pWeatherDatas[weather].mSkybox;

  m_pWeatherManager->Execute(Field::weather::WEATHER_TYPE(type));
  int mot_idx = m_pMotionTblDatas[m_EnvIndex].mMotion[weather];
  m_pEnvManager->SetAnimeSmooth(m_pDrawEnvMotionResourceRootNode[mot_idx]);
  m_pSkybox->ChangeSkyRequest( System::Skybox::Skybox::SkyType(box), SKY_CHANGE_FRAME );
}

/**
 * @brief 天候を変更中か？
 * @return bool true で変更中
 */
bool WeatherControl::IsChanging(void)
{
  //スカイボックスが変更中か？
  if ( m_pSkybox->IsChanging() )
  {
    NOZOMU_PRINT("Skybox Changing\n");
    return true;
  }
  //天候マネージャが変更中か？
  if ( m_pWeatherManager->IsChanging() )
  {
    NOZOMU_PRINT("WeatherManager Changing\n");
    return true;
  }
  //環境アニメが変更中か？
  if (!m_pEnvManager->IsAnimeChangeFinished())
  {
    NOZOMU_PRINT("EnvAnime Changing\n");
    return true;
  }
  
  return false;
}

/**
 * @brief 現時間帯の天候を取得
 * @param  zoneId 要求するゾーンＩＤ
 * @return u32 天候インデックス
 */
u32 WeatherControl::GetWeatherByZone(Field::ZoneID zoneId)
{
  ZoneDataAccessor zoneDataAccessor( m_pZoneDataLoader->GetZoneData( zoneId) );
  u32 *weather_tbl = zoneDataAccessor.GetWeatherTable();
  //タイムゾーンを取得
  int time_zone_idx = (int)GameSys::TimeZone::GetNowTimeZone();
  u32 weather = weather_tbl[time_zone_idx];
  NOZOMU_PRINT("time_zone = %d, weather = %d\n",time_zone_idx, weather);
  return weather;
}

/**
 * @brief セーブデータによる置き換え天候の取得
 * @param  zoneId 要求するゾーンＩＤ
 * @return WeatherKind 天候種類
 */
WeatherKind WeatherControl::GetReplaceableZoneWeather(Field::ZoneID zoneId)
{
  WeatherKind weather = Field::weather::FORCE_WEATHER_NONE;
  //天候置き換えを調べて取得
  ReplaceGroup group = WeatherControl::GetReplaceGroup( zoneId );
  if( group != GroupNone )
  {
    u8 idx = (u8)group;
    //セーブデータにアクセス
    Savedata::WeatherSave *p_weather_save = m_pGameData->GetWeatherSave();
    //セーブデータの天候取得
    weather = p_weather_save->GetWeather(idx);
  }
  return weather;
}

/**
 * @brief アップデート関数
 * @param zoneId ゾーンＩＤ
 */
void WeatherControl::Update(Field::ZoneID zoneId)
{
  //タイムゾーンの更新チェック
  {
    GameSys::TimeZone::Zone time_zone = GameSys::TimeZone::GetNowTimeZone();
    //更新が合った場合は天候の変更リクエスト
    if (m_TimeZone != time_zone)
    {
      NOZOMU_PRINT("TimeZoneChange %d>>%d",m_TimeZone, time_zone);
      ChangeWeatherRequest();
      m_TimeZone = time_zone;
    }
  }
  
  //天候変化中のときは処理をフック
  if ( IsChanging() ) return;

  //リクエストがかかっていない？
  if(!m_Request)
  {
    //予約をリクエストに更新
    if(m_Reserve)
    {
      m_Request = true;
      m_Reserve = false; //予約クリア
    }
  }

  //リクエストがあるか？
  if(m_Request)
  {
    //有る場合、ゾーンとタイムゾーンから天候セット
    ChangeWeather(zoneId);
    m_Request = false; //リクエストクリア
  }
}

/**
 * @brief 強制天候（ゾーンや時間帯に左右されない）セット
 * @param weather 天候種類
 */
void WeatherControl::SetForceWeather(WeatherKind weather_kind)
{
  m_ForceWeather = weather_kind;
  ChangeWeatherRequest();
}

/**
 * @brief 強制天候（ゾーンや時間帯に左右されない）リセット
 */
void WeatherControl::ResetForceWeather(void)
{
  m_ForceWeather = FORCE_WEATHER_NONE;
  ChangeWeatherRequest();
}


GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)