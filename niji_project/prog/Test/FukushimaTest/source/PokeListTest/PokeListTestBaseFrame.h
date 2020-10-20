#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_POKELIST_BASE_FRAME_H__
#define __TEST_FUKUSHIMA_TEST_POKELIST_BASE_FRAME_H__
#pragma once
//==============================================================================
/**
 * @file    PokeListTestBaseFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベースフレーム
 */
//==============================================================================

// niji
#include <App/PokeList/source/PokeListDrawListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class StaticWork;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   ポケモンリストテストフレーム
//------------------------------------------------------------------------------
class TestPokeListBaseFrame
  : public applib::frame::CellSuper
  , public App::PokeList::PokeListDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( TestPokeListBaseFrame );
  
public:
  enum{
    MM_SEQ_NONE,
    MM_SEQ_END
  };

  enum{
    LOCAL_OPEN_START,
    LOCAL_READ_START,
    LOCAL_READ_WAIT,
    LOCAL_CLOSE_WAIT
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  TestPokeListBaseFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~TestPokeListBaseFrame();
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );
  
public:
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( App::PokeList::APP_PARAM* pAppParam, App::PokeList::CONTEXT_PARAM* pContext );

  //------------------------------------------------------------------
  /**
   * @brief   PokeListのセットアップ
   */
  //------------------------------------------------------------------
  virtual void SetupPokeList( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの生成
   */
  //------------------------------------------------------------------
  virtual void CreatePokeList( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの破棄
   */
  //------------------------------------------------------------------
  virtual bool EndPokeList( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの更新
   */
  //------------------------------------------------------------------
  virtual void UpdatePokeList( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   PokeListDrawの描画
   */
  //------------------------------------------------------------------
  virtual void DrawPokeList( gfl2::gfx::CtrDisplayNo no ) = 0;


public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの終了
   */
  //------------------------------------------------------------------
  virtual void PokeListEnd( void ) = 0;

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータの設定
   * @param   endMode   終了モード
   */
  //------------------------------------------------------------------
  virtual void SetupOutData( App::PokeList::EndMode endMode ) = 0;
  
protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ
  
protected:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  App::PokeList::APP_PARAM*      m_pAppParam;
  App::PokeList::CONTEXT_PARAM*  m_pContext;

  App::PokeList::StaticWork*     m_pStaticWork;
  
  void*   m_pArcReadBuff;
  int     m_ReadSeq;
};


GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif  // __TEST_FUKUSHIMA_TEST_POKELIST_FRAME_H__

#endif // PM_DEBUG