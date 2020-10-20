//===================================================================
/**
 * @file    ZoneDataLoader.h
 * @brief   ゾーンデータ読み込みクラス
 * @author  saita_kazuki
 * @date    2015.04.02
 */
//===================================================================

#if !defined( ZONE_DATA_LOADER_H_INCLUDED )
#define ZONE_DATA_LOADER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>

#include "Field/FieldStatic/include/FieldWorldDefine.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoaderTypes.h"

namespace Field {
  class EventWork;
};

GFL_NAMESPACE_BEGIN( Field )

/**
 * @brief ゾーンデータクラス
 */
class ZoneDataLoader
{
  GFL_FORBID_COPY_AND_ASSIGN( ZoneDataLoader); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   */
  ZoneDataLoader();

  /**
   * @brief   デストラクタ
   */
  virtual ~ZoneDataLoader();

  /**
   * @brief   初期化
   * @param   pHeap 読み込みバッファ確保などに使用する
   */
  void Initialize( gfl2::heap::HeapBase* pHeap);

  /**
   * @brief   終了
   */
  void Terminate();

  /**
   * @brief   同期読み込み
   * @retval  true 成功
   * @retval  false 失敗
   */
  bool SyncLoad();

  /**
   * @brief   非同期読み込み開始
   * @retval  true 成功
   * @retval  false 失敗
   */
  bool AsyncLoadStart();

  /**
   * @brief   非同期読み込み終了待ち
   * @note    AsyncLoadStart後はこの関数で読み込み終了を待つ
   * @retval  true 完了
   * @retval  false 実行中
   */
  bool WaitAsyncLoadFinished();

  /**
   * @brief   読み込みが完了しているか
   * @retval  true 完了
   * @retval  false 実行中or読み込んでいない
   */
  bool IsLoadFinished();

  /**
   * @brief   ZONE_DATA構造体のポインタを取得
   * @param   取得したいゾーンのID。zone_id.h参照
   * @return  指定ゾーンIDのZONE_DATA構造体のポインタ
   * @note    ここで返ってきたポインタを、ZoneDataAccessorにセットして使います。
   */
  void* GetZoneData( ZoneID zoneID) const ;


  /**
   *  @brief  現在のゾーン総数を取得
   */
  u32 GetZoneNum() const;

  /**
   * @brief   ゾーンが所属するワールドIDを取得
   */
  WorldID GetZoneWorldId( ZoneID zoneID ) const;



  /**
   * @brief   捕獲制限ゾーンかどうか？
   */
  bool CheckCaptureNGZone( ZoneID zoneID, const Field::EventWork* pEventWork ) const;

  /**
   * @brief   エンカウント制限ゾーンかどうか？
   */
  bool CheckEncountNGZone( ZoneID zoneID, const Field::EventWork* pEventWork ) const;

private:

  void SetArcFileLoadRequestCommon( gfl2::fs::AsyncFileManager::ArcFileLoadDataReq* pReq);
  void SetArcFileOpenRequestCommon( gfl2::fs::AsyncFileManager::ArcFileOpenReq* pReq);
  void SetArcFileCloseRequestCommon( gfl2::fs::AsyncFileManager::ArcFileCloseReq* pReq);
  void SetArcFileLoadRequestWorldId( gfl2::fs::AsyncFileManager::ArcFileLoadDataReq* pReq);

  bool SyncOpenArcFile();
  bool SyncCloseArcFile();

  bool AsyncOpenArcFileStart();
  bool AsyncCloseArcFileStart();

  bool IsArcFileOpenFinished();
  bool IsArcFileCloseFinished();

  gfl2::fs::ArcFile::ARCID GetArcID();

  bool CheckFileResult();

  bool CheckFlag( s16 flag, const Field::EventWork* pEventWork) const ;

  gfl2::heap::HeapBase*         m_pHeap;              // 読み込み時の確保などに使用する
  gfl2::fs::AsyncFileManager*   m_pAsyncFileManager;  // garc読み込みに使用
  gfl2::fs::Result              m_fsResult;           // 非同期読みを使用するため保持する
  void*                         m_pZoneDataBuff;
  size_t                        m_zoneDataSize;

  u16*                          m_pZoneWorldIdTable;
  size_t                        m_zoneWorldIdTableSize;

#if PM_DEBUG
  gfl2::fs::AsyncFileManager::HioUseSetting::Tag    m_hioUseSetting;
#endif // PM_DEBUG

};

GFL_NAMESPACE_END( Field )

#endif // ZONE_DATA_LOADER_H_INCLUDED
