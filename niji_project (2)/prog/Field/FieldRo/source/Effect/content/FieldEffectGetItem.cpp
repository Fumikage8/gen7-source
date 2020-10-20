//======================================================================
/**
 * @file FieldEffectGetItem.cpp
 * @date 2016/01/19
 * @author saita_kazuki
 * @brief アイテム取得1枚絵
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "Field/FieldRo/include/Effect/content/FieldEffectGetItem.h"

// field
#include "Field/FieldRo/include/Window/FieldWindowManager.h"
#include "Field/FieldRo/include/FieldLayoutPriority.h"

// gfl2
#include <Layout/include/gfl2_LytSys.h>

// conv_header layout
// アイテム1枚絵エフェクトはメモリサイズの関係上、エフェクトを4つに分割しているが、
// リソースの構成は同じなので定義は↓を使う
#include <niji_conv_header/field/effect/get_item_1.h>
#include <niji_conv_header/field/effect/get_item_1_anim_list.h>
#include <niji_conv_header/field/effect/get_item_1_pane.h>

// arc
#include <arc_index/message.gaix>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

static const s32 EFFECT_LAYOUT_HEAP_SIZE    = 0xE00;
static const s32 EFFECT_LAYOUT_DEVHEAP_SIZE = 0xEE00;

/**
 *  @brief  コンストラクタ
 */
EffectGetItem::EffectGetItem( void )
: IEffectBase()
, m_appUtilHeap()
, m_pLayout( NULL )
, m_pWindowManager( NULL )
, m_animationIndex( 0)
{
}

/**
 *  @brief  デストラクタ
 */
EffectGetItem::~EffectGetItem( void )
{
}

/**
 *  @brief  初期化
 */
void EffectGetItem::Initialize( SetupData &setupData )
{
  m_pWindowManager = setupData.pWinMgr;

  // ローカルヒープ作成
  m_appUtilHeap.LocalHeapCreate( setupData.pHeap, setupData.pHeap, EFFECT_LAYOUT_HEAP_SIZE, EFFECT_LAYOUT_DEVHEAP_SIZE );

  // レイアウト生成
  app::util::AppRenderingManager *pRender = m_pWindowManager->GetRenderingManager();
  m_pLayout = GFL_NEW( setupData.pHeap ) Field::Effect::EffectLayout( &m_appUtilHeap, pRender );

  // レイアウト初期化
  {
    app::util::G2DUtil::LytResData resTbl[] = 
    {
      {
        setupData.pResource,
        0,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };

    app::util::G2DUtil::LytwkData dataTbl[] =
    {
      {
        0,
        LYTRES_GET_ITEM_1_GETITEM_WIN_UPP_01_BFLYT,
        LA_GET_ITEM_1_GETITEM_WIN_UPP_01___NUM,
        setupData.pResource,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    Field::Effect::EffectLayout::SetupParam param;
    param.layoutNum = 1;
    param.lytResTbl = resTbl;
    param.resTblNum = GFL_NELEMS( resTbl );
    param.lytDataTbl = dataTbl;
    param.dataTblNum = GFL_NELEMS( dataTbl );
    m_pLayout->Initialize( param );
  }

  // 登録
  m_pWindowManager->GetUIView()->AddSubView( m_pLayout );
  m_pLayout->SetVisible( false );
  m_pLayout->SetPriority( Field::FIELD_LAYOUT_PRIORITY_3D );
}

/**
 *  @brief  破棄
 */
bool EffectGetItem::Terminate( void )
{
  // 一応呼ぶ
  this->End();

  GFL_SAFE_DELETE( m_pLayout );
  m_appUtilHeap.Delete();
  return true;
}

/**
 *  @brief  更新
 */
void EffectGetItem::updateNormal( void )
{
}

/**
 *  @brief  終了しているか.
 */
bool EffectGetItem::IsAnimationLastFrame( void )
{
  if( this->IsLayoutInitialized() == false )
  {
    return true;
  }

  return false;
}

/**
 *  @brief  各エフェクトで必要な破棄作業
 */
void EffectGetItem::Delete( void )
{
  this->End();

  // 登録は親からするが解除は子からする
  m_pLayout->RemoveFromSuperView();
}

/**
 *  @brief  開始
 *  @param  index 番号
 */
void EffectGetItem::Start( u32 index)
{
  if( this->IsLayoutInitialized() == false)
  {
    GFL_ASSERT_MSG( 0, "Layout is not initialize\n");
    return;
  }

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();

  m_pLayout->SetVisible( true );
  m_pLayout->StartAnimation( LA_GET_ITEM_1_GETITEM_WIN_UPP_01_GETITEM_00, false, true);
  pUtil->SetAnimeSpeed( 0, 0.0f);

  this->Change( index);

#if PM_DEBUG
  SAITA_PRINT( "----- GetItem Effect Heap Dump Start -----\n");
  // sys
  {
    u32 allocatableSize = m_appUtilHeap.GetSystemHeap()->GetTotalAllocatableSize();
    bool isFragment = m_appUtilHeap.GetSystemHeap()->IsFragment();
    SAITA_PRINT( "  System Heap : Allocatable Size = %x\n", allocatableSize);
    SAITA_PRINT( "  System Heap : Fragment? = %s\n", isFragment ? "o" : "x");
  }
  // dev
  {
    u32 allocatableSize = m_appUtilHeap.GetDeviceHeap()->GetTotalAllocatableSize();
    bool isFragment = m_appUtilHeap.GetDeviceHeap()->IsFragment();
    SAITA_PRINT( "  Device Heap : Allocatable Size = %x\n", allocatableSize);
    SAITA_PRINT( "  Device Heap : Fragment? = %s\n", isFragment ? "o" : "x");
  }
  SAITA_PRINT( "----- GetItem Effect Heap Dump End -----\n");
#endif // PM_DEBUG
}

/**
 *  @brief  終了
 */
void EffectGetItem::End( void )
{
  if( this->IsLayoutInitialized() == false )
  {
    return;
  }

  m_pLayout->SetVisible( false );

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( pUtil)
  {
    pUtil->StopAnime( 0, LA_GET_ITEM_1_GETITEM_WIN_UPP_01_GETITEM_00);
  }
}

/**
 *  @brief  絵を変更する
 *  @param  index 番号
 */
void EffectGetItem::Change( u32 index)
{
  if( this->IsLayoutInitialized() == false)
  {
    GFL_ASSERT_MSG( 0, "Layout is not initialize\n");
    return;
  }

  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();

  f32 frame = index;
  if( frame > pUtil->GetAnimeMaxFrame( 0, LA_GET_ITEM_1_GETITEM_WIN_UPP_01_GETITEM_00))
  {
    GFL_ASSERT_MSG( 0, "Over animation index\n");
    return;
  }

  m_animationIndex = index;
  m_pLayout->SetAnimationFrame( LA_GET_ITEM_1_GETITEM_WIN_UPP_01_GETITEM_00, frame);
}

/**
 *  @brief  絵の番号を取得する
 *  @return 絵の番号
 */
u32 EffectGetItem::GetIndex() const
{
  return m_animationIndex;
}

/**
 *  @brief  レイアウト初期化が完了しているか
 */
bool EffectGetItem::IsLayoutInitialized()
{
  app::util::G2DUtil *pUtil = m_pLayout->GetG2DUtil();
  if( pUtil )
  {
    return true;
  }
  return false;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
