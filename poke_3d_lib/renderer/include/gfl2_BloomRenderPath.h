#if !defined(POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATH_H_INCLUDED)
#define POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATH_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_BloomRenderPath.h
 @author  kawada_koji@gamefreak.co.jp(CTR版)
 @author  ikeuchi_yuya@gamefreak.co.jp(Win32版)
 @date    2015.05.27
 @brief   ブルームの描画パス
 @note    ブルーム(bloom)と言ったり、グレア(glare)と言ったりします。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(gfx)
    class IGLAllocator;
    class Surface;
    class Texture;
  GFL_NAMESPACE_END(gfx)
  GFL_NAMESPACE_BEGIN(renderingengine)
    GFL_NAMESPACE_BEGIN(scenegraph)
      class DagNode;
      GFL_NAMESPACE_BEGIN(resource)
        class TextureResourceNode;
      GFL_NAMESPACE_END(resource)
    GFL_NAMESPACE_END(scenegraph)
  GFL_NAMESPACE_END(renderingengine)
  GFL_NAMESPACE_BEGIN(fs)
    class AsyncFileManager;
  GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(renderer)


//==============================================================================
/**
 @class     BloomRenderPath
 @brief     ブルームの描画パス
 @caution  生成中は破棄できません。
             ブルームをオフにしてから1フレーム後に破棄しないと安全ではありません。
 */
//==============================================================================
class BloomRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomRenderPath);

public:
  // 用語
  // source         ブルームを掛けられるソースの...
  // highLuminance  高輝度抽出の...
  //     highLuminanceRenderTarget  高輝度抽出レンダーターゲット
  //     highLuminanceCopy          高輝度抽出したものの縮小コピー
  // blur           加算合成ブラーの...  =  board           加算合成板ポリゴンの...

  //! @brief  ブルームを掛けられるソースの種類
  enum SourceType
  {
    SOURCE_TYPE_FRAME_BUFFER,    //!< フレームバッファ
    //SOURCE_TYPE_SURFACE,         //!< サーフェイス
    //SOURCE_TYPE_TEXTURE,         //!< テクスチャ
  };
  //! @brief  ブルームの種類
  enum BloomType
  {
    BLOOM_TYPE_GAUSSIAN_FILTER_3X3,    //!< ガウシアンフィルタ3x3(分母16で4,2,1)
    //BLOOM_TYPE_GAUSSIAN_FILTER_5X5,    //!< ガウシアンフィルタ5x5(分母256で36,24,16,6,4,1)
  };

  //! @brief  設定
  class Description
  {
  public:
    SourceType          sourceType;                                //!< ブルームを掛けられるソースの種類
    BloomType           bloomType;                                 //!< ブルームの種類
    u32                 highLuminanceRenderTargetReciprocalScale;  //!< 高輝度抽出レンダーターゲットのスケールの逆数(例:4のとき1/4にする)(1,2,4,...)
    u32                 highLuminanceCopyNumMax;                   //!< 高輝度抽出の際の縮小コピー回数(0,1,2,...)
    const gfl2::gfx::Surface* sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::NumberOf];  //!< フレームバッファ(sourceTypeがSOURCE_TYPE_FRAME_BUFFERのときだけ使用する)。
                                                                                     //   NULLの入っている画面はブルームの対象にならない。

    // 例1:
    // highLuminanceRenderTargetReciprocalScale = 2
    // highLuminanceCopyNumMax                  = 2
    // のとき
    // ソース                      (w512,h256)
    // 高輝度抽出レンダーターゲット(w256,h128)
    // 縮小コピー1                 (w128,h 64)
    // 縮小コピー2                 (w 64,h 32)

    // 例2:
    // highLuminanceRenderTargetReciprocalScale = 1
    // highLuminanceCopyNumMax                  = 3
    // のとき
    // ソース                      (w512,h256)
    // 高輝度抽出レンダーターゲット(w512,h256)
    // 縮小コピー1                 (w256,h128)
    // 縮小コピー2                 (w128,h 64)
    // 縮小コピー3                 (w 64,h 32)

    // 例3:
    // highLuminanceRenderTargetReciprocalScale = 4
    // highLuminanceCopyNumMax                  = 0 
    // のとき
    // ソース                      (w512,h256)
    // 高輝度抽出レンダーターゲット(w128,h 64)
    // 縮小コピーなし
    
  public:
    Description(void)
      : sourceType(SOURCE_TYPE_FRAME_BUFFER),
        bloomType(BLOOM_TYPE_GAUSSIAN_FILTER_3X3),
        highLuminanceRenderTargetReciprocalScale(2),
        highLuminanceCopyNumMax(1)
    {
      for(s32 i=0; i<gfl2::gfx::CtrDisplayNo::NumberOf; ++i)
      {
        sourceFrameBuffer[i] = NULL;
      }
    }
  };
  
  //! @brief  パラメータ
  class Parameter
  {
  public:
    bool               enable;
    u32                reduceNum;
    f32                strength;
    f32                range;
    f32                intensity;
    gfl2::math::Vector4 weight;
  public:
    Parameter(void)
      : enable(true),
        reduceNum(1),
        strength(2.0f),
        range(1.0f),
        intensity(0.333f),
        weight( gfl2::math::Vector4(0.333f, 0.333f, 0.333f, 0.0f) )
    {}
  };

public:
  BloomRenderPath(void);
  virtual ~BloomRenderPath();
 
  virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

public:
  //! @name  生成/破棄
  //@{
  //! @brief  非同期生成　開始
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  void StartAsyncCreate(
      gfl2::heap::HeapBase*       heapForImpl,                                   // デバイスメモリのヒープ。
      gfl2::heap::HeapBase*       heapForTemp,                                   // 一時的に使用するヒープ。普通のメモリでもデバイスメモリでもどちらでもよい。
      gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,  // VRAM限定。
      gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,          // VRAMかデバイスメモリ。Descriptionに書いたhighLuminanceCopyNumMax個テクスチャを作成します。
      gfl2::fs::AsyncFileManager* asyncFileManager,                              // ファイルマネージャ。
      const Description& desc  // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
  );
  //! @brief  同期生成
  void SyncCreate(
      gfl2::heap::HeapBase*       heapForImpl,                                   // デバイスメモリのヒープ。
      gfl2::heap::HeapBase*       heapForTemp,                                   // 一時的に使用するヒープ。普通のメモリでもデバイスメモリでもどちらでもよい。
      gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceRenderTargetTexture,  // VRAM限定。
      gfl2::gfx::IGLAllocator*    allocatorForHighLuminanceCopyTexture,          // VRAMかデバイスメモリ。Descriptionに書いたhighLuminanceCopyNumMax個テクスチャを作成します。
      gfl2::fs::AsyncFileManager* asyncFileManager,                              // ファイルマネージャ。                                                                      
      const Description& desc  // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
  );
  //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  Updateを毎フレーム呼んでいないと非同期生成処理は進みません。
  bool IsCreated(void) const;
  
  //! @brief  破棄できるか
  //! @return  破棄できるときtrueを返す。
  //!          生成中は破棄できないのでfalseを返す。
  //! @caution  ブルームをオフにしてから1フレーム後に破棄しないと安全ではありません。
  //!             この関数は「破棄できるか否か」だけを返しており、「オフにして1フレーム空けたか否か」は判定していません。
  //!             「オフにして1フレーム空けたか否か」は呼び出し元で判定して下さい。
  bool CanDestroy(void) const;
  //@}
  
  //! @brief  更新
  //! @note  毎フレーム呼んで下さい。
  //!        非同期生成処理を進めます。
  //!        DrawEnvNodeの値を反映します。つまり、Updateを呼ぶと、本クラス内の値がDrawEnvNodeの値で上書きされます。
  //!        本クラス内の値を使ってをブルームを更新します。
  void Update(void);

  //! @brief  DrawEnvNodeを設定する
  //! @note  設定しないときはNULLを渡す。
  //!        渡したDrawEnvNodeの値が変わると、それに追従して本クラス内の値も変わります(Updateを呼ぶ必要あり)。
  //!        つまり、渡したDrawEnvNodeがアニメをしているときは、ブルームがアニメをします(Updateを呼ぶ必要あり)。
  void SetDrawEnvNode(const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode);

  //! @brief  アルファマスクテクスチャを設定する(白のところにブルームが掛かり、黒のところにブルームが掛からない)
  //! @note  設定しないときはNULLを渡す(白のテクスチャを渡しているのと同じになる)
  //!        w512, h256の左下にある絵を等倍で使用します。
  void SetAlphaMaskTexture(const gfl2::gfx::Texture* alphaMaskTex);

  //! @brief  dagNode内に存在するテクスチャリソースノードを取得する(最初の1個を見付けた時点で探すのを止める)
  static gfl2::renderingengine::scenegraph::resource::TextureResourceNode* GetTextureResourceNode(gfl2::renderingengine::scenegraph::DagNode* dagNode);

  //! @name  Getter/Setter
  //@{
  //! @brief  ブルームのオン/オフ
  //! @note  trueでブルームが掛かり、falseでブルーム処理を省く
  void SetEnable(bool enable);
  bool GetEnable(void) const;

  //! @brief  使用する縮小コピーの回数
  //! @note  0=高輝度抽出レンダーターゲット; 1=縮小コピー1; 2=縮小コピー2; ...
  //!        DescriptionのhighLuminanceCopyNumMaxで設定した値より大きい値は指定できない。同じ値は指定できる。
  void SetReduceNum(u32 reduceNum);
  u32  GetReduceNum(void) const;
 
  //! @brief  ブラーの合成倍率
  //! @note  0以上
  void SetStrength(f32 strength);
  f32 GetStrength(void) const;

  //! @brief  ブラーの広がり
  //! @note  0以上
  void SetRange(f32 range);
  f32 GetRange(void) const;

  //! @brief  高輝度とみなす値
  //! @note  (r*weight.x + g*weight.y + b*weight.z > intensity)を満たすとき高輝度とみなす
  //!        0?1
  void SetIntensity(f32 intensity);
  f32 GetIntensity(void) const;

  //! @brief  各色の重み
  //! @note  (r, g, b, 使わない)
  //!        0?1
  void SetWeight(const gfl2::math::Vector4& weight);
  gfl2::math::Vector4 GetWeight(void) const;
  //@}

private:
  //! @brief  ブルームの描画パスのインプリメント
  class BloomRenderPathImpl;

private:
  const gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;
  const gfl2::gfx::Texture*                                       m_alphaMaskTex;
  
  BloomRenderPathImpl*  m_impl;

  Parameter  m_param;  // 処理はm_implにやってもらう
};


GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(poke_3d)


#endif  // POKE_3D_LIB_RENDERER_GFL2_BLOOMRENDERPATH_H_INCLUDED

