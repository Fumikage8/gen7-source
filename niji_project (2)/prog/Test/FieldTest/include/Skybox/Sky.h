//===================================================================
/**
 * @file    Sky.h
 * @brief   天球の空の制御
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#if !defined( SKY_H_INCLUDED )
#define SKY_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


namespace gfl2 {
  namespace gfx {
    class IGLAllocator;
  }
  namespace fs {
    class AsyncFileManager;
  }
}


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
GFL_NAMESPACE_BEGIN( Skybox )


class SkyboxModel;


class Sky
{
  GFL_FORBID_COPY_AND_ASSIGN(Sky); //コピーコンストラクタ＋代入禁止

public:

  ///< モデル種類
  enum ModelType
  {
    MODEL_TYPE_BOX,

    MODEL_TYPE_MAX
  };

  /**
   * @brief   コンストラクタ
   */
  Sky();
  ///< デストラクタ
  virtual ~Sky();

  /**
   * @brief   生成
   * @param   pSystemHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pAsyncFileManager garc読み込みクラス
   */
  void Create( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* m_pGLAllocator, gfl2::fs::AsyncFileManager* pAsyncFileManager);

  /**
   * @brief   破棄
   * @retval  true  破棄完了
   * @retval  false 破棄実行中
   */
  b8 Destroy();

  /**
   * @brief   初期化
   */
  void Initialzie();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   描画
   */
  void Draw();

  /**
   * @brief   表示フラグ切り替え
   * @param   isEnable trueで有効、falseで無効
   */
  void SetVisible( b8 isEnable);

  /**
   * @brief   表示フラグ取得
   * @retval  true 有効
   * @retval  false 無効
   */
  b8 IsVisible() const ;

  /**
   * @brief   モデルインスタンスの取得
   */
  SkyboxModel* GetModel( u32 index);


private:

  SkyboxModel*                m_pSkyModel[ MODEL_TYPE_MAX ];

};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // SKY_H_INCLUDED
