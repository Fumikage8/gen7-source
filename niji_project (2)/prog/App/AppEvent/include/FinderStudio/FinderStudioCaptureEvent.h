#if !defined( __FINDER_STUDIO_CAPTURE_EVENT_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_EVENT_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureEvent.h
 * @date    2016/11/09 16:21:28
 * @author  fukushima_yuya
 * @brief   イベント：ファインダースタジオ写真撮影
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/include/FinderStudioCaptureWork.h>
#include <App/Kisekae/include/KisekaeAppParam.h>

#include <GameSys/include/GameEvent.h>
#include <GameSys/include/GameEventManager.h>
#include <GameSys/include/GameManager.h>

#if PM_DEBUG
#include <Debug/Launcher/include/LauncherProc.h>
#endif


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )
class Proc;
GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )

GFL_NAMESPACE_BEGIN( Kisekae )
class Proc;
GFL_NAMESPACE_END( Kisekae )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////



GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( event )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    イベント：ファインダースタジオ写真撮影
///
//////////////////////////////////////////////////////////////////////////
class FinderStudioCaptureEvent
  : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderStudioCaptureEvent );  //! コピーを禁止

private:
  enum {
    EVSEQ_CAPTURE,
    EVSEQ_KISEKAE,
    EVSEQ_JUMP_TITLE,
    EVSEQ_END,
  };
  enum {
    PROC_TYPE_FINDER_STUDIO,
    PROC_TYPE_KISEKAE,
    PROC_TYPE_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   このイベントを起動する関数
  //-------------------------------------------------------
#if PM_DEBUG
  static FinderStudioCaptureEvent* StartEvent( GameSys::GameManager* gameMan, bool debug );
#endif
  static FinderStudioCaptureEvent* StartEvent( GameSys::GameManager* gameMan );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FinderStudioCaptureEvent( gfl2::heap::HeapBase* heap );
  virtual ~FinderStudioCaptureEvent( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual bool                    BootChk( GameSys::GameManager* /*gmgr*/ );
  virtual void                    InitFunc( GameSys::GameManager* /*gmgr*/ );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gameMan );
  virtual void                    EndFunc( GameSys::GameManager* /*gmgr*/ );

public:
  /**
   * @brief BGM再生設定
   */
  void SetPlayBGM(void);

private:
  //-------------------------------------------------------
  // @brief   プロック起動
  //-------------------------------------------------------
  void CallProc_FinderStudioCapture( void );
  void CallProc_Kisekae( GameSys::GameManager* gameMan );

  void ChangeProc( GameSys::GameManager* gameMan );

  //-------------------------------------------------------
  // @brief   イベント起動
  //-------------------------------------------------------
  void StartEvent_FatalError( GameSys::GameManager* gameMan );

  //-------------------------------------------------------
  // @brief   着せ替えデータの反映
  //-------------------------------------------------------
  void _UpdateDressUpParam( GameSys::GameManager* gameMan );

  //-------------------------------------------------------
  // @brief   表示が可能なメンバーインデックスを取得
  //-------------------------------------------------------
  static u8   _GetEnableMemberIndex( GameSys::GameManager* gameMan );
  static bool _IsPokemonEgg( const pml::pokepara::PokemonParam* pp );
  static bool _IsPokemonHinshi( const pml::pokepara::PokemonParam* pp );
  static void _CheckPokemon( GameSys::GameManager* gameMan, u8 memberIndex );

private:
  App::FinderStudio::Capture::Proc*   m_pFinderStudioCaptureProc;
  App::Kisekae::Proc*                 m_pKisekaeProc;

  App::FinderStudio::Capture::Work    m_FinderStudioWork;
  App::Kisekae::APP_PARAM             m_KisekaeAppParam;

private:
  u8    m_Seq;
  u8    m_ProcType;

  static u8   m_MemberIndex;

  bool m_isPlayBGM;

#if PM_DEBUG
  static bool m_DebugFlag;

  debug::launcher::LauncherProc::CallParam    m_LauncherParam;
#endif
};

GFL_NAMESPACE_END( event )
GFL_NAMESPACE_END( app )

#endif // __FINDER_STUDIO_CAPTURE_EVENT_H_INCLUDED__