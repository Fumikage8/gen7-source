//======================================================================
/**
 * @file    ResortPartsPaneList.cpp
 * @date    2015/10/02 15:08:33
 * @author  fukushima_yuya
 * @brief   ポケリゾート：ペインリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortPartsPaneList.h"

#include <App/Resort/source/ResortDrawerBase.h>
#include <AppLib/include/Ui/UIView.h>
#include <sound/include/Sound.h>

// arc
#include <niji_conv_header/app/resort/resort_pane.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ResortPaneList::ResortPaneList( DrawerBase* base, app::tool::PaneList::CallBack* callBack, app::tool::PaneListView::Listener* listener )
  : m_pBase( base )
  , m_pPaneListView( NULL )
  , m_pCallBack( callBack )
  , m_pPaneDataTbl( NULL )
  , m_isCreated( false )
{
  gfl2::heap::HeapBase* sysHeap = m_pBase->GetSystemHeap();

  // PaneListViewの生成
  m_pPaneListView = GFL_NEW(sysHeap) app::tool::PaneListView( m_pBase->GetHeap() );
  m_pPaneListView->SetListener( listener );
  m_pBase->GetUIView()->AddSubView( m_pPaneListView );  //!< 親ビューの追加
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
ResortPaneList::~ResortPaneList( void )
{
  Delete();

  if( m_pPaneListView )
  {
    m_pPaneListView->RemoveFromSuperView();   //!< 親ビューから削除

    GFL_SAFE_DELETE( m_pPaneListView )
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの生成
 * @param   setupParam  セットアップパラメータ
 */
//------------------------------------------------------------------------------
void ResortPaneList::Create( SETUP_PARAM& setupParam, app::tool::PaneList::PANE_DATA* paneDataTbl )
{
  gfl2::heap::HeapBase* sysHeap = m_pBase->GetSystemHeap();

  m_pPaneDataTbl = paneDataTbl;

  // リストパラメータのセット
  app::tool::PaneList::SETUP_PARAM list_setup;
  SetParamList( &list_setup, setupParam );

  // スクロール値の計算
  u32 max = 0;
  if( list_setup.valMax > list_setup.listPosMax )
  {
    max = list_setup.valMax - list_setup.listPosMax;
  }

  // スクロールバーパラメータのセット
  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup = {
    sysHeap,
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    max,

    m_pBase->GetLytWk(),
    m_pBase->GetG2D()->GetLayoutResourceID(0),
    setupParam.scrollPane,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL, NULL, NULL
  };

  // PaneListViewのセットアップ
  m_pPaneListView->CreatePaneList( &list_setup );
  m_pPaneListView->CreateScrollBar( &scroll_bar_setup );

  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    // コンフィグ設定
    paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_NONE, true, false, false, false );
    // SEの設定
    paneList->SetSE( setupParam.moveSE, setupParam.decideSE, setupParam.cancelSE );
    // 十字キー入力無効
    paneList->SetCrossKeyEnable( false );
  }

  m_isCreated = true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの破棄
 */
//------------------------------------------------------------------------------
void ResortPaneList::Delete( void )
{
  if( m_pPaneListView )
  {
    if( m_pPaneListView->GetPaneList() )
    {
      // スクロールバーの破棄
      m_pPaneListView->DeleteScrollBar();

      // ペインリストの破棄
      m_pPaneListView->DeletePaneList();

      m_isCreated = false;
    }
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   リストパラメータのセット
 * @param   param       [out]リストパラメータ
 * @param   setupParam  [in]セットアップパラメータ
 */
//------------------------------------------------------------------------------
void ResortPaneList::SetParamList( app::tool::PaneList::SETUP_PARAM* param, SETUP_PARAM& setupParam )
{
  param->heap           = m_pBase->GetDeviceHeap();;
  param->pLytWk         = m_pBase->GetLytWk();
  param->paneList       = m_pPaneDataTbl;
  param->paneListMax    = setupParam.paneMax;
  param->valMax         = setupParam.listMax;     //!< 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax     = setupParam.viewCount;   //!< カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos        = 0;                      //!< 初期カーソル位置 ( 0 〜 listPosMax-1 の範囲 )
  param->listScroll     = 0.0f;                   //!< 初期スクロール位置 ( 0 〜 valMax-listPosMax-1 の範囲 )
  param->defPY          = setupParam.defPY;       //!< ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
  param->defSX          = setupParam.paneWidth;   //!< ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY          = setupParam.paneHeight;  //!< ペインのYサイズ(レイアウトデディターで厳密に確認）
  param->writeOffsetY   = -1;                     //!< 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)
  param->listStartPane  = m_pBase->GetPane( setupParam.listStartPane );   //!< 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )
  param->cbFunc         = m_pCallBack;            //!< コールバック関数
  param->cbWork         = NULL;                   //!< コールバック関数に渡すワーク
  param->uiDevMan       = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();   //!< UIデバイスマネージャ
  param->menuCursor     = NULL;                   //!< カーソル
}

//------------------------------------------------------------------------------
/**
 * @brief   PaneListのリセット
 *
 * @param   setupParam  セットアップパラメータ
 */
//------------------------------------------------------------------------------
void ResortPaneList::ResetPaneList( SETUP_PARAM& setupParam )
{
  app::tool::PaneList* paneList = m_pPaneListView->GetPaneList();
  {
    u32 curPos = 0;
    f32 scrollVal = 0.0f;
    
    // カーソルデータの取得
    paneList->GetCursorData( &curPos, &scrollVal );
    
    // スクロール値補正   // @fix GFNMCat[3176]
    u32 scrollMax = 0;
    {
      if( setupParam.listMax >= setupParam.viewCount )
      {
        scrollMax = setupParam.listMax - setupParam.viewCount;
      }
      if( scrollVal > static_cast<f32>(scrollMax) )
      {
        scrollVal = static_cast<f32>(scrollMax);
      }
    }
    // カーソル位置補正   // @fix GFNMCat[3176]
    u32 posMax = 0;
    {
      if( scrollMax != 0 )
      {
        posMax = setupParam.viewCount - 1;
      }
      else if( setupParam.listMax != 0 )
      {
        posMax = setupParam.listMax - 1;
      }
      if( curPos > posMax )
      {
        curPos = posMax;
      }
    }

    // セットアップパラメータの初期化
    app::tool::PaneList::SETUP_PARAM list_setup;
    {
      SetParamList( &list_setup, setupParam );
      
      list_setup.listPos    = curPos;
      list_setup.listScroll = scrollVal;

      // @fix GFNMCat[3176]
      //// @fix GFNMCat[2271]
      //if( (list_setup.listPosMax-1) < curPos )
      //{
      //  list_setup.listPos = list_setup.listPos - 1;
      //}
      //// @fix GFNMCat[2271]
      //if( (list_setup.valMax-list_setup.listPosMax-1) < list_setup.listScroll )
      //{
      //  list_setup.listScroll = list_setup.listScroll - 1;
      //}
    }
    
    // リストの再設定
    m_pPaneListView->ResetPaneList( &list_setup );

    // コンフィグ設定
    paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_NONE, true, false, false, false );
    // SEの設定
    paneList->SetSE( setupParam.moveSE, setupParam.decideSE, setupParam.cancelSE );
    // 十字キー入力無効
    paneList->SetCrossKeyEnable( false );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   入力の有効設定
 * @param   enable    有効フラグ
 */
//------------------------------------------------------------------------------
void ResortPaneList::SetInputEnable( bool enable )
{
  if( m_pPaneListView )
  {
    m_pPaneListView->SetInputEnabled( enable );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   PaneListViewの取得
 * @return  PaneListViewポインタ
 */
//------------------------------------------------------------------------------
app::tool::PaneListView* ResortPaneList::GetPaneListView( void ) const
{
  return m_pPaneListView;
}

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )
