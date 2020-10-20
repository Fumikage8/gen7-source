//=============================================================================
/**
 *
 *	@file		gfl2_FadeSuper.h
 *	@brief  このクラスを継承してフェードを作成する
 *	@author k.ohno
 *	@data		2015.03.31
 *
 */
//=============================================================================
#ifndef __GFL2_FADE_SUPER_H__
#define __GFL2_FADE_SUPER_H__
#pragma once


#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <Layout/include/gfl2_GraphicsType.h>
#include <gfx/include/gfl2_types.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)


class FadeSuper {
  GFL_FORBID_COPY_AND_ASSIGN(FadeSuper);

public:
  FadeSuper(void){};
  ~FadeSuper(){};

  
  /**
   *	@brief  主処理
   */
  virtual void UpdateFunc(void) = 0;

  /**
   *	@brief  描画処理(下準備)
   */
  virtual void PreDrawFunc(void) {}

  /**
   *	@brief  描画処理
   */
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no) = 0;

  /**
   *  @brief  フェードアウト リクエスト
   *  @param  type            対象面
   *  @param  start_col       スタート時のRGBA (0-255.0f) 
   *  @param  end_col         終了時のRGBA (0-255.0f)
   *  @param  sync            フェードにかけるSYNC
   *  @param  is_keep_buffer  開始時のバッファを保持するか？
   */
  virtual void RequestOut( gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4* start_col, const gfl2::math::Vector4* end_col, u32 sync, bool is_keep_buffer) = 0;

  /**
   *  @brief  フェードイン リクエスト
   *  @param  type   演出の種類
   *  @param  sync   フェードにかけるSYNC
   */
  virtual void RequestIn( gfl2::Fade::FADE_TYPE type, u32 sync ) = 0;

  /**
   *	@brief  強制的にフェードアウト状態にする
   *	@param	DISP disp  対象面
   *	@param	end_col    RGBA (0-1.0f)
   */
  virtual void ForceOut(const gfl2::math::Vector4* col) = 0;

  /**
   *	@brief  画面遷移 終了検知
   *	@retval TRUE：終了している / FALSE：終了していない
   */
  virtual bool IsEnd(void) = 0;

  /**
   *	@brief  画面遷移ステータスが終了しているかどうか
   *	@retval TRUE：終了している / FALSE：終了していない
   */
  virtual bool IsEndStatus(void) = 0;

  /**
   *	@brief  ポーズON/OFF
   *  @param  DISP disp  対象面
   *	@param	is_pause ポーズフラグ
   */
  virtual void SetPause(bool is_pause) = 0;

  /**
   *	@brief  ポーズ状態取得
   *	@retval ポーズ状態
   */
  virtual bool IsPause(void) const = 0;

  /**
   *	@brief  フェード状態リセット
   *	@param	DISP disp  対象面
   */
  virtual void Reset(void) = 0;

  /**
   *  @brief  フェード再生速度をコントロールする
   *
   *  @note   レイアウトアニメーション用に実装しているので
   *          FADE_TYPE_ALPHA,FADE_TYPE_CROSSには非対応
   */
  virtual void SetAnimeSpeed( f32 speed ){}

  /**
   *  @brief  HomeNixリクエスト
   */
  virtual void RequestHomeNix( void ){};
  /**
   *  @brief  HomeNix確認
   */
  virtual bool IsHomeNixRunning( void ){ return false; };


  /**
  *	@brief  フェード完了後の状態を取得
  */
  virtual FADE_RESULT GetFadeResult(void) const { return gfl2::Fade::FADE_RESULT_IN; }

  /**
  *	@brief  フェードのカラーを取得
  * @note   一度もフェードアウトリクエストが実行されていない場合は無効
  */
  virtual void GetColor( gfl2::math::Vector4* start_col, gfl2::math::Vector4* end_col ) const {}

};


GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)

#endif // __GFL_FADE_SUPER_H__
