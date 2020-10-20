#if !defined( __FINDER_STUDIO_DECORATIONE_UPPER_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATIONE_UPPER_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationUpperView.h
 * @date    2017/01/31 17:22:30
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：上画面View
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
/// @brief    上画面View
///
//////////////////////////////////////////////////////////////////////////
class UpperView
  : public DecoViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView ); //コピーコンストラクタ＋代入禁止

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  UpperView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~UpperView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  void ShowBackground( void );
  void HideBackground( void );

  //-------------------------------------------------------
  // @brief   スタンプ
  //-------------------------------------------------------
  void CreateStamp( u8 decoCount, DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  void DeleteStamp( u8 index );

  void VisibleStamp( u8 index );
  void InvisibleStamp( u8 index );

  void SetStampPos( u8 index, gfl2::math::VEC3 pos );

  //-------------------------------------------------------
  // @brief   フレーム
  //-------------------------------------------------------
  void CreateFrame( u32 frameIndex );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );

  //-------------------------------------------------------
  // @brief   スタンプ：非公開
  //-------------------------------------------------------
  void _CreateStampTexture( u8 decoCount, DECO_TYPE type, u32 decoIndex, gfl2::math::VEC3 pos );
  void _DeleteStampTexture( u8 index );

  void _StuffingStamp( u8 startIndex );

  bool _IsStampNull( u8 index );

  //-------------------------------------------------------
  // @brief   フレーム：非公開
  //-------------------------------------------------------
  void _CreateFrameTexture( u32 frameIndex );
  void _DeleteFrameTexture( void );

  //-------------------------------------------------------
  // @brief   ユーティリティ：非公開
  //-------------------------------------------------------
  s32 _GetIntegerPart_Float( f32 val );
  gfl2::math::VEC3 _GetIntegerPart_VEC3( gfl2::math::VEC3 vec );

private:
  static const u8 STAMP_MAX = Static::DECO_ITEM_MAX;

private:
  gfl2::lyt::LytPane*   m_pBackGroundPane;
  gfl2::lyt::LytPane*   m_pDecoBasePane;
  gfl2::lyt::LytPane*   m_pFrameBasePane;
  gfl2::lyt::LytPane*   m_pStampBasePane;
  gfl2::lyt::LytPane*   m_pStampMovePane;

  gfl2::lyt::LytPicture*    m_pFramePicture;
  gfl2::lyt::LytPicture*    m_pStampPicture[STAMP_MAX];

  u32     m_FrameIndex;
  u32     m_StampCount;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATIONE_UPPER_VIEW_H_INCLUDED__
