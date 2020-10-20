//======================================================================
/**
 * @file    BoxSearchPokeIcon.cpp
 * @date    2015/08/06 13:24:46
 * @author  fukushima_yuya
 * @brief   ポケモンの条件検索で表示するポケモンアイコン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "App/BoxSearch/source/parts/BoxSearchPokeIcon.h"
#include "App/BoxSearch/source/BoxSearchDrawerBase.h"

#include <AppLib/include/Tool/PokeIcon.h>
#include <PokeTool/include/PokeTool.h>

// arc
#include <niji_conv_header/app/box/res2d/srclow_pane.h>
#include <niji_conv_header/app/box/res2d/srclow_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  リストボタンパーツ
  static const u32 c_ListButtonParts[] = {
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_00,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_01,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_02,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_03,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_04,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_05,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_06,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_07,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_08,
    PANENAME_BOX_SRC_LOW_000_PANE_LIST_S_09,
  };

  //! @brief  アイコンパーツ
  static const u32 POKE_ICON_PARTS = PANENAME_BOX_BTN_LOW_000_PANE_ICON_POKE;

  //! @brief  アイコンピクチャペイン
  static const u32 POKE_ICON_PICTURE = PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON;
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeIcon::PokeIcon( DrawerBase* pBase, SortData* pSortData, Savedata::ZukanData* pZukanData, u32 btnMax )
  : m_pBase( pBase )
  , m_pSortData( pSortData )
  , m_pZukanData( pZukanData )
  , m_pPokeIcon( NULL )
  , m_ButtonMax( btnMax )
  , m_pIconPane( NULL )
  , m_pSimpleParam( NULL )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // ポケモンアイコン
  m_pPokeIcon = GFL_NEW(devHeap) app::tool::PokeIcon( devHeap, devHeap, btnMax );
  GFL_ASSERT( m_pPokeIcon );

  m_pPokeIcon->FileOpenSync( devHeap ); //!< 同期版

  // ピクチャペインのセット
  m_pIconPane = GFL_NEW_ARRAY(devHeap) gfl2::lyt::LytPicture*[btnMax];
  GFL_ASSERT( m_pIconPane );

  for( u32 i=0; i<btnMax; ++i )
  {
    gfl2::lyt::LytParts* btnParts = m_pBase->GetParts( c_ListButtonParts[i] );
    gfl2::lyt::LytParts* iconParts = m_pBase->GetParts( POKE_ICON_PARTS, btnParts );
    m_pIconPane[i] = m_pBase->GetPicture( POKE_ICON_PICTURE, iconParts );
  }

  // 描画用パラメータ構造体
  m_pSimpleParam = GFL_NEW_ARRAY(devHeap) PokeTool::SimpleParam[btnMax];
  GFL_ASSERT( m_pSimpleParam );

  // 読み込み確認フラグ
  m_pLoadingIcon = GFL_NEW_ARRAY(devHeap) bool[btnMax];
  GFL_ASSERT( m_pLoadingIcon );

  for( u32 i=0; i<btnMax; ++i )
  {
    m_pLoadingIcon[i] = false;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeIcon::~PokeIcon( void )
{
  GFL_SAFE_DELETE_ARRAY( m_pLoadingIcon );
  GFL_SAFE_DELETE_ARRAY( m_pSimpleParam );
  GFL_SAFE_DELETE_ARRAY( m_pIconPane );

  if( m_pPokeIcon )
  {
    m_pPokeIcon->FileCloseSync();
    
    GFL_DELETE m_pPokeIcon;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void PokeIcon::Update( void )
{
  for( u32 i=0; i<m_ButtonMax; ++i )
  {
    ReplaceIcon( i ); //!< アイコンの差し替え
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンアイコンのセット
 * @param   pane_index    ペインのインデックス
 * @param   monsNo        ポケモンNo
 */
//------------------------------------------------------------------------------
void PokeIcon::SetPokeIcon( u32 pane_index, MonsNo monsNo )
{
  u32 index = pane_index;

  gfl2::lyt::LytPicture* picture = m_pIconPane[index];

  if( m_pLoadingIcon[index] )
  {
    bool res = m_pPokeIcon->CancelLoadTextureRequest( index );
    //GFL_ASSERT( res );  //!< 読み込みキャンセルに失敗
    if( !res ) return;

    m_pLoadingIcon[index] = false;
  }

  // アイコン描画用パラメータの変更
  PokeTool::SimpleParam* sp = &m_pSimpleParam[pane_index];

  if( sp->monsNo == monsNo ) return;  //!< 同じ場所に同じポケモンを指定した場合は何もしない

  sp->monsNo = monsNo;

  // 読み込みリクエスト
  if( m_pPokeIcon->ReadRequest( index, sp ) )
  {
    m_pLoadingIcon[index] = true;

    // アイコンを非表示
    m_pBase->SetVisiblePane( picture, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   index   アイコンインデックス
 */
//------------------------------------------------------------------------------
void PokeIcon::ReplaceIcon( u32 index )
{
  if( !m_pLoadingIcon[index] ) return;    //!< 読み込みをしていない
  if( !m_pPokeIcon->IsLoadFinished( index ) ) return;   //!< テクスチャの読み込みが出来ていない

  gfl2::lyt::LytPicture* picture = m_pIconPane[index];

  // テクスチャの置き換え
  m_pPokeIcon->ReplaceReadTexture( index, picture );

  m_pLoadingIcon[index] = false;

  // アイコンを非表示
  m_pBase->SetVisiblePane( picture, true );
}


GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )
