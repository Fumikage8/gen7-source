//=============================================================================
/**
 *
 *	@file		gfl2_FadeManager.h
 *	@brief  フェードマネージャー   上画面、下画面、リクエストの管理のみ 描画はここでは管理しない
 *	@author k.ohno
 *	@data		2015.03.31
 *
 */
//=============================================================================
#ifndef __GFL2_FADE_MANAGER_H__
#define __GFL2_FADE_MANAGER_H__
#pragma once



#include "gfl2_FadeTypes.h"

#include <base/include/gfl2_Singleton.h>

#include <math/include/gfl2_math.h>

#include "macro/include/gfl2_Macros.h"
#include <heap/include/gfl2_Heap.h>
#include <gfx/include/gfl2_types.h>
#include <gfx/include/gfl2_GLMemory.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(gfx)
class Texture;
GFL_NAMESPACE_END(gfx)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(renderer)
class RenderingPipeLine;
GFL_NAMESPACE_END(renderer)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_BEGIN(Fade)

class FadeSuper;

/**
 * @brief フェードクラス
 */
class FadeManager : public base::Singleton<FadeManager> {
  GFL_FORBID_COPY_AND_ASSIGN(FadeManager);
public:

  /**
   *  @brief  画面キャプチャ用定義
   */
  enum
  {
    COLOR_BUFFER_UPPER,   //!< 上画面
    COLOR_BUFFER_LOWER,   //!< 下画面
    COLOR_BUFFER_MAXI,    //!< 最大値
  };

  /**
   *	@brief  フェードマネージャー
   */
  FadeManager(void);
  /**
   *	@brief  デストラクタ
   */
  ~FadeManager(void);
  /**
   *  @brief  PipeLine生成
   */
  void createPipeLine( gfl2::heap::HeapBase *pHeap, gfl2::gfx::IGLAllocator *pAllocator, void *gfbmdlBinary, void *vertexShaderBinary, void *fragmentShaderBinary );
  /**
   *	@brief  主処理
   */
  void Update(void);
  /**
   *	@brief  描画処理(下準備)
   */
  void PreDraw(void);
  /**
   *	@brief  描画処理
   */
  void Draw(gfl2::gfx::CtrDisplayNo no);

  /**
   *  @brief  フェードアウト リクエスト
   *  @param  disp        対象
   *  @param  type        種類
   *  @param  start_col   開始色
   *  @param  end_col     終了色
   *  @param  sync        時間
   *  @param  capture     開始時スクリーンキャプチャするか
   */
  void RequestOut( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4 *start_col, const gfl2::math::Vector4 *end_col, u32 sync = FADE_DEFAULT_SYNC, bool capture = false );

  /**
   *  @brief  フェードイン リクエスト
   *  @param  disp  対象
   *  @param  type  種類
   *  @param  sync        時間
   */
  void RequestIn( gfl2::Fade::DISP disp, gfl2::Fade::FADE_TYPE type, u32 sync = FADE_DEFAULT_SYNC );

  /**
   *	@brief  強制的にフェードアウト状態にする
   *  @param  bDefault   デフォルトを使うのならtrue、AddFadeで指定されたフェードを使うならfalse
   *	@param	DISP disp  対象面
   *	@param	end_col    RGBA (0-255.0f)
   */
  void ForceOut(gfl2::Fade::DISP disp, const gfl2::math::Vector4* col);

  /**
   *	@brief  強制的にフェードアウト状態にする(対象面の色が黒固定)
   *	@param	DISP disp  対象面
   */
  void ForceDefaultOut(gfl2::Fade::DISP disp);

  /**
   *	@brief  画面遷移 終了検知 対称面指定版
   *	@param	DISP disp  対象面
   *	@retval TRUE：終了している 登録が無い / FALSE：終了していない
   */
  bool IsEnd(gfl2::Fade::DISP disp);


  //フェード機能が活動しているかどうか、してない場合は true
//  bool IsEndStatus(void);
  bool IsEndStatus(gfl2::Fade::DISP disp = DISP_DOUBLE);

  
  /**
   *	@brief  ポーズON/OFF
   *  @param  DISP disp  対象面
   *	@param	is_pause ポーズフラグ
   */
  void SetPause(gfl2::Fade::DISP disp,bool is_pause);
  /**
   *	@brief  ポーズ状態取得
   *	@retval ポーズ状態
   */
  bool IsPause(gfl2::Fade::DISP disp) const;
  /**
   *	@brief  フェード状態リセット 全部の稼働中のフェード処理のリセット
   */
  void Reset(gfl2::Fade::DISP disp);

  /**
   *  @brief  フェード再生速度をコントロールする
   *
   *  @note   レイアウトアニメーション用に実装しているので
   *          FADE_TYPE_ALPHA,FADE_TYPE_CROSSには非対応
   *  @note   RequestOut,RequestIn関数内部にてリセットがかかるので
   *          Request***の後に呼び出すこと
   */
  void SetAnimeSpeed(gfl2::Fade::DISP disp, f32 speed);

  /**
   *	@brief  フェード登録
   *	@param	DISP disp  対象面
   *	@param	フェードのインターフェイスを継承したクラス
   */
  void AddFade(gfl2::Fade::DISP disp,FadeSuper* pFade);
  /**
   *	@brief  フェード登録解除（クラス解放は行いません）
   *	@param	DISP disp  対象面
   */
  void RemoveFade(gfl2::Fade::DISP disp);

  /**
   *  @brief  フェード取得
   */
  FadeSuper *GetFade(gfl2::Fade::DISP disp);

  /**
   *	@brief  デフォルトフェードアウト リクエスト  色は黒、フレーム固定
   *	@param	DISP disp  対象面
   */
  void RequestDefaultOut(gfl2::Fade::DISP disp);

  /**
   *  @brief  カラーバッファ作成
   */
  void CreateColorBuffer( gfl2::gfx::IGLAllocator *pAllocator, gfl2::gfx::Pool poolUpper, gfl2::gfx::Pool poolLower );

  /**
   *  @brief  カラーバッファ破棄
   */
  void DeleteColorBuffer( void );

  /**
   *  @brief  カラーバッファ取得 リクエスト
   *  @param  target    取得対象
   */
  void RequestTransferColorBuffer( u32 target );

  /**
   *  @brief  カラーバッファ取得
   */
  void TransferColorBuffer( gfl2::gfx::CtrDisplayNo no );

  /**
   *  @brief  テクスチャ取得
   */
  gfl2::gfx::Texture *GetColorBufferTexture( u32 target );

  /**
   *  @brief  クロスフェード用パイプラインの取得
   */
  gfl2::renderingengine::renderer::RenderingPipeLine *GetCrossFadePipeLine( void ){ return m_pPipeLine; }

  /**
   *  @brief  HomeNixリクエスト
   */
  void RequestHomeNix( void );
  /**
   *  @brief  HomeNix確認
   */
  bool IsHomeNixRunning( void );

private:

  FadeSuper* m_pFade[DISP_BUFF_MAX];  // 登録フェードはデフォルトとそうでないかの２種類２画面

  gfl2::gfx::Texture  *m_pColorBuffer[COLOR_BUFFER_MAXI];
  bool                m_bTransColorBuffer[COLOR_BUFFER_MAXI];

  gfl2::renderingengine::renderer::RenderingPipeLine *m_pPipeLine;
};


GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl2::Fade::FadeManager );


#endif // __GFL2_FADE_MANAGER_H__
