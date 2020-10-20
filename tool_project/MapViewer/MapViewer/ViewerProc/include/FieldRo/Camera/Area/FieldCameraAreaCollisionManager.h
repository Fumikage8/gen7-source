//======================================================================
/**
* @file FieldCameraAreaCollisionManager.h
* @date 2015/07/01 18:31:52
* @author ikuta_junya
* @brief フィールドカメラエリアのコリジョンデータを管理するクラス
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDCAMERAAREACOLLISIONMANAGER_H_INCLUDED__
#define __FIELDCAMERAAREACOLLISIONMANAGER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>
#include <Collision/include/gfl2_CollisionModel.h>

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class PrimitiveCollisionDataAccessor;
class AreaDataManager;

/**
* @brief フィールドカメラエリアのコリジョンデータを管理するクラス
* @note  現仕様では1エリア分のデータがフィールド初期化時に渡され、以降管理する
*/
class AreaCollisionManager
{
  GFL_FORBID_COPY_AND_ASSIGN( AreaCollisionManager );

public:

  class HitResult
  {
  public:

    HitResult( void ) : m_IsHit( false )
      ,m_AreaDataNo( AreaDataNo::INVALID )
      ,m_Ratio( 0.0f )
      ,m_Priority( 0 )
    {

    }

    // 無効化
    void Clear( void )
    {
      m_IsHit = false;
      m_AreaDataNo = AreaDataNo::INVALID;
      m_Ratio = 0.0f;
      m_Priority = 0;
    }

    b32 m_IsHit;
    s32 m_AreaDataNo;
    f32 m_Ratio;
    s32 m_Priority;
  };

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:

    InitializeDescription( void ) :
      m_pSystemHeap( NULL )
      ,m_pDeviceAllocator( NULL )
      ,m_pPrimitiveData( NULL )
      ,m_pModelData( NULL )
    {
    }

    gfl2::heap::HeapBase* m_pSystemHeap;
    gfl2::gfx::IGLAllocator* m_pDeviceAllocator;
    void* m_pPrimitiveData;
    void* m_pModelData;
  };

  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  AreaCollisionManager( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~AreaCollisionManager( void );

  /**
  * @brief 指定座標の判定を行う
  *
  * @param  rPos 指定座標
  * @param  rAreaDataManager エリアデータ(forプライオリティ)
  * @param  pOut 結果格納先
  *
  * @return 無し
  */
  void Check( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, HitResult* pOut ) const;

private:

  /**
  * @brief 指定座標の判定を行う( 円形状 )
  *
  * @param  rPos 指定座標
  * @param  rAreaDataManager エリアデータ(forプライオリティ)
  * @param  rPresent         現状のHit状態、他の形状の結果が格納されている( pOut と同じインスタンスでも正常動作する )
  * @param  pOut             結果格納先
  *
  * @return 無し
  */
  void CheckCircle( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, const HitResult& rPresent, HitResult* pOut ) const;

  /**
  * @brief 指定座標の判定を行う( 自由形状 )
  *
  * @param  rPos 指定座標
  * @param  rAreaDataManager エリアデータ(forプライオリティ)
  * @param  rPresent         現状のHit状態、他の形状の結果が格納されている( pOut と同じインスタンスでも正常動作する )
  * @param  pOut             結果格納先
  *
  * @return 無し
  */
  void CheckMesh( const gfl2::math::Vector3& rPos, const AreaDataManager& rAreaDataManager, const HitResult& rPresent, HitResult* pOut ) const;



  PrimitiveCollisionDataAccessor*                             m_pPrimitiveCollisionDataAccessor;
  gfl2::collision::CollisionModel*                            m_pModelClollisionData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*  m_pModelClollisionDataResource;
};


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );


#endif // __FIELDCAMERAAREACOLLISIONMANAGER_H_INCLUDED__