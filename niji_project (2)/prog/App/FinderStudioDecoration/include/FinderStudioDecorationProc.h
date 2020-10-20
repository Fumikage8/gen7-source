#if !defined __FINDER_STUDIO_DECORATION_PROC_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_PROC_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationProc.h
 * @date    2016/11/17 18:14:34
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：プロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioDecoration/source/FinderStudioDecorationWork.h>
#include <App/FinderStudioDecoration/source/View/FinderStudioDecorationViewListener.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include <GameSys/include/GameProc.h>
#include "System/include/GflUse.h"

#include <str/include/gfl2_str.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class MenuCursor;
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
struct AlbumWork;
class ViewBase;
class ViewList;
class ParentView;
class MessageMenuView;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_BEGIN( Viewer )
class FinderStudioViewer;
GFL_NAMESPACE_END( Viewer )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class FrameManager;
class UpperView;
class BGView;
class LowerMainView;
class FrameListView;
class StampListView;
class DecoItemManager;
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
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc ); //コピーコンストラクタ＋代入禁止

private:
  enum Sequence
  {
    SEQ_FADE_IN,
    SEQ_MAIN,
    SEQ_FADE_OUT,
    SEQ_END,
  };

  enum Result
  {
    RES_CONTINUE,   //!< 継続
    RES_NEXT_SEQ,   //!< 次のシーケンスへ
    RES_NUM,
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
  void SetAlbumWork( Static::AlbumWork* work ) { m_pAlbumWork = work; }

private:
  //-------------------------------------------------------
  // @brief   生成 破棄 関数群
  //-------------------------------------------------------
  CREATE_FUNC_SET( Heap );
  CREATE_FUNC_SET( ViewList );
  CREATE_FUNC_SET( FrameManager );
  CREATE_FUNC_SET( 3DViewer );
  CREATE_FUNC_SET( AppRenderingManager );
  CREATE_FUNC_SET( Font );
  LOAD_FUNC_SET( LayoutData );
  CREATE_FUNC_SET( MessageData );
  CREATE_FUNC_SET( View );
  CREATE_FUNC_SET( MenuCursor );
  CREATE_FUNC_SET( MessageMenuView );
  CREATE_FUNC_SET( DecoItemManager );
  CREATE_FUNC_SET( PhotoData );

  bool _DeleteView( Static::ViewBase* view );

private:
  //-------------------------------------------------------
  // @brief   更新関数
  //-------------------------------------------------------
  Result _UpdateFadeIn( void );
  Result _UpdateMain( void );
  Result _UpdateFadeOut( void );

  void _UpdateBackGroundAnim( void );

  //-------------------------------------------------------
  // @brief   リスナー
  //-------------------------------------------------------
  virtual void MoveStartNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  virtual void MoveEndNewStamp( DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  virtual void MoveStartOldStamp( u8 index );
  virtual void MoveEndOldStamp( u8 index, gfl2::math::VEC3 pos );
  virtual void DeleteOldStamp( u8 index );
  virtual void SetFrame( u32 frameIndex );
  virtual bool IsCanPutStamp( void );

private:
  gfl2::heap::HeapBase*   m_pRootHeap;
  gfl2::heap::HeapBase*   m_pProcHeap;
  gfl2::heap::HeapBase*   m_pManagerHeap;
  gfl2::heap::HeapBase*   m_pImgdbHeap;
  gfl2::heap::HeapBase*   m_pViewerHeap;
  app::util::Heap*        m_pAppHeap;

private:
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  Viewer::FinderStudioViewer*       m_pViewer;

  gfl2::str::MsgData*   m_pMsgData;
  print::WordSet*       m_pWordSet;

  FrameManager*         m_pFrameManager;
  Static::AlbumWork*    m_pAlbumWork;
  Static::ViewList*     m_pViewList;

  DecoWork              m_DecoWork;

  void*                 m_pLytDatBuff;

private:
  Static::ParentView*       m_pParentView;
  Static::MessageMenuView*  m_pMessageMenuView;
  UpperView*                m_pUpperView;
  BGView*                   m_pBGView;
  LowerMainView*            m_pLowerMainView;
  FrameListView*            m_pFrameListView;
  StampListView*            m_pStampListView;

  app::tool::MenuCursor*    m_pMenuCursor;
  void*                     m_pMenuCursorBuf;

  Static::IntegratedData    m_IntegratedData;

  DecoItemManager*          m_pDecoItemManager;

private:
  u8  m_InitSeq;    //!< 初期化用
  u8  m_EndSeq;     //!< 終了用
  u8  m_MainSeq;    //!< メイン
  u8  m_SubSeq;     //!< サブ
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_PROC_H_INCLUDED__