//======================================================================
/**
 * @file WeatherControl.h
 * @date 2015/12/03 15:41:13
 * @author saito_nozomu
 * @brief 天候管理クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __WEATHER_CONTROL_H_INCLUDED__
#define __WEATHER_CONTROL_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Fs/include/gfl2_FsAsyncFileManager.h>

#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"
#include "GameSys/include/TimeZone.h"

#include "niji_conv_header/field/WeatherKindDef.h"
#include "niji_reference_files/script/WeatherReplaceGroupDefine.h"

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(resource)
class ResourceNode;
GFL_NAMESPACE_END(resource)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_BEGIN(Skybox)
class Skybox;
GFL_NAMESPACE_END(Skybox)
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(GameSys)
class GameData;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(Field)
class EnvManager;
class ZoneDataLoader;
GFL_NAMESPACE_BEGIN(weather)

class WeatherManager;

/**
 * @class MyClass
 * @brief クラスの説明
 */
class WeatherControl
{
  GFL_FORBID_COPY_AND_ASSIGN(WeatherControl);

private:
  struct WeatherData
  {
    u32 mSkybox;
    u32 mManager;
  };

  struct MotionTblData
  {
    u32 mMotion[11];
  };

  struct ReplacableZoneData
  {
    ZoneID zone_id;
    ReplaceGroup group;
  };

  

  /**
   * @brief リソースパックインデックス
   */
  enum PackIndex
  {
    PACK_INDEX1,
    PACK_INDEX2,
    PACK_INDEX_MAX,
  };

  enum MotionIndex
  {
    MOTION_INDEX1,
    MOTION_INDEX2,
    MOTION_INDEX_MAX,
  };

public:

  /**
   * @brief 天候置き換えができるゾーンを取得
   * @param  zone_id ゾーンＩＤ
   * @return ReplaceGroupグループ
   */
 static ReplaceGroup GetReplaceGroup( Field::ZoneID zone_id );

  /**
   * @brief コンストラクタ
   */
  WeatherControl(){
    m_pSkybox = NULL;
    m_pWeatherManager = NULL;
    m_pEnvManager = NULL;
    m_pZoneDataLoader = NULL;
    m_pGameData = NULL;
    m_pWeatherDatas = NULL;
    m_pMotionTblDatas = NULL;
    m_pEnvAnimeDatas = NULL;
    m_pDrawEnvMotionResourceRootNode[MOTION_INDEX1] = NULL;
    m_pDrawEnvMotionResourceRootNode[MOTION_INDEX2] = NULL;
    m_EnvIndex = 0;
    m_Reserve  = false;
    m_Request = false;
    m_TimeZone = GameSys::TimeZone::MORNING;
    m_NowWeather = FORCE_WEATHER_NONE;
    m_ForceWeather  = FORCE_WEATHER_NONE;
  }

  struct InitParam
  {
    gfl2::heap::HeapBase *pHeap;
    System::nijiAllocator* pGLAllocator;
    gfl2::fs::AsyncFileManager* pAsyncFileReadManager;
    System::Skybox::Skybox *pSkybox;
    WeatherManager *pWeatherManager;
    EnvManager *pEnvManager;
    ZoneDataLoader* pZoneDataLoader;
    GameSys::GameData* pGameData;
    int EnvIndex;
    Field::ZoneID ZoneId;
  };

  /**
   * @brief 初期化
   * @param param 初期化パラメータ
   */
  void Initialize(InitParam &param);

  /**
   * @brief 終了
   */
  void Terminate(void);

  /**
   * @brief 天候の変更リクエスト
   */
  void ChangeWeatherRequest(void);

  /**
   * @brief アップデート関数
   * @param zoneId ゾーンＩＤ
   */
  void Update(Field::ZoneID zoneId);

  /**
   * @brief 強制天候（ゾーンや時間帯に左右されない）セット
   * @param weather_kind 天候種類
   */
  void SetForceWeather(WeatherKind weather_kind);

  /**
   * @brief 強制天候（ゾーンや時間帯に左右されない）リセット
   */
  void ResetForceWeather(void);

  /**
   * @brief 現在の天候を取得
   * @return Weatherkind 天候種類
   */
  WeatherKind GetNowWeatherKind(void){ return m_NowWeather; }

  /**
   * @brief 天候を変更中か？
   * @return bool true で変更中
   */
  bool IsChanging(void);

  /**
   * @brief 天候の変更
   * @param zoneId ゾーンＩＤ
   */
  void ChangeWeatherFast(Field::ZoneID zoneId);

private:

  /**
   * @brief 天候の変更リクエスト
   * @param zoneId ゾーンＩＤ
   */
  void ChangeWeather(Field::ZoneID zoneId);

  /**
   * @brief 現時間帯の天候を取得
   * @param  zoneId 要求するゾーンＩＤ
   * @return u32 天候インデックス
   */
  u32 GetWeatherByZone(Field::ZoneID zoneId);

  /**
   * @brief セーブデータによる置き換え天候の取得
   * @param  zoneId 要求するゾーンＩＤ
   * @return WeatherKind 天候種類
   */
  WeatherKind GetReplaceableZoneWeather(Field::ZoneID zoneId);

private:

  System::Skybox::Skybox  *m_pSkybox;
  WeatherManager          *m_pWeatherManager;
  EnvManager              *m_pEnvManager;
  ZoneDataLoader          *m_pZoneDataLoader;
  GameSys::GameData       *m_pGameData;

  WeatherData *m_pWeatherDatas;
  MotionTblData *m_pMotionTblDatas;
  void *m_pEnvAnimeDatas;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pDrawEnvMotionResourceRootNode[2];  ///< Envモーションリソースノード

  int m_EnvIndex;

  bool m_Reserve;
  bool m_Request;

  GameSys::TimeZone::Zone m_TimeZone;

  WeatherKind m_NowWeather;
  WeatherKind m_ForceWeather;
};

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)
#endif // __WEATHER_CONTROL_H_INCLUDED__