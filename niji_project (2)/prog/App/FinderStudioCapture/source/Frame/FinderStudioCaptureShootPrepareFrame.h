#if !defined __FINDER_STUDIO_CAPTURE_SHOOT_PREPARE_FRAME_H_INCLUDED__
#define __FINDER_STUDIO_CAPTURE_SHOOT_PREPARE_FRAME_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCaptureShootPrepareFrame.h
 * @date    2016/11/09 20:33:02
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：撮影準備フレーム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureFrameBase.h"


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class UpperView;
class PokeListView;
class BGListView;
class DressupListView;
class CompositionView;
class SelectButtonView;
class PrepareSelectView;
class PrepareCommonView;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
///
/// @brief    撮影準備フレーム
///
//////////////////////////////////////////////////////////////////////////
class ShootPrepareFrame
  : public FrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ShootPrepareFrame );

public:
  /// 画面シーケンス
  enum {
    SEQ_SELECT_PREPARE,             //!< 準備選択
    SEQ_SELECT_CAPTURE_MODE,        //!< 撮影モード選択
    SEQ_SELECT_POKEMON,             //!< ポケモン選択
    SEQ_SELECT_BACKGROUND,          //!< 背景選択
    SEQ_SELECT_DRESSUP,             //!< 衣装選択
    SEQ_SELECT_COMPOSITION,         //!< 構図選択
    SEQ_SELECT_FINISH_CONFIRM,      //!< 終了確認
    SEQ_CURTAIN_CLOSE,              //!< カーテンを閉じる
    SEQ_NUM,
  };

private:
  /// カーテン用シーケンス
  enum {
    CURTAIN_SEQ_IDLE,
    CURTAIN_SEQ_OPEN,
    CURTAIN_SEQ_OPEN_WAIT,
    CURTAIN_SEQ_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ShootPrepareFrame( void );
  virtual ~ShootPrepareFrame( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );
  virtual applib::frame::Result EndFunc( void );
  virtual applib::frame::Result UpdateFunc( void );

private:
  //-------------------------------------------------------
  // @brief   更新関数：非公開
  //-------------------------------------------------------
  void   _UpdateCurtainOpen( void );
  Result _UpdateSelectPrepare( void );
  Result _UpdateSelectCaptureMode( void );
  Result _UpdateSelectPokemon( void );
  Result _UpdateSelectBackGround( void );
  Result _UpdateSelectDressup( void );
  Result _UpdateSelectComposition( void );
  Result _UpdateSelectFinishConfirm( void );
  Result _UpdateCurtainClose( void );

  //-------------------------------------------------------
  // @brief   画面 表示・非表示：非公開
  //-------------------------------------------------------
  void _ShowPrepareSelect( void );
  void _HidePrepareSelect( void );
  void _ShowPokeList( void );
  void _HidePokeList( void );
  void _ShowBGList( void );
  void _HideBGList( void );
  void _ShowDressupList( void );
  void _HideDressupList( void );
  void _ShowCompositionSelect( void );
  void _HideCompositionSelect( void );
  void _ShowSelectButton( void );
  void _HideSelectButton( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _IsEventFlag( u32 flg );

private:
  UpperView*          m_pUpperView;
  PokeListView*       m_pPokeListView;
  BGListView*         m_pBGListView;
  DressupListView*    m_pDressUpView;
  CompositionView*    m_pCompositionView;
  SelectButtonView*   m_pSelectButtonView;
  PrepareSelectView*  m_pPrepareSelectView;
  PrepareCommonView*  m_pPrepareCommonView;

  u8    m_CurtainSeq;
  u8    m_PrevSeq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_SHOOT_PREPARE_FRAME_H_INCLUDED__
