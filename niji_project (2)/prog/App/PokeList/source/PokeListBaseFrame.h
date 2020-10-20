#ifndef NIJI_PROJECT_APP_POKELIST_BASE_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_BASE_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListBaseFrame.h
 * @author  fukushima_yuya
 * @date    2015.06.22
 * @brief   ポケモンリストベースフレーム
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListDrawListener.h"
#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include <App/Bag/include/BagAppParam.h>
#include <App/Status/include/StatusAppParam.h>

// gflib2
#include <macro/include/gfl2_Macros.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class StaticWork;
class LocalWork;
class TimeLimit;
class PokeListBaseDraw;
class PokeListUpperDraw;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_BEGIN( Tool )
class TimeLimit;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )

class WordSetLoader;


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   ポケモンリストベースフレーム
//------------------------------------------------------------------------------
class PokeListBaseFrame
  : public applib::frame::CellSuper
  , public PokeListDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListBaseFrame );
  
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
  PokeListBaseFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListBaseFrame();
  
private:
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

  //------------------------------------------------------------------
  /**
   * @brief	   描画できる状態になった事を返すための関数
   * @return	 描画できる状態になった場合trueを返す
   */
  //------------------------------------------------------------------
  virtual bool IsDrawEnable(void) { return m_DrawEnable; }
  
private:
  //------------------------------------------------------------------
  /**
   * @brief	  レイアウトデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadLayoutData( void );

  //------------------------------------------------------------------
  /**
   * @brief	  メッセージデータの読み込み
   */
  //------------------------------------------------------------------
  bool LoadMsgData( void );
  
  //------------------------------------------------------------------
  /**
   * @brief	  ワードセットローダーの生成
   */
  //------------------------------------------------------------------
  bool CreateWordSetLoader( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pAppParam, CONTEXT_PARAM* pContext );

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータのセット
   * @param   pStatusParam    ステータス用外部設定パラメータのポインタ
   * @param   pBagParam       バッグ用外部設定パラメータのポインタ
   */
  //------------------------------------------------------------------
  void SetOutParam( Status::APP_PARAM* pStatusParam, app::bag::APP_PARAM* pBagParam );

  //------------------------------------------------------------------
  /**
   * @brief	  DRAWのセット
   * @param   pBaseDraw   ベースDRAW
   */
  //------------------------------------------------------------------
  void SetBaseDraw( PokeListBaseDraw* pBaseDraw );

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

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリストの再起動
   * @note    タッグ戦のみ使用
   */
  //------------------------------------------------------------------
  void RebootPokeList( App::PokeList::CONTEXT_PARAM& rContextParam );


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
  virtual void SetupOutData( EndMode endMode ) = 0;
  
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンステータスの呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //------------------------------------------------------------------
  virtual void CallPokeStatus( u8 pokeIndex );

  //------------------------------------------------------------------
  /**
   * @brief   バッグの呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //------------------------------------------------------------------
  virtual void CallBag( u8 pokeIndex );

  //------------------------------------------------------------------
  /**
   * @brief   バッグ(かいふく)の呼び出し
   * @param   pokeIndex   ポケモンのインデックス
   */
  //------------------------------------------------------------------
  virtual void CallBagRepair( u8 pokeIndex );

  //------------------------------------------------------------------
  /**
   * @brief   参加選択で自分の選択を終えた
   */
  //------------------------------------------------------------------
  virtual void FinishJoinSelect( void );
  
protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ
  
protected:
  app::util::Heap*                  m_pAppHeap;
  
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  APP_PARAM*      m_pAppParam;

  CONTEXT_PARAM*  m_pContext;

  Status::APP_PARAM*        m_pStatusAppParam;    //!< ステータス用外部設定パラメータ

  app::bag::APP_PARAM*      m_pBagAppParam;       //!< バッグ用外部設定パラメータ

  App::Tool::TimeLimit*     m_pTimeLimit;
  
  StaticWork*               m_pStaticWork;

  LocalWork*                m_pLocalWork;

  PokeListBaseDraw*         m_pBaseDraw;

  PokeListUpperDraw*        m_pUpperDraw;

  gfl2::str::MsgData*       m_pMsgData;

  print::WordSet*           m_pWordSet;

  WordSetLoader*            m_pWordSetLoader;

  void*   m_pArcReadBuff;
  void*   m_pMsgReadBuff;
  
  u8      m_ReadSeq;
  u8      m_InitSeq;

  bool    m_DrawEnable;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // NIJI_PROJECT_APP_POKELIST_BASE_FRAME_H_INCLUDED
