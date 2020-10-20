//======================================================================
/**
 * @file StaticModelCollisionDataAccessor.h
 * @date 2015/08/21 19:46:28
 * @author saita_kazuki
 * @brief 配置モデルコリジョンデータアクセサー
 * @note このアクセサーに渡すバイナリデータはFieldResidentから取得してください
 * @note 上記バイナリデータに含まれるのは、エリア常駐している配置モデルのみです
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __STATIC_MODEL_COLLISION_DATA_ACCESSOR_H_INCLUDED__
#define __STATIC_MODEL_COLLISION_DATA_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( StaticModel )

/**
 * @class 配置モデルコリジョンデータアクセサー
 */
class CollisionDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( CollisionDataAccessor );

public:

  /**
   * @brief コリジョンデータ
   */
  struct COLLISION_DATA
  {
    u32 shape;              ///< 形状
    f32 size[3];            ///< サイズ
    f32 centerOffset[3];    ///< 中心までのオフセット
  };

public:

  /**
   * @brief コンストラクタ
   * @param pData パックデータ。FieldResidentから取得してください
   */
  CollisionDataAccessor();
  CollisionDataAccessor( void* pData);

  /**
   * @brief デストラクタ
   */
  virtual ~CollisionDataAccessor();

  /**
   * @brief パックデータセット
   * @param pData パックデータ。FieldResidentから取得してください
   */
  void SetData( void* pData);

  /**
   * @brief データ取得
   * @param staticModelID 配置モデルID
   * @return コリジョンデータ
   * @note 見つからない場合はNULL
   */
  const COLLISION_DATA* GetData( u16 staticModelID) const ;

private:

  /**
   * @brief ヘッダー
   */
  struct HEADER
  {
    u16 staticModelID;  ///< 配置モデルID
    u8  padding[2];
  };

  /**
   * @brief バイナリデータ全体
   */
  struct DATA
  {
    HEADER            header;     ///< ヘッダー
    COLLISION_DATA    collision;  ///< コリジョンデータ
  };

private:

  void*                       m_pData;      ///< エリア常駐モデル全てのコリジョンデータが含まれているバイナリデータ
  gfl2::fs::BinLinkerAccessor m_Accessor;   ///< 1つ1つのバイナリを取得するためのアクセサー

};

GFL_NAMESPACE_END( StaticModel )
GFL_NAMESPACE_END( Field )

#endif // __STATIC_MODEL_COLLISION_DATA_ACCESSOR_H_INCLUDED__