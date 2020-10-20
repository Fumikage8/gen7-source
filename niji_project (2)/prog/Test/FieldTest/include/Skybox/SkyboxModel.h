//===================================================================
/**
 * @file    SkyboxModel.h
 * @brief   天球リソース
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#if !defined( SKYBOXMODEL_H_INCLUDED )
#define SKYBOXMODEL_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>

namespace gfl2
{
  namespace renderingengine { namespace scenegraph
  {
    namespace resource
    {
      class ResourceNode;
    }
    namespace instance
    {
      class ModelInstanceNode;
    }
  }}

  namespace gfx {
    class IGLAllocator;
  }
}

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
GFL_NAMESPACE_BEGIN( Skybox )


class SkyboxModel
{
  GFL_FORBID_COPY_AND_ASSIGN(SkyboxModel); //コピーコンストラクタ＋代入禁止

public:

  // 設定情報
  struct Description
  {
    gfl2::fs::AsyncFileManager*   pAsyncFileManager;    // garc読み込みに使用
    gfl2::heap::HeapBase*         pSystemHeap;          // インスタンス確保などに使用するヒープ
    gfl2::gfx::IGLAllocator*      pGLAllocator;         // リソース確保用アロケータ
    gfl2::fs::ArcFile::ARCID      arcId;                // ARCID
    gfl2::fs::ArcFile::ARCDATID   datId;                // アーカイブファイル内データID
  };

  /**
   * @brief   コンストラクタ
   */
  SkyboxModel();
  ///< デストラクタ
  virtual ~SkyboxModel();

  /**
   * @brief   生成
   * @param   desc 設定情報
   */
  void Create( const Description& desc);

  /**
   * @brief   破棄
   * @retval  true  破棄完了
   * @retval  false 破棄実行中
   */
  bool Destroy();

  /**
   * @brief   表示フラグ切り替え
   * @param   isEnable trueで有効、falseで無効
   */
  void SetVisible( b32 isEnable);

  /**
   * @brief   表示フラグ取得
   * @retval  true 有効
   * @retval  false 無効
   */
  b32 IsVisible() const ;

  /**
   * @brief   リソースルートノード取得
   * @return   リソースルートノード
   */
  gfl2::renderingengine::scenegraph::resource::ResourceNode* GetResourceRootNode() const ;

  /**
   * @brief   インスタンスノード取得
   * @return   インスタンスノード
   */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() const ;

private:

  u8      m_seq;

  c8*     m_pModelPackBinary;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*        m_pResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*   m_pModelInstanceNode;

  gfl2::gfx::IGLAllocator* m_pGLAllocator;
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // SKYBOXMODEL_H_INCLUDED
