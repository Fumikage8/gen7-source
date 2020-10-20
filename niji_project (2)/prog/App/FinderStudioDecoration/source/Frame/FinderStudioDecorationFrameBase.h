#if !defined __FINDER_STUDIO_DECORATION_FRAME_BASE_H_INCLUDED__
#define __FINDER_STUDIO_DECORATION_FRAME_BASE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioDecorationFrameBase.h
 * @date    2016/11/22 17:07:55
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「デコレーション」：フレームベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDecorationFrameManager.h"
#include "../FinderStudioDecorationWork.h"

#include <AppLib/include/Frame/AppFrameCellSuper.h>

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )
class ViewList;
struct AlbumWork;
class MessageMenuView;
GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Decoration )

//////////////////////////////////////////////////////////////////////////
///
/// @brief    フレームベース
///
//////////////////////////////////////////////////////////////////////////
class FrameBase
  : public applib::frame::CellSuper
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameBase );

protected:
  /// リザルト
  enum Result
  {
    RESULT_CONTINUE,    //!< 継続
    RESULT_FINISH,      //!< 終了
    RESULT_CHANGE_SEQ,  //!< シーケンス変更
    RESULT_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FrameBase( void )
    : m_pManager( NULL )
    , m_pViewList( NULL )
    , m_pWork( NULL )
    , m_pDecoWork( NULL )
    , m_pMsgMenuView( NULL )
    , m_MainSeq( 0 )
    , m_SubSeq( 0 )
  {
    ;
  }
  virtual ~FrameBase( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual applib::frame::Result InitFunc( void ) { return applib::frame::RES_FINISH; }
  virtual applib::frame::Result EndFunc( void ) { return applib::frame::RES_FINISH; }
  virtual applib::frame::Result UpdateFunc( void ) { return applib::frame::RES_FINISH; }
  virtual void                  DrawFunc( gfl2::gfx::CtrDisplayNo no ) { ; }

public:
  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetFrameManager( FrameManager* manager ) { m_pManager = manager; }
  void SetViewList( Static::ViewList* list )    { m_pViewList = list; }
  void SetAlbumWork( Static::AlbumWork* work )  { m_pWork = work; }
  void SetDecoWork( DecoWork* work )            { m_pDecoWork = work; }

  //-------------------------------------------------------
  // @brief   取得関数群
  //-------------------------------------------------------
  FrameManager* GetFrameManager( void ) const { return m_pManager; }

protected:
  //-------------------------------------------------------
  // @brief   シーケンス関数
  //-------------------------------------------------------
  /// メインシーケンスの設定
  void SetMainSeq( u8 seq )
  {
    m_MainSeq = seq;
    SetSubSeq( 0 );
  }
  /// サブシーケンスの設定
  void SetSubSeq( u8 seq )
  {
    m_SubSeq = seq;
  }

private:
  FrameManager*   m_pManager;

protected:
  Static::ViewList*   m_pViewList;
  Static::AlbumWork*  m_pWork;
  DecoWork*           m_pDecoWork;

  Static::MessageMenuView*    m_pMsgMenuView;

protected:
  u8    m_MainSeq;
  u8    m_SubSeq;
};

GFL_NAMESPACE_END( Decoration )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_DECORATION_FRAME_BASE_H_INCLUDED__
