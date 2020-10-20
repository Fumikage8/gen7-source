#if !defined __FINDER_STUDIO_CAPTURE_PROC_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_PROC_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureProc.h
 * @date    2016/08/25
 * @author  harada_takumi
 * @brief   ファインダースタジオ「撮影」：プロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioCapture/source/View/FinderStudioCaptureViewListener.h>
#include <App/FinderStudioCapture/source/Frame/FinderStudioCaptureFrameManager.h>
#include <App/FinderStudioCapture/source/Debug/FinderStudioCaptureDebug.h>

#include <GameSys/include/GameProc.h>
#include <PokeTool/include/PokeTool.h>
#include <System/include/GflUse.h>

#include <util/include/gfl2_std_string.h>
#include <str/include/gfl2_str.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuCursor;
class AppCommonGrpIconData;
class PokeIcon;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_BEGIN( util )
class Heap;
class AppRenderingManager;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( applib )
GFL_NAMESPACE_BEGIN( frame )
class Manager;
GFL_NAMESPACE_END( frame )
GFL_NAMESPACE_END( applib )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ViewBase;
class ViewList;
class ParentView;
class MessageMenuView;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
struct Work;
class FrameManager;
class UpperView;
class BGView;
class PokeListView;
class BGListView;
class DressupListView;
class CompositionView;
class SelectButtonView;
class ShootView;
class ZMotionView;
class CameraView;
class PhotoListView;
class PrepareSelectView;
class PrepareCommonView;
class ShootCommonView;
class LayoutView;
class ViewerManager;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/// ---[ マクロ begin ]---
#if !defined(CREATE_FUNC_SET)
#define CREATE_FUNC_SET(name) \
  bool Create##name( void );  \
  bool Delete##name( void );
#endif
#if !defined(LOAD_FUNC_SET)
#define LOAD_FUNC_SET(name)   \
  bool Load##name( void );    \
  bool Unload##name( void );
#endif
/// ---[ マクロ end ]---
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファインダースタジオプロック
///
//////////////////////////////////////////////////////////////////////////
class Proc
  : public GameSys::GameProc
  , public ViewListener
  , public FrameListener
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc ); //コピーコンストラクタ＋代入禁止

private:
  enum Sequence
  {
    SEQ_FADE_IN,
    SEQ_FADE_IN_WAIT,
    SEQ_MAIN,
    SEQ_FADE_OUT,
    SEQ_FADE_OUT_WAIT,
    SEQ_END,
  };
  enum ViewerUpdate
  {
    VIEWER_UPD_NONE,
    VIEWER_UPD_POKEMON,
    VIEWER_UPD_BACKGROUND,
    VIEWER_UPD_DRESSUP,
    VIEWER_UPD_POKE_MOTION_TYPE,
    VIEWER_UPD_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  Proc( void );
  virtual ~Proc( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* manager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* manager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* manager );
  virtual void               PreDrawFunc( gfl2::proc::Manager* manager );
  virtual void               DrawFunc( gfl2::proc::Manager* manager, gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetWork( Work* work ) { m_pWork = work; }

private:
  //-------------------------------------------------------
  // @brief   生成 破棄 関数群
  //-------------------------------------------------------
  CREATE_FUNC_SET( Heap );
  CREATE_FUNC_SET( ViewList );
  CREATE_FUNC_SET( FrameManager );
  CREATE_FUNC_SET( 3DViewer );
  CREATE_FUNC_SET( AppRenderingManager );
  LOAD_FUNC_SET( LayoutData );
  CREATE_FUNC_SET( MessageData );
  CREATE_FUNC_SET( View );
  CREATE_FUNC_SET( PrepareView );
  CREATE_FUNC_SET( ShootMainView );
  CREATE_FUNC_SET( MenuCursor );
  CREATE_FUNC_SET( CommonIcon );
  CREATE_FUNC_SET( PokeIcon );
  CREATE_FUNC_SET( MessageMenuView );
  CREATE_FUNC_SET( PhotoTexture );

  bool _DeleteView( Static::ViewBase* view );

private:
  //-------------------------------------------------------
  // @brief   更新関数群
  //-------------------------------------------------------
  void _UpdateViewer( void );

  void _UpdateCompositionID( void );

private:
  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  // ViewListener
  virtual bool IsUpdateViewer( void );
  virtual bool IsFinishUpdateViewer( void );
  virtual bool IsRequestUpdateViewer( void );
  virtual void StartScreenTransition( void );
  virtual void ViewerReset( void );
  // PrepareListener
  virtual void SetCaptureMode( Viewer::CaptureMode mode );
  virtual Viewer::CaptureMode GetCaptureMode( void );
  virtual void SetPokemon( pml::pokepara::PokemonParam* pp, u32 index );
  virtual void SetBackGround( u32 index, Viewer::TimeZone timeZone );
  virtual void SetTimeZone( Viewer::TimeZone timeZone );
  virtual Viewer::TimeZone GetTimeZone( void );
  virtual void SetDressUpSaveIndex( poke_3d::model::DressUpParam dressUpParam, u8 index );
  virtual void SetDressUp( poke_3d::model::DressUpParam dressUpParam, u8 dressupIndex );
  virtual void SetComposition( Viewer::Composition composition );
  virtual Viewer::Composition GetComposition( void );
  virtual bool IsCanUseComposition( Viewer::Composition composition );
  // CameraControlListener
  virtual void CameraRotateXUp( void );
  virtual void CameraRotateXDown( void );
  virtual void CameraRotateYLeft( void );
  virtual void CameraRotateYRight( void );
  virtual f32  GetCameraRotateY( void );
  virtual void CameraMoveYUp( void );
  virtual void CameraMoveYDown( void );
  virtual void SetCameraHeightRate( f32 rate );
  virtual f32  GetCameraHeightRate( void );
  virtual void CameraLookAtPlayer( void );
  virtual void CameraLookAtPokemon( void );
  virtual void SetCameraLookAtRate( f32 rate );
  virtual f32  GetCameraLookAtRate( void );
  virtual void CameraZoomIn( void );
  virtual void CameraZoomOut( void );
  virtual void SetCameraZoomRate( f32 rate );
  virtual f32  GetCameraZoomRate( void );
  // TargetControlListener
  virtual bool PlayerRotateLeft( void );
  virtual bool PlayerRotateRight( void );
  virtual bool SetPlayerRotate( f32 degree );
  virtual f32  GetPlayerRotate( void );
  virtual f32  GetViewerPlayerRotate( void );
  virtual bool PokemonRotateLeft( void );
  virtual bool PokemonRotateRight( void );
  virtual bool SetPokemonRotate( f32 degree );
  virtual f32  GetPokemonRotate( void );
  virtual f32  GetViewerPokemonRotate( void );
  virtual bool SetPokemonPosition( gfl2::math::Vector3 pos, bool isLift );
  virtual gfl2::math::Vector3 GetPokemonPosition( void );
  virtual gfl2::math::Vector3 GetPokemonViewerPosition( void );
  virtual gfl2::math::AABB GetPlayerAABB( void );
  virtual gfl2::math::AABB GetPokemonAABB( void );
  // MotionControlListener
  virtual void SetTrainerMotionType( Viewer::ZMotionType type );
  virtual Viewer::ZMotionType GetTrainerMotionType( void );
  virtual void TrainerMotionStart( Viewer::ZMotionType type = Viewer::Z_MOTION_TYPE_MAX );
  virtual void TrainerMotionStop( void );
  virtual void TrainerMotionReset( void );
  virtual bool IsPlayingTrainerMotion( void );
  virtual void PokemonMotionStart( void );
  virtual Viewer::PokeMotionType ChangePokemonMotionType( bool bReset );
  virtual Viewer::PokeMotionType GetPokemonMotionType( void );
  virtual bool IsNeedResetPokemonMotionType( void );
  // ポケモンの再生可能なモーションの割合
  virtual f32 GetPokemonMotionPlayRate( void );
  virtual void SetPokemonMotionAuto( bool enable );
  // CaptureListener
  virtual void StartCapture( void );
  virtual bool IsFinishCapture( void );
  // PhotoListener
  virtual void  SetUpperPhotoImage( u32 index );
  virtual void* GetPhotoTexture( u32 index );
  // UpperViewListener
  virtual void VisibleSight( void );
  virtual void InvisibleSight( void );
  virtual void ShutterOpen( void );
  virtual bool IsFinishShutterOpen( void );
  virtual void ShutterClose( void );
  virtual bool IsFinishShutterClose( void );

  // FrameListener
  virtual void ShowViewer( void ) { m_bShowViewer = true; }
  virtual void HideViewer( void ) { m_bShowViewer = false; }

private:
  gfl2::heap::HeapBase*   m_pRootHeap;
  gfl2::heap::HeapBase*   m_pProcHeap;
  gfl2::heap::HeapBase*   m_pManagerHeap;
  gfl2::heap::HeapBase*   m_pImgdbHeap;
  gfl2::heap::HeapBase*   m_pViewerHeap;
  gfl2::heap::HeapBase*   m_pPhotoTextureHeap;
  app::util::Heap*        m_pAppHeap;

private:
  ViewerManager*        m_pViewerManager;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::str::MsgData*   m_pMsgData;
  print::WordSet*       m_pWordSet;

  FrameManager*         m_pFrameManager;
  Work*                 m_pWork;
  Static::ViewList*     m_pViewList;

  void*                 m_pLytDatBuff;

private:
  Static::ParentView*       m_pParentView;
  UpperView*                m_pUpperView;
  BGView*                   m_pBGView;
  PokeListView*             m_pPokeListView;
  BGListView*               m_pBGListView;
  DressupListView*          m_pDressupListView;
  CompositionView*          m_pCompositionView;
  SelectButtonView*         m_pSelectButtonView;
  ShootView*                m_pShootView;
  ZMotionView*              m_pZMotionView;
  CameraView*               m_pCameraView;
  PhotoListView*            m_pPhotoListView;
  PrepareSelectView*        m_pPrepareSelectView;
  PrepareCommonView*        m_pPrepareCommonView;
  ShootCommonView*          m_pShootCommonView;
  LayoutView*               m_pLayoutView;
  Static::MessageMenuView*  m_pMessageMenuView;

  app::tool::MenuCursor*    m_pMenuCursor;
  void*                     m_pMenuCursorBuf;

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;
  app::tool::PokeIcon*                m_pPokeIcon;

private:
  u8  m_InitSeq;    //!< 初期化用
  u8  m_EndSeq;     //!< 終了用
  u8  m_MainSeq;    //!< メイン
  u8  m_SubSeq;     //!< サブ
  u8  m_ViewerSeq;  //!< ビューワ更新シーケンス

  bool  m_bShowViewer;

#if PM_DEBUG
private:
  DebugClass    m_Debug;
#endif
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_PROC_H_INCLUDED__