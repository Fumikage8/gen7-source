//======================================================================
/**
 * @file EventDebugCharaViewer.h
 * @date 2016/03/16
 * @author saita_kazuki
 * @brief デバッグ：CharaViewer
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __EVENT_DEBUG_CHARAVIEWER_H_INCLUDED__
#define __EVENT_DEBUG_CHARAVIEWER_H_INCLUDED__
#pragma once

// gfl2
#include <heap/include/gfl2_heap.h>
#include <str/include/gfl2_Str.h>
// event
#include "GameSys/include/GameEvent.h"
// location
#include "Field/FieldStatic/include/FieldLocation.h"
// param
#include "App/StrInput/include/StrInputAppParam.h"

namespace test { namespace chara_viewer {
  class CharaViewerProc;
}}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
 *  @class  EventDebugCharaViewer
 *  @brief デバッグ：キャラクタ名入力
 */
class EventDebugCharaViewer : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventDebugCharaViewer);

public:

  /**
   *  @brief 設定
   */
  struct Description
  {
    bool      isFieldControl;       ///< フィールドマップのクローズ、オープンをEventで行うか

    Description()
      : isFieldControl( false)
    {}
  };

  /**
   * @brief   イベント起動
   * @param   gmgr ゲームマネージャクラス
   * @param   desc 設定
   * @return  EventDebugItemMakeクラス
   */
  static EventDebugCharaViewer* StartEvent( GameSys::GameManager *gmgr, const Description& desc );

public:

  /**
   *  @brief  コンストラクタ
   *  @param  pHeap   workをとるためのメモリクラス
   */
  EventDebugCharaViewer( gfl2::heap::HeapBase *pHeap );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EventDebugCharaViewer( void ){}

public:
  /**
   *  @brief  イベントを動かしてよいかどうかの判定関数
   *  @param  pGameManager  ゲームマネージャ
   *  @retval true  起動して良い
   *  @retval false 起動してはいけない
   */
  virtual bool BootChk( GameSys::GameManager *pGameManager );

  /**
   *  @brief  初期化関数
   *  @param  pGameManager  ゲームマネージャ
   */
  virtual void InitFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  メイン関数
   *  @param  pGameManager    ゲームマネージャ
   *  @return GMEVENT_RESUTLT イベント制御関数の戻り値
   */
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager *pGameManager );

  /**
   *  @brief  終了関数
   *  @param  pGameManager    ゲームマネージャ
   */
  virtual void EndFunc( GameSys::GameManager *pGameManager );

  /**
   * @brief   設定
   * @param   desc 設定
   */
  void SetDescription( const Description& desc );

private:

  Description                             m_desc;     ///< 設定
  test::chara_viewer::CharaViewerProc*    m_pProc;    ///< CharaViewerProc

}; // class EventDebugCharaViewer : public GameSys::GameEvent

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __EVENT_DEBUG_CHARAVIEWER_H_INCLUDED__

#endif // PM_DEBUG

