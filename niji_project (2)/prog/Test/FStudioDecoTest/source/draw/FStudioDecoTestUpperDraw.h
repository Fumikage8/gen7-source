#if PM_DEBUG

#if !defined(__FINDER_STUDIO_DECO_TEST_UPPER_DRAW_H_INCLUDED__)
#define __FINDER_STUDIO_DECO_TEST_UPPER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FStudioDecoTestUpperDraw.h
 * @date    2016/10/27 12:57:55
 * @author  fukushima_yuya
 * @brief   ファインダースタジオデコテスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <AppLib/include/UI/UIView.h>

#include <Test/FStudioDecoTest/source/FStudioDecoTestDefine.h>


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FSDeco )

class UIUtility;
class DrawListener;
class ItemList;

//----------------------------------------------------------------------
// @brief   デコ上画面
//----------------------------------------------------------------------
class UpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperDraw );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  UpperDraw( app::util::Heap* heap, app::util::AppRenderingManager* renderMan );
  virtual ~UpperDraw( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   継承関数群
  //-------------------------------------------------------
  virtual void Update( void );
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //-------------------------------------------------------
  // @brief   初期化・終了処理
  //-------------------------------------------------------
  bool InitializeSystem( void );
  void Initialize2D( void* lytDatBuf );
  void InitializeMember( void );
  void InitializePhoto( void* photoBuf );
  bool Terminate( void );

  //-------------------------------------------------------
  // @brief   更新処理
  //-------------------------------------------------------
  void UpdateTouchPoint( gfl2::math::VEC3 vec );

  //-------------------------------------------------------
  // @brief   デコアイテムの追加と削除
  //-------------------------------------------------------
  void AddDecoItem( void );
  void RemoveDecoItem( void );
  void SetDecoItem( u32 index );

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetDrawListener( DrawListener* listener ) { m_pListener = listener; }

private:
  app::util::Heap*                  m_pHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  UIUtility*      m_pUtil;
  DrawListener*   m_pListener;

  ItemList*       m_pItemList;

private:
  gfl2::lyt::LytPicture*    m_pActiveDeco;
  gfl2::lyt::LytPane*       m_pNullDecoParent;
  u8                        m_Wait;
};

GFL_NAMESPACE_END( FSDeco )
GFL_NAMESPACE_END( Test )

#endif // __FINDER_STUDIO_DECO_TEST_UPPER_DRAW_H_INCLUDED__

#endif // #if PM_DEBUG
