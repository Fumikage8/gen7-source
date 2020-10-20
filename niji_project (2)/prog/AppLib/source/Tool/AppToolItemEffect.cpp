//=============================================================================
/**
 * @file    AppToolItemEffect.cpp
 * @brief   道具関連のエフェクト処理
 * @author  Hiroyuki Nakamura
 * @date    2016.01.18
 */
//=============================================================================

// module
#include "AppLib/include/Tool/AppToolItemEffect.h"
// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/EffectUtil.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/ItemEffect.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//! エフェクトデータテーブル
const ItemEffect::EFFECT_DATA ItemEffect::EffectDataTable[ItemEffect::TYPE_MAX] =
{
  { // Zビーズ: ほのお
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0295,
    0,
  },
  { // Zビーズ: みず
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0296,
    0,
  },
  { // Zビーズ: くさ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0297,
    0,
  },
  { // Zビーズ: でんき
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0298,
    0,
  },
  { // Zビーズ: こおり
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0299,
    0,
  },
  { // Zビーズ: かくとう
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0300,
    0,
  },
  { // Zビーズ: どく
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0301,
    0,
  },
  { // Zビーズ: じめん
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0302,
    0,
  },
  { // Zビーズ: ひこう
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0303,
    0,
  },
  { // Zビーズ: エスパー
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0304,
    0,
  },
  { // Zビーズ: むし
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0305,
    0,
  },
  { // Zビーズ: いわ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0306,
    0,
  },
  { // Zビーズ: ゴースト
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0307,
    0,
  },
  { // Zビーズ: ドラゴン
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0308,
    0,
  },
  { // Zビーズ: あく
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0309,
    0,
  },
  { // Zビーズ: はがね
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0310,
    0,
  },
  { // Zビーズ: フェアリー
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0311,
    0,
  },
  { // Zビーズ: ノーマル
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0294,
    0,
  },
  { // Zビーズ: ジュナイパー
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0312,
    0,
  },
  { // Zビーズ: ガオガエン
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0313,
    0,
  },
  { // Zビーズ: アシレーヌ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0314,
    0,
  },
  { // Zビーズ: ピカチュウ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0315,
    0,
  },
  { // Zビーズ: カプ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0316,
    0,
  },
  { // Zビーズ: A.ライチュウ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0317,
    0,
  },
  { // Zビーズ: カビゴン
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0318,
    0,
  },
  { // Zビーズ: イーブイ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0319,
    0,
  },
  { // Zビーズ: ミュウ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0320,
    0,
  },
  { // Zビーズ: マーシャドー
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0321,
    0,
  },
  { // Zビーズ: サトピカ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0322,
    0,
  },
  { // Zビーズ: ネクロズマ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0323,
    0,
  },
  { // Zビーズ: ソルガレオ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0324,
    0,
  },
  { // Zビーズ: ルナアーラ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0283,
    0,
  },
  { // Zビーズ: ミミッキュ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0291,
    0,
  },
  { // Zビーズ: ルガルガン
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0292,
    0,
  },
  { // Zビーズ: ジャラランガ
    GARC_ItemEffect_ea_z_beads_bptcl_COMP,
    SEQ_SE_EXTRA_0293,
    0,
  },

  { // ギラティナ（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_gira_bptcl_COMP,
    SEQ_SE_SYS_020,
    13,
  },
  { // シェイミ（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_syeimi_bptcl_COMP,
    SEQ_SE_SYS_019,
    18,
  },
  { // トルネロス（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_torne_bptcl_COMP,
    SEQ_SE_SYS_025,
    13,
  },
  { // ボルトロス（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_torne_bptcl_COMP,
    SEQ_SE_SYS_025,
    13,
  },
  { // ランドロス（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_torne_bptcl_COMP,
    SEQ_SE_SYS_025,
    13,
  },
  { // ゼクロム合体
    GARC_ItemEffect_ea_formchange_zeku_bptcl_COMP,
    SEQ_SE_SYS_021,
    65,
  },
  { // レシラム合体
    GARC_ItemEffect_ea_formchange_reshi_bptcl_COMP,
    SEQ_SE_SYS_022,
    65,
  },
  { // キュレム合体
    GARC_ItemEffect_ea_formchange_kyure_bptcl_COMP,
    ItemEffect::SE_NONE,
    65,
  },
  { // ブラックキュレム分離
    GARC_ItemEffect_ea_formchange_kyure_bptcl_COMP,
    SEQ_SE_SYS_021,
    65,
  },
  { // ホワイトキュレム分離
    GARC_ItemEffect_ea_formchange_kyure_bptcl_COMP,
    SEQ_SE_SYS_022,
    65,
  },
  { // フーパ（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_fupa_bptcl_COMP,
    SEQ_SE_SYS_012,
    8,
  },
  { // ジガルデ（10%=>50%へのフォルムチェンジ）
    GARC_ItemEffect_ea_formchange_ziga_up_bptcl_COMP,
    SEQ_SE_SYS_NJ_009,
    58,
  },
  { // ジガルデ（50%=>10%へのフォルムチェンジ）
    GARC_ItemEffect_ea_formchange_ziga_down_bptcl_COMP,
    SEQ_SE_SYS_NJ_009,
    58,
  },
  { // ミツスイ（フォルムチェンジ）
    GARC_ItemEffect_ea_formchange_mitsu_bptcl_COMP,
    SEQ_SE_SYS_NJ_010,
    56,
  },

  { // ソルガレオ合体
    GARC_ItemEffect_ea_formchange_ns_bptcl_COMP,
    SEQ_SE_MJ_SYS_001,
    70,
  },
  { // ルナアーラ合体
    GARC_ItemEffect_ea_formchange_nr_bptcl_COMP,
    SEQ_SE_MJ_SYS_001,
    70,
  },
  { // ネクロズマ合体
    GARC_ItemEffect_ea_formchange_n_bptcl_COMP,
    ItemEffect::SE_NONE,
    70,
  },
  { // ネクロズマソル分離
    GARC_ItemEffect_ea_formchange_n_bptcl_COMP,
    SEQ_SE_MJ_SYS_001,
    70,
  },
  { // ネクロズマルナ分離
    GARC_ItemEffect_ea_formchange_n_bptcl_COMP,
    SEQ_SE_MJ_SYS_001,
    70,
  },
};

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap                ヒープ
 * @param   rendering_manager   描画マネージャ
 * @param   disp                表示画面
 */
//-----------------------------------------------------------------------------
ItemEffect::ItemEffect( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, app::util::AppRenderingManager::RenderPlaceType::Tag disp )
  : m_heap( heap )
  , m_renderingManager( rendering_manager )
  , m_disp( disp )
  , m_pEffectUtil( NULL )
  , m_initializeSeq( 0 )
  , m_terminateSeq( 0 )
{
  InitWork();
  for( u32 i=0; i<EFFECT_MAX; i++ )
  {
    m_addIndex[i] = ADD_INDEX_NONE;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
ItemEffect::~ItemEffect()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    InitWork
 * @brief   ワーク初期化（個別）
 * @date    2016.01.18
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void ItemEffect::InitWork( u32 index )
{
  gfl2::math::VEC3 pos( 0.0f, 0.0f, 0.0f );

  m_work[index].type = TYPE_MAX;
  m_work[index].buf = NULL;
  m_work[index].seq = 0;
  m_work[index].is_update = 0;
  m_work[index].frame = 0;
  m_work[index].pos = pos;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitWork
 * @brief   ワーク初期化（全体）
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::InitWork(void)
{
  for( u32 i=0; i<EFFECT_MAX; i++ )
  {
    InitWork( i );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::Update(void)
{
  if( m_pEffectUtil != NULL )
  {
    m_pEffectUtil->UpdateForMode1Res1EmitterSet1Group();

    for( u32 i=0; i<EFFECT_MAX; i++ )
    {
      if( m_work[i].is_update != 0 && m_work[i].frame != EFFECT_FRAME_MAX )
      {
        if( IsEnd( i ) == false )
        {
          m_work[i].frame++;
        }
        else
        {
          m_work[i].is_update = 0;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateEffectUtil
 * @brief   EffectUtil作成
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::CreateEffectUtil(void)
{
  m_pEffectUtil = GFL_NEW(m_heap->GetSystemHeap()) app::util::EffectUtil();

  app::util::EffectUtil::Description decc;
  u32 group_array[1] = { 2 };

  decc.layerNum = 1;
  decc.effectResNumMax = EFFECT_MAX;
  decc.mode = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
  decc.effectGroupIDNumMaxArray = group_array;

  m_pEffectUtil->Create( m_heap->GetDeviceHeap(), decc );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteEffectUtil
 * @brief   EffectUtil削除
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::DeleteEffectUtil(void)
{
  GFL_SAFE_DELETE( m_pEffectUtil );
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize
 * @brief   初期化処理
 * @date    2016.01.18
 *
 * @param   is_sync   true = 同期, false = 非同期
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemEffect::Initialize( bool is_sync )
{
  enum
  {
    SEQ_CREATE_UTIL = 0,
    SEQ_FILE_OPEN,
    SEQ_FILE_OPEN_WAIT,
    SEQ_END
  };

  switch( m_initializeSeq )
  {
  case SEQ_CREATE_UTIL:
    CreateEffectUtil();
    m_initializeSeq++;
    /* FALL THROUGH */
  case SEQ_FILE_OPEN:
    if( is_sync == false )
    {
      app::util::FileAccessor::FileOpen( ARCID_ITEMEFFECT, m_heap->GetSystemHeap() );
    }
    else
    {
      app::util::FileAccessor::FileOpenSync( ARCID_ITEMEFFECT, m_heap->GetSystemHeap() );
    }
    m_initializeSeq++;
    /* FALL THROUGH */
  case SEQ_FILE_OPEN_WAIT:
    if( is_sync == false )
    {
      if( app::util::FileAccessor::IsFileOpen( ARCID_ITEMEFFECT ) == false )
      {
        break;
      }
    }
    m_initializeSeq++;
    /* FALL THROUGH */
  case SEQ_END:
    return true;
  }
  
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate
 * @brief   終了処理
 * @date    2016.01.18
 *
 * @param   is_sync   true = 同期, false = 非同期
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemEffect::Terminate( bool is_sync )
{
  enum
  {
    SEQ_FILE_CLOSE = 0,
    SEQ_FILE_CLOSE_WAIT,
    SEQ_DELETE_UTIL,
    SEQ_END
  };

  switch( m_terminateSeq )
  {
  case SEQ_FILE_CLOSE:
    if( is_sync == false )
    {
      app::util::FileAccessor::FileClose( ARCID_ITEMEFFECT, m_heap->GetSystemHeap() );
    }
    else
    {
      app::util::FileAccessor::FileCloseSync( ARCID_ITEMEFFECT );
    }
    m_terminateSeq++;
    /* FALL THROUGH */
  case SEQ_FILE_CLOSE_WAIT:
    if( is_sync == false )
    {
      if( app::util::FileAccessor::IsFileClose( ARCID_ITEMEFFECT ) == false )
      {
        break;
      }
    }
    m_terminateSeq++;
    /* FALL THROUGH */
  case SEQ_DELETE_UTIL:
    DeleteEffectUtil();
    m_terminateSeq++;
    /* FALL THROUGH */
  case SEQ_END:
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    Create
 * @brief   エフェクト生成
 * @date    2016.01.18
 *
 * @param   index     インデックス
 * @param   type      エフェクトタイプ
 * @param   pos       表示座標
 * @param   is_sync   true = 同期, false = 非同期
 */
//-----------------------------------------------------------------------------
void ItemEffect::Create( u32 index, Type type, gfl2::math::VEC3 * pos, bool is_sync )
{
  if( index >= EFFECT_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  EFFECT_WORK * wk = &m_work[index];

  if( wk->buf != NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  wk->type = type;
  wk->pos = *pos;

  if( is_sync == false )
  {
    app::util::FileAccessor::FileLoad(
      ARCID_ITEMEFFECT,
      EffectDataTable[type].dat_id,
      &wk->buf,
      m_heap->GetDeviceHeap(),
      true,
      128 );

    wk->seq = 0;
  }
  else
  {
    app::util::FileAccessor::FileLoadSync(
      ARCID_ITEMEFFECT,
      EffectDataTable[type].dat_id,
      &wk->buf,
      m_heap->GetDeviceHeap(),
      true,
      128 );

    m_pEffectUtil->EffectSystemEntryResource( wk->buf, index );
    Push( index );

    wk->seq = 0xff;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsCreate
 * @brief   エフェクト生成待ち
 * @date    2016.01.18
 *
 * @param   index     インデックス
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemEffect::IsCreate( u32 index )
{
  if( index >= EFFECT_MAX )
  {
    GFL_ASSERT( 0 );
    return true;
  }

  EFFECT_WORK * wk = &m_work[index];

  switch( wk->seq )
  {
  case 0:
    if( app::util::FileAccessor::IsFileLoad( &wk->buf ) == false )
    {
      break;
    }
    wk->seq++;
    /* FALL THROUGH */
  case 1:
    m_pEffectUtil->EffectSystemEntryResource( wk->buf, index );
    Push( index );
    wk->seq++;
    /* FALL THROUGH */
  default:
    return true;
  }

  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    Delete
 * @brief   エフェクト削除
 * @date    2016.01.18
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void ItemEffect::Delete( u32 index )
{
  if( index >= EFFECT_MAX )
  {
    GFL_ASSERT( 0 );
    return;
  }

  EFFECT_WORK * wk = &m_work[index];

  if( wk->buf != NULL )
  {
    Pop( index );
    m_pEffectUtil->EffectSystemClearResource( index );
    GflHeapFreeMemory( wk->buf );
    InitWork( index );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Push
 * @brief   リストに追加
 * @date    2016.01.19
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void ItemEffect::Push( u32 index )
{
  if( m_addIndex[EFFECT_MAX-1] != ADD_INDEX_NONE )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_pEffectUtil->GetData(0)->effectGroupIDList->PushFront( index );

  for( s32 i=EFFECT_MAX-1; i>0; i-- )
  {
    m_addIndex[i] = m_addIndex[i-1];
  }
  m_addIndex[0] = index;
}

//-----------------------------------------------------------------------------
/**
 * @func    Pop
 * @brief   リストから削除
 * @date    2016.01.19
 *
 * @param   index   インデックス
 */
//-----------------------------------------------------------------------------
void ItemEffect::Pop( u32 index )
{
  if( m_addIndex[0] == index )
  {
    m_pEffectUtil->GetData(0)->effectGroupIDList->PopFront();
    for( s32 i=0; i<EFFECT_MAX-1; i++ )
    {
      m_addIndex[i] = m_addIndex[i+1];
    }
    m_addIndex[EFFECT_MAX-1] = ADD_INDEX_NONE;
  }
  else
  {
    for( s32 i=EFFECT_MAX-1; i>=0; i-- )
    {
      if( m_addIndex[i] != ADD_INDEX_NONE )
      {
        if( m_addIndex[i] == index )
        {
          m_pEffectUtil->GetData(0)->effectGroupIDList->PopBack();
          m_addIndex[i] = ADD_INDEX_NONE;
          break;
        }
        else
        {
          GFL_ASSERT( 0 );
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   エフェクト開始
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::Start(void)
{
  m_renderingManager->SetEffectRenderPathConfig( m_disp, m_pEffectUtil->GetData(0)->effectRenderPathConfig );
  m_pEffectUtil->CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();

  for( u32 i=0; i<EFFECT_MAX; i++ )
  {
    if( m_work[i].buf != NULL )
    {
      gfl2::Effect::Handle * handle = m_pEffectUtil->GetEffectHandleForMode1Res1EmitterSet1Group( i );
      if( handle->IsValid() == false )
      {
        GFL_ASSERT( 0 );
      }
      else
      {
        nw::eft::EmitterSet * set = handle->GetEmitterSet();
        if( set == NULL )
        {
          GFL_ASSERT( 0 );
        }
        else
        {
          gfl2::math::Vector3 pos( m_work[i].pos.x, m_work[i].pos.y, m_work[i].pos.z );
          gfl2::Effect::EmitterSet::SetGFPos( set, pos );
        }
      }
      if( EffectDataTable[m_work[i].type].se != ItemEffect::SE_NONE )
      {
        Sound::PlaySE( EffectDataTable[m_work[i].type].se );
        // @note Zビーズの場合はさらにSEを再生
        if( m_work[i].type >= TYPE_Z_BEADS_HONOO && m_work[i].type <= TYPE_Z_BEADS_ZYARARANGA )
        {
          Sound::PlaySE( SEQ_SE_ZENRYOKUBEAD );
        }
      }
      m_work[i].is_update = 1;
      m_work[i].frame = 0;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   エフェクト終了
 * @date    2016.01.18
 */
//-----------------------------------------------------------------------------
void ItemEffect::End(void)
{
  for( u32 i=0; i<EFFECT_MAX; i++ )
  {
    if( m_work[i].buf != NULL )
    {
      m_work[i].is_update = 0;
      m_work[i].frame = 0;
    }
  }

  m_pEffectUtil->KillAllEmitterSetForMode1Res1EmitterSet1Group();
  m_renderingManager->UnsetEffectRenderPathConfig( m_disp );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsEnd
 * @brief   エフェクト終了チェック
 * @date    2016.01.18
 *
 * @param   index   インデックス
 *
 * @retval  true  = 終了
 * @retva;  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemEffect::IsEnd( u32 index )
{
  if( m_pEffectUtil->GetEffectSystem()->GetNwObject()->GetNumEmitter( index ) == 0 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsUpdateTiming
 * @brief   エフェクトごとの更新タイミングかをチェック
 * @date    2016.01.18
 *
 * @param   index   インデックス
 *
 * @retval  true  = 更新タイミング
 * @retva;  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemEffect::IsUpdateTiming( u32 index )
{
  if( m_work[index].frame == EffectDataTable[m_work[index].type].timing )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetFormChangeEffectType
 * @brief   フォルムチェンジエフェクトタイプを取得
 * @date    2016.02.01
 *
 * @param   mons  ポケモン番号
 * @param   form  フォルム番号
 * @param   item  アイテム番号
 *
 * @return  エフェクトタイプ
 */
//-----------------------------------------------------------------------------
App::Tool::ItemEffect::Type ItemEffect::GetFormChangeEffectType( MonsNo mons, FormNo form, u32 item )
{
  // 基本テーブル
  struct Table
  {
    MonsNo mons;
    Type type;
  };
  static const Table tbl[] =
  {
    { MONSNO_GIRATHINA,  TYPE_GRATHINA },
    { MONSNO_SHEIMI,     TYPE_SHEIMI },
    { MONSNO_TORUNEROSU, TYPE_TORUNEROSU },
    { MONSNO_BORUTOROSU, TYPE_BORUTOROSU },
    { MONSNO_RANDOROSU,  TYPE_RANDOROSU },
    { MONSNO_HUUPA,      TYPE_HUUPA },
    { MONSNO_MITUSUI,    TYPE_MITUSUI },
    { MONSNO_ZEKUROMU,   TYPE_KYUREMU_Z_COMBINE },
    { MONSNO_RESIRAMU,   TYPE_KYUREMU_R_COMBINE },
    { MONSNO_NIKKOU,     TYPE_NEKUROZUMA_S_COMBINE },   // ソルガレオ
    { MONSNO_GEKKOU,     TYPE_NEKUROZUMA_L_COMBINE },   // ルナアーラ
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( mons == tbl[i].mons )
    {
      return tbl[i].type;
    }
  }

  // 個別
  // キュレム
  if( mons == MONSNO_KYUREMU )
  {
    if( item == ITEM_IDENSINOKUSABI )
    {
      return TYPE_KYUREMU_C_COMBINE;
    }
    else
    {
      if( form == FORMNO_KYUREMU_BLACK )
      {
        return TYPE_KYUREMU_Z_SEPARATE;
      }
      else if( form == FORMNO_KYUREMU_WHITE )
      {
        return TYPE_KYUREMU_R_SEPARATE;
      }
    }
  }
  // ジガルデ
  if( mons == MONSNO_ZIGARUDE )
  {
    if( form == FORMNO_ZIGARUDE_SW10PER )
    {
      return TYPE_ZIGARUDE_UP;
    }
    else if( form == FORMNO_ZIGARUDE_SW50PER )
    {
      return TYPE_ZIGARUDE_DOWN;
    }
  }
  // ネクロズマ
  if( mons == MONSNO_PURIZUMU )
  {
    if( item == ITEM_SORUPURASU || item == ITEM_RUNAPURASU )
    {
      return TYPE_NEKUROZUMA_N_COMBINE;
    }
    else if( item == ITEM_SORUPURASU_1 )
    {
      return TYPE_NEKUROZUMA_S_SEPARATE;
    }
    else if( item == ITEM_RUNAPURASU_1 )
    {
      return TYPE_NEKUROZUMA_L_SEPARATE;
    }
  }

  GFL_ASSERT( 0 );
  return TYPE_GRATHINA;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetZPowerItemEffectType
 * @brief   Zビース用エフェクトタイプを取得
 * @date    2017.02.27
 *
 * @param   item  アイテム番号
 *
 * @return  エフェクトタイプ
 */
//-----------------------------------------------------------------------------
App::Tool::ItemEffect::Type ItemEffect::GetZPowerItemEffectType( u16 item )
{
  static const u16 tbl[] =
  {
    ITEM_HONOOBIIZU_1,      // TYPE_Z_BEADS_HONOO       Zビーズ: ほのお
    ITEM_MIZUBIIZU_1,       // TYPE_Z_BEADS_MIZU        Zビーズ: みず
    ITEM_KUSABIIZU_1,       // TYPE_Z_BEADS_KUSA        Zビーズ: くさ
    ITEM_DENKIBIIZU_1,      // TYPE_Z_BEADS_DENKI       Zビーズ: でんき
    ITEM_KOORIBIIZU_1,      // TYPE_Z_BEADS_KOORI       Zビーズ: こおり
    ITEM_KAKUTOUBIIZU_1,    // TYPE_Z_BEADS_KAKUTOU     Zビーズ: かくとう
    ITEM_DOKUBIIZU_1,       // TYPE_Z_BEADS_DOKU        Zビーズ: どく
    ITEM_ZIMENBIIZU_1,      // TYPE_Z_BEADS_JIMEN       Zビーズ: じめん
    ITEM_HIKOUBIIZU_1,      // TYPE_Z_BEADS_HIKOU       Zビーズ: ひこう
    ITEM_ESUPAABIIZU_1,     // TYPE_Z_BEADS_ESPER       Zビーズ: エスパー
    ITEM_MUSIBIIZU_1,       // TYPE_Z_BEADS_MUSHI       Zビーズ: むし
    ITEM_IWABIIZU_1,        // TYPE_Z_BEADS_IWA         Zビーズ: いわ
    ITEM_GOOSUTOBIIZU_1,    // TYPE_Z_BEADS_GHOST       Zビーズ: ゴースト
    ITEM_DORAGONBIIZU_1,    // TYPE_Z_BEADS_DRAGON      Zビーズ: ドラゴン
    ITEM_AKUBIIZU_1,        // TYPE_Z_BEADS_AKU         Zビーズ: あく
    ITEM_HAGANEBIIZU_1,     // TYPE_Z_BEADS_HAGANE      Zビーズ: はがね
    ITEM_FEARIIBIIZU_1,     // TYPE_Z_BEADS_FAIRY       Zビーズ: フェアリー
    ITEM_NOOMARUBIIZU_1,    // TYPE_Z_BEADS_NORMAL      Zビーズ: ノーマル
    ITEM_HUKUROU3BIIZU_1,   // TYPE_Z_BEADS_ZYUNAIPAA   Zビーズ: ジュナイパー
    ITEM_TORA3BIIZU_1,      // TYPE_Z_BEADS_GAOGAEN     Zビーズ: ガオガエン
    ITEM_ASIKA3BIIZU_1,     // TYPE_Z_BEADS_ASIREENU    Zビーズ: アシレーヌ
    ITEM_PIKATYUUBIIZU_1,   // TYPE_Z_BEADS_PiKATYUU    Zビーズ: ピカチュウ
    ITEM_THIKIBIIZU_1,      // TYPE_Z_BEADS_KAPU        Zビーズ: カプ
    ITEM_ARORAIBIIZU_1,     // TYPE_Z_BEADS_A_RAITYUU   Zビーズ: A.ライチュウ
    ITEM_KABIGONBIIZU_1,    // TYPE_Z_BEADS_KABIGON     Zビーズ: カビゴン
    ITEM_IIBUIBIIZU_1,      // TYPE_Z_BEADS_IIBUI       Zビーズ: イーブイ
    ITEM_MYUUBIIZU_1,       // TYPE_Z_BEADS_MYUU        Zビーズ: ミュウ
    ITEM_HAISINBIIZU_1,     // TYPE_Z_BEADS_MAASYADOO   Zビーズ: マーシャドー
    ITEM_SATOPIKABIIZU_1,   // TYPE_Z_BEADS_SATOPIKA    Zビーズ: サトピカ
    ITEM_NEKUROZUMAz_1,     // TYPE_Z_BEADS_NEKUROZUMA  Zビーズ: ネクロズマ
    ITEM_SORUGAREOz_1,      // TYPE_Z_BEADS_SORUGAREO   Zビーズ: ソルガレオ
    ITEM_RUNAAARAz_1,       // TYPE_Z_BEADS_RUNAAARA    Zビーズ: ルナアーラ
    ITEM_MIMIKKYUz_1,       // TYPE_Z_BEADS_MIMIKKYU    Zビーズ: ミミッキュ
    ITEM_RUGARUGANz_1,      // TYPE_Z_BEADS_RUGARUGAN   Zビーズ: ルガルガン
    ITEM_ZYARARANGAz_1,     // TYPE_Z_BEADS_ZYARARANGA  Zビーズ: ジャラランガ
  };

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    if( item == tbl[i] )
    {
      return static_cast<Type>( TYPE_Z_BEADS_HONOO + i );
    }
  }
  GFL_ASSERT( 0 );
  return TYPE_Z_BEADS_NORMAL;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
