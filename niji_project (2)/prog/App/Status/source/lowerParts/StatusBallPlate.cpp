//======================================================================
/**
 * @file    StatusBallPlate.cpp
 * @date    2015/12/03 16:50:15
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ボールプレート(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusBallPlate.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>
#include <niji_conv_header/app/field_status/field_status_anim_list.h>

#include <niji_conv_header/message/msg_pokemon_status.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  //! @brief  ボールのアニメ
  const u32 c_BallAnimTbl[BallPlate::BALL_MAX][BallPlate::BALL_STATE_NUM] = {
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_00,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_00,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_00,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_01,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_01,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_01,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_02,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_02,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_02,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_03,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_03,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_03,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_04,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_04,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_04,
    },
    {
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_SELECT_05,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_UNSELECT_05,
      LA_FIELD_STATUS_FIELDSTATUS_MIN_LOW_000__PARTS_BALLS_KEEP_05,
    },
  };

  static const u32 BALL_NULL = pml::PokeParty::MAX_MEMBERS;
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
BallPlate::BallPlate( DrawerBase* base )
  : m_pBase(base)
  , m_pListener(NULL)
  , m_StatusMode(MODE_PARTY)
  , m_MemberCount(0)
  , m_SelectIndex(0xFFFFFFFF)
  , m_TriggerIndex(BALL_NULL)
  , m_pPlateParts(NULL)
  , m_BallID(item::BALLID_NULL)
  , m_IconSeq(0)
{
  for( u32 i=0; i<BALL_MAX; ++i )
  {
    m_BallState[i] = BALL_STATE_NULL;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool BallPlate::Terminate( void )
{
  if( !DeleteItemIcon() ) return false;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 *
 * @param   listener    リスナー
 * @param   mode        ステータス画面の表示モード
 */
//------------------------------------------------------------------------------
void BallPlate::Initialize( BallPlateListener* listener, Mode mode )
{
  // ボールプレート全体の非表示フラグ
  m_Disable     = ((mode != MODE_PARTY) && (mode != MODE_BFC));
  // ボールプレート(下)リスナーをセット
  m_pListener   = listener;
  // ステータス画面の表示モードをセット
  m_StatusMode  = mode;

  // ペインのセット
  {
    // ボールプレートパーツ
    m_pPlateParts = m_pBase->GetParts( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_PARTS_BALLS );

    // ボールペイン
    const u32 c_BallPane[] = {
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_00,
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_01,
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_02,
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_03,
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_04,
      PANENAME_STATUS_PLT_LOW_002_PANE_BALL_05,
    };
    // ボール境界ペイン
    const u32 c_BallBound[] = {
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_00,
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_01,
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_02,
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_03,
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_04,
      PANENAME_STATUS_PLT_LOW_002_PANE_BOUND_05,
    };

    for( u32 i=0; i<BALL_MAX; ++i )
    {
      m_pBallPane[i]  = m_pBase->GetPicture( c_BallPane[i], m_pPlateParts );
      m_pBallBound[i] = m_pBase->GetPane( c_BallBound[i], m_pPlateParts );
    }

    // 捕獲ボールアイコン
    m_pPicGetBallIcon = m_pBase->GetPicture( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_BALL_00 );
  }

  // アイテムアイコンの生成
  CreateItemIcon();

  // ボールプレートの表示切替
  {
    // 表示フラグに合わせて入力もフラグも設定しておく
    m_InputEnable = !m_Disable;

    // ボールプレートの表示切替
    m_pBase->SetVisiblePane( m_pPlateParts, !m_Disable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void BallPlate::Update( void )
{
  if( !m_Disable )
  {
    // ボールアニメの更新
    UpdateBallAnim();
  }
  else {
    // ボールアイコンの更新
    UpdateBallIcon();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ
 *
 * @param   party   ポケパーティ
 */
//------------------------------------------------------------------------------
void BallPlate::Setup( const pml::PokeParty* party )
{
  if( (m_StatusMode == MODE_PARTY) || (m_StatusMode == MODE_BFC) )
  {
    // 手持ちの数を取得
    m_MemberCount = party->GetMemberCount();

    for( u32 i=0; i<BALL_MAX; ++i )
    {
      if( i < m_MemberCount )
      {
        // ポケモンパラメータの取得
        const pml::pokepara::PokemonParam* pp = party->GetMemberPointerConst( i );

        bool fastmode_flag = pp->StartFastMode();
        {
          SetupBallPlate( i, pp );
        }
        pp->EndFastMode( fastmode_flag );
      }
      else {
        // ボールプレートのセットアップ
        SetupBallPlate( i, NULL );
      }
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボールプレートのセットアップ
 *
 * @param   index     手持ちのインデックス
 * @param   pp        ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void BallPlate::SetupBallPlate( u32 index, const pml::pokepara::PokemonParam* pp )
{
  if( pp )
  {
    // 捕まえたボールを取得
    item::BALL_ID ballID = static_cast<item::BALL_ID>( pp->GetGetBall() );

    // 念のための不正チェック
    if( ballID == item::BALLID_NULL || ballID > item::BALLID_MAX )
    {
      ballID = item::BALLID_MONSUTAABOORU;
    }

    // ボールIDからアイテムNoに変更
    u32 itemNo = item::ITEM_BallID2ItemID( ballID );

    // アイコンの変更
    app::tool::ItemIconToolEx* itemIcon = m_pBase->GetItemIcon();
    itemIcon->SetItemIcon( index, itemNo, m_pBallPane[index] );
  }

  // ボールアイコンの表示切替
  m_pBase->SetVisiblePane( m_pBallPane[index], (pp != NULL) );
}

//------------------------------------------------------------------------------
/**
 * @brief   セットアップ：捕獲のボール
 *
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void BallPlate::Setup_GetBall( const pml::pokepara::PokemonParam* pp )
{
  if( m_Disable )
  {
    bool fastmode = pp->StartFastMode();
    {
      // 捕獲したボールID
      m_BallID = pp->GetGetBall();
    }
    pp->EndFastMode( fastmode );

    // シーケンスの初期化
    m_IconSeq = 1;
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンの生成
 */
//------------------------------------------------------------------------------
void BallPlate::CreateItemIcon( void )
{
  app::tool::ItemIconToolEx::ItemIconToolParam param;
  {
    param.pUIView = m_pBase->GetUIView();
    param.pHeap   = m_pBase->GetHeap();
    param.iconMax = BALL_MAX;
    param.sync    = true;
  }

  // アイテムアイコンの生成
  m_pBase->CreateItemIcon( param );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムアイコンの破棄
 */
//------------------------------------------------------------------------------
bool BallPlate::DeleteItemIcon( void )
{
  app::tool::ItemIconToolEx* itemIcon = m_pBase->GetItemIcon();

  if( itemIcon )
  {
    if( !itemIcon->EndFunc() ) return false;

    // アイテムアイコンの破棄
    m_pBase->DeleteItemIcon();
  }

  return true;
}


//------------------------------------------------------------------------------
/**
 * @brief   表示切替
 *
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void BallPlate::SetVisible( bool visible )
{
  if( !m_Disable )
  {
    // 表示フラグに合わせて入力もフラグも設定しておく
    m_InputEnable = visible;

    // ボールプレートの表示切替
    m_pBase->SetVisiblePane( m_pPlateParts, visible );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボールインデックスの設定
 *
 * @param   index   ボールインデックス
 * @param   playSE  SEの再生フラグ(デフォルトはtrue)
 */
//------------------------------------------------------------------------------
void BallPlate::SetBallIndex( u32 index, bool playSE )
{
  if( m_Disable ) return;
  if( !m_InputEnable ) return;

  if( m_SelectIndex != index )
  {
    // ボールアニメの開始
    StartBallAnim( index );

    // SE再生
    if( playSE )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );
    }

    // インデックスを更新
    m_SelectIndex = index;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチ入力
 *
 * @param   touch   タッチパネル
 */
//------------------------------------------------------------------------------
void BallPlate::TouchBall( gfl2::ui::TouchPanel* touch )
{
  //if( m_Disable ) return;
  //if( !m_InputEnable ) return;

  //for( u32 i=0; i<m_MemberCount; ++i )
  //{
  //  // ペインのタッチ判定
  //  if( m_pBase->IsHoldPane( touch, m_pBallBound[i] ) )
  //  {
  //    // 選択されていない場所なら反映する
  //    if( m_SelectIndex != i )
  //    {
  //      // ボールインデックスの設定
  //      SetBallIndex( i );

  //      // リスナーへの通知
  //      m_pListener->SetTouchedBallIndex( i );
  //      break;
  //    }
  //  }
  //}

  if( !m_Disable && m_InputEnable )
  {
    TouchBall_Trigger( touch );
    TouchBall_Hold( touch );
    TouchBall_Release( touch );
  }
}

void BallPlate::TouchBall_Trigger( gfl2::ui::TouchPanel* touch )
{
  if( touch->IsTouchTrigger() )
  {
    u32 ballIndex = _TouchBall( touch );

    if( ballIndex != BALL_NULL )
    {
      // ボールインデックスの設定
      SetBallIndex( ballIndex );

      // リスナーへの通知
      m_pListener->SetTouchedBallIndex( ballIndex );

      m_TriggerIndex = ballIndex;
    }
  }
}

void BallPlate::TouchBall_Hold( gfl2::ui::TouchPanel* touch )
{
  if( m_TriggerIndex < m_MemberCount )
  {
    u32 ballIndex = _TouchBall( touch );

    if( ballIndex == BALL_NULL ) return;

    // 選択されていない場所なら反映する
    if( m_SelectIndex != ballIndex )
    {
      // ボールインデックスの設定
      SetBallIndex( ballIndex );

      // リスナーへの通知
      m_pListener->SetTouchedBallIndex( ballIndex );
    }
  }  
}

void BallPlate::TouchBall_Release( gfl2::ui::TouchPanel* touch )
{
  if( touch->IsTouchRelease() )
  {
    m_TriggerIndex = BALL_NULL;
  }
}

u32 BallPlate::_TouchBall( gfl2::ui::TouchPanel* touch )
{
  for( u32 i=0; i<m_MemberCount; ++i )
  {
    // ペインのタッチ判定
    if( m_pBase->IsHoldPane( touch, m_pBallBound[i] ) )
    {
      return i;
    }
  }

  return BALL_NULL;
}

//------------------------------------------------------------------------------
/**
 * @brief   ボールアニメの開始
 *
 * @param   index   ボールインデックス
 */
//------------------------------------------------------------------------------
void BallPlate::StartBallAnim( u32 index )
{
  const u32* newSelectAnim = c_BallAnimTbl[index];
  const u32* prevSelectAnim = c_BallAnimTbl[m_SelectIndex];

  for( u8 i=0; i<BALL_STATE_NUM; ++i )
  {
    m_pBase->StopAnim( newSelectAnim[i] );
      
    if( m_SelectIndex < BALL_MAX )
    {
      m_pBase->StopAnim( prevSelectAnim[i] );
    }
  }

  // 選択アニメ
  m_pBase->StartAnim( newSelectAnim[BALL_STATE_SELECT] );
  m_BallState[index] = BALL_STATE_SELECT;

  // 選択解除アニメ
  if( m_SelectIndex < m_MemberCount )
  {
    m_pBase->StartAnim( prevSelectAnim[BALL_STATE_UNSELECT] );
    m_BallState[m_SelectIndex] = BALL_STATE_UNSELECT;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ボールアニメの更新
 */
//------------------------------------------------------------------------------
void BallPlate::UpdateBallAnim( void )
{
  for( u32 i=0; i<m_MemberCount; ++i )
  {
    // 選択されたボール
    if( m_BallState[i] == BALL_STATE_SELECT )
    {
      // 選択アニメが終了したらキープアニメに変更
      if( m_pBase->IsAnimEnd( c_BallAnimTbl[i][BALL_STATE_SELECT] ) )
      {
        m_pBase->StopAnim( c_BallAnimTbl[i][BALL_STATE_SELECT] );
        m_pBase->StartAnim( c_BallAnimTbl[i][BALL_STATE_KEEP] );

        m_BallState[i] = BALL_STATE_KEEP;
      }
    }
    // 選択が解除されたボール
    else if( m_BallState[i] == BALL_STATE_UNSELECT )
    {
      // 選択解除アニメが終了したらアニメを停止させる
      if( m_pBase->IsAnimEnd( c_BallAnimTbl[i][BALL_STATE_UNSELECT] ) )
      {
        m_pBase->StopAnim( c_BallAnimTbl[i][BALL_STATE_UNSELECT] );

        m_BallState[i] = BALL_STATE_NULL;
      }
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ボールアイコンの変更
 */
//------------------------------------------------------------------------------
void BallPlate::UpdateBallIcon( void )
{
  switch( m_IconSeq )
  {
  case 1:
    {
      // ボールアイコンを非表示
      m_pBase->SetVisiblePane( m_pPicGetBallIcon, false );

      m_IconSeq++;
    } break;

  case 2:
    {
      // 捕まえたボールを取得
      item::BALL_ID ballID = static_cast<item::BALL_ID>( m_BallID );

      // 念のための不正チェック
      if( ballID == item::BALLID_NULL || ballID > item::BALLID_MAX )
      {
        ballID = item::BALLID_MONSUTAABOORU;
      }

      // ボールIDからアイテムNoに変更
      u32 itemNo = item::ITEM_BallID2ItemID( ballID );

      app::tool::ItemIconToolEx* itemIcon = m_pBase->GetItemIcon();
      {
        // アイコンの差し替え
        itemIcon->SetItemIcon( 0, itemNo, m_pPicGetBallIcon );
      }

      m_IconSeq++;
    } break;

  case 3:
    {
      // ボールアイコンを表示
      m_pBase->SetVisiblePane( m_pPicGetBallIcon, true );

      m_IconSeq = 0;
    } break;
  }
}

GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
