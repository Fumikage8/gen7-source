//==============================================================================
/**
 * @file    PokeListCursorDraw.cpp
 * @author  fukushima_yuya
 * @date    2015.03.12
 * @brief   ポケモンリスト画面カーソル描画クラス
 */
//==============================================================================


// niji
#include "App/PokeList/source/parts/PokeListCursorDraw.h"
#include "App/PokeList/source/parts/PokeListDrawerBase.h"
#include "App/PokeList/source/parts/PokeListPlateDraw.h"
#include "App/PokeList/source/work/PokeListStaticWork.h"

#include <AppLib/include/Ui/UIView.h>
#include <Sound/include/Sound.h>

// arc
#include <niji_conv_header/app/common_pokeList/common_pokeList_pane.h>
#include <niji_conv_header/app/common_pokeList/common_pokeList_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )


//-----------------------------------------------------------------------------
// @brief   定数
//-----------------------------------------------------------------------------
enum {
  SEQ_INIT,
  SEQ_LOAD,
  SEQ_MAIN,
  SEQ_DELETE,
  SEQ_END,
  SEQ_NUM,
};

//-----------------------------------------------------------------------------
// @brief   変数
//-----------------------------------------------------------------------------
// @fix GFNMCat[1366]
static CursorDraw::CursorData c_DefCursorData_Normal[] = {
  { CPOS_PLATE_TL, true, { CPOS_PLATE_BR, CPOS_PLATE_TR, CPOS_PLATE_BL, CPOS_PLATE_ML } },    //!< 左上
  { CPOS_PLATE_TR, true, { CPOS_PLATE_TL, CPOS_PLATE_ML, CPOS_PLATE_BR, CPOS_PLATE_MR } },    //!< 右上
  { CPOS_PLATE_ML, true, { CPOS_PLATE_TR, CPOS_PLATE_MR, CPOS_PLATE_TL, CPOS_PLATE_BL } },    //!< 左中
  { CPOS_PLATE_MR, true, { CPOS_PLATE_ML, CPOS_PLATE_BL, CPOS_PLATE_TR, CPOS_PLATE_BR } },    //!< 右中
  { CPOS_PLATE_BL, true, { CPOS_PLATE_MR, CPOS_PLATE_BR, CPOS_PLATE_ML, CPOS_PLATE_TL } },    //!< 左下
  { CPOS_PLATE_BR, true, { CPOS_PLATE_BL, CPOS_PLATE_TL, CPOS_PLATE_MR, CPOS_PLATE_TR } },    //!< 右下
};
// @fix GFNMCat[1366]
static CursorDraw::CursorData c_DefCursorData_Decide[] = {
  { CPOS_PLATE_TL, true,  { CPOS_DECIDE,   CPOS_PLATE_TR, CPOS_DECIDE,   CPOS_PLATE_ML } },   //!< 左上
  { CPOS_PLATE_TR, true,  { CPOS_PLATE_TL, CPOS_PLATE_ML, CPOS_DECIDE_R, CPOS_PLATE_MR } },   //!< 右上
  { CPOS_PLATE_ML, true,  { CPOS_PLATE_TR, CPOS_PLATE_MR, CPOS_PLATE_TL, CPOS_PLATE_BL } },   //!< 左中
  { CPOS_PLATE_MR, true,  { CPOS_PLATE_ML, CPOS_PLATE_BL, CPOS_PLATE_TR, CPOS_PLATE_BR } },   //!< 右中
  { CPOS_PLATE_BL, true,  { CPOS_PLATE_MR, CPOS_PLATE_BR, CPOS_PLATE_ML, CPOS_DECIDE   } },   //!< 左下
  { CPOS_PLATE_BR, true,  { CPOS_PLATE_BL, CPOS_DECIDE_R, CPOS_PLATE_MR, CPOS_DECIDE_R } },   //!< 右下
  { CPOS_DECIDE,   false, { CPOS_PLATE_BR, CPOS_PLATE_TL, CPOS_PLATE_BL, CPOS_PLATE_TL } },   //!< 決定(左)
  { CPOS_DECIDE,   false, { CPOS_PLATE_BR, CPOS_PLATE_TL, CPOS_PLATE_BR, CPOS_PLATE_TR } },   //!< 決定(右)
};



//! @brief  入れ替えのプレート選択アニメ
const u32 c_CursorPosAnim[PLATE_NUM][2] = {
  {//!< 左上
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_00_BG_OWN_SELECTED,
  },
  {//!< 右上
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_03_BG_OWN_SELECTED,
  },
  {//!< 左中
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_01_BG_OWN_SELECTED,
  },
  {//!< 右中
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_04_BG_OWN_SELECTED,
  },
  {//!< 左下
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_02_BG_OWN_SELECTED,
  },
  {//!< 右下
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_OWN,
    LA_COMMON_POKELIST_POKELIST_MIN_LOW_000__PARTS_05_BG_OWN_SELECTED,
  },
};



//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   pBase               DrawerBaseクラスポインタ
 * @param   pRenderingManager   レンダリングマネージャ
 * @param   member_count        メンバー数
 */
//-----------------------------------------------------------------------------
CursorDraw::CursorDraw( DrawerBase* pBase, app::util::AppRenderingManager* pRenderingManager, u8 member_count )
  : m_pBase( pBase )
  , m_pAppRenderingManager( pRenderingManager )
  , m_pMenuCursor( NULL )
  , m_pMenuCursorBuffer( NULL )
  , m_Seq(SEQ_INIT)
  , m_DeleteSeq( 0 )
  , m_MemberCount(member_count)
  , m_pCursorData(NULL)
  , m_CursorDataNum(0)
{
  // 初期化
  Init();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
CursorDraw::~CursorDraw()
{
  GFL_SAFE_DELETE_ARRAY( m_pCursorData );

  GFL_SAFE_DELETE( m_pMenuCursor );
}



//-----------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//-----------------------------------------------------------------------------
void CursorDraw::Init( void )
{
  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();
  {
    // メニューカーソルの生成
    m_pMenuCursor = GFL_NEW(heap) app::tool::MenuCursor( m_pBase->GetHeap(), &m_pMenuCursorBuffer );
  }

  // 配置ペインの設定
  {
    // プレートのカーソル配置ペイン
    for( u8 i=0; i<CPOS_PLATE_NUM; ++i )   // @fix cov_ctr[11875]
    {
      CursorPos cpos = static_cast<CursorPos>(i);

      gfl2::lyt::LytParts* plateParts = GetPlateParts( m_pBase, cpos );
      {
        m_pCursorPane[i] = m_pBase->GetPane( PANENAME_POKELIST_PLT_LOW_000_PANE_NULL_CUR, plateParts );
      }
    }

    // 決定ボタンのカーソル配置ペイン
    gfl2::lyt::LytParts* decideBtnParts = m_pBase->GetParts( PANENAME_POKELIST_MIN_LOW_000_PANE_PARTS_OK );
    {
      m_pCursorPane[CPOS_DECIDE] = m_pBase->GetPane( PANENAME_POKELIST_BTN_LOW_000_PANE_NULL_CUR, decideBtnParts );
    }
  }

  // カーソル移動の設定
  SetCursorMoveDefault();

  m_Seq = SEQ_LOAD;
}

//-----------------------------------------------------------------------------
/**
 * @brief   更新
 */
//-----------------------------------------------------------------------------
void CursorDraw::Update( void )
{
  if( m_pMenuCursor == NULL ) return;

  m_pMenuCursor->Update();

  switch( m_Seq )
  {
  case SEQ_LOAD:
    {
      if( !m_pMenuCursor->LoadResource() ) break;

      StartMenuCursor();  //! メニューカーソルの開始
      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_MAIN:
    {
      PutCursor();
    } break;

  case SEQ_DELETE:
    {
      if( EndFunc() )
      {
        m_Seq = SEQ_END;
      }
    } break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   終了処理
 *
 * @return  "true  == 終了"
 * @return  "false == それ以外"
 */
//-----------------------------------------------------------------------------
bool CursorDraw::EndFunc( void )
{
  enum {
    DEL_SEQ_CHECK_LOADED,
    DEL_SEQ_LYTWK,
    DEL_SEQ_RESOURCE,
    DEL_SEQ_END,
    DEL_SEQ_NUM,
  };

  switch( m_DeleteSeq )
  {
  case DEL_SEQ_CHECK_LOADED:  //!< 読み込みが完了しているかチェック
    {
      if( !m_pMenuCursor->IsModuleFree() )
      {
        m_pMenuCursor->LoadResource();
        break;
      }

      ++m_DeleteSeq;
    } break;

  case DEL_SEQ_LYTWK:
    {
      if( !m_pMenuCursor->DeleteLayoutWork() ) break;

      ++m_DeleteSeq;
    } break;

  case DEL_SEQ_RESOURCE:
    {
      m_pMenuCursor->DeleteResource();

      ++m_DeleteSeq;
    } break;

  case DEL_SEQ_END:
    {
      return true;  //!< 完了
    }
  }

  return false; //!< それ以外
}


//-----------------------------------------------------------------------------
/**
 * @brief   描画
 */
//-----------------------------------------------------------------------------
void CursorDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  if( m_pMenuCursor == NULL ) return;
  if( m_Seq != SEQ_MAIN ) return;

  m_pMenuCursor->Draw( m_pAppRenderingManager, no, gfl2::lyt::DISPLAY_LOWER );
}

//-----------------------------------------------------------------------------
/**
 * @brief   準備完了か？
 * @return  true == 完了
 */
//-----------------------------------------------------------------------------
bool CursorDraw::IsReady( void )
{
  return m_Seq == SEQ_MAIN;
}

//-----------------------------------------------------------------------------
/**
 * @brief   終了可能か？
 * @return  true == 終了、false == 否
 */
//-----------------------------------------------------------------------------
bool CursorDraw::IsEnd( void )
{
  return m_Seq == SEQ_END;
}



//-----------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの開始
 */
//-----------------------------------------------------------------------------
void CursorDraw::StartMenuCursor( void )
{
  if( m_pMenuCursor == NULL ) return;

  app::util::G2DUtil* pG2D = m_pBase->GetG2D();

  // メニューカーソルの作成
  m_pMenuCursor->CreateLayoutWork(
    pG2D->GetLayoutSystem(),
    pG2D->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    pG2D->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );

  // 初期位置の設定
  SetCursorPos( CPOS_PLATE_TL );

  // アニメの開始
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @brief   メニューカーソルの終了
 */
//-----------------------------------------------------------------------------
void CursorDraw::EndMenuCursor( void )
{
  if( m_pMenuCursor == NULL ) return;

  SetVisible( false );

  m_DeleteSeq = 0;
  m_Seq = SEQ_DELETE;
}

//------------------------------------------------------------------
/**
 * @brief   メニューカーソルのバッファの取得
 * @return  カーソルのバッファ
 */
//------------------------------------------------------------------
void* CursorDraw::GetMenuCursorBuffer( void )
{
  return m_pMenuCursorBuffer;
}



//-----------------------------------------------------------------------------
/**
 * @brief   カーソルタイプの設定
 * @param   cid   カーソルタイプ
 * @param   cpos  カーソルの初期位置指定
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetCursorID( CursorID cid, CursorPos cpos )
{
  StaticWork* staticWork = m_pBase->GetStaticWork();
  {
    staticWork->SetCursorID( cid );
  }

  if( cpos != CPOS_NULL )
  {
    SetCursorPos( cpos );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソル位置の設定
 * @param   cpos  カーソルの位置
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetCursorPos( CursorPos cpos )
{
  StaticWork* staticWork = m_pBase->GetStaticWork();
  {
    CursorID cid = staticWork->GetCursorID();
    {
      // カーソル位置の設定
      staticWork->SetCursorPos( cid, cpos );
    }
  }

  // カーソルの位置を更新
  PutCursor();
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソル位置の取得
 * @param   cid   カーソルID
 * @return  カーソルの位置
 */
//-----------------------------------------------------------------------------
CursorPos CursorDraw::GetCursorPos( CursorID cid )
{
  CursorPos cpos = _GetCursorPos( cid );
  
  if( m_CursorDataNum < static_cast<u32>(cpos) )
  {
    GFL_ASSERT(0);
    return CPOS_NUM;
  }

  return m_pCursorData[cpos].cpos;
}

// @fix GFNMCat[1462]
//-----------------------------------------------------------------------------
/**
 * @brief   カーソル位置の取得：StaticWorkから取得
 *
 * @param   cid   カーソルID
 *
 * @return  カーソルの位置
 */
//-----------------------------------------------------------------------------
CursorPos CursorDraw::_GetCursorPos( CursorID cid )
{
  StaticWork* staticWork = m_pBase->GetStaticWork();
  {
    CursorID cursor_id = staticWork->GetCursorID();

    if( cid != CID_NUM )
    {
      cursor_id = cid;
    }

    return staticWork->GetCursorPos( cursor_id );
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief   表示の切り替え
 * @param   visible   表示フラグ
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetVisible( bool visible )
{
  if( m_pMenuCursor == NULL ) return;

  StaticWork* staticWork = m_pBase->GetStaticWork();
  Mode mode = staticWork->GetProcMode();

  u32 type = app::tool::MenuCursor::CURSOR_ALL;

  if( mode == MODE_BATTLE && visible )
  {
    type = app::tool::MenuCursor::CURSOR_LEFT;
  }

  m_pMenuCursor->SetVisible( visible, type );
}



//-----------------------------------------------------------------------------
/**
 * @brief   カーソルの移動(デフォルト)
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetCursorMoveDefault( void )
{
  // @fix GFNMCat[1366]

  // カーソルデータの破棄
  GFL_SAFE_DELETE_ARRAY( m_pCursorData );

  // カーソルデータの数を更新
  m_CursorDataNum = GFL_NELEMS(c_DefCursorData_Normal);
  // カーソルデータの生成
  m_pCursorData = GFL_NEW(m_pBase->GetDeviceHeap()) CursorData[m_CursorDataNum];
  {
    GFL_ASSERT(m_pCursorData);

    for( u32 i=0; i<m_CursorDataNum; ++i )
    {
      m_pCursorData[i] = c_DefCursorData_Normal[i];
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   カーソルの移動(決定ボタンあり)
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetCursorMoveDecide( void )
{
  // @fix GFNMCat[1366]

  // カーソルデータの破棄
  GFL_SAFE_DELETE_ARRAY( m_pCursorData );

  // カーソルデータの数を更新
  m_CursorDataNum = GFL_NELEMS(c_DefCursorData_Decide);
  // カーソルデータの生成
  m_pCursorData = GFL_NEW(m_pBase->GetDeviceHeap()) CursorData[m_CursorDataNum];
  {
    GFL_ASSERT(m_pCursorData);

    for( u32 i=0; i<m_CursorDataNum; ++i )
    {
      m_pCursorData[i] = c_DefCursorData_Decide[i];
    }
  }
}

// @fix GFNMCat[1366]
//-----------------------------------------------------------------------------
/**
 * @brief   カーソルデータの入力設定を変更
 *
 * @param   index   インデックス
 * @param   enable  有効フラグ
 */
//-----------------------------------------------------------------------------
void CursorDraw::SetCursorEnable( u32 index, bool enable )
{
  GFL_ASSERT( index < m_CursorDataNum );

  // フラグを更新
  m_pCursorData[index].enable = enable;

  if( index == CPOS_DECIDE )
  {
    m_pCursorData[CPOS_DECIDE_R].enable = enable;
  }
}




//-----------------------------------------------------------------------------
/**
 * @brief   カーソルの配置
 */
//-----------------------------------------------------------------------------
void CursorDraw::PutCursor( void )
{
  CursorPos index = GetCursorPos();

  if( m_CursorDataNum < static_cast<u32>(index) )
  {
    // 範囲外はカーソルが置けない箇所のため何もしない
    return;
  }

  CursorPos cpos = m_pCursorData[index].cpos;
  {
    if( m_pCursorPane[cpos] != NULL )
    {
      m_pMenuCursor->PutNonVisible( m_pBase->GetLytWk(), m_pCursorPane[cpos] );
    }
  }
}




//-----------------------------------------------------------------------------
/**
 * @brief   ボタンの入力処理
 * @param   pButton   ボタン
 */
//-----------------------------------------------------------------------------
void CursorDraw::InputButton( gfl2::ui::Button* pButton )
{
  u8 dir = GetInputDir( pButton );

  if( dir != DIR_NULL )
  {
    // @fix GFNMCat[1366]

    // 今の位置
    //CursorPos nowPos = GetCursorPos();
    CursorPos nowPos = _GetCursorPos();  // @fix GFNMCat[1462]
    // 次の位置
    CursorPos nextPos = m_pCursorData[nowPos].moveData.pos[dir];

    for ever
    {
      // 次の位置は有効か？
      if( m_pCursorData[nextPos].enable )
      {
        break;
      }

      nowPos  = nextPos;
      nextPos = m_pCursorData[nowPos].moveData.pos[dir];
    }

    // カーソルのセット
    SetCursorPos( nextPos );

    StaticWork* staticWork = m_pBase->GetStaticWork();
    {
      if( staticWork->IsPlaySound() ) // @fix GFNMCat[1363]：サウンドを鳴らすのか判定
      {
        // カーソル移動音
        Sound::PlaySE( SEQ_SE_SELECT1 );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief   ボタンの入力処理
 * @param   pButton   ボタン
 */
//-----------------------------------------------------------------------------
u8 CursorDraw::GetInputDir( gfl2::ui::Button* pButton )
{
  if( pButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )  return DIR_LEFT;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) ) return DIR_RIGHT;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_UP ) )    return DIR_UP;
  if( pButton->IsRepeat( gfl2::ui::BUTTON_DOWN ) )  return DIR_DOWN;

  return DIR_NULL;
}


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )
