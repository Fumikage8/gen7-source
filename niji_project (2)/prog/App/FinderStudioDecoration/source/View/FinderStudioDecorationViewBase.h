#if !defined( __FINDER_STUDIO_DECORATIONE_VIEW_BASE_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATIONE_VIEW_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationViewBase.h
 * @date    2017/01/31 17:25:50
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：デコ専用Viewベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioDefine.h>

#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_deco_type_def.h>
#include <niji_conv_header/app/data/finder_studio/finder_studio_frame_def.h>
#include <niji_conv_header/app/studio_deco/FinderStudioDeco.h>
#include <niji_conv_header/app/studio_deco/FinderStudioDeco_pane.h>
#include <niji_conv_header/app/studio_deco/FinderStudioDeco_anim_list.h>
#include <niji_conv_header/message/msg_fs_deco.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( Savedata )
class FinderStudioSave;
GFL_NAMESPACE_END( Savedata )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class ViewListener;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    デコ専用Viewベース
///
//////////////////////////////////////////////////////////////////////////
class DecoViewBase
  : public Static::ViewBase
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( DecoViewBase ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  DecoViewBase( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~DecoViewBase( void ) { ; }

  //-------------------------------------------------------
  // @brief   設定関数
  //-------------------------------------------------------
  void SetViewListener( ViewListener* listener ) { m_pListener = listener; }

protected:
  //-------------------------------------------------------
  // @brief   初期化・終了：非公開
  //-------------------------------------------------------
  void _InitView( void );
  void _TermView( void );

protected:
  ViewListener*     m_pListener;

  Savedata::FinderStudioSave*   m_pFinderStudioSave;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATIONE_VIEW_BASE_H_INCLUDED__
