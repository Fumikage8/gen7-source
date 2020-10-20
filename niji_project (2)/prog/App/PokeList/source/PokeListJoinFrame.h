#ifndef NIJI_PROJECT_APP_POKELIST_JOIN_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_JOIN_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListJoinFrame.h
 * @author  fukushima_yuya
 * @date    2015.03.25
 * @brief   ポケモンリスト画面参加選択フレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseFrame.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListJoinDraw;
class PokeListJoinUpperDraw;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト画面参加選択フレーム
//------------------------------------------------------------------------------
class PokeListJoinFrame
  : public PokeListBaseFrame
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListJoinFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListJoinFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListJoinFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PokeListのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの生成
   */
  //------------------------------------------------------------------
  virtual void CreatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの破棄
   */
  //------------------------------------------------------------------
  virtual bool EndPokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの更新
   */
  //------------------------------------------------------------------
  virtual void UpdatePokeList( void );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの描画
   */
  //------------------------------------------------------------------
  virtual void DrawPokeList( gfl2::gfx::CtrDisplayNo no );


public: // PokeListDrawListener
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの終了
   */
  //------------------------------------------------------------------
  virtual void PokeListEnd( void );

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetupOutData( EndMode endMode );


private:
  ////------------------------------------------------------------------
  ///**
  // * @brief   プレートメッセージウィンドウの更新
  // * @param   pListDraw   ListDrawクラスポインタ
  // * @param   cpos        カーソル位置
  // */
  ////------------------------------------------------------------------
  //void UpdatePlateMessageWindow( ListDraw* pListDraw, CursorPos cpos );

  ////------------------------------------------------------------------
  ///**
  // * @brief   参加条件を満たしていないときのNGメッセージの表示
  // * @param   pWindow   ウィンドウ
  // * @param   join_min  最小参加数
  // */
  ////------------------------------------------------------------------
  //void OpenJoinFailedMessage( MenuWindowDraw* pWindow, u8 join_min );

  ////------------------------------------------------------------------
  ///**
  // * @brief   上限以上の参加をしようとしたときのNGメッセージの表示
  // * @param   pWindow   ウィンドウ
  // * @param   join_max  最大参加数
  // */
  ////------------------------------------------------------------------
  //void OpenJoinOverMessage( MenuWindowDraw* pWindow, u8 join_max );

  ////------------------------------------------------------------------
  ///**
  // * @brief   サブウィンドウを閉じる
  // * @param   pWindow   ウィンドウ
  // */
  ////------------------------------------------------------------------
  //void CloseSubWindow( MenuWindowDraw* pWindow );

  //------------------------------------------------------------------
  /**
   * @brief   制限時間切れの参加ポケモンのセット
   */
  //------------------------------------------------------------------
  void SetJoinOrderTimeOut( void );


private:
  PokeListJoinDraw*         m_pPokeListLowerDraw;

  PokeListJoinUpperDraw*    m_pPokeListUpperDraw;

  u32   m_Seq;

  u32   m_FrameCount;

  bool  m_ForceOut;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_JOIN_FRAME_H_INCLUDED
