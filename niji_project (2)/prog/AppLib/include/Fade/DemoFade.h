//======================================================================
/**
 * @file DemoFade.h
 * @date 2015/08/19 12:19:28
 * @author miyachi_soichi
 * @brief 戦闘開始演出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __DEMOFADE_H_INCLUDED__
#define __DEMOFADE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_heap.h>
#include <Fade/include/gfl2_FadeTypes.h>
#include <fade/include/gfl2_FadeSuper.h>
#include <RenderingEngine/include/SceneGraph/gfl2_DagNode.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Sequence)
class SequenceViewSystem;
GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

GFL_NAMESPACE_BEGIN(AppLib)
GFL_NAMESPACE_BEGIN(Fade)

/**
 * @class DemoFade
 * @brief 戦闘導入演出
 */
class DemoFade : public gfl2::Fade::FadeSuper
{
  GFL_FORBID_COPY_AND_ASSIGN(DemoFade);

public:
  /**
   *  @enum  状態
   */
  enum State
  {
    STATE_NONE,   // 未遷移
    STATE_create,
    STATE_first,
    STATE_running,
    STATE_finish,
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  DemoFade( void );
  DemoFade( u32 dummy ){}

  /**
   *  @brief  デストラクタ
   */
  ~DemoFade( void );

  /**
   *  @brief  初期化
   *  @param  pHeap           ヒープ
   *  @param  nSysHeapSize    システムサイズ
   *  @param  nDevHeapSize    デバイスサイズ
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, s32 nSysHeapSize, s32 nDevHeapSize );

  /**
   *  @brief  解放
   */
  void Terminate( void );

  /**
   *  @brief  更新処理
   */
  virtual void UpdateFunc( void );

  /**
   *  @brief  描画準備
   */
  virtual void PreDrawFunc( void );

  /**
   *  @brief  描画処理
   */
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo displayNo );

  /**
   *  @brief  フェードアウト リクエスト
   *    無効化
   */
  virtual void RequestOut( gfl2::Fade::FADE_TYPE type, const gfl2::math::Vector4* start_col, const gfl2::math::Vector4* end_col, u32 sync, bool capture ){}

  /**
   *  @brief  フェードイン  リクエスト
   *    無効化
   */
  virtual void RequestIn( gfl2::Fade::FADE_TYPE type, u32 sync ){}

  /**
   *  @brief  強制的にフェードアウト状態にする
   *    無効化
   */
  virtual void ForceOut( const gfl2::math::Vector4 *col ){}

  /**
   *  @brief  画面遷移終了検知
   */
  virtual bool IsEnd( void );

  /**
   *  @brief  画面遷移ステータスが終了しているかどうか
   */
  virtual bool IsEndStatus( void );

  /**
   *  @brief  ポーズON/OFF
   */
  virtual void SetPause( bool is_pause );

  /**
   *  @brief  ポーズ状態取得
   */
  virtual bool IsPause( void ) const;

  /**
   *  @brief  フェード状態リセット
   *    無効化
   */
  virtual void Reset( void ){}

  /**
   *  @brief  デモ開始リクエスト
   */
  void StartDemo( u32 demoID ){ m_nDemoID = demoID; m_eState = STATE_create; }

  /**
   *  @brief  シーケンス取得
   */
  DemoLib::Sequence::SequenceViewSystem *GetSequence( void ){ return m_pSequenceViewSystem; }

  /**
   *
   */
  void SetReplaceTexture( s32 index, gfl2::renderingengine::scenegraph::DagNode *pTexture );

  /**
   *  @brief  状態取得
   */
  State GetState( void ) const { return m_eState; }

  /**
   * @brief シーケンスプレイヤーの初期化完了後にシーケンス再生を待機させるモードの設定
   * @param trueにするとシーケンスプレイヤー初期化完了後にシーケンスが再生されなくなる
   * @note デフォルトはfalseです。
   * @note trueに設定した場合、明示的にfalseを設定しないとシーケンスが再生されません
   */
  void SetSequenceStartupWaitMode( bool isEnable);

private:
  DemoLib::Sequence::SequenceViewSystem   *m_pSequenceViewSystem;
  gfl2::heap::HeapBase                    *m_pHeap;
  State                                   m_eState;
  s32                                     m_nSysHeapSize;
  s32                                     m_nDevHeapSize;
  u32                                     m_nDemoID;
  bool                                    m_bPause;

  s32                                         m_nIndex;
  gfl2::renderingengine::scenegraph::DagNode  *m_pTextureNode;

  bool                                        m_isSequenceStartupWaitMode;
};

GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(AppLib)

#endif // __DEMOFADE_H_INCLUDED__
