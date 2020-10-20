//======================================================================
/**
 * @file LayoutFade.h
 * @date 2015/07/24 11:21:07
 * @author miyachi_soichi
 * @brief フェード：レイアウト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __LAYOUTFADE_H_INCLUDED__
#define __LAYOUTFADE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeSuper.h>
#include <math/include/gfl2_Vector4.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class AsyncFileManager;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class Heap;
class AppRenderingManager;
class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)

/**
  * @class LayoutFade
  * @brief フェード：レイアウト
  */
class LayoutFade : public gfl2::Fade::FadeSuper
{
  // コピー禁止にする
  GFL_FORBID_COPY_AND_ASSIGN(LayoutFade);

  /**
   *  @brief  状態定義
   */
  enum
  {
    STATE_NONE,     //!< 未初期化
    STATE_WAIT,     //!< 待機
    STATE_OUT,      //!< フェードアウトちゅう
    STATE_IN,       //!< フェードイン中
  };

  /**
   *  @brief  HomeNix定義
   */
  enum
  {
    HOME_NIX_FADE_IN = 5,
    HOME_NIX_FADE_DISP = 50,
    HOME_NIX_FADE_OUT = 10,
    HOME_NIX_FADE_COUNT = HOME_NIX_FADE_IN + HOME_NIX_FADE_DISP + HOME_NIX_FADE_OUT
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  LayoutFade( void );

  /**
   *  @brief  デストラクタ
   */
  ~LayoutFade( void );

  /**
   *  @brief  初期化
   *  @param  pAppHeap        ヒープアロケータ
   *  @param  pRenderManager  レンダーマネージャ
   *  @param  pAsyncManager   ファイルマネージャ
   */
  void Initialize( app::util::Heap *pAppHeap, app::util::AppRenderingManager *pRenderManager, gfl2::fs::AsyncFileManager *pAsyncManager );

  /**
   *  @brief  破棄
   */
  void Terminate( void );

  /**
   *  @brief  更新処理
   */
  virtual void UpdateFunc( void );

  /**
   *  @brief  描画処理
   */
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo displayNo );
  
  /**
   *  @brief  フェードアウト リクエスト
   *  @param  type      演出の種類
   *  @param  start_col スタート時のRGBA (0-1.0f) NULL だと前回の色からスタート
   *  @param  end_col   終了時のRGBA (0-1.0f)
   *  @param  sync      フェードにかけるSYNC
   *  @param  capture   開始時のバッファを保持するか？
   */
  virtual void RequestOut( gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4* start_col, const gfl2::math::Vector4* end_col, u32 sync, bool capture );

  /**
   *  @brief  フェードイン リクエスト
   *  @param  type      演出の種類
   *  @param  sync      フェードにかけるSYNC
   */
  virtual void RequestIn( gfl2::Fade::FADE_TYPE type, u32 sync );

  /**
   *  @brief  強制的にフェードアウト状態にする
   *  @param  DISP disp  対象面
   *  @param  end_col    RGBA (0-1.0f)
   */
  virtual void ForceOut(const gfl2::math::Vector4* col);

  /**
   *  @brief  画面遷移 終了検知
   *  @retval TRUE：終了している / FALSE：終了していない
   */
  virtual bool IsEnd(void);

  /**
   *  @brief  画面遷移ステータスが終了しているかどうか
   *  @retval TRUE：終了している / FALSE：終了していない
   */
  virtual bool IsEndStatus(void);

  /**
   *  @brief  ポーズON/OFF
   *  @param  DISP disp  対象面
   *  @param  is_pause ポーズフラグ
   */
  virtual void SetPause(bool is_pause);

  /**
   *  @brief  ポーズ状態取得
   *  @retval ポーズ状態
   */
  virtual bool IsPause(void) const;

  /**
   *  @brief  フェード状態リセット
   *  @param  DISP disp  対象面
   */
  virtual void Reset(void);

  /**
   *  @brief  フェード再生速度をコントロールする
   *
   *  @note   レイアウトアニメーション用に実装しているので
   *          FADE_TYPE_ALPHA,FADE_TYPE_CROSSには非対応
   *  @note   RequestOut,RequestIn関数内部にてリセットがかかるので
   *          Request***の後に呼び出すこと
   */
  virtual void SetAnimeSpeed( f32 speed );

  /**
  *	@brief  フェード完了後の状態を取得
  */
  virtual gfl2::Fade::FADE_RESULT GetFadeResult(void) const;

  /**
  *	@brief  フェードのカラーを取得
  * @note   一度もフェードアウトリクエストが実行されていない場合は無効
  */
  virtual void GetColor( gfl2::math::Vector4* start_col, gfl2::math::Vector4* end_col ) const;

  /**
   *  @brief  リソースをクローンしてレイアウト作成する
   *  @param  pParent   リソースのクローン元
   */
  void CloneLayout( LayoutFade *pParent );

private:
  /**
   *  @brief  ブロックロード
   */
  void fileLoadBlock( void );

  /**
   *  @brief  レイアウト作成
   *  @param  bClone  クローンか＝下画面か
   */
  void createLayout( bool bClone = false );

  /**
   *  @brief  更新処理：レイアウトアニメーション
   */
  bool updateLayoutMode( void );

  /**
   *  @brief  更新処理：カラーアニメーション
   */
  bool updateColorMode( void );

  /**
   *  @brief  更新処理：クロスフェード
   */
  bool updateCrossMode( void );

  /**
   *  @brief  画面スナップショットをマウントする
   */
  void mountSnapshot( void );

  /**
   *  @brief  HomeNix更新
   */
  bool UpdateHomeNix( void );

  /**
   *  @brief  HomeNixリクエスト
   */
  virtual void RequestHomeNix( void );

  /**
   *  @brief  HomeNix確認
   */
  virtual bool IsHomeNixRunning( void );

private:
  gfl2::fs::AsyncFileManager      *m_pAsyncFileManager;   //!< ファイルマネージャ
  app::util::Heap                 *m_pAppHeap;            //!< ヒープアロケータ
  app::util::AppRenderingManager  *m_pRenderManager;      //!< レンダーマネージャ
  app::util::G2DUtil              *m_pG2DUtil;            //!< レイアウト
  void                            *m_pLayoutData;         //!< リソース

  u32                             m_eMode;                //!< フェード種別
  u32                             m_eState;               //!< 状態
  s32                             m_nOutAnim;             //!< Outに使用するアニメーション
  s32                             m_nOutPane;             //!< Outに使用するペイン
  s32                             m_nInAnim;              //!< Inに使用するアニメーション
  s32                             m_nInPane;              //!< Inに使用するペイン
  s32                             m_nFadeLifeNow;         //!< 寿命：現在値
  s32                             m_nFadeLifeMax;         //!< 寿命：最大値

  gfl2::math::Vector4             m_ColorNow;
  gfl2::math::Vector4             m_ColorAdd;
  gfl2::math::Vector4             m_ColorEnd;

  bool                            m_bPause;               //!< フラグ：一時停止
  bool                            m_bClone;               //!< フラグ：下画面

  u32                             m_homeNixFade;          //!<  HomeNixカウント
  bool                            m_isHomeNixRequest;     //!<  HomeNixリクエスト
  u64                             m_homeNixStartTime;     //!<  HomeNix開始時間


  gfl2::Fade::FADE_RESULT         m_FadeResult;           //!< フェード遷移後の状態
};

GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)

#endif // __LAYOUTFADE_H_INCLUDED__
