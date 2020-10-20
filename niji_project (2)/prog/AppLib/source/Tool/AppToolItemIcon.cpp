//==============================================================================
/**
 * @file    AppToolItemIcon.h
 * @author  fukushima_yuya
 * @date    2015.04.03
 * @brief   アイテムアイコン
 */
//==============================================================================


// niji
#include <AppLib/include/Tool/AppToolItemIcon.h>
#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/app_util_heap.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum {
    SEQ_READY,
    SEQ_REPLACE,
    SEQ_MAIN,
    SEQ_NUM,
  };

  enum {
    LOAD_NONE,    //!< 読み込みなし
    LOAD_START,   //!< 読み込み開始
    LOAD_END,     //!< 読み込み終了
    LOAD_NUM,
  };
}


//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ItemIconToolEx::ItemIconToolEx( ItemIconToolParam param )
  : m_pItemIcon( NULL )
  , m_pHeap( NULL )
  , m_pG2D( NULL )
  , m_pLytwk( NULL )
  , m_ResID()
  , m_IconMax( 0 )
  , m_LytID( 0 )
  , m_bSync( false )
  , m_EndSeq( 0 )
  , m_MainSeq( SEQ_READY )
  , m_pLoadIcon( NULL )
  , m_pIconParam( NULL )
{
  // 変数の初期化
  {
    m_pHeap = param.pHeap;
    m_pG2D = param.pUIView->GetG2DUtil();
    m_LytID = param.layout_id;
    m_IconMax = param.iconMax;
    m_bSync = param.sync;

    m_pLytwk = m_pG2D->GetLayoutWork( m_LytID );
    m_ResID = m_pG2D->GetLayoutResourceID( param.res_id );

    if( m_bSync ) m_MainSeq = SEQ_MAIN;   //!< 同期フラグがtrue
  }

  gfl2::heap::HeapBase* heap = m_pHeap->GetDeviceHeap();

  // アイコン生成パラメータの生成
  m_pIconParam = GFL_NEW_ARRAY(heap) CreateIconParam[m_IconMax];
  GFL_ASSERT( m_pIconParam );

  // 読み込みフラグ配列の生成
  {
    m_pLoadIcon = GFL_NEW_ARRAY(heap) u8[m_IconMax];
    GFL_ASSERT( m_pLoadIcon );

    for( u32 i=0; i<m_IconMax; ++i )
    {
      m_pLoadIcon[i] = LOAD_NONE;
    }
  }


  // アイテムアイコンの生成
  {
    m_pItemIcon = GFL_NEW(heap) app::tool::ItemIcon( heap, heap, m_IconMax );
    GFL_ASSERT( m_pItemIcon );
   
    if( param.allocBuffer )
    {
      // あらかじめメモリを確保しておく
      for( u32 i=0; i<m_IconMax; ++i )
      {
        m_pItemIcon->AllocTextureBuffer( i );
      }
    }

    // ファイルオープン
    if( !m_bSync )
    {
      m_pItemIcon->FileOpen( heap );
    }
    else {
      m_pItemIcon->FileOpenSync( heap );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
ItemIconToolEx::~ItemIconToolEx( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::Update( void )
{
  switch( m_MainSeq )
  {
  case SEQ_READY:
    {
      if( IsReady() )
      {
        m_MainSeq = SEQ_REPLACE;
      }
    } break;

  case SEQ_REPLACE:
    {
      for( u32 i=0; i<m_IconMax; ++i )
      {
        CreateIconParam* iconParam = &m_pIconParam[i];

        if( !iconParam->IsInvalid() )
        {
          // アイコンの差し替え
          SetItemIcon( iconParam->id, iconParam->itemNo, iconParam->pic );

          // リセット
          iconParam->Reset();
        }
      }

      m_MainSeq = SEQ_MAIN;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 * @return  "true" == 完了
 * @return  "false" == 準備中
 *
 * @note    非同期の時のみ使用
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::IsReady( void )
{
  if( m_bSync ) return true;

  return m_pItemIcon->IsFileOpen();
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @return  "true" == 終了
 * @return  "false" == 終了中
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::EndFunc( void )
{
  enum {
    END_SEQ_READY,
    END_SEQ_LOAD,
    END_SEQ_CLOSE,
    END_SEQ_CLOSE_WAIT,
    END_SEQ_MODULE_FREE,
    END_SEQ_TERM,
    END_SEQ_END,
  };

  switch( m_EndSeq )
  {
  case END_SEQ_READY:
    {
      if( !IsReady() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_LOAD:
    {
      bool result = true;

      for( u32 i=0; i<m_IconMax; ++i )
      {
        if( m_pLoadIcon[i] == LOAD_START )
        {
          IsLoadItemIcon( i );
          result = false;
        }
      }

      if( result ) m_EndSeq++;
      else         break;
    } // no break;

  case END_SEQ_CLOSE:
    {
      if( !m_bSync )
      {
        m_pItemIcon->FileClose( m_pHeap->GetDeviceHeap() );   //!< 非同期
        m_EndSeq++;
      }
      else {
        m_pItemIcon->FileCloseSync();   //!< 同期
        m_EndSeq = END_SEQ_MODULE_FREE;
        break;
      }
    } // no break;

  case END_SEQ_CLOSE_WAIT:
    {
      if( !m_pItemIcon->IsFileClose() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_MODULE_FREE:
    {
      if( !m_pItemIcon->IsModuleFree() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_TERM:
    {
      GFL_SAFE_DELETE( m_pItemIcon );
      GFL_SAFE_DELETE_ARRAY( m_pLoadIcon );
      GFL_SAFE_DELETE_ARRAY( m_pIconParam );

      m_EndSeq++;
    } // no break;

  case END_SEQ_END:
    {
      return  true;
    } // no break;
  }

  return false;
}



//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id      アイコンID
 * @param   itemNo  アイテムNo
 * @param   pane    差し替え先のピクチャペイン
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::SetItemIcon( u32 id, u16 itemNo, gfl2::lyt::LytPicture* pane )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( itemNo == ITEM_DUMMY_DATA )
  {
    GFL_ASSERT(0);
    return;
  }

  if( pane == NULL )
  {
    GFL_ASSERT(0);
    return;
  }

  if( IsReady() )
  {
    m_pItemIcon->ReplacePaneTexture( id, itemNo, pane );
  }
  else {
    CreateIconParam* iconParam = &m_pIconParam[id];

    iconParam->id = id;
    iconParam->itemNo = itemNo;
    iconParam->pic = pane;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id          アイコンID
 * @param   itemNo      アイテムNo
 * @param   paneIndex   差し替え先のピクチャペインインデックス
 * @param   parts       差し替え先の部品ペイン
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::SetItemIcon( u32 id, u16 itemNo, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts )
{
  gfl2::lyt::LytPicture* picture = NULL;

  if( !parts )
  {
    picture = m_pLytwk->GetPicturePane( paneIndex );
  }
  else {
    picture = m_pLytwk->GetPicturePane( parts, paneIndex, &m_ResID );
  }

  SetItemIcon( id, itemNo, picture );
}



//------------------------------------------------------------------------------
/**
 * @brief   アイコンの読み込み
 * @param   id      アイコンID
 * @param   itemNo  アイテムNo
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::LoadItemIcon( u32 id, u16 itemNo )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( m_pLoadIcon[id] == LOAD_START )
  {
    bool res = m_pItemIcon->CancelLoadTextureRequest( id );

    if( !res )
    {
      // 読み込みのキャンセルに失敗
      GFL_ASSERT(0);
      return;
    }

    m_pLoadIcon[id] = LOAD_NONE;
  }

  // アイコンの読み込み
  m_pItemIcon->LoadRequest( id, itemNo ) ;

  m_pLoadIcon[id] = LOAD_START;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id      アイコンID
 * @param   pane    差し替え先のピクチャペイン
 * @return  "true" == 差し替え完了
 * @return  "false" == それ以外
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::ReplaceIcon( u32 id, gfl2::lyt::LytPicture* pane )
{
  // アイコンIDが範囲外
  if( id >= m_IconMax ) return false;
  // 置き換えるペイン情報がない
  if( !pane ) return false;
  // テクスチャの読み込みができていない
  if( !IsLoadItemIcon( id ) ) return false;

  // テクスチャの置き換え
  m_pItemIcon->ReplaceReadTexture( id, pane );

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの読み込み終了確認
 * @return  "true" == 完了
 * @return  "false" == 読み込み中
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::IsLoadItemIcon( u32 id )
{
  // 読み込みリクエストがない
  if( m_pLoadIcon[id] == LOAD_NONE ) return false;
  // 読み込み終了
  if( m_pLoadIcon[id] == LOAD_END ) return true;

  if( m_pItemIcon->IsLoadFinished( id ) )
  {
    m_pLoadIcon[id] = LOAD_END;

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
  * @brief   指定したアイコン張替えが終っているか
  * @return  "true" == 終っている
  * @return  "false" == 終っていない or 張替えをしていない
  */
//------------------------------------------------------------------------------
bool ItemIconToolEx::IsReplaced( const u32 id )
{
  if( m_IconMax <= id )
  {
    GFL_ASSERT( 0 );
    return  true;
  }

  CreateIconParam* iconParam = &m_pIconParam[ id ];

  return ( iconParam->IsInvalid() == true );
}

//------------------------------------------------------------------------------
/**
  * @brief   全てのアイコン張替えが終っているか
  * @return  "true" == 終っている
  * @return  "false" == 終っていない or 張替えをしていない
  */
//------------------------------------------------------------------------------
bool ItemIconToolEx::IsAllReplaced( void )
{
  for( u32 i=0; i<m_IconMax; ++i )
  {
    if( !IsReplaced( i ) )
    {
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id          アイコンID
 * @param   paneIndex   差し替え先のピクチャペインインデックス
 * @param   parts       差し替え先の部品ペイン
 * @return  "true" == 差し替え完了
 * @return  "false" == それ以外
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::ReplaceIcon( u32 id, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts )
{
  gfl2::lyt::LytPicture* picture = NULL;

  if( !parts )
  {
    picture = m_pLytwk->GetPicturePane( paneIndex );
  }
  else {
    picture = m_pLytwk->GetPicturePane( parts, paneIndex, &m_ResID );
  }

  return ReplaceIcon( id, picture );
}



//------------------------------------------------------------------------------
/**
 * @brief	  アニメーションの再生
 * @param   anmID         アニメーションID
 * @param   auto_anim     
 * @param   auto_unbind   
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::StartAnim( u32 anmID, bool auto_anim, bool auto_unbind )
{
  m_pG2D->StartAnime( m_LytID, anmID, auto_anim, auto_unbind );
}
  
//------------------------------------------------------------------------------
/**
 * @brief	  アニメーションの停止
 * @param   anmID   アニメーションID
 * @param   clear   
 */
//------------------------------------------------------------------------------
void ItemIconToolEx::StopAnim( u32 anmID, bool clear )
{
  m_pG2D->StopAnime( m_LytID, anmID, clear );
}
  
//------------------------------------------------------------------------------
/**
 * @brief	  アニメーションの終了チェック
 * @param   anmID   アニメーションID
 * @return  true == 終了
 */
//------------------------------------------------------------------------------
bool ItemIconToolEx::IsAnimEnd( u32 anmID )
{
  return m_pG2D->IsAnimeEnd( m_LytID, anmID );
}

GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(tool)
