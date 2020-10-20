#if PM_DEBUG
//=============================================================================
/**
 * @file    ItemMakeFrame.cpp
 * @brief   デバッグ用アイテム作成フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.26
 */
//=============================================================================

// module
#include "ItemMakeFrame.h"
#include "ItemMakeDraw.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSave.h"
#include "Savedata/include/MyItemSaveDef.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Debug/DebugNumInput/include/DebugNumInput.h"
// resource
#include "arc_def_index/arc_def.h"


GFL_NAMESPACE_BEGIN(Debug)

//! メインシーケンス
enum MainSeq
{
  SEQ_MAIN = 0,         //!< メイン
  SEQ_NUM_INPUT,        //!< 数値入力メイン
  SEQ_NUM_INPUT_END,    //!< 数値入力終了
  SEQ_ITEMID_INPUT,     //!< 道具ID入力メイン
  SEQ_ITEMID_INPUT_END, //!< 道具ID入力終了
  SEQ_END
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
ItemMakeFrame::ItemMakeFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
  , m_exitID( NULL )
  , m_pLytResBuff( NULL )
  , m_pDraw( NULL )
  , m_pNumInput( NULL )
  , m_isNumInputDraw( false )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ItemMakeFrame::~ItemMakeFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.05.26
 *
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 * @param   exit_id     終了ID格納場所
 * @param   item_id     初期道具ID
 * @param   item_num    個数格納場所
 * @param   item_name   道具名メッセージデータ
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man, u32 * exit_id, u16 * item_id, u16 * item_num, gfl2::str::MsgData * item_name )
{
  m_heap             = heap;
  m_renderingManager = render_man;
  m_exitID           = exit_id;
  m_itemID           = item_id;
  m_itemNum          = item_num;
  m_itemName         = item_name;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ItemMakeFrame::InitFunc(void)
{
  GetGameData();
  LoadLayoutResource();
  CreateDraw();
  InitFade();
  SetFadeIn();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ItemMakeFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  UnloadLayoutResource();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.05.26
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ItemMakeFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_MAIN:
    break;

  case SEQ_NUM_INPUT:
    UpdateNumInput();
    break;
  case SEQ_NUM_INPUT_END:
    DeleteNumInput();
    m_pDraw->PutNumber( *m_itemNum );
    m_pDraw->SetInputEnabled( true );
    SetSubSeq( SEQ_MAIN );
    break;

  case SEQ_ITEMID_INPUT:
    UpdateItemIDInput();
    break;
  case SEQ_ITEMID_INPUT_END:
    DeleteNumInput();
    m_pDraw->PutItem( *m_itemID );
    m_pDraw->SetInputEnabled( true );
    SetSubSeq( SEQ_MAIN );
    break;

  case SEQ_END:
    if( IsFadeEnd() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2015.05.26
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );

  if( m_isNumInputDraw != false && no == gfl2::gfx::CtrDisplayNo::DOWN )
  {
    m_renderingManager->AddDrawableLytWkForOneFrame(
      app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pNumInput->GetLayoutWork() );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_myItem = gd->GetMyItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2015.11.17
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.11.17
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.11.17
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.11.17
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemMakeFrame::IsFadeEnd(void)
{
  gfl2::Fade::FadeManager * man = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  if( man->IsEnd(gfl2::Fade::DISP_UPPER) == false || man->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    AddItem
 * @brief   道具追加（個別）
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::AddItem(void)
{
  if( *m_itemID == ITEM_DUMMY_DATA )
  {
    return;
  }

  { // swapアイテムチェック
    static const u16 swap_item[][2] =
    {
      { ITEM_IDENSINOKUSABI, ITEM_IDENSINOKUSABI_1 },
      { ITEM_SORUPURASU,     ITEM_SORUPURASU_1 },
      { ITEM_RUNAPURASU,     ITEM_RUNAPURASU_1 },
    };

    for( u32 i=0; i<GFL_NELEMS(swap_item); i++ )
    {
      for( u32 j=0; j<2; j++ )
      {
        if( *m_itemID == swap_item[i][j] )
        {
          if( m_myItem->Check(swap_item[i][j^1],1) != false )
          {
            return;
          }
        }
      }
    }
  }

  u32 num = m_myItem->GetItemNum( *m_itemID );
  if( *m_itemNum == num )
  {
    return;
  }

  if( *m_itemNum == 0 )
  {
    m_myItem->Sub( *m_itemID, num );
  }
  else if( *m_itemNum < num )
  {
    num = num - *m_itemNum;
    m_myItem->Sub( *m_itemID, num );
  }
  else if( *m_itemNum > num )
  {
    num = *m_itemNum - num;
    if( num > 1 && item::ITEM_DATA::GetParam( *m_itemID, item::ITEM_DATA::PRM_ID_F_POCKET ) == BAG_POCKET_EVENT )
    {
      num = 1;
    }
    m_myItem->Add( *m_itemID, num );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    AddItem
 * @brief   全道具追加
 * @date    2015.05.26
 *
 * @param   num   追加する個数
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::AddItem( u32 num )
{
  u32 swap_item1 = ITEM_IDENSINOKUSABI_1;
  u32 swap_item2 = ITEM_SORUPURASU_1;
  u32 swap_item3 = ITEM_RUNAPURASU_1;
  if( m_myItem->Check(ITEM_IDENSINOKUSABI_1,1) != false )
  {
    swap_item1 = ITEM_IDENSINOKUSABI;
  }
  if( m_myItem->Check(ITEM_SORUPURASU_1,1) != false )
  {
    swap_item2 = ITEM_SORUPURASU;
  }
  if( m_myItem->Check(ITEM_RUNAPURASU_1,1) != false )
  {
    swap_item3 = ITEM_RUNAPURASU;
  }

  static const gfl2::str::STRCODE * dummy_name1 = L"？？？";
  static const gfl2::str::STRCODE * dummy_name2 = L"???";
  static const gfl2::str::STRCODE * dummy_name3 = L"(?)";
  gfl2::str::StrBuf * str = m_pDraw->GetTempStrBuf();

  for( u32 i=1; i<=ITEM_DATA_MAX; i++ )
  {
    if( i == swap_item1 || i == swap_item2 || i == swap_item3 )
    {
      continue;
    }
    m_itemName->GetString( i, *str );
    if( gfl2::str::StrComp( str->GetPtr(), dummy_name1 ) != false ||
        gfl2::str::StrComp( str->GetPtr(), dummy_name2 ) != false ||
        gfl2::str::StrComp( str->GetPtr(), dummy_name3 ) != false )
    {
      continue;
    }
    m_myItem->Sub( i, m_myItem->GetItemNum(i) );
    if( num > 1 && item::ITEM_DATA::GetParam( *m_itemID, item::ITEM_DATA::PRM_ID_F_POCKET ) == BAG_POCKET_EVENT )
    {
      m_myItem->Add( i, 1 );
    }
    else
    {
      m_myItem->Add( i, num );
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetDeviceHeap()) ItemMakeDraw( m_heap, m_renderingManager );
  m_pDraw->SetItemMakeListener( this );
  m_pDraw->Init( m_pLytResBuff );
  m_pDraw->PutString( *m_itemID, *m_itemNum );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.05.26
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ItemMakeFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::LoadLayoutResource(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_ITEM_MAKE, m_heap->GetDeviceHeap() );
  app::util::FileAccessor::FileLoadSync(
    ARCID_DEBUG_ITEM_MAKE, 0, &m_pLytResBuff, m_heap->GetDeviceHeap(), true, 128 );
  app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_ITEM_MAKE );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.05.26
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateNumInput
 * @brief   数値入力生成
 * @date    2015.05.27
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::CreateNumInput(void)
{
  m_pNumInput = GFL_NEW(m_heap->GetDeviceHeap()) debug::NumInput( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), m_pDraw->GetLayoutSystem() );
  m_pNumInput->Start( m_pDraw->GetNumInputTitle()->GetPtr(), *m_itemNum, 0, ITEM_HAVE_MAX );
  m_isNumInputDraw = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateItemIDInput
 * @brief   数値入力生成（道具ID入力）
 * @date    2016.04.11
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::CreateItemIDInput(void)
{
  m_pNumInput = GFL_NEW(m_heap->GetDeviceHeap()) debug::NumInput( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap(), m_pDraw->GetLayoutSystem() );
  m_pNumInput->Start( m_pDraw->GetItemIDInputTitle()->GetPtr(), *m_itemID, 1, ITEM_DATA_MAX );
  m_isNumInputDraw = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteNumInput
 * @brief   数値入力削除
 * @date    2015.05.27
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::DeleteNumInput(void)
{
  GFL_SAFE_DELETE( m_pNumInput );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateNumInput
 * @brief   数値入力更新
 * @date    2015.05.27
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::UpdateNumInput(void)
{
  debug::NumInput::Result result = m_pNumInput->Update();

  if( result == debug::NumInput::RES_CANCEL )
  {
    m_isNumInputDraw = false;
    SetSubSeq( SEQ_NUM_INPUT_END );
  }
  else if( result == debug::NumInput::RES_DECIDE )
  {
    *m_itemNum = m_pNumInput->GetValue();
    m_isNumInputDraw = false;
    SetSubSeq( SEQ_NUM_INPUT_END );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateItemIDInput
 * @brief   道具ID入力更新
 * @date    2016.04.11
 */
//-----------------------------------------------------------------------------
void ItemMakeFrame::UpdateItemIDInput(void)
{
  debug::NumInput::Result result = m_pNumInput->Update();

  if( result == debug::NumInput::RES_CANCEL )
  {
    m_isNumInputDraw = false;
    SetSubSeq( SEQ_ITEMID_INPUT_END );
  }
  else if( result == debug::NumInput::RES_DECIDE )
  {
    *m_itemID = m_pNumInput->GetValue();
    m_isNumInputDraw = false;
    SetSubSeq( SEQ_ITEMID_INPUT_END );
  }
}


//--------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力リスナー
 * @date    2015.05.26
 *
 * @param   id  入力ID
 */
//--------------------------------------------------------------------------
void ItemMakeFrame::OnAction( u32 id )
{
  switch( id )
  {
  case ItemMakeListener::ACTION_INPUT_NAME:
    *m_exitID = EXIT_CALL_KEYBORD;
    SetSubSeq( SEQ_END );
    break;
  case ItemMakeListener::ACTION_INPUT_ITEM_ID:
    m_pDraw->SetInputEnabled( false );
    CreateItemIDInput();
    SetSubSeq( SEQ_ITEMID_INPUT );
    break;
  case ItemMakeListener::ACTION_INPUT_NUM:
    m_pDraw->SetInputEnabled( false );
    CreateNumInput();
    SetSubSeq( SEQ_NUM_INPUT );
    break;
  case ItemMakeListener::ACTION_INPUT_ENTER:
    AddItem();
    break;

  case ItemMakeListener::ACTION_ADD_ITEM_ONE:
    AddItem( 1 );
    break;
  case ItemMakeListener::ACTION_ADD_ITEM_MAX:
    AddItem( ITEM_HAVE_MAX );
    break;

  case ItemMakeListener::ACTION_EXIT:
    *m_exitID = EXIT_END;
    SetFadeOut();
    SetSubSeq( SEQ_END );
    break;
  }
}


GFL_NAMESPACE_END(Debug)

#endif  // PM_DEBUG
