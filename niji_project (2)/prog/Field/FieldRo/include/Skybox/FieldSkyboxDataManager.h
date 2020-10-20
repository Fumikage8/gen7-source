//======================================================================
/**
 * @file FieldSkyboxDataManager.h
 * @date 2016/2/19
 * @author saita_kazuki
 * @brief 天球データ管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_SKYBOX_DATA_MANAGER_H_INCLUDED__
#define __FIELD_SKYBOX_DATA_MANAGER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include <niji_conv_header/field/world/world_id.h>

// 前方参照
namespace gfl2 { namespace fs {
  class AsyncFileManager;
}}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球データ管理
 */
class DataManager
{
  GFL_FORBID_COPY_AND_ASSIGN( DataManager );

public:

  /**
   * @brief 初期化情報
   */
  struct Description
  {
    gfl2::heap::HeapBase*       pHeap;              ///< リソース確保に使用するヒープ
    gfl2::heap::HeapBase*       pTempHeap;          ///< 一時確保用に使用するヒープ
    u16                         worldID;            ///< ワールドID
    gfl2::fs::AsyncFileManager* pAsyncFileManager;  ///< ファイル読み込み

    Description()
      : pHeap( NULL)
      , pTempHeap( NULL)
      , worldID( WORLD_ERROR)
      , pAsyncFileManager( NULL)
    {}
  };

public:

  /**
   * @brief コンストラクタ
   */
  DataManager();

  /**
   * @brief デストラクタ
   */
  virtual ~DataManager();

  /**
   * @brief 初期化
   */
  void Initialize( const Description& desc);

  /**
   * @brief 初期化待ち
   * @retval true 完了
   * @retval true 実行中
   */
  bool WaitInitialize();

  /**
   * @brief 破棄
   * @retval true 完了
   * @retval true 実行中
   */
  bool Terminate();

  /**
   * @brief 位置データが存在するか
   * @retval true 存在する
   * @retval false 存在しない
   */
  bool IsExistPosData();

  /**
   * @brief 位置変換
   * @return 位置情報に合わせた位置
   */
  gfl2::math::Vector3 ConvertPosition( const gfl2::math::Vector3& sourcePos);

  /**
   * @brief 位置を固定中か
   * @retval true 固定中
   * @retval false 固定中ではない
   */
  bool IsFixPosition() const ;

  /**
   * @brief 固定位置を取得
   * @return 位置情報に合わせた位置
   */
  const gfl2::math::Vector3& GetFixPosition() const ;

  /**
   * @brief 固定位置の登録
   * @param fixPos 固定したい位置
   */
  void RegistFixPosition( const gfl2::math::Vector3& fixPos );

  /**
   * @brief 固定位置の解除
   */
  void UnRegistFixPosition();


private:

  void AddOpenReq();
  void AddCloseReq();
  void AddAsyncLoad( u32 datId, void** ppBuff, u32* pSize, u32 align);

  void InitializePosData();

private:

  Description     m_desc;           ///< 初期化情報
  u32             m_seq;            ///< シーケンス

  void*           m_pPosData;       ///< 位置データバイナリ
  bool            m_isPosDataExist; ///< このワールドに位置データが存在するか
  f32             m_posY;           ///< このワールドに設定されているY座標

  bool                m_isFixPos;   ///< 位置を固定中か
  gfl2::math::Vector3 m_fixPos;     ///< 固定中の位置

};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_SKYBOX_DATA_MANAGER_H_INCLUDED__
