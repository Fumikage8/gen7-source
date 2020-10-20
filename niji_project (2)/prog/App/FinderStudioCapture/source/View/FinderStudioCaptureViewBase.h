#if !defined (__FINDER_STUDIO_CAPTURE_VIEW_BASE_H_INCLUDED__)
#define __FINDER_STUDIO_CAPTURE_VIEW_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureViewBase.h
 * @date    2017/03/03 17:25:29
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影専用Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( Savedata )
class FinderStudioSave;
GFL_NAMESPACE_END( Savedata )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewListener;
struct Work;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影専用Viewベース
///
//////////////////////////////////////////////////////////////////////////
class CaptureViewBase
  : public Static::ViewBase
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( CaptureViewBase ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  CaptureViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf = NULL );
  virtual ~CaptureViewBase( void ) { ; }

  //-------------------------------------------------------
  // @brief   設定関数
  //-------------------------------------------------------
  void SetViewListener( ViewListener* listener ) { m_pListener = listener; }
  void SetWork( Work* work ) { m_pWork = work; }

protected:
  //-------------------------------------------------------
  // @brief   初期化・終了：非公開
  //-------------------------------------------------------
  void _InitView( void );
  void _TermView( void );

  //-------------------------------------------------------
  // @brief   インフォ用パラメータ：非公開
  //-------------------------------------------------------
  void _CreateInfoParam( const u32 pageNum, const u32 charNum = 1024 );
  void _DeleteInfoParam( void );

protected:
  Savedata::FinderStudioSave*   m_pFinderStudioSave;

  ViewListener*   m_pListener;
  Work*           m_pWork;

  gfl2::str::StrBuf**                     m_pInfoStrArray;
  const gfl2::lyt::LytResTextureInfo**    m_pInfoTexArray;

protected:
  u32     m_InfoParamCount;
  bool    m_bCreateInfoParam;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_VIEW_BASE_H_INCLUDED__