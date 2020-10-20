// ============================================================================
/*
 * @file JoinFestaRecruiterEvent.h
 * @brief ジョインフェスタのリクルーターで人物操作するイベント
 * @date 2016.02.19
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( JOINFESTA_RECRUITER_EVENT_H_INCLUDE )
#define JOINFESTA_RECRUITER_EVENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)


class JoinFestaRecruiterEvent : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRecruiterEvent );
public:

  /*
   * @enum BootMode
   *       起動モード
   */
  enum BootMode{
    BOOT_MODE_LOAD,     //! リクルーターで選択した人物を出現させる
    BOOT_MODE_UNLOAD,   //! リクルーターで選択した人物を破棄する
  };

  //--------------------------------------------------------------
  /**
   * @brief   ジョインフェスタのリクルーターで人物操作するイベント呼び出し
   * 
   * @param   pGameManager  ゲームマネージャ
   * @param   bootMode      起動モード
   */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* pGameManager, const BootMode bootMode );

public:
  JoinFestaRecruiterEvent( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaRecruiterEvent();

  virtual bool BootChk( GameSys::GameManager* pGameManager );
  virtual void InitFunc( GameSys::GameManager* pGameManager );
  virtual void EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );

  // 起動モード設定
  void SetBootMode( const BootMode bootMode ){ m_bootMode = bootMode; }

private:

  // 起動モード別Update
  bool UpdateBootModeLoad( GameSys::GameManager* pGameManager );
  bool UpdateBootModeUnload( GameSys::GameManager* pGameManager );

private:

  BootMode m_bootMode;  // 起動モード

};


GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTA_RECRUITER_EVENT_H_INCLUDE
