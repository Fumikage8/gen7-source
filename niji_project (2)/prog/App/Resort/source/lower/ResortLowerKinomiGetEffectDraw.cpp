//======================================================================
/**
 * @file    ResortLowerKinomiGetEffectDraw.cpp
 * @date    2016/01/22 20:48:57
 * @author  fukushima_yuya
 * @brief   ポケリゾート：きのみ取得エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerKinomiGetEffectDraw.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <App/Resort/source/ResortDrawListener.h>
#include <App/Resort/source/ResortConst.h>
#include <App/Resort/source/ResortUtility.h>

#include <Savedata/include/ResortSave.h>
#include <Savedata/include/MyItemSave.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort.h>
#include <niji_conv_header/app/resort/resort_pane.h>
#include <niji_conv_header/app/resort/resort_anim_list.h>
#include <niji_conv_header/message/msg_pokeresort_system.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace
{
  // エフェクトアニメID
  static const u32 ANIM_ID_GET_EFFECT = LA_RESORT_RESORT_KINOMIEFFECT_LOW_000_MAINANIM;
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   生成パラメータ
 */
//------------------------------------------------------------------------------
LowerKinomiGetEffectDraw::LowerKinomiGetEffectDraw( CreateParam& param )
  : LowerBaseDraw( param )
  , m_DiaryFlag(false)
  , m_pCenterPane(NULL)
  , m_pKinomiIconPane(NULL)
  , m_pTextGetNum(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::Initialize2D( InitParam& param )
{
  const app::util::G2DUtil::LytResData resTbl[] = {
    { param.lytBuff, 1, app::util::G2DUtil::ATTACH_ROOT },
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    {
      0,
      LYTRES_RESORT_RESORT_KINOMIEFFECT_LOW_000_BFLYT,
      LA_RESORT_RESORT_KINOMIEFFECT_LOW_000___NUM,
      param.lytBuff,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  // 2Dの生成
  Create2D(
    m_pLytHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    param.msgSys,
    param.wordSet );

  this->SetVisible( false );
  this->SetInputEnabled( false );
}

//------------------------------------------------------------------------------
/**
 * @brief   クラスの初期化
 *
 * @param   param   初期化パラメータ
 */
//------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::InitializeClass( InitParam& param )
{
  gfl2::heap::HeapBase* heap = m_pLytHeap->GetDeviceHeap();
  {
    // 描画共通処理
    m_pDrawerBase = GFL_NEW(heap) DrawerBase( this, m_pLytHeap, 0, param.wordSet, param.msgSys );
    {
      GFL_ASSERT( m_pDrawerBase );
    }

    // アイテムアイコン
    app::tool::ItemIconToolEx::ItemIconToolParam itemIconParam;
    {
      itemIconParam.pUIView = this;
      itemIconParam.pHeap   = m_pIconHeap;
      itemIconParam.iconMax = 1;
      itemIconParam.sync    = true;
      itemIconParam.allocBuffer = true;
    }
    m_pDrawerBase->CreateItemIcon( itemIconParam );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインの初期化
 */
//------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::InitializePane( void )
{
  m_pCenterPane     = m_pDrawerBase->GetPane( PANENAME_RESORT_KINOMIEFFECT_LOW_000_PANE_NULL_CENTER );
  m_pKinomiIconPane = m_pDrawerBase->GetPicture( PANENAME_RESORT_KINOMIEFFECT_LOW_000_PANE_KINOMIICON );
  m_pMameIconPane   = m_pDrawerBase->GetPicture( PANENAME_RESORT_KINOMIEFFECT_LOW_000_PANE_MAMEICON );
  m_pTextGetNum     = m_pDrawerBase->GetTextBox( PANENAME_RESORT_KINOMIEFFECT_LOW_000_PANE_TEXTBOX_00 );
}


//------------------------------------------------------------------------------
/**
 * @brief   エフェクト再生
 *
 * @param   type    取得するアイテムの種類
 * @param   index   きのみのインデックス
 * @param   x       X座標
 * @param   y       Y座標
 */
//------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::PlayGetEffect( ItemType type, int index, int x, int y )
{
  if( type == ITEM_TYPE_KINOMI )
  {
    PlayGetEffect_Kinomi( index, x, y );
  }
  else {
    PlayGetEffect_Mame( x, y );
  }

  m_pDrawerBase->SetVisiblePane( m_pKinomiIconPane, (type == ITEM_TYPE_KINOMI) );
  m_pDrawerBase->SetVisiblePane( m_pMameIconPane,  !(type == ITEM_TYPE_KINOMI) );
}

//------------------------------------------------------------------------------
/**
 * @brief   エフェクト再生：きのみ取得
 *
 * @param   index   きのみのインデックス
 * @param   x       X座標
 * @param   y       Y座標
 */
 //------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::PlayGetEffect_Kinomi( int index, int x, int y )
{
  // きのみが収穫できるかを確認
  {
    TimeParam time;
    // 時間を取得
    m_pResortSave->GetFarmTime( index, &time.hour, &time.min, &time.sec );

    if( time.GetTotalSec() > 0 )
    {
      // 時間が０より大きいとき、収穫はできない
      return;
    }
  }


  FUKUSHIMA_PRINT( "------------------------------------\n" );
  FUKUSHIMA_PRINT( "PlayGetEffect\n" );

  // 座標の更新
  gfl2::math::VEC3 pos(0, 0, 0);
  {
    // スクリーン座標からレイアウト座標へ変更
    pos.x = x - 160;
    pos.y = 120 - y;

    m_pCenterPane->SetTranslate( pos );

    FUKUSHIMA_PRINT( "touch : x[%d] y[%d]\n", x, y );
    FUKUSHIMA_PRINT( "pos : x[%.2f] y[%.2f]\n", pos.x, pos.y );
  }

  // きのみアイコンの設定
  u16 kinomiID = m_pResortSave->GetFarmItemKind( index );
  {
    app::tool::ItemIconToolEx* itemIcon = m_pDrawerBase->GetItemIcon();
    {
      itemIcon->SetItemIcon( 0, kinomiID, m_pKinomiIconPane );

      FUKUSHIMA_PRINT( "ItemNo[%d]\n", kinomiID );
    }
  }

  // 取得するきのみの数
  u8 num = m_pResortSave->GetFarmNum( index );
  {
    this->SetTextboxPaneNumber( m_pTextGetNum, msg_prsys_common_15, num, 2 );

    FUKUSHIMA_PRINT( "ItemNum[%d]\n", num );
  }

  // セーブデータへの反映
  {
    // 収穫アイテムを反映
    m_pItemSave->Add( kinomiID, num );

    // 初期化
    m_pResortSave->SetFarmItemKind( index, ITEM_DUMMY_DATA );   //!< アイテム
    m_pResortSave->SetFarmNum( index, 0 );                      //!< アイテム数
    //m_pResortSave->SetFarmTime( index, 0xFF, 0xFF, 0xFF );      //!< 時間// @fix GFNMCat[4177]

    // レコード：回数をカウント
    Savedata::IncRecord( Savedata::Record::RECID_BERRY_HARVEST );

    // ダイアリーのセット
    if( !m_DiaryFlag )
    {
      PokeDiary::Set( PDID_COUNT_GET_SEED, kinomiID, num );
      m_DiaryFlag = true;
    }
  }

  // アニメの再生
  m_pDrawerBase->StartAnim( ANIM_ID_GET_EFFECT );

  // SE再生
  Sound::PlaySE( SEQ_SE_SYS_NJ_047 );   // @fix GFNMCat[3418]

  // Viewの表示
  this->SetVisible( true );

  // 上画面への反映
  m_uiListener->UpdateUpper( RESORT_ID_FARM );

  FUKUSHIMA_PRINT( "------------------------------------\n" );
}

//------------------------------------------------------------------------------
/**
 * @brief   エフェクト再生：マメ取得
 *
 * @param   x       X座標
 * @param   y       Y座標
 */
//------------------------------------------------------------------------------
void LowerKinomiGetEffectDraw::PlayGetEffect_Mame( int x, int y )
{
  // 座標の更新
  gfl2::math::VEC3 pos(0, 0, 0);
  {
    // スクリーン座標からレイアウト座標へ変更
    pos.x = x - 160;
    pos.y = 120 - y;

    m_pCenterPane->SetTranslate( pos );

    FUKUSHIMA_PRINT( "touch : x[%d] y[%d]\n", x, y );
    FUKUSHIMA_PRINT( "pos : x[%.2f] y[%.2f]\n", pos.x, pos.y );
  }

  const u32 pokeMameCount = 7;

  // 取得するマメの数
  this->SetTextboxPaneNumber( m_pTextGetNum, msg_prsys_common_15, pokeMameCount, 2 );

  // セーブデータに反映
  util::AddPokeMame( m_pResortSave, Savedata::ResortSave::POKEBEANS_RANK_NORMAL, pokeMameCount );

  // アニメの再生
  m_pDrawerBase->StartAnim( ANIM_ID_GET_EFFECT );

  // SE再生
  Sound::PlaySE( SEQ_SE_SYS_NJ_046 );

  // Viewの表示
  this->SetVisible( true );
}


//------------------------------------------------------------------------------
/**
 * @brief   エフェクト再生の終了チェック
 *
 * @return  "true  == 終了"
 * @return  "false == 再生中"
 */
//------------------------------------------------------------------------------
bool LowerKinomiGetEffectDraw::IsAnimEndGetEffect( void )
{
  return m_pDrawerBase->IsAnimEnd( ANIM_ID_GET_EFFECT );
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
