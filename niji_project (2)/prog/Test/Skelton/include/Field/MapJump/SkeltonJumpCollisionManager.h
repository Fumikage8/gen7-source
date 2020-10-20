#if defined(GF_PLATFORM_WIN32)
//*************************************************************************************************
/**
 @file jump_collision_managera.h
 @brief 擬似マップジャンプの衝突管理
 @date 2015.01.22
 @author saita_kazuki
 */
//*************************************************************************************************

#ifndef __SKELTON_MAPJUMP_JUMPCOLLISIONMANAGER_H__
#define __SKELTON_MAPJUMP_JUMPCOLLISIONMANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_MathAABB.h>
#include <xml/include/rapidxml.hpp> // no need to link

// 前方参照
namespace gfl2 {
namespace collision {
  class CollisionModel;
}
}

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)



class JumpDataManager;

//=================================================================================================
// xmlコリジョンデータ
//=================================================================================================
struct COLLISION_DATA
{
  u32               id;         // 通し番号
  u32               priority;   // 優先度(0〜。値が小さいほど優先度高い)
  c8                name[512];  // 名前(マップジャンプデータと一致するもの)
  gfl2::math::AABB  aabb;       // AABB
};

//=================================================================================================
// コリジョンデータ管理
//=================================================================================================
class CollisionDataManager
{
public:

  static const u32         COLLISION_DATA_MAX;    // 読み込みデータ最大数
  //static const std::string NAME_PREFIX;           // データの名前の接頭語

public:

  CollisionDataManager( void);
  virtual ~CollisionDataManager();

  void Initialize();
  void Terminate();

  b32 LoadData( const c8* pFilePath);
  b32 LoadData( const c16* pFilePath);


  /**
  * @brief 衝突データ名の取得
  * @param pos 衝突をチェックするオブジェクトの位置
  * @param pCollisionModel コリジョンモデル
  * @return 衝突データ名
  */
  const c8* GetHitName( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel = NULL) const;

  const COLLISION_DATA* GetData();
  u32 GetDataNum();

private:

  const c8* GetHitName_CollisionModel( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel) const;
  const c8* GetHitName_CollisionData( gfl2::math::Vector4& pos) const;

  b32 UpdateImpl( c8* pData );
  b32 LoadCollisionData( const rapidxml::xml_node<>* pCollisionDataNode, COLLISION_DATA* pCollisionData);


  COLLISION_DATA*     m_pCollisionDataTable;    // 当たり判定用データテーブル xmlから読み込んだ情報を格納する
  u32                 m_useDataNum;             // データ数
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // __SKELTON_MAPJUMP_JUMPCOLLISIONMANAGER_H__
#endif // GF_PLATFORM_WIN32


