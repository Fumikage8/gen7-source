#if !defined(NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERPATH_H_INCLUDED)
#define NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERPATH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    MobiclipRenderPath.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.18
 @brief   mobiclip用の描画パス
 @note    gf内製コード。mobiclipとは無関係。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)
class IGLAllocator;
class Texture;
GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(resource)
class ResourceNode;
class TextureResourceNode;
GFL_NAMESPACE_END(resource)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(instance)
class ModelInstanceNode;
GFL_NAMESPACE_END(instance)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(movie)
GFL_NAMESPACE_BEGIN(renderer)


//==============================================================================
/**
 @class     MobiclipRenderPath
 @brief     mobiclip用の描画パス
 @caution  生成中は破棄できません。
             描画をやめてから1フレーム後に破棄しないと安全ではありません。
 */
//==============================================================================
class MobiclipRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN(MobiclipRenderPath);

public:
  MobiclipRenderPath(void);
  virtual ~MobiclipRenderPath();
 
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

public:
  static void StartAsyncCreateFileOpne(
      gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
      gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
    );
  static void StartAsyncCreateFileClose(
      gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
      gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
    );

  static bool UpdateAsyncCreateFile(gfl2::fs::AsyncFileManager* asyncFileManager);

  //! @brief  非同期生成　開始
  //! @note  UpdateAsyncCreateを毎フレーム呼んでいないと非同期生成処理は進みません。
  void StartAsyncCreate(
      gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
      gfl2::gfx::IGLAllocator*    glAllocator,        // デバイスメモリのglアロケータ
      gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
  );
  //! @brief  同期生成
  void SyncCreate(
      gfl2::heap::HeapBase*       devHeap,            // デバイスメモリのヒープ
      gfl2::gfx::IGLAllocator*    glAllocator,        // デバイスメモリのglアロケータ
      gfl2::fs::AsyncFileManager* asyncFileManager    // ファイルマネージャ
  );
  //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  UpdateAsyncCreateを毎フレーム呼んでいないと非同期生成処理は進みません。
  bool IsCreated(void) const;
  //! @brief  破棄できるか
  //! @return  破棄できるときtrueを返す。
  //!          生成中は破棄できないのでfalseを返す。
  //! @caution  描画をやめてから1フレーム後に破棄しないと安全ではありません。
  //!             この関数は「破棄できるか否か」だけを返しており、「描画をやめてから1フレーム空けたか否か」は判定していません。
  //!             「描画をやめてから1フレーム空けたか否か」は呼び出し元で判定して下さい。
  bool CanDestroy(void) const;
  
  //! @brief  非同期生成　更新
  //! @return  生成が完了したらtrueを返す。
  //! @note  trueを返すまで毎フレーム呼んで下さい。
  //!        非同期生成処理を進めます。
  bool UpdateAsyncCreate(void);

public:
  //! @brief  表示する絵が描かれたバッファを設定する
  void SetBufForTexture(
      gfl2::gfx::IGLAllocator*  glAllocator,     // デバイスメモリのglアロケータ
      const void*               bufForTexture,   // バッファのアドレス
      gfl2::gfx::Pool           pool,            // メモリ配置
      u32                       width,           // 幅
      u32                       height,          // 高さ
      gfl2::gfx::Format         format,           // ピクセルフォーマット
      float                     imageClockwiseDegreeRotationZ //イメージ画像回転
  );
  //! @brief  表示する絵が描かれたバッファを解除する
  void UnsetBufForTexture(void);

private:
  //! @brief  生成状態
  enum CreateState
  {
    CREATE_STATE_NONE,      //!< 生成していない
    CREATE_STATE_CREATING,  //!< 生成中
    CREATE_STATE_CREATED,   //!< 生成済み
  };

  //! @brief  リソース
  enum ResourceType
  {
    RESOURCE_TYPE_MODEL,
    RESOURCE_TYPE_VSH,
    RESOURCE_TYPE_FSH,
    RESOURCE_TYPE_TEX,
    RESOURCE_TYPE_NUM
  };

private:
  void syncCreateBinBuf(void);
  void aSyncCreateBinBuf(void);
  void unpackBinLink(void);
  void createResourceNode(void);
  void createInstanceNode(void);

  void createTexture(
      gfl2::gfx::IGLAllocator*  glAllocator,  
      const void*               bufForTexture, 
      gfl2::gfx::Pool           pool,          
      u32                       width,         
      u32                       height,        
      gfl2::gfx::Format         format
  );
  void destroyTexture(void);

private:
  // 外部から受け取るもの
  gfl2::heap::HeapBase*       m_devHeap;
  gfl2::gfx::IGLAllocator*    m_glAllocator;
  gfl2::fs::AsyncFileManager* m_asyncFileManager;
 
  // 生成状態
  CreateState m_createState;

  // バイナリ
  void*  m_packBuf;
  size_t m_packBufSize;
  void*  m_binBuf[RESOURCE_TYPE_NUM];      // m_packBuf内の各リソースのバイナリのアドレスを覚えておくだけ
  size_t m_binBufSize[RESOURCE_TYPE_NUM];

  // リソースノード、インスタンスノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode*               m_resourceNode[RESOURCE_TYPE_NUM];
  //const gfl2::renderingengine::scenegraph::resource::TextureResourceNode*  m_textureResourceNode;              // m_resourceNode内のテクスチャリソースノードのアドレスを覚えておくだけ  // 使っていないのでコメントアウト。
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*          m_modelInstanceNode;

  // 表示する絵
  gfl2::gfx::IGLAllocator*  m_glAllocatorForTexture;
  const void*               m_bufForTexture;
  gfl2::gfx::Texture*       m_texture;

  // 回転
  float m_imageClockwiseDegreeRotationZ;
};


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(movie)


#endif  // NIJI_PROJECT_MOVIE_RENDERER_MOBICLIPRENDERPATH_H_INCLUDED

