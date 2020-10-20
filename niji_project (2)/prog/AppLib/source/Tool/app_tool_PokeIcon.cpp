//==============================================================================
/**
 * @file    app_tool_PokeIcon.h
 * @author  fukushima_yuya
 * @date    2015.03.13
 * @brief   ポケモンアイコン
 */
//==============================================================================

// niji
#include <AppLib/include/Tool/app_tool_PokeIcon.h>
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
PokeIconToolEx::PokeIconToolEx( PokeIconToolParam param )
  : m_pPokeIcon( NULL )
  , m_pHeap( NULL )
  , m_pG2D( NULL )
  , m_pLytwk( NULL )
  , m_ResID()
  , m_IconMax( 0 )
  , m_LytID( 0 )
  , m_bSync( false )
  , m_EndSeq( 0 )
  , m_MainSeq( 0 )
  , m_pLoadIcon( NULL )
  , m_pIconParamCP( NULL )
  , m_pIconParamSP( NULL )
  , m_pIconPicturePane( NULL )
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

  // アイコン生成パラメータ
  m_pIconParamCP = GFL_NEW_ARRAY(heap) IconParamCP[m_IconMax];
  GFL_ASSERT(m_pIconParamCP);
  m_pIconParamSP = GFL_NEW_ARRAY(heap) IconParamSP[m_IconMax];
  GFL_ASSERT(m_pIconParamSP);

  // 読み込みフラグ配列の生成
  {
    m_pLoadIcon = GFL_NEW_ARRAY(heap) u8[m_IconMax];
    GFL_ASSERT( m_pLoadIcon );

    for( u32 i=0; i<m_IconMax; ++i )
    {
      m_pLoadIcon[i] = LOAD_NONE;
    }
  }


  // ポケモンアイコンの生成
  {
    m_pPokeIcon = GFL_NEW(heap) app::tool::PokeIcon( heap, heap, m_IconMax );
    GFL_ASSERT( m_pPokeIcon );

    if( param.allocBuffer )
    {
      // あらかじめメモリを確保しておく
      for( u32 i=0; i<m_IconMax; ++i )
      {
        m_pPokeIcon->AllocTextureBuffer( i );
      }
    }

    // ファイルオープン
    if( !m_bSync )
    {
      m_pPokeIcon->FileOpen( heap );
    }
    else {
      m_pPokeIcon->FileOpenSync( heap );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeIconToolEx::~PokeIconToolEx( void )
{
  ;
}



//------------------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::Update( void )
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
        IconParamCP* cp = &m_pIconParamCP[i];
        IconParamSP* sp = &m_pIconParamSP[i];

        if( !cp->IsInvalid() )
        {
          // アイコンの差し替え
          SetPokeIcon( cp->id, cp->cp, cp->pic, cp->bFlip );

          // リセット
          cp->Reset();
        }

        if( !sp->IsInvalid() )
        {
          // アイコンの差し替え
          SetPokeIcon( sp->id, sp->sp, sp->pic, sp->bFlip );

          // リセット
          sp->Reset();
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
bool PokeIconToolEx::IsReady( void )
{
  if( m_bSync ) return true;

  return m_pPokeIcon->IsFileOpen();
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 * @return  "true" == 終了
 * @return  "false" == 終了中
 */
//------------------------------------------------------------------------------
bool PokeIconToolEx::EndFunc( void )
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
          IsLoadPokeIcon( i );
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
        m_pPokeIcon->FileClose( m_pHeap->GetDeviceHeap() );   //!< 非同期
        m_EndSeq++;
      }
      else {
        m_pPokeIcon->FileCloseSync();   //!< 同期
        m_EndSeq = END_SEQ_MODULE_FREE;
        break;
      }
    } // no break;

  case END_SEQ_CLOSE_WAIT:
    {
      if( !m_pPokeIcon->IsFileClose() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_MODULE_FREE:
    {
      if( !m_pPokeIcon->IsModuleFree() ) break;

      m_EndSeq++;
    } // no break;

  case END_SEQ_TERM:
    {
      GFL_SAFE_DELETE( m_pPokeIcon );
      GFL_SAFE_DELETE_ARRAY( m_pLoadIcon );
      GFL_SAFE_DELETE_ARRAY( m_pIconParamSP );
      GFL_SAFE_DELETE_ARRAY( m_pIconParamCP );

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
 * @brief   アイコン張替え終っているか
 * @return  "true" == 終っている
 * @return  "false" == 終っていない or 張替えをしていない
 */
//------------------------------------------------------------------------------
bool PokeIconToolEx::IsReplaced( const u32 id )
{
  if( m_IconMax <= id )
  {
    GFL_ASSERT( 0 );
    return  true;
  }

  IconParamCP* cp = &m_pIconParamCP[ id ];
  IconParamSP* sp = &m_pIconParamSP[ id ];

  return ( ( cp->IsInvalid() == true ) && ( sp->IsInvalid() == true ) );
}

//------------------------------------------------------------------------------
/**
 * @brief   全てのアイコン張替えが終っているか
 * @return  "true" == 終っている
 * @return  "false" == 終っていない or 張替えをしていない
 */
//------------------------------------------------------------------------------
bool PokeIconToolEx::IsAllReplaced( void )
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
 * @param   id      アイコンID
 * @param   cp      ポケモンコアパラメータ
 * @param   pane    差し替え先のピクチャペイン
 * @param   bFlip   アイコンを反転するか
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::SetPokeIcon( u32 id, const pml::pokepara::CoreParam* cp, gfl2::lyt::LytPicture* pane, bool bFlip )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( cp == NULL )
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
    m_pPokeIcon->ReplacePaneTexture( id, cp, pane, 0, 0, bFlip );
  }
  else {
    IconParamCP* iconParam = &m_pIconParamCP[id];

    iconParam->id = id;
    iconParam->cp = cp;
    iconParam->pic = pane;
    iconParam->bFlip  = bFlip;
  }
  m_pIconPicturePane = pane;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id          アイコンID
 * @param   cp          ポケモンコアパラメータ
 * @param   paneIndex   差し替え先のピクチャペインインデックス
 * @param   parts       差し替え先の部品ペイン
 * @param   bFlip   アイコンを反転するか
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::SetPokeIcon( u32 id, const pml::pokepara::CoreParam* cp, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts, bool bFlip )
{
  gfl2::lyt::LytPicture* picture = NULL;

  if( !parts )
  {
    picture = m_pLytwk->GetPicturePane( paneIndex );
  }
  else {
    picture = m_pLytwk->GetPicturePane( parts, paneIndex, &m_ResID );
  }

  SetPokeIcon( id, cp, picture, bFlip );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id      アイコンID
 * @param   sp      アイコン描画用の簡易パラメータ
 * @param   pane    差し替え先のピクチャペイン
 * @param   bFlip   アイコンを反転するか
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::SetPokeIcon( u32 id, PokeTool::SimpleParam& sp, gfl2::lyt::LytPicture* pane, bool bFlip )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( sp.monsNo == MONSNO_NULL )
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
    m_pPokeIcon->ReplacePaneTexture( id, &sp, pane, 0, 0, bFlip );
  }
  else {
    IconParamSP* iconParam = &m_pIconParamSP[id];

    iconParam->id = id;
    iconParam->sp = sp;
    iconParam->pic = pane;
    iconParam->bFlip  = bFlip;
  }
  m_pIconPicturePane = pane;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの差し替え
 * @param   id          アイコンID
 * @param   sp          アイコン描画用の簡易パラメータ
 * @param   paneIndex   差し替え先のピクチャペインインデックス
 * @param   parts       差し替え先の部品ペイン
 * @param   bFlip   アイコンを反転するか
 *
 * @note    アイコンの読み込みと差し替えを行う。
 *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::SetPokeIcon( u32 id, PokeTool::SimpleParam& sp, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts, bool bFlip )
{
  gfl2::lyt::LytPicture* picture = NULL;

  if( !parts )
  {
    picture = m_pLytwk->GetPicturePane( paneIndex );
  }
  else {
    picture = m_pLytwk->GetPicturePane( parts, paneIndex, &m_ResID );
  }

  SetPokeIcon( id, sp, picture, bFlip );
}



//------------------------------------------------------------------------------
/**
 * @brief   アイコンの読み込み
 * @param   id    アイコンID
 * @param   cp    ポケモンコアパラメータ
 * @param   bFlip   アイコンを反転するか
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::LoadPokeIcon( u32 id, const pml::pokepara::CoreParam* cp, bool bFlip )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( m_pLoadIcon[id] == LOAD_START )
  {
    bool res = m_pPokeIcon->CancelLoadTextureRequest( id );
    GFL_ASSERT( res );    //!< 読み込みのキャンセルに失敗

    m_pLoadIcon[id] = LOAD_NONE;
  }

  // アイコンの読み込み
  if( m_pPokeIcon->ReadRequest( id, cp, bFlip ) )
  {
    m_pLoadIcon[id] = LOAD_START;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの読み込み
 * @param   id    アイコンID
 * @param   sp    アイコン描画用の簡易パラメータ
 * @param   bFlip   アイコンを反転するか
 */
//------------------------------------------------------------------------------
void PokeIconToolEx::LoadPokeIcon( u32 id, const PokeTool::SimpleParam& sp, bool bFlip )
{
  if( id >= m_IconMax )
  {
    GFL_ASSERT(0);
    return;
  }

  if( m_pLoadIcon[id] == LOAD_START )
  {
    bool res = m_pPokeIcon->CancelLoadTextureRequest( id );
    GFL_ASSERT( res );    //!< 読み込みのキャンセルに失敗

    m_pLoadIcon[id] = LOAD_NONE;
  }

  // アイコンの読み込み
  if( m_pPokeIcon->ReadRequest( id, &sp, bFlip ) )
  {
    m_pLoadIcon[id] = LOAD_START;
  }
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
bool PokeIconToolEx::ReplaceIcon( u32 id, gfl2::lyt::LytPicture* pane )
{
  // アイコンIDが範囲外
  if( id >= m_IconMax ) return false;
  // 置き換えるペイン情報がない
  if( !pane ) return false;
  // テクスチャの読み込みができていない
  if( !IsLoadPokeIcon( id ) ) return false;

  // テクスチャの置き換え
  m_pPokeIcon->ReplaceReadTexture( id, pane );

  m_pIconPicturePane = pane;

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイコンの読み込み終了確認
 * @return  "true" == 完了
 * @return  "false" == 読み込み中
 */
//------------------------------------------------------------------------------
bool PokeIconToolEx::IsLoadPokeIcon( u32 id )
{
  // 読み込みリクエストがない
  if( m_pLoadIcon[id] == LOAD_NONE ) return false;
  // 読み込み終了
  if( m_pLoadIcon[id] == LOAD_END ) return true;

  if( m_pPokeIcon->IsLoadFinished( id ) )
  {
    m_pLoadIcon[id] = LOAD_END;

    return true;
  }

  return false;
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
bool PokeIconToolEx::ReplaceIcon( u32 id, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts )
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
void PokeIconToolEx::StartAnim( u32 anmID, bool auto_anim, bool auto_unbind )
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
void PokeIconToolEx::StopAnim( u32 anmID, bool clear )
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
bool PokeIconToolEx::IsAnimEnd( u32 anmID )
{
  return m_pG2D->IsAnimeEnd( m_LytID, anmID );
}

//------------------------------------------------------------------------------
/**
 * @brief   差し替えを行ったピクチャペインの取得
 * @return  ピクチャペイン
 *
 * @note    差し替え処理の度に差し替え先のピクチャペインが登録される
 */
//------------------------------------------------------------------------------
gfl2::lyt::LytPicture* PokeIconToolEx::GetPicturePane( void ) const
{
  return m_pIconPicturePane;
}

GFL_NAMESPACE_END(app)
GFL_NAMESPACE_END(tool)
