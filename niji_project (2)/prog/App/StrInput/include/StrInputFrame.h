#ifndef NIJI_PROJECT_APP_STRINPUT_FRAME_H_INCLUDED
#define NIJI_PROJECT_APP_STRINPUT_FRAME_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    StrInputFrame.h
 * @brief   Frame関連
 * @author  fukushima_yuya
 * @date    2015.04.30
 * @note    sango_project app strinput.hから移植
 */
//==============================================================================

// niji
#include "App/StrInput/include/StrInputAppParam.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <GameSys/include/GameFrameMode.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )
class MainSystem;    // メインシステム
GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( ngc )
class NGWordChecker;
GFL_NAMESPACE_END( ngc )
GFL_NAMESPACE_END( gfl2 )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( StrInput )

//! 入力タイプ
enum {
  TYPE_CHARACTER_NAME = 0,    //!< 00: 人物名（主人公）
  TYPE_POKEMON_NAME,          //!< 01: ポケモン名
  TYPE_BOX_NAME,              //!< 02: ボックス名
  TYPE_FREE_WORD_PSS,         //!< 03: 一言メッセージ ( PSS )
  TYPE_FREE_WORD_GTS,         //!< 04: 一言メッセージ ( GTS )
  TYPE_NICKNAME,              //!< 05: ニックネーム
  TYPE_POKE_SEARCH,           //!< 06: GTSポケモン検索
  TYPE_POKE_WANT,             //!< 07: GTSポケモン検索（欲しいポケモン）
  TYPE_HAPPY_SIGN,            //!< 08: ハッピーサイン

  TYPE_INTERVIEW_LONG,        //!< 09: 一言メッセージ ( インタビュー: 16文字 )
  TYPE_INTERVIEW_SHORT,       //!< 10: 一言メッセージ ( インタビュー: 8文字 )
  TYPE_INTERVIEW_VERY_SHORT,  //!< 11: 一言メッセージ ( インタビュー: 6文字 )

  TYPE_BASE_WORD,             //!< 12: 秘密基地：団名と合言葉
  TYPE_BASE_HAPPY,            //!< 13: 秘密基地：嬉しいときの言葉
  TYPE_BASE_CHEER,            //!< 14: 秘密基地：励ますときの言葉
  TYPE_BASE_GREETING,         //!< 15: 秘密基地：挨拶
  TYPE_BASE_GOODS,            //!< 16: 秘密基地：グッズ

  TYPE_TEAM_NAME,             //!< 17: チーム名 NIJI新規追加
  TYPE_FESCIRCLE_NAME,        //!< 18: フェスサークル名 NIJI新規追加

#if PM_DEBUG
  TYPE_CHARACTER_NAME_DEBUG   //!< かわいがりデバッグ用
#endif
};

//------------------------------------------------------------------------------
/**
 * @brief  Frame
 */
//------------------------------------------------------------------------------
class StrInputFrame
  : public applib::frame::CellSuper    // 基低クラス[applib::frame::CellSuper]を継承
{
public:
  //----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //----------------------------------------------------------------------------
  StrInputFrame( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //----------------------------------------------------------------------------
  virtual ~StrInputFrame( void );

public:
  //----------------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータのセット
   *
   * @param   prm    外部設定パラメータ
   */
  //----------------------------------------------------------------------------
  void Setup( APP_PARAM* prm );

public:
  //----------------------------------------------------------------------------
  /**
   * @brief   Proc初期化処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //----------------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   Proc終了処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //----------------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   Procアップデート処理
   *
   * @param   proc_manager    gfl2::proc::Manager
   *
   * @return  gfl2::proc::Result
   */
  //----------------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   Proc描画処理
   *
   * @param   no    ディスプレイNo
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //----------------------------------------------------------------------------
  /**
   * @brief   FrameModeの取得
   *
   * @param   none
   *
   * @return  FrameMode
   *
   * @note    フレームモードを切り替えるときは、戻り値を変えてください。
   */
  //----------------------------------------------------------------------------
  GameSys::FrameMode GetFrameMode( void ) const;


private:
  //----------------------------------------------------------------------------
  /**
   * @brief   システムヒープ作成
   *
   * @param   prm   外部設定パラメータ
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void CreateHeap( APP_PARAM * prm );

  //----------------------------------------------------------------------------
  /**
   * @brief   システムヒープ削除
   *
   * @param   none
   *
   * @return  none
   */
  //----------------------------------------------------------------------------
  void DeleteHeap( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   開始時間設定
   *
   * @param   none
   *
   * @return  none
   *
   *  ポケモン名入力用
   */
  //----------------------------------------------------------------------------
  void StartTime( void );

  //----------------------------------------------------------------------------
  /**
   * @brief   終了時間設定
   *
   * @param   none
   *
   * @return  none
   *
   *  ポケモン名入力用
   */
  //----------------------------------------------------------------------------
  void EndTime( void );


private:
  gfl2::heap::HeapBase * m_pHeap;  //!< システムヒープ
  MainSystem * m_pMain;            //!< メインシステム
  APP_PARAM * m_pParam;            //!< 外部設定パラメータ

#ifdef GF_PLATFORM_CTR
  nn::os::Tick m_startTick;        //!< 開始時のチック（ポケモン名入力時のTV番組作成用）
#endif

  gfl2::ngc::NGWordChecker * m_pNGWordChecker;
};


GFL_NAMESPACE_END( StrInput )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_STRINPUT_FRAME_H_INCLUDED
