//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		GameClearSaveProc.h
 *	@brief  ゲームクリアー時のセーブ画面プロセスクラス
 *	@author	Toru=Nagihashi
 *	@date		2013.01.29
 *	@author koji_kawada@gamefreak.co.jp
 *	@date   2016.01.18
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_GAMECLEARSAVE_PROC_H__)
#define __APP_GAMECLEARSAVE_PROC_H__

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <Savedata/include/SaveLoadControl.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <GameSys/include/GameProc.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(game_clear_save)
  
class View;

struct APP_PARAM
{
  u32 dummy;
};


class Proc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(Proc);
public:
  //-----------------------------------------------------------------------
  // コンストラクタ・デストラクタ
  //-----------------------------------------------------------------------
  /**
   *	@brief  コンストラクタ
   *	@param  param 引数
   */
  Proc( APP_PARAM *param );
  /**
   *	@brief  デストラクタ
   */
  virtual ~Proc();

  //-----------------------------------------------------------------------
  // GameProcオーバーライト
  //-----------------------------------------------------------------------
  /**
   *	@brief  PROC初期化
   *	@param	Manager プロセスマネージャー
   *	@return Result 動作終了コード
   */
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager * );
  /**
   *	@brief  PROC破棄
   *	@param	Manager プロセスマネージャー
   *	@return Result 動作終了コード
   */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager * );
  /**
   *	@brief  PROCメイン
   *	@param	Manager プロセスマネージャー
   *	@return Result 動作終了コード
   */
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager * );

  virtual void PreDrawFunc(gfl2::proc::Manager * );
  
  /**
   *	@brief  PROC描画
   *	@param	Manager プロセスマネージャー
   *	@return Result 動作終了コード
   */
  virtual void DrawFunc(gfl2::proc::Manager * , gfl2::gfx::CtrDisplayNo displayNo);

private:
  static const int SYSTEM_HEAP_SIZE   = 0x300000;
  static const int DEVICE_HEAP_SIZE   = 0x800000;
 
  enum Sequence
  {
    SEQ_START,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_SUCCESS_MSG,
    SEQ_START_FAILURE_MSG,
    SEQ_WAIT_MSG,
    SEQ_END,
  };
  
private:
  bool UpdateSequence( void );
  
  void StartSave( void );
  enum SaveResult
  {
    SAVE_RESULT_PROCESSING, //更新中
    SAVE_RESULT_SUCCESS,    //セーブ完了
    SAVE_RESULT_FAILURE,    //セーブ失敗
  };
  SaveResult UpdateSave( void );
  bool IsLockSave( void );

private:
  app::util::Heap m_heap;
  View  *m_view;
  APP_PARAM *m_param;
  Sequence m_seq;
  gfl2::fs::FsCtrBackupSystem::Result m_save_result;

  GameSys::GameManager::BUFFER_CLEAR_SETTING m_bufferClearSettingOriginal[gfl2::gfx::CtrDisplayNo::NumberOf];
};

GFL_NAMESPACE_END(game_clear_save)
GFL_NAMESPACE_END(app)
#endif		// __APP_GAMECLEARSAVE_PROC_H__

