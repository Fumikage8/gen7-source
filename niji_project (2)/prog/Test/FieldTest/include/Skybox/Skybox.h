//===================================================================
/**
 * @file    Skybox.h
 * @brief   天球
 * @author  saita_kazuki
 * @date    2015.02.19
 */
//===================================================================

#if !defined( SKYBOX_H_INCLUDED )
#define SKYBOX_H_INCLUDED
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


class Sky;
class Cloud;


// 天球クラス
class Skybox
{
  GFL_FORBID_COPY_AND_ASSIGN(Skybox); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   */
  Skybox();
  ///< デストラクタ
  virtual ~Skybox();

  /**
   * @brief   生成
   * @param   pHeap アプリケーションヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pAsyncFileManager garc読み込みクラス
   */
  void StartCreate( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, gfl2::fs::AsyncFileManager* pAsyncFileManager);

  /**
   * @brief   生成処理の更新
   * @retval  true 完了
   * @retval  false 実行中
   */
  b8 UpdateCreate();

  /**
   * @brief   生成処理が完了しているか
   * @retval  true 完了
   * @retval  false 実行中
   */
  b8 IsFinishCreate() const ;

  /**
   * @brief   破棄
   * @retval  true  破棄完了
   * @retval  false 破棄実行中
   */
  b8 Destroy();

  /**
   * @brief   初期化
   */
  void Initialize();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   描画
   */
  void Draw();

  /**
   * @brief   有効/無効フラグ切り替え
   * @param   isEnable trueで有効、falseで無効
   * @note    falseにすると更新、描画の処理が無効となる
   */
  void SetEnable( b8 isEnable);

  /**
   * @brief   有効/無効フラグ取得
   * @retval  true 有効
   * @retval  false 無効
   */
  b8 IsEnable() const ;

  /**
   * @brief   空の取得
   */
  Sky* GetSky();

  /**
   * @brief   雲の取得
   */
  Cloud* GetCloud();

private:

  ///< 生成状況
  enum CreateState
  {
    CREATA_STATE_NONE,    ///< 生成していない
    CREATE_STATE_UPDATE,  ///< 生成処理実行中
    CREATE_STATE_FINISH,  ///< 生成完了
  };

  b8        m_isEnable;       ///< 天球の更新が有効か
  u8        m_seq;            ///< シーケンス制御
  u8        m_createState;    ///< 生成状況

  Sky*      m_pSky;           ///< 空制御
  Cloud*    m_pCloud;         ///< 雲制御

  gfl2::heap::HeapBase*       m_pHeap;
  gfl2::gfx::IGLAllocator*    m_pGLAllocator;
  gfl2::fs::AsyncFileManager* m_pAsyncFileManager;
};

GFL_NAMESPACE_END( Skybox )
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

#endif // SKYBOX_H_INCLUDED
