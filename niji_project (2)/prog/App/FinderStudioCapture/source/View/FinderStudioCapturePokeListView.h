#if !defined( __FINDER_STUDIO_CAPTURE_POKE_LIST_VIEW_H_INCLUDED__ )
#define __FINDER_STUDIO_CAPTURE_POKE_LIST_VIEW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioCapturePokeListView.h
 * @date    2016/11/24 16:06:43
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ「撮影」：ポケリストView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCaptureViewBase.h"

#include <pml/include/pmlib.h>


//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
class PokeIcon;
GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Capture )

//////////////////////////////////////////////////////////////////////////
/// ---[ 前方宣言 begin ]---
class PrepareCommonViewListener;
/// ---[ 前方宣言 end ]---
//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
///
/// @brief    ポケリストView
///
//////////////////////////////////////////////////////////////////////////
class PokeListView
  : public CaptureViewBase
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListView ); //コピーコンストラクタ＋代入禁止

private:
  enum {
    BTN_ID_POKE_00,
    BTN_ID_POKE_01,
    BTN_ID_POKE_02,
    BTN_ID_POKE_03,
    BTN_ID_POKE_04,
    BTN_ID_POKE_05,
    BTN_ID_NUM,
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  PokeListView( app::util::Heap* heap, app::util::AppRenderingManager* renderMan, void* cursorBuf );
  virtual ~PokeListView( void ) { ; }

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

  //-------------------------------------------------------
  // @brief   設定関数群
  //-------------------------------------------------------
  void SetPrepareCommonViewListener( PrepareCommonViewListener* listener ) { m_pPreCmnViewListener = listener; }
  void SetPokeIcon( app::tool::PokeIcon* icon ) { m_pPokeIcon = icon; }

private:
  //-------------------------------------------------------
  // @brief   初期化：非公開
  //-------------------------------------------------------
  virtual void Init2D( void* lytDatBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );
  virtual void InitPane( void );
  virtual void InitText( void );
  virtual void InitButton( void );

  //-------------------------------------------------------
  // @brief   更新：非公開
  //-------------------------------------------------------
  void _UpdateCommonButton( void );

  //-------------------------------------------------------
  // @brief   ポケモンの設定：非公開
  //-------------------------------------------------------
  void _SetPokemon( void );
  void _SetPokeIcon( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp );
  void _SetPokeGender( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp );
  void _SetSickIcon( u32 index, gfl2::lyt::LytParts* parts, const pml::pokepara::PokemonParam* pp );

  //-------------------------------------------------------
  // @brief   ポケモン情報の取得：非公開
  //-------------------------------------------------------
  u32                          _GetMemberCount( void ) const;
  pml::pokepara::PokemonParam* _GetPokeParam( u32 index ) const;
  bool                         _IsPokeEgg( const pml::pokepara::PokemonParam* pp );
  bool                         _IsPokeHinshi( const pml::pokepara::PokemonParam* pp );

  //-------------------------------------------------------
  // @brief   取得関数：非公開
  //-------------------------------------------------------
  u32 _GetGenerationBackGroundID( const pml::pokepara::PokemonParam* pp );
  u32 _GetDefaultBackGroundID( void );

  //-------------------------------------------------------
  // @brief   チェック：非公開
  //-------------------------------------------------------
  bool _IsGenerationBackGround( void );
  bool _IsCompareGenerationBackGround( const pml::pokepara::PokemonParam* pp );
  bool _IsButtonAction( void );

  //-------------------------------------------------------
  // @brief   リスナー関数群
  //-------------------------------------------------------
  virtual void                                     OnLayoutPaneStartingAnimeEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneId );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* button, gfl2::ui::VectorDevice* key, gfl2::ui::VectorDevice* stick );

private:
  PrepareCommonViewListener*    m_pPreCmnViewListener;

  pml::PokeParty*           m_pMyParty;
  app::tool::PokeIcon*      m_pPokeIcon;

  gfl2::lyt::LytTextBox*    m_pNameText[BTN_ID_NUM];
  gfl2::lyt::LytPane*       m_pCursorPane[BTN_ID_NUM];

  u8    m_ButtonMax;

  u8    m_Seq;
};

GFL_NAMESPACE_END( Capture )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_CAPTURE_POKE_LIST_VIEW_H_INCLUDED__
