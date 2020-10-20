//===================================================================
/**
 * @file    SkyboxResourceManager.h
 * @brief   天球グラフィックリソース管理クラス
 * @author  saita_kazuki
 * @date    2015.04.27
 */
//===================================================================

#if !defined( SKYBOX_RESOURCE_MANAGER_H_INCLUDED )
#define SKYBOX_RESOURCE_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球グラフィックリソース管理クラス
 */
class ResourceManager
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( ResourceManager );

public:

  /**
   * @brief リソースの種類
   */
  enum ResourceType
  {
    RESOURCE_TYPE_COMMON,   ///< 共通リソースパック

    RESOURCE_TYPE_MAX
  };

  /**
   * @brief 共通リソースパックのインデックス
   */
  enum PackCommonIndex
  {
    // モデル
    PACK_COMMON_INDEX_CLOUD_MODEL,                    ///< 雲
    PACK_COMMON_INDEX_MOON_MODEL,                     ///< 月
    PACK_COMMON_INDEX_CLOUDY_SKY_MODEL,               ///< 曇り空
    PACK_COMMON_INDEX_SUNNY_SKY_MODEL,                ///< 晴れ空
    PACK_COMMON_INDEX_SUN_MODEL,                      ///< 太陽

    // アニメーション
    PACK_COMMON_INDEX_CLOUD_ALLTIME_SHADER_MOTION,    ///< 雲1日表現
    PACK_COMMON_INDEX_CLOUD_UV_MOTION,                ///< 雲通常
    PACK_COMMON_INDEX_MOON_ALLTIME_JOINT_MOTION,      ///< 月1日軌道
    PACK_COMMON_INDEX_MOON_ALLTIME_SHADER_MOTION,     ///< 月1日表現
    PACK_COMMON_INDEX_MOON_CHANGE_UV_MOTION,          ///< 月満ち欠けUVパターン
    PACK_COMMON_INDEX_SUNNY_SKY_ALLTIME_MOTION,       ///< 空1日表現
    PACK_COMMON_INDEX_SUN_ALLTIME_JOINT_MOTION,       ///< 太陽1日軌道
    PACK_COMMON_INDEX_SUN_UV_MOTION,                  ///< 太陽表現
  };

public:

  /**
   * @brief コンストラクタ
   */
  ResourceManager();

  /**
   * @brief デストラクタ
   */
  virtual ~ResourceManager();

  /**
   * @brief 初期化
   * @param リソース読み込みで使用するヒープの親ヒープ
   * @param pTemporaryHeap リソース読み込みに使用する一時確保用ヒープ
   */
  void Initialize( gfl2::heap::HeapBase* pParent, gfl2::heap::HeapBase* pTemporaryHeap);

  /**
   * @brief 終了
   */
  void Terminate();

  /**
   * @brief リソース非同期読み込み開始
   */
  void StartAsyncLoad();

  /**
   * @brief リソース非同期読み込み完了待ち
   * @retval true 完了
   * @retval false 実行中
   */
  bool WaitAsyncLoad();

  /**
   * @brief アクセサーの取得
   * @param type  リソースの種類
   */
  gfl2::fs::BinLinkerAccessor* GetAccessor( ResourceType type); 

  /**
   * @brief パック内のバイナリデータの取得
   * @param type パック種類
   * @param index パック内インデックス
   * @return バイナリデータ。ダミーの場合はNULL
   */
  void* GetData( u32 type, u32 index) const ;

private:

  /**
   * @brief リソースデータ
   */
  struct ResourceData
  {
    void* data;   ///< データバッファ
    u32   size;   ///< 読み込んだサイズを格納する
  };

private:

  gfl2::fs::ArcFile::ARCID GetArcID() const ;

  void AddOpenReq();
  void AddCloseReq();
  void AddAsyncLoadRequest( u32 datId, void** ppBuff, u32* pSize);

private:

  u8                          m_seq;                                ///< 読み込みシーケンス
  gfl2::heap::HeapBase*       m_pHeap;                              ///< 読み込みに使用するヒープ
  gfl2::heap::HeapBase*       m_pTemporaryHeap;                     ///< 読み込みに使用する一時確保用ヒープ
  ResourceData                m_resource[ RESOURCE_TYPE_MAX ];      ///< 天球リソースパックデータ
  gfl2::fs::BinLinkerAccessor m_accessor[ RESOURCE_TYPE_MAX ];      ///< データアクセサー
  gfl2::fs::AsyncFileManager* m_pAsyncFileManager;                  ///< ファイル読み込みマネージャー
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_RESOURCE_MANAGER_H_INCLUDED

