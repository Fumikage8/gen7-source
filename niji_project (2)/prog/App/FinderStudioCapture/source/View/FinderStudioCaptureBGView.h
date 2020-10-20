#if !defined( __FINDER_STUDIO_CAPTURE_BG_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_BG_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureBGView.h
 * @date    2016/11/24 14:49:58
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：背景View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景View
///
//////////////////////////////////////////////////////////////////////////
class BGView
  : public Static::ViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( BGView ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  BGView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~BGView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitAnim( void );
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_BG_VIEW_H_INCLUDED__
