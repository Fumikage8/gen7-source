//=============================================================================
/**
 * @file    BagPokemon.cpp
 * @brief   バッグ画面ポケモン関連
 * @author  Hiroyuki Nakamura
 * @date    2015.03.18
 */
//=============================================================================

// module
#include "BagPokemon.h"
#include "BagPokemonListener.h"
// pmlib
#include <pml/include/pmlib.h>
// niji
#include "AppLib/include/Tool/PokeIcon.h"
#include "AppLib/include/Tool/app_tool_Gauge.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Util/app_util_Common.h"
#include "PokeTool/include/PokeTool.h"
#include "Sound/include/Sound.h"
// resource
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/common_bag/CommonBag.h"
#include "niji_conv_header/app/common_bag/CommonBag_pane.h"
#include "niji_conv_header/app/common_bag/CommonBag_anim_list.h"

#include "niji_conv_header/field/WeatherKindDef.h"
#include "PokeTool/include/EvolutionUtil.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//! プレート部品ペインインデックステーブル
const gfl2::lyt::LytPaneIndex BagPokemon::PlatePartsTable[] =
{
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_00,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_01,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_02,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_03,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_04,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSPOKE_05,
};

//! アニメデータテーブル
const BagPokemon::ANIME_DATA BagPokemon::AnimeDataTable[] =
{
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_EGG_UNSELECT,
  },
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_EGG_UNSELECT,
  },
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_EGG_UNSELECT,
  },
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_EGG_UNSELECT,
  },
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_EGG_UNSELECT,
  },
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_GENDER_ICON_MALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_GENDER_ICON_FEMALE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_NORMAL_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_NORMAL_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_WAZAOBOE_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_WAZAOBOE_UNSELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_EGG_SELECT,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_EGG_UNSELECT,
  },
};


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   pokemon       手持ちポケモン
 * @param   icon          ポケモンアイコン
 * @param   heap          ヒープ
 * @param   g2d           G2DUtil
 * @param   catch_enable  true = キャッチ処理有効, false = 無効
 * @param   single_pos    シングルモード時のポケモンの並び位置
 * @param   is_demo_mode  デモモードフラグ
 */
//-----------------------------------------------------------------------------
BagPokemon::BagPokemon(
      App::PokeList::IntermediateData * pokemon,
      app::tool::PokeIcon * icon,
      app::tool::AppCommonGrpIconData * common_icon,
      app::util::Heap * heap,
      app::util::G2DUtil * g2d,
      bool catch_enable,
      u32 single_pos,
      const bool * is_demo_mode )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_g2dUtil( g2d )
  , m_pokemon( pokemon )
  , m_commonIcon( common_icon )
  , m_pPokeIcon( icon )
  , m_initLoadCount( 0 )
  , m_listener( NULL )
  , m_selectPos( pml::PokeParty::MAX_MEMBERS )
  , m_subSeq( 0 )
  , m_catchCount( 0 )
  , m_ctrlMode( CTRL_NORMAL )
  , m_catchEnable( catch_enable )
  , m_singleModePos( single_pos )
  , m_isTouchStart( false )
  , m_isDemoMode( is_demo_mode )
{
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    m_plateMode[i] = PLATE_NORMAL;
  }
  GetPane();
  CreateButton();
  CreateHpGauge();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagPokemon::~BagPokemon()
{
  DeleteHpGauge();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputEnabled
 * @brief   入力判定の切り替え
 * @date    2015.06.25
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagPokemon::SetInputEnabled( bool flg )
{
  if( *m_isDemoMode != false )
  {
    flg = false;
  }
  app::ui::UIResponder::SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化
 * @date    2015.04.10
 *
 * @retval  false = 初期化中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagPokemon::Init(void)
{
  if( InitPokeIcon() == false )
  {
    return false;
  }
  InitPane();
  SetInputListener( this );
  SetInputEnabled( false );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.03.19
 */
//-----------------------------------------------------------------------------
void BagPokemon::Update(void)
{
  app::ui::UIView::Update();
  UpdateHpGauge();
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.03.19
 */
//-----------------------------------------------------------------------------
void BagPokemon::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListener
 * @brief   リスナーをセット
 * @date    2015.04.01
 *
 * @param   listener  セットするリスナー
 */
//-----------------------------------------------------------------------------
void BagPokemon::SetListener( BagPokemonListener * listener )
{
  m_listener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPutParty
 * @brief   手持ちポケモンの全てを表示するか（シングルモードか）
 * @date    2015.05.15
 *
 * @retval  true  = 手持ち全て表示（シングルモードじゃない）
 * @retval  false = １匹表示（シングルモード）
 */
//-----------------------------------------------------------------------------
bool BagPokemon::IsPutParty(void)
{
  if( m_singleModePos == pml::PokeParty::MAX_MEMBERS )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetMemberCount
 * @brief   ポケモンの数を取得
 * @date    2015.07.16
 *
 * @return  ポケモンの数
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetMemberCount(void)
{
  if( m_pokemon == NULL )
  {
    return 0;
  }
  return m_pokemon->GetMemberCount();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitMode
 * @brief   表示状態の初期化
 * @date    2015.05.15
 *
 * @param   sel_pos   選択中の位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::InitMode( u32 sel_pos )
{
  // シングルモード
  if( IsPutParty() == false )
  {
    // 全体の座標記憶
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      m_platePos[i] = m_pane[i].plate->GetTranslate();
      if( i != m_singleModePos )
      {
        m_pane[i].plate->SetVisible( false );
      }
    }
    // 指定位置のプレートを固定位置へ
    SetSingleSelectPos();
    // 選択プレートへ変更
    ChangeSelectPlate( m_singleModePos );
  }
  // 通常
  else
  {
    u32 max = GetMemberCount();
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      if( i < max )
      {
        m_pane[i].plate->SetVisible( true );
      }
      else
      {
        m_pane[i].plate->SetVisible( false );
      }
    }
    // 選択プレートへ変更
    if( sel_pos != pml::PokeParty::MAX_MEMBERS )
    {
      ChangeSelectPlate( sel_pos );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSingleSelectPos
 * @brief   シングルモードの表示プレートのY座標にデフォルト値をセット
 * @date    2015.05.15
 */
//-----------------------------------------------------------------------------
void BagPokemon::SetSingleSelectPos(void)
{
  gfl2::math::VEC3 pos = m_platePos[m_singleModePos];
  pos.y = m_platePos[SINGLE_POS_INDEX].y;
  m_pane[m_singleModePos].plate->SetTranslate( pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePlateParty
 * @brief   シングルモード時の手持ち全表示切り替え
 * @date    2015.05.15
 *
 * @param   flg   true = 手持ち表示, false = １匹表示
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangePlateParty( bool flg )
{
  if( IsPutParty() != false )
  {
    return;
  }

  // 表示
  if( flg != false )
  {
    for( u32 i=0; i<GetMemberCount(); i++ )
    {
      m_pane[i].plate->SetVisible( true );
    }
    m_pane[m_singleModePos].plate->SetTranslate( m_platePos[m_singleModePos] );
  }
  // 非表示
  else
  {
    for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      if( i != m_singleModePos )
      {
        m_pane[i].plate->SetVisible( false );
      }
    }
    // 指定位置のプレートを固定位置へ
    SetSingleSelectPos();
    // 選択プレートへ変更
    ChangeSelectPlate( m_singleModePos );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPokeIcon
 * @brief   ポケモンアイコン初期化
 * @date    2015.04.10
 *
 * @retval  false = 初期化中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagPokemon::InitPokeIcon(void)
{
#if 0
  // 非同期版 @note こちらを使用する場合はフレームのアップデートで処理する必要がある
  switch( m_subSeq )
  {
  case 0:
    if( m_initLoadCount >= GetMemberCount() )
    {
      return true;
    }
    m_pPokeIcon->ReadRequest( m_initLoadCount, m_pokemon->GetPokeConst(m_initLoadCount) );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pPokeIcon->IsLoadFinished(m_initLoadCount) == false )
    {
      break;
    }
    m_pPokeIcon->ReplaceReadTexture( m_initLoadCount, m_pane[m_initLoadCount].poke_icon );
    m_initLoadCount++;
    m_subSeq = 0;
    break;
  }
  return false;
#else
  if( m_initLoadCount >= GetMemberCount() )
  {
    return true;
  }
  for( m_initLoadCount=0; m_initLoadCount<GetMemberCount(); m_initLoadCount++ )
  {
    ReplaceTexture( m_initLoadCount, m_pokemon->GetPokeConst(m_initLoadCount) );
  }
  return false;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateButton
 * @brief   ボタン生成
 * @date    2015.04.13
 */
//-----------------------------------------------------------------------------
void BagPokemon::CreateButton(void)
{
  static const app::ui::ButtonInfoEx def_info[] =
  {
    {
      0, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00_PASSIVE,
    },
    {
      1, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01_PASSIVE,
    },
    {
      2, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02_PASSIVE,
    },
    {
      3, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03_PASSIVE,
    },
    {
      4, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04_PASSIVE,
    },
    {
      5, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_TOUCH_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05_PASSIVE,
    },
  };

  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

	app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[pml::PokeParty::MAX_MEMBERS];

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PlatePartsTable[i] );
    info[i] = def_info[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, PANENAME_BAG_PLT_LOW_002_PANE_BOUND_00, &res_id );
  }
  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, pml::PokeParty::MAX_MEMBERS );

  GFL_DELETE_ARRAY info;

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    man->SetButtonSelectSE( i, SEQ_SE_DECIDE1 );
    man->SetButtonInputEnable( i, false );  // 初期化時は押せないようにしておく
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateHpGauge
 * @brief   HPゲージクラス生成
 * @date    2015.04.01
 */
//-----------------------------------------------------------------------------
void BagPokemon::CreateHpGauge(void)
{
  static const u32 anm[][4] =
  {
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_COLOR_FEW,
    },

    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_00__PARTS_HP_01_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_01__PARTS_HP_01_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_02__PARTS_HP_01_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_03__PARTS_HP_01_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_04__PARTS_HP_01_COLOR_FEW,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_01_GAUGE_HP,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_01_COLOR_NORMAL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_01_COLOR_HALF,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSPOKE_05__PARTS_HP_01_COLOR_FEW,
    },
  };

  app::tool::AppToolHPGaugePartsLayout::INIT_PARAM param;
  param.p_g2d_util        = m_g2dUtil;
  param.layout_id         = 0;
  param.number_message_id = msg_bag_02_05;
  param.number_keta       = 3;

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    MakeHpGaugeData( &param, PANENAME_BAG_PLT_LOW_002_PANE_PARTS_HP, i, anm[i] );
    m_pHpGauge[i] = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppToolHPGaugePartsLayout( param, m_heap->GetSystemHeap() );

    MakeHpGaugeData( &param, PANENAME_BAG_PLT_LOW_002_PANE_PARTS_HP_01, i, anm[i+pml::PokeParty::MAX_MEMBERS] );
    m_pHpGauge[i+pml::PokeParty::MAX_MEMBERS] = GFL_NEW(m_heap->GetSystemHeap()) app::tool::AppToolHPGaugePartsLayout( param, m_heap->GetSystemHeap() );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MakeHpGaugeData
 * @brief   HPゲージクラスのデータ生成
 * @date    2015.04.02
 *
 * @param   param   生成場所
 * @param   pane    親ペイン
 * @param   pos     並び位置
 * @param   anm     アニメテーブル
 */
//-----------------------------------------------------------------------------
void BagPokemon::MakeHpGaugeData( app::tool::AppToolHPGaugePartsLayout::INIT_PARAM * param, gfl2::lyt::LytPaneIndex pane, u32 pos, const u32 * anm )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * gauge = lytwk->GetPartsPane( m_pane[pos].plate, pane, &res_id );

  param->p_left_number_text_box_pane   = lytwk->GetTextBoxPane( gauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_00, &res_id );
  param->p_center_number_text_box_pane = lytwk->GetTextBoxPane( gauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01, &res_id );
  param->p_right_number_text_box_pane  = lytwk->GetTextBoxPane( gauge, PANENAME_COMMON_GAG_000_PANE_TEXTBOX_02, &res_id );
  param->gauge_hp_anime_idx            = anm[0];
  param->color_normal_anime_idx        = anm[1];
  param->color_half_anime_idx          = anm[2];
  param->color_few_anime_idx           = anm[3];
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteHpGauge
 * @brief   HPゲージクラス削除
 * @date    2015.04.01
 */
//-----------------------------------------------------------------------------
void BagPokemon::DeleteHpGauge(void)
{
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS*2; i++ )
  {
    GFL_SAFE_DELETE( m_pHpGauge[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateHpGauge
 * @brief   HPゲージ更新
 * @date    2015.04.01
 */
//-----------------------------------------------------------------------------
void BagPokemon::UpdateHpGauge(void)
{
  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS*2; i++ )
  {
    m_pHpGauge[i]->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPane
 * @brief   ペインデータを取得
 * @date    2015.03.19
 */
//-----------------------------------------------------------------------------
void BagPokemon::GetPane(void)
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  for( u32 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    // プレート
    m_pane[i].plate = lytwk->GetPartsPane( PlatePartsTable[i] );
    // 境界ペイン
    m_pane[i].bound = lytwk->GetBoundingPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_BOUND_00, &res_id );
    {
      gfl2::lyt::LytParts * icon = lytwk->GetPartsPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_PARTS_ICONPOKE, &res_id );
      // ポケモンアイコン
      m_pane[i].poke_icon = lytwk->GetPicturePane( icon, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &res_id );
      // 持ち物アイコン
      m_pane[i].item_icon = lytwk->GetPicturePane( icon, PANENAME_COMMON_BTN_LOW_001_PANE_ICON_ITEM, &res_id );
    }
    // 性別
    m_pane[i].sex_icon = lytwk->GetPartsPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_PARTS_GENDER, &res_id );
    // 名前
    m_pane[i].name = lytwk->GetTextBoxPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_TEXTBOX_02, &res_id );
    // 使用できるかなどのコメント
    m_pane[i].use = lytwk->GetTextBoxPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_TEXTBOX_03, &res_id );
    // レベル数値
    m_pane[i].lv = lytwk->GetTextBoxPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_TEXTBOX_01, &res_id );
    // 状態異常アイコン本体
    m_pane[i].st_base = lytwk->GetPartsPane( m_pane[i].plate, PANENAME_BAG_PLT_LOW_002_PANE_PARTS_SICK, &res_id );
    // 状態異常アイコン
    m_pane[i].st_icon = lytwk->GetPicturePane( m_pane[i].st_base, PANENAME_COMMON_ICN_002_PANE_ICON_SICK, &res_id );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ReplaceTexture
 * @brief   ポケモンアイコンのテクスチャを変更
 * @date    2015.03.19
 *
 * @param   id  アイコンID
 * @param   pp  ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::ReplaceTexture( u32 id, const pml::pokepara::PokemonParam * pp )
{
  m_pPokeIcon->ReplacePaneTexture( id, pp, m_pane[id].poke_icon );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPaneCore
 * @brief   プレートの表示を初期化
 * @date    2015.04.17
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::InitPaneCore( u32 pos )
{
  gfl2::lyt::LytWk * lytwk = m_g2dUtil->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = m_g2dUtil->GetLayoutResourceID( 0 );

  const pml::pokepara::PokemonParam * pp = m_pokemon->GetPokeConst( pos );
  bool fast = pp->StartFastMode();
  // 持ち物
  PutItemIcon( pos, pp );
  // 性別
  PutSex( pos, pp );
  // 名前
  m_g2dUtil->SetRegisterPokeNickName( 0, pp );
  m_g2dUtil->SetTextBoxPaneStringExpand( m_pane[pos].name, msg_bag_01_00 );
  // レベル
  PutLevel( pos, pp );
  //「Lv」
  m_g2dUtil->SetTextBoxPaneString(
    lytwk->GetTextBoxPane(m_pane[pos].plate,PANENAME_BAG_PLT_LOW_002_PANE_TEXTBOX_00,&res_id),
    static_cast<u32>(msg_bag_02_04) );
  // HP
  PutHp( pos, pp );
  { //「/」
    static const gfl2::lyt::LytPaneIndex pane[] =
    {
      PANENAME_BAG_PLT_LOW_002_PANE_PARTS_HP,
      PANENAME_BAG_PLT_LOW_002_PANE_PARTS_HP_01,
    };
    for( u32 j=0; j<GFL_NELEMS(pane); j++ )
    {
      gfl2::lyt::LytParts * gauge = lytwk->GetPartsPane( m_pane[pos].plate, pane[j], &res_id );
      m_g2dUtil->SetTextBoxPaneString(
        lytwk->GetTextBoxPane(gauge,PANENAME_COMMON_GAG_000_PANE_TEXTBOX_01,&res_id),
        static_cast<u32>(msg_bag_02_06) );
    }
  }
  pp->EndFastMode( fast );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPane
 * @brief   全プレートの表示を初期化
 * @date    2015.03.20
 */
//-----------------------------------------------------------------------------
void BagPokemon::InitPane(void)
{
  for( u32 i=0; i<GetMemberCount(); i++ )
  {
//    m_pPokeIcon->ReplaceReadTexture( i, m_pane[i].poke_icon );
    InitPaneCore( i );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetPane
 * @brief   全プレートの表示を初期化（ポケモンアイコンを読み込む）
 * @date    2015.04.17
 */
//-----------------------------------------------------------------------------
void BagPokemon::ResetPane(void)
{
  u32 i;
  for( i=0; i<GetMemberCount(); i++ )
  {
    ReplaceTexture( i, m_pokemon->GetPokeConst(i) );
    InitPaneCore( i );
    m_pane[i].plate->SetVisible( true );
  }
  for( ; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    m_pane[i].plate->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemIcon
 * @brief   アイテムアイコンの表示切り替え
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutItemIcon( u32 pos, const pml::pokepara::PokemonParam * pp )
{
  if( pp->GetItem() == ITEM_DUMMY_DATA )
  {
    m_pane[pos].item_icon->SetVisible( false );
  }
  else
  {
    m_pane[pos].item_icon->SetVisible( true );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemIcon
 * @brief   アイテムアイコンの表示切り替え
 * @date    2015.05.01
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutItemIcon( u32 pos, bool flg )
{
  m_pane[pos].item_icon->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemIconPos
 * @brief   アイテムアイコンの表示座標を取得
 * @date    2015.05.01
 *
 * @param   index   並び位置
 *
 * @return  表示座標
 */
//-----------------------------------------------------------------------------
gfl2::math::VEC3 BagPokemon::GetItemIconPos( u32 index )
{
  return m_g2dUtil->GetPanePos( m_pane[index].item_icon );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutSex
 * @brief   性別表示切り替え
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutSex( u32 pos, const pml::pokepara::PokemonParam * pp )
{
  pml::Sex sex = PokeTool::GetDrawSex( pp );
  if( sex == pml::SEX_MALE )
  {
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].sex_male );
    m_pane[pos].sex_icon->SetVisible( true );
  }
  else if( sex == pml::SEX_FEMALE )
  {
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].sex_female );
    m_pane[pos].sex_icon->SetVisible( true );
  }
  else
  {
    m_pane[pos].sex_icon->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutLevel
 * @brief   レベル表示切り替え
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutLevel( u32 pos, const pml::pokepara::PokemonParam * pp )
{
  m_g2dUtil->SetRegisterNumber( 0, pp->GetLevel(), 3 );
  m_g2dUtil->SetTextBoxPaneStringExpand( m_pane[pos].lv, msg_bag_02_05 );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutHp
 * @brief   HP表示切り替え
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutHp( u32 pos, const pml::pokepara::PokemonParam * pp )
{
  u32 hp  = pp->GetHp();
  u32 mhp = pp->GetMaxHp();

  m_pHpGauge[pos]->SetNumber( hp, mhp );
  m_pHpGauge[pos+pml::PokeParty::MAX_MEMBERS]->SetNumber( hp, mhp );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartHpRecoverAnime
 * @brief   HP回復アニメ開始
 * @date    2015.04.21
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 * @param   old   回復前のHP
 */
//-----------------------------------------------------------------------------
void BagPokemon::StartHpRecoverAnime( u32 pos, const pml::pokepara::PokemonParam * pp, u32 old )
{
  u32 hp  = pp->GetHp();
  u32 mhp = pp->GetMaxHp();

  m_pHpGauge[pos]->StartAnime( hp, old, mhp );
  m_pHpGauge[pos+pml::PokeParty::MAX_MEMBERS]->StartAnime( hp, old, mhp );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPlayHpRecoverAnime
 * @brief   HP回復アニメを再生中か
 * @date    2015.04.21
 *
 * @param   pos   並び位置
 *
 * @retval  true  = 再生中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagPokemon::IsPlayHpRecoverAnime( u32 pos )
{
  if( m_pHpGauge[pos]->IsAnime() == false &&
      m_pHpGauge[pos+pml::PokeParty::MAX_MEMBERS]->IsAnime() == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPlate
 * @brief   プレートの表示を初期化（全体）
 * @date    2015.03.23
 *
 * @param   man           アイテムマネージャ
 * @param   item          選択中の道具
 * @param   select_off    true = 非選択状態にする
 */
//-----------------------------------------------------------------------------
void BagPokemon::InitPlate( itemman::ITEM_MANAGER * man, u32 item, bool select_off )
{
  u32 i;
  for( i=0; i<GetMemberCount(); i++ )
  {
    const pml::pokepara::PokemonParam * pp = m_pokemon->GetPokeConst( i );
    InitPlate( i, pp, m_pokemon->IsSickMoudoku(i), man, item, select_off );
  }
/*
  for( i=i; i<pml::PokeParty::MAX_MEMBERS; i++ )
  {
    VisiblePlate( i, false );
  }
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPlate
 * @brief   プレートの表示を初期化（個別）
 * @date    2015.03.23
 *
 * @param   pos           並び位置
 * @param   pp            ポケモンパラメータ
 * @param   man           アイテムマネージャ
 * @param   item          選択中の道具
 * @param   select_off    true = 非選択状態にする
 */
//-----------------------------------------------------------------------------
void BagPokemon::InitPlate( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku, itemman::ITEM_MANAGER * man, u32 item, bool select_off )
{
  // タマゴ
  if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) != false )
  {
    PutEggPlate( pos );
  }
  // 技マシン
  else if( item::ITEM_CheckWazaMachine(item) != false )
  {
    PutWazaMachinePlate( pos, pp, item );
  }
  // 進化アイテム
  else if( man->GetParam( item, item::ITEM_DATA::PRM_ID_WORK_TYPE ) == item::ITEM_DATA::WKTYPE_POKEUSE &&
           man->GetParam( item, item::ITEM_DATA::PRM_ID_EVOLUTION ) != 0 )
  {
    PutEvolutionPlate( pos, pp, item );
  }
  // ビーズ
  else  if( item::ITEM_CheckBeads(item) != false )
  {
    PutBeadsPlate( pos, pp, man, item );
  }
  // その他
  else
  {
    PutPlate( pos, pp, is_moudoku );
  }

  ChangeSelectPlate( pos, !select_off );
}

//-----------------------------------------------------------------------------
/**
 * @func    VisiblePlate
 * @brief   プレートの表示切り替え
 * @date    2015.05.15
 *
 * @param   pos   並び位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagPokemon::VisiblePlate( u32 pos, bool flg )
{
  m_pane[pos].plate->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPlate
 * @brief   プレート配置（通常）
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutPlate( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku )
{
  m_plateMode[pos] = PLATE_NORMAL;

  // 状態異常アイコン
  PutStatusIcon( pos, pp, is_moudoku );
  // HPテキスト
//  m_pHpGauge[pos]->SetTextVisible( false );
  // 性別
  m_pane[pos].sex_icon->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutEggPlate
 * @brief   プレート配置（タマゴ用）
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutEggPlate( u32 pos )
{
  m_plateMode[pos] = PLATE_EGG;

  // 状態異常アイコン
  m_pane[pos].st_base->SetVisible( false );
  // 性別
  m_pane[pos].sex_icon->SetVisible( false );

/*
	// 状態異常アイコン
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].status, false );
	// レベル
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].lv, false );
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].lv_val, false );
	// HPバー
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].hpBar, false );
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].hpBarCol, false );
	// テキスト
	pG2dUtil->SetPaneVisible( BAG_LAYOUT_ID_LOWER, PokePane[pos].msg, false );
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    PutWazaMachinePlate
 * @brief   プレート配置（技マシン用）
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 * @param   item  選択中の道具
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutWazaMachinePlate( u32 pos, const pml::pokepara::PokemonParam * pp, u32 item )
{
  m_plateMode[pos] = PLATE_WAZA;

  // 覚えている
  if( pp->HaveWaza( static_cast<WazaNo>(item::ITEM_GetWazaNo(item)) ) == true ){
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_03 );
//    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].plate_use[0] );
//    m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].plate_use[1] );
  // 覚えられない
  }else if( pp->CheckWazaMachine( item::ITEM_GetWazaMashineNo(item) ) == false ){
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_02 );
//    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].plate_use[0] );
//    m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].plate_use[1] );
  // 覚えられる
  }else{
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_01 );
//    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].plate_use[1] );
//    m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].plate_use[0] );
  }

  // 状態異常アイコン
  m_pane[pos].st_base->SetVisible( false );
  // 性別
  m_pane[pos].sex_icon->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutEvolutionPlate
 * @brief   プレート配置（進化アイテム用）
 * @date    2015.03.23
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 * @param   item  選択中の道具
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutEvolutionPlate( u32 pos, const pml::pokepara::PokemonParam * pp, u32 item )
{
  m_plateMode[pos] = PLATE_WAZA;

  MonsNo mons;
  u32 evolution_root;
  // アローラ以外の場所では通常ポケモンに進化したい要望
  pml::pokepara::EvolveSituation evoSituation;
  // 進化設定 場所の情報だけ欲しいので天気と逆さ状況は適当
  PokeTool::SetupEvolveSituation( &evoSituation, GFL_SINGLETON_INSTANCE(GameSys::GameManager), false, Field::weather::SUNNY );
  // 使える
  if( pp->CanEvolveByItem( &evoSituation, item, &mons, &evolution_root ) == true ){
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_04 );
//    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].plate_use[0] );
//    m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].plate_use[1] );
  // 使えない
  }else{
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_05 );
//    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].plate_use[0] );
//    m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].plate_use[1] );
  }

  // 状態異常アイコン
  m_pane[pos].st_base->SetVisible( false );
  // 性別
  m_pane[pos].sex_icon->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutBeadsPlate
 * @brief   プレート配置（ビーズ用）
 * @date    2015.11.05
 *
 * @param   pos   並び位置
 * @param   pp    ポケモンパラメータ
 * @param   man   アイテムマネージャ
 * @param   item  選択中の道具
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutBeadsPlate( u32 pos, const pml::pokepara::PokemonParam * pp, itemman::ITEM_MANAGER * man, u32 item )
{
  m_plateMode[pos] = PLATE_WAZA;

  u32 piece = GetPiece(man,item);

  // もっている
  if( pp->GetItem() == piece )
  {
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_11 );
  }
  // こうかあり
  else if( pml::waza::ZenryokuWazaSystem::IsZenryokuWazaEnable( pp, piece ) != false )
  {
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_10 );
  }
  // こうかなし
  else
  {
    m_g2dUtil->SetTextBoxPaneString( m_pane[pos].use, msg_bag_09_12 );
  }

  // 状態異常アイコン
  m_pane[pos].st_base->SetVisible( false );
  // 性別
  m_pane[pos].sex_icon->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutStatusIcon
 * @brief   状態異常アイコンを表示
 * @date    2015.07.02
 *
 * @param   pos         並び位置
 * @param   pp          ポケモンパラメータ
 * @param   is_moudoku  true = 猛毒
 */
//-----------------------------------------------------------------------------
void BagPokemon::PutStatusIcon( u32 pos, const pml::pokepara::PokemonParam * pp, bool is_moudoku )
{
  // 瀕死
  if( pp->GetHp() == 0 )
  {
    m_commonIcon->ReplacePaneTextureByPokeHinshiIcon( m_pane[pos].st_icon );
    m_pane[pos].st_base->SetVisible( true );
  }
  // 猛毒
  else if( is_moudoku != false )
  {
    m_commonIcon->ReplacePaneTextureByPokeDokudokuIcon( m_pane[pos].st_icon );
    m_pane[pos].st_base->SetVisible( true );
  }
  // 状態異常
  else if( pp->HaveSick() != false )
  {
    m_commonIcon->ReplacePaneTextureByPokeSickIcon( pp->GetSick(), m_pane[pos].st_icon );
    m_pane[pos].st_base->SetVisible( true );
  }
  // それ以外
  else
  {
    m_pane[pos].st_base->SetVisible( false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    VanishStatusIcon
 * @brief   状態異常アイコンを非表示にする
 * @date    2015.04.21
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::VanishStatusIcon( u32 pos )
{
  m_pane[pos].st_base->SetVisible( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    PlateAnimeOff
 * @brief   プレートアニメを全て停止
 * @date    2015.04.02
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::PlateAnimeOff( u32 pos )
{
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].normal_select_on );
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].normal_select_off );
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].waza_select_on );
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].waza_select_off );
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].egg_select_on );
  m_g2dUtil->StopAnime( 0, AnimeDataTable[pos].egg_select_off );
}

//-----------------------------------------------------------------------------
/**
 * @func    PlateSelectOn
 * @brief   プレートを選択中にする
 * @date    2015.04.02
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::PlateSelectOn( u32 pos )
{
  switch( m_plateMode[pos] )
  {
  case PLATE_NORMAL:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].normal_select_on );
    break;
  case PLATE_WAZA:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].waza_select_on );
    break;
  case PLATE_EGG:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].egg_select_on );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PlateSelectOff
 * @brief   プレートを非選択にする
 * @date    2015.04.02
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::PlateSelectOff( u32 pos )
{
  switch( m_plateMode[pos] )
  {
  case PLATE_NORMAL:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].normal_select_off);
    break;
  case PLATE_WAZA:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].waza_select_off );
    break;
  case PLATE_EGG:
    m_g2dUtil->StartAnime( 0, AnimeDataTable[pos].egg_select_off );
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSelectPlate
 * @brief   プレートの選択状態を切り替え
 * @date    2015.04.01
 *
 * @param   pos   並び位置
 * @param   flg   true = 選択中へ, false = 非選択へ
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangeSelectPlate( u32 pos, bool flg )
{
  PlateAnimeOff( pos );

  if( flg == true )
  {
    PlateSelectOn( pos );
    {
      const pml::pokepara::PokemonParam * pp = m_pokemon->GetPokeConst( pos );
      bool fast = pp->StartFastMode();
      PutSex( pos, pp );
      if( m_plateMode[pos] == PLATE_WAZA )
      {
        PutStatusIcon( pos, pp, m_pokemon->IsSickMoudoku(pos) );
      }
      pp->EndFastMode( fast );
    }
    // 選択時のプライオリティ操作は外で行う
  }
  else
  {
    PlateSelectOff( pos );
    if( m_plateMode[pos] == PLATE_WAZA )
    {
      m_pane[pos].st_base->SetVisible( false );
    }
    m_pane[pos].sex_icon->SetVisible( false );
    m_g2dUtil->AppendChild( 0, PANENAME_BAG_MIN_LOW_000_PANE_NULL_L, PlatePartsTable[pos] );
  }
  // HPテキスト
//  m_pHpGauge[pos]->SetTextVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSelectPlate
 * @brief   選択プレートを切り替え
 * @date    2015.04.13
 *
 * @param   new_pos   新しい位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangeSelectPlate( u32 new_pos )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

  m_selectPos = new_pos;
  for( u32 i=0; i<GetMemberCount(); i++ )
  {
    if( m_selectPos == static_cast<s16>(i) )
    {
      ChangeSelectPlate( i, true );
      if( m_ctrlMode == CTRL_POKE_SELECT )
      {
        man->SetButtonInputEnable( i, true );
      }
      else
      {
        man->SetButtonInputEnable( i, false );
      }
    }
    else
    {
      ChangeSelectPlate( i, false );
//      if( m_ctrlMode == CTRL_POKE_SELECT )
//      {
        man->SetButtonInputEnable( i, false );
//      }
    }
  }
  if( m_selectPos != pml::PokeParty::MAX_MEMBERS )
  {
    m_g2dUtil->AppendChild( 0, PANENAME_BAG_MIN_LOW_000_PANE_NULL_L, PlatePartsTable[m_selectPos] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeSelectPlateSingle
 * @brief   選択プレートを切り替え（シングルモード時）
 * @date    2015.05.15
 *
 * @param   new_pos   新しい位置
 *
 * @li  表示はそのままで、ボタンの切り替えのみ
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangeSelectPlateSingle( u32 new_pos )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

  m_selectPos = new_pos;
  for( u32 i=0; i<GetMemberCount(); i++ )
  {
    if( m_selectPos == static_cast<s16>(i) )
    {
      if( m_ctrlMode == CTRL_POKE_SELECT )
      {
        man->SetButtonInputEnable( i, true );
      }
      else
      {
        man->SetButtonInputEnable( i, false );
      }
    }
    else
    {
//      if( m_ctrlMode == CTRL_POKE_SELECT )
//      {
        man->SetButtonInputEnable( i, false );
//      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveSelectPlate
 * @brief   十字キーでの選択プレート変更
 * @date    2015.04.13
 *
 * @param   mv      移動方向
 * @param   is_trg  true = トリガー入力
 *
 * @retval  true  = 変更した
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagPokemon::MoveSelectPlate( s32 mv, bool is_trg )
{
  u32 max = GetMemberCount();
  if( max <= 1 )
  {
    return false;
  }

  s16 old = m_selectPos;
  m_selectPos += mv;
  if( m_selectPos < 0 )
  {
    if( is_trg != false )
    {
      m_selectPos = max - 1;
    }
    else
    {
      m_selectPos = 0;
    }
  }
  else if( m_selectPos >= static_cast<s16>(max) )
  {
    if( is_trg != false )
    {
      m_selectPos = 0;
    }
    else
    {
      m_selectPos = max - 1;
    }
  }

  if( m_selectPos != old )
  {
    ChangeSelectPlate( m_selectPos );

    app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
    man->SetButtonInputEnable( old, false );
    man->SetButtonInputEnable( m_selectPos, true );
    Sound::PlaySE( SEQ_SE_SELECT1 );

    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePokeSelectMode
 * @brief   ポケモン選択モードを切り替え
 * @date    2015.04.13
 *
 * @param   flg               true = 選択中へ, false = 非選択へ
 * @param   pos               並び位置
 * @param   is_off_anm_lock   true = 非選択アニメをしない
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangePokeSelectMode( bool flg, u32 pos, bool is_off_anm_lock )
{
  if( flg == false )
  {
    pos = m_selectPos;
    m_selectPos = pml::PokeParty::MAX_MEMBERS;
    m_ctrlMode = CTRL_NORMAL;
  }
  else
  {
    m_selectPos = pos;
    m_ctrlMode = CTRL_POKE_SELECT;
  }

  if( flg == false && is_off_anm_lock != false )
  {
    ChangeSelectPlateSingle( m_selectPos );
  }
  else
  {
    ChangeSelectPlate( m_selectPos );
  }
/*
  // @note ChangeSelectPlate()でモードを参照している
  if( flg == false )
  {
    m_ctrlMode = CTRL_NORMAL;
  }
  else
  {
    m_ctrlMode = CTRL_POKE_SELECT;
  }
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    GetSelectPos
 * @brief   プレートの選択位置を取得
 * @date    2015.04.15
 *
 * @return  選択位置
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetSelectPos(void)
{
  return m_selectPos;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSelectPos
 * @brief   プレートの選択位置を設定（シングルモード時のみ）
 * @date    2015.05.15
 */
//-----------------------------------------------------------------------------
void BagPokemon::SetSelectPos(void)
{
  if( IsPutParty() != false )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_selectPos = m_singleModePos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItem
 * @brief   ポケモンの所持道具を取得
 * @date    2015.05.01
 *
 * @param   pos   並び位置
 *
 * @return  所持道具
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetItem( u32 pos )
{
  return m_pokemon->GetPokeConst(pos)->GetItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCatchItem
 * @brief   ポケモンの所持道具をキャッチできるか
 * @date    2015.05.01
 *
 * @retval  true  = 可
 * @retval  false = 不可
 */
//-----------------------------------------------------------------------------
bool BagPokemon::IsCatchItem(void)
{
  // キャッチ操作禁止
  if( m_catchEnable == false )
  {
    return false;
  }

  const pml::pokepara::PokemonParam * pp = m_pokemon->GetPokeConst( m_selectPos );
  if( pp->GetItem() == ITEM_DUMMY_DATA )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeListItemCatchMode
 * @brief   操作を道具リストキャッチモードへ変更
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagPokemon::ChangeListItemCatchMode(void)
{
  m_catchCount = 0;
  m_ctrlMode   = CTRL_LIST_ITEM_CATCH;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTouchPokemon
 * @brief   タッチ中のポケモンの位置を取得
 * @date    2015.05.01
 *
 * @return  並び位置
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetTouchPokemon(void)
{
  for( u32 i=0; i<GetMemberCount(); i++ )
  {
    if( m_g2dUtil->IsPaneTouchHold( 0, m_pane[i].bound ) != false )
    {
      return i;
    }
  }
  return pml::PokeParty::MAX_MEMBERS;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTouchPokemonSingle
 * @brief   タッチ中のポケモンの位置を取得（シングルモード時
 * @date    2015.05.15
 *
 * @return  並び位置
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetTouchPokemonSingle(void)
{
  if( m_g2dUtil->IsPaneTouchHold( 0, m_pane[m_singleModePos].bound ) != false )
  {
    return m_singleModePos;
  }
  return pml::PokeParty::MAX_MEMBERS;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPiece
 * @brief   ビーズに対応したピースを取得
 * @date    2015.05.15
 *
 * @param   man   アイテムマネージャ
 * @param   item  アイテム番号
 *
 * @return  アイテム番号
 */
//-----------------------------------------------------------------------------
u32 BagPokemon::GetPiece( itemman::ITEM_MANAGER * man, u32 item )
{
  u32 max;
  const u16 * tbl = item::ITEM_GetPieceTable( &max );

  int beads_id = man->GetParam( item, item::ITEM_DATA::PRM_ID_WORK );

  for( u32 i=0; i<max; i++ )
  {
    if( man->GetParam(tbl[i],item::ITEM_DATA::PRM_ID_WORK) == beads_id )
    {
      return tbl[i];
    }
  }
  GFL_ASSERT( 0 );
  return tbl[0];
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPokeIconPos
 * @brief   ポケモンアイコンの表示座標を取得
 * @date    2016.01.19
 *
 * @param   index   並び位置
 *
 * @return  表示座標
 */
//-----------------------------------------------------------------------------
gfl2::math::VEC3 BagPokemon::GetPokeIconPos( u32 index )
{
  return m_g2dUtil->GetPanePos( m_pane[index].poke_icon );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetSingleModePos
 * @brief   シングルモード時の対象位置をセット
 * @date    2016.02.05
 *
 * @param   pos   並び位置
 */
//-----------------------------------------------------------------------------
void BagPokemon::SetSingleModePos( u32 pos )
{
  m_singleModePos = pos;
}


//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent
 * @brief   タッチパネルイベントの検知
 * @date    2015.03.31
 *
 * @param   touch_panel タッチパネル
 * @param   is_touch    true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch )
{
  if( GetButtonManager()->GetUpdateState() != app::tool::ButtonManager::UPDATE_STATE_IDLE )
  {
    return ENABLE_ACCESS;
  }

  switch( m_ctrlMode )
  {
  case CTRL_NORMAL:
    return OnTouchEvent_Normal( touch_panel, is_touch );
  case CTRL_POKE_SELECT:
    return OnTouchEvent_PokeSelect(  touch_panel, is_touch );
  case CTRL_ITEM_CATCH:
    return OnTouchEvent_ItemCatch( touch_panel, is_touch );
  case CTRL_LIST_ITEM_CATCH:
    return OnTouchEvent_ListItemCatch( touch_panel, is_touch );
  }
  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.04.13
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnLayoutPaneEvent( const u32 button_id )
{
  m_listener->SelectPlate( ACTION_PLATE_SELECT );
  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.04.10
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_ctrlMode != CTRL_POKE_SELECT )
  {
    return ENABLE_ACCESS;
  }

  if( button->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
    man->StartSelectedAct( m_selectPos );
    return DISABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_UP) )
  {
    MoveSelectPlate( -1, true );
    return DISABLE_ACCESS;
  }

  if( key->IsTrigger(gfl2::ui::DIR_DOWN) )
  {
    MoveSelectPlate( 1, true );
    return DISABLE_ACCESS;
  }

  if( key->IsRepeat(gfl2::ui::DIR_UP) )
  {
    MoveSelectPlate( -1, false );
    return DISABLE_ACCESS;
  }

  if( key->IsRepeat(gfl2::ui::DIR_DOWN) )
  {
    MoveSelectPlate( 1, false );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent_Normal
 * @brief   タッチパネルイベント（通常）
 * @date    2015.05.01
 *
 * @param   tp        タッチパネル
 * @param   is_touch  true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnTouchEvent_Normal( gfl2::ui::TouchPanel * tp, bool is_touch )
{
  if( IsPutParty() == false )
  {
    return ENABLE_ACCESS;
  }

  if( is_touch == false )
  {
    for( u32 i=0; i<GetMemberCount(); i++ )
    {
      ChangeSelectPlate( i, false );
    }
    m_selectPos = pml::PokeParty::MAX_MEMBERS;
    m_listener->SelectPlate( ACTION_PLATE_OFF );
    m_isTouchStart = false;
  }
  else
  {
    if( m_isTouchStart == false )
    {
      if( tp->IsTouchTrigger() == false )
      {
        return ENABLE_ACCESS;
      }
    }

    u32 select = GetTouchPokemon();
    if( m_selectPos != static_cast<s16>(select) )
    {
      ChangeSelectPlate( select );
      Sound::PlaySE( SEQ_SE_SELECT1 );
      m_listener->SelectPlate( ACTION_PLATE_ON );
      m_catchCount = 0;
      if( select != pml::PokeParty::MAX_MEMBERS )
      {
        m_isTouchStart = true;
        return MYSUBVIEW_ONLY_ACCESS;
      }
    }
    else
    {
      if( select != pml::PokeParty::MAX_MEMBERS )
      {
        if( IsCatchItem() != false )
        {
          m_catchCount++;
          if( m_catchCount >= app::util::DEFAULT_GET_COUNT_30F )
          {
//            ChangeItemCatchMode();
            m_catchCount = 0;
            m_ctrlMode = CTRL_ITEM_CATCH;
            {
              u16 x, y;
              tp->GetXY( &x, &y );
              m_listener->PokeItemCatch( x, y, m_selectPos );
            }
            m_listener->SelectPlate( ACTION_GET_ITEM );
            m_isTouchStart = false;
            return DISABLE_ACCESS;
          }
        }
        return MYSUBVIEW_ONLY_ACCESS;
      }
    }
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent_PokeSelect
 * @brief   タッチパネルイベント（ポケモン選択時）
 * @date    2015.05.01
 *
 * @param   tp        タッチパネル
 * @param   is_touch  true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnTouchEvent_PokeSelect( gfl2::ui::TouchPanel * tp, bool is_touch )
{
  if( is_touch != false )
  {
    if( m_isTouchStart == false )
    {
      if( tp->IsTouchTrigger() == false )
      {
        return ENABLE_ACCESS;
      }
    }

    u32 select = GetTouchPokemon();
    if( m_selectPos == static_cast<s16>(select) )
    {
      return ENABLE_ACCESS;
    }
    if( select != pml::PokeParty::MAX_MEMBERS )
    {
      ChangeSelectPlate( select );
      Sound::PlaySE( SEQ_SE_SELECT1 );
      m_isTouchStart = true;
      return MYSUBVIEW_ONLY_ACCESS;
    }
  }
  else
  {
    m_isTouchStart = false;
  }

  return ENABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent_ItemCatch
 * @brief   タッチパネルイベント（ポケモンの所持道具のキャッチムーブ時）
 * @date    2015.05.01
 *
 * @param   tp        タッチパネル
 * @param   is_touch  true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnTouchEvent_ItemCatch( gfl2::ui::TouchPanel * tp, bool is_touch )
{
  if( is_touch == true )
  {
    u16 x, y;
    tp->GetXY( &x, &y );
    if( IsPutParty() != false )
    {
      m_listener->PokeItemCatch( x, y, GetTouchPokemon() );
    }
    else
    {
      m_listener->PokeItemCatch( x, y, GetTouchPokemonSingle() );
    }
    return MYSUBVIEW_ONLY_ACCESS;
  }
  else
  {
    m_listener->SelectPlate( ACTION_PUT_ITEM );
    m_ctrlMode = CTRL_NORMAL;
    return DISABLE_ACCESS;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    OnTouchEvent_ListItemCatch
 * @brief   タッチパネルイベント（道具リストのキャッチムーブ時）
 * @date    2015.05.12
 *
 * @param   tp        タッチパネル
 * @param   is_touch  true = タッチされている
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagPokemon::OnTouchEvent_ListItemCatch( gfl2::ui::TouchPanel * tp, bool is_touch )
{
  if( is_touch == true )
  {
    if( IsPutParty() != false )
    {
      u32 select = GetTouchPokemon();
      if( m_selectPos != static_cast<s16>(select) )
      {
        ChangeSelectPlate( select );  // m_selectPosが変更される
      }
    }
    else
    {
      m_selectPos = GetTouchPokemonSingle();
    }
    u16 x, y;
    tp->GetXY( &x, &y );
    m_listener->PokeItemCatch( x, y, m_selectPos );
    return MYSUBVIEW_ONLY_ACCESS;
  }
  else
  {
    m_listener->SelectPlate( ACTION_PUT_LIST_ITEM );
    m_ctrlMode = CTRL_NORMAL;
    return DISABLE_ACCESS;
  }
}


GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
