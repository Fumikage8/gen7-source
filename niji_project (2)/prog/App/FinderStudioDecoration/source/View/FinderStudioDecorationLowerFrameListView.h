#if !defined( __FINDER_STUDIO_DECORATIONE_FRAME_LIST_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_DECORATIONE_FRAME_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationLowerFrameListView.h
 * @date    2017/01/31 19:25:10
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームリストView
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
/// @brief    フレームリストView
///
//////////////////////////////////////////////////////////////////////////
class FrameListView
  : public DecoViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameListView ); //コピーコンストラクタ＋代入禁止

public:
  enum {
    RES_CONTINUE,
    RES_CLOSE,
    RES_NUM,
  };

private:
  enum {
    BTN_ID_FRAME_00,
    BTN_ID_FRAME_01,
    BTN_ID_FRAME_02,
    BTN_ID_FRAME_03,
    BTN_ID_FRAME_04,
    BTN_ID_FRAME_05,
    BTN_ID_FRAME_06,
    BTN_ID_FRAME_07,
    BTN_ID_FRAME_08,
    BTN_ID_FRAME_09,
    BTN_ID_FRAME_10,
    BTN_ID_BACK,
    BTN_ID_NUM,
  };

  enum {
    FRAME_MAX = DECO_FRAME_KIND_NUM + 1,   //!< フレーム10種＋フレームなし
  };
  static const DECO_TYPE DEFAULT_DECO_TYPE = DECO_TYPE_FRAME;

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FrameListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~FrameListView( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   初期化・終了
  //-------------------------------------------------------
  virtual void Initialize( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新・描画
  //-------------------------------------------------------
  virtual void Update( void );

  //-------------------------------------------------------
  // @brief   表示・非表示
  //-------------------------------------------------------
  virtual void Show( void );
  virtual void Hide( void );

  //-------------------------------------------------------
  // @brief   背景アニメ
  //-------------------------------------------------------
  void SetBackGroundAnimFrame( f32 frame );

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitAnim( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   サムネイル：非公開
  //-------------------------------------------------------
  void _CreateFrameList( void );
  void _DeleteFrameList( void );

  void _InitThumb( void );
  void _InitThumb_Frame( void );
  void _InitThumb_Texture( void );
  void _InitThumb_Button( void );
  void _InitThumb_NewIcon( void );

  void _ReplaceTexture( u32 pnaeIndex, u32 frameIndex );

  u8   _GetLangIndex( void );

  void _SetInvisibleNewIcon( u32 frameIndex );
  bool _IsNewVisible( u32 frameIndex );

  //-------------------------------------------------------
  // @brief   フレーム：非公開
  //-------------------------------------------------------
  void _SelectFrameAnim( u32 paneIndex );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );

private:
  u8    m_FrameCount;
  u32*  m_pFrameList;

  gfl2::lyt::LytPicture*    m_pFramePicture[FRAME_MAX];
  gfl2::lyt::LytPane*       m_pFrameNewIcon[FRAME_MAX];

  u8    m_Seq;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATIONE_FRAME_LIST_VIEW_H_INCLUDED__
