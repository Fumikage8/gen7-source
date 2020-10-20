/**
* @brief  フィールドカメラのシステム常駐アニメのインスタンスを管理するクラス
* @file   FieldCameraSystemAnimationResource.h
* @author ikuta_junya
* @data   2015.10.01
*/

#if !defined( __FIELDCAMERASYSTEMANIMATIONRESOURCE_H_INCLUDED__ )
#define __FIELDCAMERASYSTEMANIMATIONRESOURCE_H_INCLUDED__ 
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

// niji
#include "System/include/nijiAllocator.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

/**
* @brief フィールドカメラを管理するクラス
*/
class CameraSystemAnimationResource
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraSystemAnimationResource );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    System::nijiAllocator*        m_pDeviceAllocator;
    void*                         m_pData;
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraSystemAnimationResource( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraSystemAnimationResource( void );
  
  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief リソース取得
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
   gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResource( u32 index );

private:

  // 制御

 
  // 内部オブジェクト
  gfl2::renderingengine::scenegraph::resource::ResourceNode**   m_ppResourceNodeTable;
  u32                                                           m_ResourceNodeNum;     

};  // class CameraSystemAnimationResource


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERASYSTEMANIMATIONRESOURCE_H_INCLUDED__