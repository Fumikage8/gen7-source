//===================================================================
/**
 * @file    Skybox.h
 * @brief   天球クラス
 * @author  saita_kazuki
 * @date    2015.05.01
 *
 * ▼使用可能までの手順
 *  １、GameManagerからこのクラスのインスタンスを取得する
 *  ２、CreateHeapでSkyboxのローカルヒープを生成
 *  ３、StartCreateResourceでリソース生成リクエストを出す
 *  ４、UpdateCreateResourceを回し、trueが返ってくるまで待つ
 *  ５、CreatePartsで天球を構成するパーツを生成
 *  ６、Initialize
 *
 * ▼破棄の手順
 *  １、Terminate
 *  リソースも全て破棄する場合
 *  ２、DestroyAll
 *  ３、DeleteHeap
 *  リソースは残してパーツ(空や太陽月など)を破棄する場合
 *  ２、DestroyParts
 *
 * ▼Fieldでの運用
 *  １、FieldResidentの初期化でヒープ生成
 *      (既にリソースがある場合はヒープ生成はスキップ)
 *  ２、Fieldmapの初期化でリソース生成、パーツ生成、初期化
 *      (既にリソースがある場合はリソース生成はスキップ)
 *  ３、EventFieldmapCloseに渡すDescで天球を保持するかどうかを決定する(trueで残す、falseで破棄)
 *      (バトルコールはここでtrueを渡し、リソースを常駐させます)
 *  ４、Fieldmapの破棄でEventFieldmapCloseから渡ってきたフラグを参照し、このクラスのDestroyほにゃららが呼ばれます
 *
 *  フィールド～アプリ間での天球の常駐をさせたい場合は上記の運用で。
 *  天球のリソースから作って使いたい場合は、ヒープメモリの断片化に気をつけてください。
 *  (APP_DEVICEの先頭にSkyboxのメモリが確保されるようにするなど。CreateHeapで何も指定しないとAPP_DEVICEからメモリを取ります)
 *
 */
//===================================================================

#if !defined( SKYBOX_H_INCLUDED )
#define SKYBOX_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>
#include <gfx/include/gfl2_types.h>

#include "System/include/nijiAllocator.h"

// 前方参照
namespace System { namespace Skybox{ 
  class Sky;
  class SunMoon;
  class CloudySky;
  class Cloud;
  class PartsBase;
  class ResourceManager;
}}
namespace poke_3d { namespace model {
  class BaseModel;
}}
namespace Field {
  class EventWork;
}


GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球クラス
 */
class Skybox
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( Skybox );

public:

  /**
   * @brief 設定情報
   */
  struct Description
  {
    f32   rotateRadY;    ///< Y軸回転(ラジアン)
  };

  /**
   * @brief 空の種類
   */
  enum SkyType
  {
    SKY_TYPE_SUNNY,     ///< 晴れ
    SKY_TYPE_CLOUDY,    ///< 曇り

    SKY_TYPE_MAX,
    SKY_TYPE_NONE = SKY_TYPE_MAX,
  };

  /**
   * @brief 空の種類
   */
  enum FadeRequest
  {
    IDLE,
    REQUEST,
    FADEWAIT,
  };

  ///< 部品の種類
  enum PartsType
  {
    PARTS_TYPE_SKY,        ///< 空
    PARTS_TYPE_SUN,        ///< 太陽
    PARTS_TYPE_MOON,       ///< 月
    PARTS_TYPE_CLOUDY_SKY, ///< 曇り空
    PARTS_TYPE_CLOUD,      ///< 雲

    PARTS_TYPE_MAX,
  };

public:

  /**
   * @brief コンストラクタ
   * @param pEventWork フラグチェックに使用するイベントワーク
   */
  Skybox( Field::EventWork* pEventWork );

  /**
   * @brief デストラクタ
   */
  virtual ~Skybox();

  /**
   * @brief ヒープ生成
   * @param pParentDeviceHeap 親ヒープ。NULLの場合はAPP_DEVICE
   */
  void CreateHeap( gfl2::heap::HeapBase* pParentDeviceHeap = NULL);

  /**
   * @brief ヒープ破棄
   */
  void DeleteHeap();

  /**
   * @brief ヒープが生成されているか
   * @retval true 完了
   * @retval false 実行中
   */
  bool IsCreateHeap() const ;

  /**
   * @brief リソース生成開始
   * @param desc 設定情報
   * @param pTemporaryHeap リソース読み込みに使用する一時確保用ヒープ
   */
  void StartCreateResource( const Description& desc, gfl2::heap::HeapBase* pTemporaryHeap);

  /**
   * @brief リソース生成更新
   * @retval true 完了
   * @retval false 実行中
   */
  bool UpdateCreateResource();

  /**
   * @brief リソース生成が完了しているか
   * @retval true 完了
   * @retval false 未完了
   */
  bool IsCreateResource() const ;

  /**
   * @brief 破棄
   * @note 部品もリソースも全て破棄されます。リソースを残す場合はDestroyPartsのみ呼んでください
   */
  void DestroyAll();

  /**
   * @brief 部品(空などのインスタンス)生成
   * @param 設定情報。NULLの場合、StartCreateResoureで設定されたものを使用します
   * @param isAllocateByVRAM trueでテクスチャをVRAMに乗せる
   * @param vramMallocMode isAllocateByVRAMがtrueのときに指定されるVRAM確保タイプ
   */
  void CreateParts( const Description* pDesc, bool isAllocateByVRAM, System::nijiAllocator::VramMallocMode vramMallocMode);

  /**
   * @brief 部品(空などのインスタンス)破棄
   */
  void DestroyParts();

  /**
   * @brief 初期化
   * @param skyType 空の種類
   */
  void Initialize( SkyType skyType);

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief 更新
   * @param pos 天球に設定する位置
   * @param hour 時間
   * @param minute 分
   * @param second 秒
   */
  void Update( const gfl2::math::Vector3& pos, s32 hour, s32 minute, s32 second);

  /**
   * @brief 有効無効フラグの設定
   * @param isEnable trueで有効
   */
  void SetEnable( bool isEnable);

  /**
   * @brief 部品ごとの有効無効フラグの設定
   * @param type 部品の種類
   * @param isEnable trueで有効
   */
  void SetEnableParts( PartsType type, bool isEnable);

  /**
   * @brief 天球が有効か
   * @retval true 有効
   * @retval false 無効
   */
  bool IsEnable() const ;

  /**
   * @brief 指定した部品が有効か
   * @retval true 有効
   * @retval false 無効
   */
  bool IsEnableParts( PartsType type) const ;

  /**
   * @brief 空の変更リクエスト
   * @param type 空の種類
   * @param changeFrame 変更するまでにかけるフレーム数
   */
  void ChangeSkyRequest( SkyType type, u32 changeFrame = 1);

  /**
   * @brief 空の種類を取得
   * @return 現在の空の種類
   */
  SkyType GetSkyType() const ;

  /**
   * @brief 部品のベースモデルインスタンスの取得
   * @param type 部品の種類
   * @return ベースモデル
   */
  poke_3d::model::BaseModel* GetPartsModel( PartsType type);

  /**
   * @brief 太陽の位置を取得
   * @return 太陽の位置
   */
  const gfl2::math::Vector3& GetSunPosition() const ;

  /**
   * @brief 月の位置を取得
   * @return 月の位置
   */
  const gfl2::math::Vector3& GetMoonPosition() const ;

  /**
   * @brief スカイボックスが晴れ曇りの変化中か？
   * @return tureで変更中
   */
  bool IsChanging(void);

  void ChangeSky( SkyType type);

private:

  ///< セットアップまでの状態
  enum SetupState
  {
    SETUP_STATE_NONE,             ///< 何もしていない
    SETUP_STATE_CREATED_RESOURCE, ///< リソース読み込みまで完了
    SETUP_STATE_CREATED_PARTS,    ///< 部品(空などのインスタンス)作成まで完了
    SETUP_STATE_ACTIVE,           ///< 使用可能
  };

  ///< 時間保存用ワーク
  struct TimeWork
  {
    TimeWork() :
      hour( -1),
      minute( -1),
      second( -1)
    {}

    s32 hour;
    s32 minute;
    s32 second;
  };

  ///< 空の操作用ワーク
  struct SkyWork
  {
    SkyType now;
    SkyType req;
    FadeRequest fade;
    u32     changeFrame;
    //u32     changeFrameCnt;
  };

private:

  void CreateLocalHeap( gfl2::heap::HeapBase* pParentDeviceHeap);
  void DeleteLocalHeap();

  bool IsUpdateAllTimeAnimation( s32 hour, s32 minute);
  f32 CalculateTimeToAllTimeAnimationFrame( s32 hour, s32 minute, s32 second);

  void UpdateChangeSky();

private:

  static const f32 ALL_TIME_ANIMATION_FRAME_MAX;              ///< 1日アニメーションの最大フレーム数
  static const f32 ALL_TIME_ANIMATION_ONE_HOUR_FRAME;         ///< 1時間ごとのフレーム数
  static const f32 ALL_TIME_ANIMATION_ONE_MINUTE_FRAME;       ///< 1分ごとのフレーム数
  static const f32 ALL_TIME_ANIMATION_ONE_SECOND_FRAME;       ///< 1秒ごとのフレーム数

private:

  System::nijiAllocator*        m_pGLAllocator;               ///< GLアロケーター
  gfl2::heap::HeapBase*         m_pDeviceHeap;                ///< デバイスヒープ

  ResourceManager*              m_pResourceManager;           ///< リソース管理

  Sky*                          m_pSky;                       ///< 空
  SunMoon*                      m_pSunMoon;                   ///< 月と太陽
  CloudySky*                    m_pCloudySky;                 ///< 曇り空
  Cloud*                        m_pCloud;                     ///< 雲

  SetupState                    m_setupState;                 ///< セットアップの状態
  Description                   m_description;                ///< 設定
  bool                          m_isEnable;                   ///< 有効か

  TimeWork                      m_beforeUpdateTime;           ///< 前回更新時間
  SkyWork                       m_skyWork;                    ///< 空の操作用ワーク

  Field::EventWork*             m_pEventWork;                 ///< フラグチェックに使用するイベントワーク
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_H_INCLUDED

