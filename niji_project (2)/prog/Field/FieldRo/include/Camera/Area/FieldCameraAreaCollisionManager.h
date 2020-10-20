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

#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"

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
      ,m_Height( 0.0f )
      ,m_Priority( 0 )
    {

    }

    // 無効化
    void Clear( void )
    {
      m_IsHit = false;
      m_AreaDataNo = AreaDataNo::INVALID;
      m_Ratio = 0.0f;
      m_Height = 0.0f;
      m_Priority = 0;
    }

    // 優先度チェック、引数が勝つならtureが返る
    b32 CheckPriority( const gfl2::math::Vector3& rPos, s32 priority, f32 height ) const
    {
      if( m_IsHit == false )
      {
        return true;
      }

      if( priority > m_Priority )
      {
        return true;
      }
      // 同じプライオリティなら近い方
      else if( priority == m_Priority )
      {
        if( gfl2::math::Abs( height - rPos.y ) < gfl2::math::Abs( m_Height - rPos.y ) )
        {
          return true;
        }
        else
        {
          // 同じ高さのときは先勝ち
          return false;
        }
      }
      else
      {
        return false;
      }
    }

    // 優先度チェック、引数が勝つならtureが返る
    b32 CheckPriority( const gfl2::math::Vector3& rPos, const HitResult& rTarget ) const
    {
      // 引数不正
      if( rTarget.m_IsHit == false )
      {
        return false;
      }

      return CheckPriority( rPos, rTarget.m_Priority, rTarget.m_Height );
    }

    // データを設定
    void Set( u32 areaDataNo, s32 priority, f32 height, f32 ratio = 0.0f )
    {
      m_IsHit = true;
      m_AreaDataNo = areaDataNo;
      m_Height = height;
      m_Ratio = ratio;
      m_Priority = priority;
    }

    // Hitしているならtrueが返る
    b32 IsHit( void ) const
    {
      return m_IsHit;
    }


    b32 m_IsHit;
    u32 m_AreaDataNo;
    f32 m_Ratio;
    f32 m_Height;
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

#if PM_DEBUG
  /**
  *  @brief  カメラエリアをデバッグ表示へ登録
  *
  *  @param  color  コリジョン表示色
  *  @param  onlyActiveFlag true:アクティブなコリジョンのみ表示する false：全て表示する
  */
  void DebugRegistRenderCollisionArea( gfl2::math::Vector4 color, s32 hitAttr = -1, b32 onlyActiveFlag = true );
#endif  //#if PM_DEBUG

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