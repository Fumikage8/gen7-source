#if !defined( __FINDER_STUDIO_DECORATION_LOWER_BG_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATION_LOWER_BG_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationLowerBGView.h
 * @date    2017/02/08 17:27:54
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：背景View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationViewBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    背景View
///
//////////////////////////////////////////////////////////////////////////
class BGView
  : public DecoViewBase
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

  //-------------------------------------------------------
  // @brief   背景アニメ
  //-------------------------------------------------------
  f32 GetBackGroundAnimFrame( void );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitAnim( void );
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_LOWER_BG_VIEW_H_INCLUDED__
