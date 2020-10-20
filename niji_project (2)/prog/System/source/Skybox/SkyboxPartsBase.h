//===================================================================
/**
 * @file    SkyboxPartsBase.h
 * @brief   天球オブジェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.05.07
 */
//===================================================================

#if !defined( SKYBOX_PARTS_BASE_H_INCLUDED )
#define SKYBOX_PARTS_BASE_H_INCLUDED
#pragma once


#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_SRT.h>
#include <math/include/gfl2_math.h>
#include <model/include/gfl2_BaseModel.h>

#include "System/include/nijiAllocator.h"

// 前方参照
namespace poke_3d { namespace model {
  class BaseModel;
}}
namespace gfl2 {
  namespace renderingengine { namespace scenegraph {
    namespace resource {
      class ResourceNode;
    }
    namespace instance {
      class ModelInstanceNode;
    }
  }}
}

GFL_NAMESPACE_BEGIN( System )
GFL_NAMESPACE_BEGIN( Skybox )

/**
 * @class 天球モデルクラス
 */
class PartsBase
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( PartsBase );

public:

  /**
   * @brief コンストラクタ
   * @param pParent 親ヒープ
   * @param heapSize ローカルヒープサイズ
   * @param heapName ローカルヒープ名
   * @param workSize ワークサイズ
   * @param workSize ワークサイズ
   * @param isAllocateByVRAM trueでテクスチャをVRAMに乗せる
   * @param vramMallocMode isAllocateByVRAMがtrueのときに指定されるVRAM確保タイプ
   */
  PartsBase( gfl2::heap::HeapBase* pParent, u32 heapSize, c8* heapName, u32 modelWorkSize, u32 motionWorkSize, bool isAllocateByVRAM = false, System::nijiAllocator::VramMallocMode vramMallocMode = nijiAllocator::VRAM_MALLOC_AB);

  /**
   * @brief デストラクタ
   */
  virtual ~PartsBase();

  /**
   * @brief 初期化
   * @param pBinLinkerAccessor アクセサー
   */
  virtual void Initialize( void* pResourceData){};

  /**
   * @brief 破棄
   */
  virtual void Terminate(){};

  /**
   * @brief 更新
   */
  virtual void Update( const gfl2::math::Vector3& pos){};

  /**
   * @brief 1日アニメーションフレーム更新
   */
  virtual void UpdateAllTimeAnimationFrame( f32 frame);

  /**
   * @brief アニメーション更新リクエスト
   */
  void RequestUpdateAnimation();

  /**
   * @brief 表示設定 全モデルに設定版
   * @param isVisible trueで表示
   */
  void SetVisibleAll( bool isVisible);

  /**
   * @brief 表示設定
   * @param index モデル番号
   * @param isVisible trueで表示
   */
  void SetVisible( u32 index, bool isVisible);

  /**
   * @brief 表示されているか 全モデルをチェック版
   * @retval true 全て表示
   * @retval false どれかひとつでも非表示
   */
  bool IsVisibleAll();

  /**
   * @brief 表示されているか
   * @param index モデル番号
   * @retval true 表示
   * @retval false 非表示
   */
  bool IsVisible( u32 index);

  /**
   * @brief モデルの取得
   * @param index モデル番号
   */
  poke_3d::model::BaseModel* GetModel( u32 index) const ;

  /**
   * @brief モデル数の取得
   * @return 生成したモデル数
   */
  u32 GetModelNum() const;

  /**
   * @brief 有効無効フラグの設定
   * @param isEnable trueで有効
   * @note 表示ON/OFFも同時に制御
   */
  void SetEnable( bool isEnable);

  /**
   * @brief 有効無効フラグの取得
   * @retval true 有効
   * @retval false 無効
   */
  bool IsEnable();

private:

  gfl2::heap::HeapBase* CreateLocalHeap( gfl2::heap::HeapBase* pParent, u32 size, c8* name);
  void DeleteLocalHeap();

  void CreateWorks( u32 modelNum,  u32 motionNum);
  void DeleteWorks();

protected:

  poke_3d::model::BaseModel* CreateModel( gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceNode);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateModelResourceNode( void* pData);
  gfl2::renderingengine::scenegraph::resource::ResourceNode* CreateMotionResourceNode( void* pData);

  void AttachAnimation( poke_3d::model::BaseModel* pModel,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceNode,
    bool isLoop = true,
    u32 slotIndex = 0,
    poke_3d::model::BaseModel::AnimationBit animationBit = poke_3d::model::BaseModel::ANIMATION_BIT_ALL);

  gfl2::gfx::Pool GetPool( System::nijiAllocator::VramMallocMode vramMallocMode ) const ;

protected:

  gfl2::heap::HeapBase*         m_pHeap;
  System::nijiAllocator*        m_pGLAllocator;
  u32                           m_modelNum;
  bool                          m_isEnable;

  bool                          m_isUpdateAnimation;
  f32                           m_allTimeAnimationFrame;

  poke_3d::model::BaseModel**                                 m_ppModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_ppMotionResourceRootNode;

  bool                                    m_isAllocateByVRAM;
  System::nijiAllocator::VramMallocMode   m_vramMallocMode;
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( System )

#endif // SKYBOX_PARTS_BASE_H_INCLUDED

