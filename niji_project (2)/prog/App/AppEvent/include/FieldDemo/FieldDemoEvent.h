//==============================================================================
/**
 * @file        FieldDemoEvent.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/06/15(月) 18:51:04
 */
//==============================================================================

#if !defined(__FIELDDEMOEVENT_H__)  // 重複定義防止
#define __FIELDDEMOEVENT_H__  // 重複定義防止マクロ
#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "heap/include/gfl2_Heap.h"
// niji
#include "GameSys/include/GameEvent.h"
#include "GameSys/include/GameManager.h"
#include "Field/FieldStatic/include/FieldLocation.h"

#include "niji_reference_files/script/FieldPawnTypes.h"

#include <model/include/gfl2_DressUpParam.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//=============================================================================
/**
 * @class FieldDemoEvent
 * @brief バッグ画面イベントクラス
 * @date  2015.03.04
 */
//=============================================================================
class FieldDemoEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldDemoEvent);   // コピーコンストラクタ＋代入禁止

private:
  enum
  {
    EVSEQ_PROC_FIELCLOSE = 0,
    EVSEQ_PROC_CREATE,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_FIELDOPEN,
    EVSEQ_PROC_END,
  };

public:
  struct FieldDemoEventDesk{
    s32         id;            ///<デモのID
    s32         demoType;      ///<デモのコールパラメータ
    bool        isFade;        ///<フェードをイベントに任せるか、マニュアルで操作するか
    bool        isFieldOpen;   ///<フィールドをオープンするかどうかのフラグ
    Field::Demo::DressUpSuitType suitType;      ///<着せ替えスーツタイプ

    FieldDemoEventDesk(){
      id = 0;
      demoType = 0;
      isFade = true;
      isFieldOpen = true;
      suitType = Field::Demo::SUIT_TYPE_NONE;
    };

  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  FieldDemoEvent( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~FieldDemoEvent();

  //-----------------------------------------------------------------------------
  /**
   * @func    StartEvent
   * @brief   イベント起動
   * @date    2015.06.11
   *
   * @param   man     ゲームマネージャクラス
   *
   * @return  FieldDemoEventクラス
   */
  //-----------------------------------------------------------------------------
  static FieldDemoEvent * StartEvent( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   外部設定パラメータをセット
   * @date    2015.06.11
   *
   * @param   param   外部設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetupParam( const s32 iId, const bool isFade = false, const s32 demoType = 0, const Field::Demo::DressUpSuitType suitType = Field::Demo::SUIT_TYPE_NONE);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetupParam
   * @brief   外部設定パラメータをセット
   * @date    2016.04.11
   *
   * @param   param   外部設定パラメータ
   */
  //-----------------------------------------------------------------------------
  void SetupParam( const FieldDemoEventDesk&  desk );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @retval  true  = 起動してよい
   * @retval  false = 起動しない
   */
  //-----------------------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
    virtual void InitFunc( GameSys::GameManager * gmgr);

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   *
   * @return  イベント制御関数の戻り値
   */
  //-----------------------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager * gmgr );

  //-----------------------------------------------------------------------------
  /**
   * @func    BookChk
   * @brief   呼び出される関数 純粋仮想関数
   * @date    2015.06.11
   *
   * @param   gmgr  ゲームマネージャークラス
   */
  //-----------------------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager * gmgr );


private:
  FieldDemoEventDesk m_desk;

  Field::Location m_playerLocation;

  poke_3d::model::DressUpParam m_dress_up_param;

};

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)

#endif // __FIELDDEMOEVENT_H__ 重複定義防止
