#ifndef NIJI_PROJECT_APP_POKELIST_SIMPLE_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_SIMPLE_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListSimpleFrame.h
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面対象選択フレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListBaseFrame.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListSimpleDraw;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリスト画面対象選択フレーム
//------------------------------------------------------------------------------
class PokeListSimpleFrame
  : public PokeListBaseFrame
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListSimpleFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListSimpleFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListSimpleFrame( void );

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

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポーズ中かどうか
   *
   * @return  "true  = ポーズ中"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsPause( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   外部からの強制終了のチェック
   */
  //------------------------------------------------------------------
  void CheckListBreak( void );

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

  //------------------------------------------------------------------
  /**
   * @brief   描画許可フラグの有効化
   */
  //------------------------------------------------------------------
  virtual void SetEnableFlag_Draw( void ) { m_DrawEnable = true; }

  //------------------------------------------------------------------
  /**
   * @brief   描画許可フラグの取得
   *
   * @return  "true  == 有効"
   * @return  "false == 無効"
   */
  //------------------------------------------------------------------
  virtual bool GetEnableFlag_Draw( void ) { return m_DrawEnable; }

private:
  PokeListSimpleDraw*   m_pPokeListDraw;

private:
  bool    m_Closed;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_SIMPLE_FRAME_H_INCLUDED
