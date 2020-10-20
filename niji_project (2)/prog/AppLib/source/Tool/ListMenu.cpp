//============================================================================================
/**
 * @file		ListMenu.cpp
 * @brief		リストメニュー処理
 * @author	Hiroyuki Nakamura  => 移植k.ohno
 * @date		12.07.05
 */
//============================================================================================

#include "AppLib/include/Tool/ListMenu.h"
#include <niji_conv_header/app/list_menu/res_2d/listmenu.h>
#include <niji_conv_header/app/list_menu/res_2d/listmenu_anim_list.h>
#include <niji_conv_header/app/list_menu/res_2d/listmenu_pane.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/listmenu.gaix>

#include <Sound/include/Sound.h>

#include "AppLib/include/Util/AppRenderingManager.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::tool
 * @brief			アプリツール
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )

//! テキストボックスペインテーブル
static const gfl2::lyt::LytPaneIndex TextBoxPaneTable[] = {
  PANENAME_LIST_MENU_PANE_LABEL_00,
  PANENAME_LIST_MENU_PANE_LABEL_01,
  PANENAME_LIST_MENU_PANE_LABEL_02,
  PANENAME_LIST_MENU_PANE_LABEL_03,
};

static const int LIST_POS_MAX = GFL_NELEMS(TextBoxPaneTable);		//!< 項目表示最大数
static const u32 UP_SCROLL_POS = 1;			//!< 上スクロール開始カーソル位置
static const u32 DOWN_SCROLL_POS = 2;		//!< 下スクロール開始カーソル位置

static const f32 TEXTBOX_PANE_HEIGHT  = 20.0f;	//!< テキストボックスペインの高さ
static const f32 WINDOW_TEX_SIZE_H    = 26.0f;			//!< ウィンドウペインのテクスチャドットサイズ (Horizontal: 13x2)
static const f32 WINDOW_TEX_SIZE_V    = 14.0f;			//!< ウィンドウペインのテクスチャドットサイズ (Vertical  :  7x2)

static const int TEXT_WIDTH_MINIMUM = 70;   //!< テキストボックスペインの最小幅

//! アニメID
/*
enum{
  ANMID_CURSOR_NORMAL = 0,	//!< カーソル通常アニメ
  ANMID_ARROW,							//!< スクロール矢印アニメ
  ANMID_CURSOR_DECIDE,			//!< カーソル決定アニメ
  ANMID_WIN_WHITE,					//!< 白ウィンドウ
  ANMID_WIN_BLACK,					//!< 黒ウィンドウ
};
*/

static const u32 LISTMENU_SE_DECIDE      = SEQ_SE_DECIDE0;	//!< SE定義：決定音
static const u32 LISTMENU_SE_CANCEL      = SEQ_SE_CANCEL0;	//!< SE定義：キャンセル音
static const u32 LISTMENU_SE_CURSOR_MOVE = SEQ_SE_SELECT0;	//!< SE定義：カーソル移動音


//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		初期化パラメータ
 */
//--------------------------------------------------------------------------------------------
ListMenu::ListMenu( const PARAM * prm )
   : app::ui::UIView( prm->pHeap, prm->pHeap->GetSystemAllocator(), prm->pHeap->GetDeviceAllocator() ),
     m_pResBuff( NULL ),
     m_curPos( 0 ),
     m_scroll( 0 ),
     m_addCount( 0 ),
     m_paneCount( 0 ),
     m_mainseq( 0 ),
     m_cancelSeq( 0 )
{
  int i;
  gfl2::std::MemCopy(prm,&m_param,sizeof(PARAM));

  GFL_ASSERT(m_param.listMax < ITEMLIST_MAX);

  for(i=0;i<m_param.listMax;i++){
    m_itemArray[i] = GFL_NEW(prm->pHeap->GetSystemHeap()) ITEM;
  }
  for(;i<ITEMLIST_MAX;i++){
    m_itemArray[i] =NULL;
  }
  mLytTextWork = print::CreateLytTextExWork(m_param.pHeap->GetSystemHeap());

  m_Sequence = SELECT_NONE;


    m_loadseq = _END;
  
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
ListMenu::~ListMenu()
{
  int i;
  RemoveLayout();
  for( i=0; i<m_addCount; i++ ){
    GFL_SAFE_DELETE( m_itemArray[i]->str );
  }
  for(i=0;i<ITEMLIST_MAX;i++){
    GFL_SAFE_DELETE( m_itemArray[i] );
  }

  GflHeapFreeMemory( m_pArcReadBuff );

  print::DeleteLytTextExWork(mLytTextWork);

}


//--------------------------------------------------------------------------------------------
/**
 * @brief		スタート
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
void ListMenu::Start(void)
{
  if( m_param.lytRes == NULL ){
    LoadResource();
    m_loadseq = _OPEN;
  }else{
    m_loadseq = _END;
  }
  m_mainseq=SEQ_CHECK;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
void ListMenu::Update(void)
{
  u32	ret = SELECT_NONE;

  switch( m_mainseq ){
  case SEQ_START:
    break;
  case SEQ_CHECK:
    if( CheckLoadResource() == false ){
      CreateLayout();
      Put( m_param.initPos );
      m_mainseq++;
    }
    break;

  case SEQ_MAIN:
    if( m_param.startWait != 0 ){
      m_param.startWait--;
      break;
    }
    PutCursor();
    SetVisiblePane( PANENAME_LIST_MENU_PANE_ARROW_00, true );
    SetInputListener( this );
    m_mainseq++;

  case SEQ_SELECT:
    if( m_param.visible == true ){
      ret = Main();
    }
    if( ret == SELECT_SUCCESS || ret == SELECT_CANCEL){
      m_mainseq = SEQ_ENDSETUP;
    }
    else{
      break;
    }
    break;
  case SEQ_ENDSETUP:
    if( CheckLoadResource() == false ){
      StopAnime( LA_LISTMENU_LIST_MENU_ARROW_CUR );
      if( GetSelectId() != SELECT_CANCEL )
      {
        SetAnime( LA_LISTMENU_LIST_MENU_KETTEI );
      }
      m_mainseq++;
    }
    break;
  case SEQ_ENDCHK:
    app::ui::UIView::Update();
    if( CheckAnime(LA_LISTMENU_LIST_MENU_KETTEI) == false )
    {
//      ret = m_itemArray[m_scroll+m_curPos]->id;
      m_mainseq++;
    }
    break;
  case SEQ_END:
    break;
  }

  //	if( pLytWk != NULL && param.visible == true ){
  //	pLytWk->Update();
  //	}

  //	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		描画
 *
 * @param		none
 *
 * @return	none
 *
 *	オート描画の場合は必要なし
 */
//--------------------------------------------------------------------------------------------
void ListMenu::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{

  if(m_param.visible == true){
		if(GetLayoutWork(0)){
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      pG2DUtil->AddDrawableLytWkForOneFrame(m_param.pAppRenderingManager,displayNo,0);
//      m_param.pAppRenderingManager->AddDrawableLytWkForOneFrame( app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, GetLayoutWork(0) );
		}
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目追加
 *
 * @param		id		決定時の戻り値
 * @param		str		文字列
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::AddItem( u32 id, gfl2::str::StrBuf * str )
{
  if( m_addCount >= m_param.listMax ){
    GFL_ASSERT( 0 );
    return;
  }

  ITEM * p = m_itemArray[m_addCount];

  p->id  = id;
  p->str = GFL_NEW( m_param.pHeap->GetSystemHeap() ) gfl2::str::StrBuf( *str, m_param.pHeap->GetSystemHeap() );
  
  m_addCount++;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::DeleteMenu(void)
{
  m_mainseq = SEQ_ENDSETUP;
}




//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

void ListMenu::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetDeviceHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}

bool ListMenu::fileReadBlock( app::util::FileAccessor::RequestData* pfrs, gfl2::fs::AsyncFileManager* pAsyncFileMan )
{

  switch(m_loadseq){
  case _OPEN:
    pAsyncFileMan->AddArcFileOpenReq(pfrs->openr);
    m_loadseq = _OPENING;
  case _OPENING:

    if(  pAsyncFileMan->IsArcFileOpenFinished( pfrs->openr.arcId ))
    {
      pAsyncFileMan->AddArcFileLoadDataReq(pfrs->loadr);
      m_loadseq = _LOAD;
    }
    else{
      break;
    }
  case _LOAD:
    if(pAsyncFileMan->IsArcFileLoadDataFinished( pfrs->loadr.ppBuf ) )
    {
      pAsyncFileMan->AddArcFileCloseReq(pfrs->closer);
      m_loadseq = _CLOSE;
    }
    else{
      break;
    }
  case _CLOSE:
    if(pAsyncFileMan->IsArcFileCloseFinished( pfrs->openr.arcId ) )
    {
      m_loadseq = _END;
      return true;
    }
    break;
  case _END:
    return true;
  }
  return false;
}



//--------------------------------------------------------------------------------------------
/**
 * @brief		リソースをこちらで読み込む場合
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::LoadResource(void)
{
  fileReadStructSetup(&m_frs, ARCID_LISTMENU, GARC_listmenu_listmenu_applyt_COMP, m_param.pHeap, &m_pArcReadBuff);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リソース読み込みチェック
 *
 * @param		none
 *
 * @retval	"true = 読み込み中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::CheckLoadResource(void)
{
  if(!fileReadBlock(&m_frs, m_param.pFileManager))
  {
    return true;
  }
  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リソース読み込み中止
 *
 * @param		none
 *
 * @retval	"true = 処理中"
 * @retval	"false = 処理終了"
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::CancelLoadResource(void)
{
  return CheckLoadResource();  //@attention 現状中止はしていない読み込み終了を返すそんなに速度が違わないはず
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レイアウト作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::CreateLayout(void)
{
  /*
	static const gfl::grp::g2d::LytArcIndex anm[] = {
		LYTRES_LIST_MENU_RES_LIST_MENU_ARROW_CUR_BCLAN,
		LYTRES_LIST_MENU_RES_LIST_MENU_UPDOWN_CUR_BCLAN,
		LYTRES_LIST_MENU_RES_LIST_MENU_KETTEI_BCLAN,
		LYTRES_LIST_MENU_RES_LIST_MENU_WINDOW_CHANGE_W_BCLAN,
		LYTRES_LIST_MENU_RES_LIST_MENU_WINDOW_CHANGE_B_BCLAN,
	};

	if( loadFlag == true ){
		param.lytResID = param.lytSys->LoadLytRes( param.devAllocator, resBuff );
		param.lytSys->TransferAllTexture( param.lytResID, gfl::grp::MEMORY_AREA_FCRAM );
	}

	pLytWk = param.lytSys->CreateLytWk(
						param.lytResID, LYTRES_LIST_MENU_RES_LIST_MENU_BCLYT, anm, GFL_NELEMS(anm), GFL_NELEMS(anm), &param.setup, false );

//	pLytWk->SetAutoDrawFlag( param.autoDraw );

	SetAnime( ANMID_CURSOR_NORMAL );
	SetAnime( ANMID_ARROW );
	if( param.winType == WIN_TYPE_WHITE ){
		SetAnime( ANMID_WIN_WHITE );
	}else{
		SetAnime( ANMID_WIN_BLACK );
	}
   */


  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  m_pArcReadBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  app::util::G2DUtil::LytwkData setupData[] = {
    {//上
      0,
      LYTRES_LISTMENU_LIST_MENU_BFLYT,
      LA_LISTMENU_LIST_MENU___NUM,
      m_pArcReadBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
      },
  };


  Create2D( m_param.pHeap , NULL, 1,
            resTbl, GFL_NELEMS(resTbl),
            setupData, GFL_NELEMS(setupData),
            UNUSE_MESSAGE_DATA );

  GetG2DUtil()->StartAnime( 0, LA_LISTMENU_LIST_MENU_ARROW_CUR );
  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( 0 );
  switch(m_param.listMax){
  case 1:
    GetG2DUtil()->StartAnime( 0, LA_LISTMENU_LIST_MENU_WINPATERN_L1,false,false );
    pLytWk->SetAnimeFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L1, pLytWk->GetAnimeMaxFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L1));
    break;
  case 2:
    GetG2DUtil()->StartAnime( 0, LA_LISTMENU_LIST_MENU_WINPATERN_L2_01,false,false );
    pLytWk->SetAnimeFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L2_01, pLytWk->GetAnimeMaxFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L2_01));
    break;
  case 3:
    GetG2DUtil()->StartAnime( 0, LA_LISTMENU_LIST_MENU_WINPATERN_L3,false,false );
    pLytWk->SetAnimeFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L3, pLytWk->GetAnimeMaxFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L3));
    break;
  case 4:
    GetG2DUtil()->StartAnime( 0, LA_LISTMENU_LIST_MENU_WINPATERN_L4,false,false );
   pLytWk->SetAnimeFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L4, pLytWk->GetAnimeMaxFrame(LA_LISTMENU_LIST_MENU_WINPATERN_L4));
    break;
  }
  app::ui::UIView::Update();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レイアウト削除
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::RemoveLayout(void)
{
  Delete2D();
  DeleteGraphicsSystem();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		インスタンスを消してよいかどうか
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::IsInstanceDelete(void)
{
  if(m_mainseq==SEQ_END){
    return true;
  }
  return false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		レイアウトの表示プライオリティ設定
 *
 * @param		priority		プライオリティ
 *
 * @return	none
 *
 *	オート描画時のみ有効な設定
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetPriority( u32 priority )
{
  //	pLytWk->SetPriority( priority );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メニュー表示フラグ設定
 *
 * @param		flg		true = 表示, false = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetVisible( bool flg )
{
  //  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  //	pG2DUtil->GetLayoutWork(0)->Se


  //  SetVisible(flg);

  m_param.visible = flg;
  //	if( param.autoDraw == true ){
  //	pLytWk->SetAutoDrawFlag( flg );
  //	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		現在選択している項目のIdを取得
 *
 * @return	現在選択している項目のId  キャンセルされたらSELECT_CANCEL
 */
//--------------------------------------------------------------------------------------------
u32 ListMenu::GetSelectId( void ) const
{
  if( m_scroll + m_curPos >= m_addCount )
  {
    GFL_ASSERT( 0 );
    return 0;
  }
  if(m_Sequence==SELECT_CANCEL){
     return SELECT_CANCEL;
  }

  return m_itemArray[m_scroll+m_curPos]->id;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		メイン処理
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
u32 ListMenu::Main(void)
{
  app::ui::UIView::Update();

  return m_Sequence;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   カーソル移動
 *
 * @param   mv      移動方向
 * @param   is_trg  true = トリガー入力
 *
 * @retval  "true = 移動"
 * @retval  "false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::MoveCursor( int mv, bool is_trg )
{
  int	pos = m_curPos;

  // スクロールなし
  if( m_addCount <= LIST_POS_MAX )
  {
    pos += mv;
    if( pos < 0 )
    {
      if( is_trg != false )
      {
        pos = m_addCount-1;
      }
      else
      {
        pos = 0;
      }
    }
    else if( pos >= m_addCount )
    {
      if( is_trg != false )
      {
        pos = 0;
      }
      else
      {
        pos = m_addCount-1;
      }
    }
    if( m_curPos != pos )
    {
      m_curPos = pos;
      PutCursor();
      return true;
    }
    return false;
  }

  int	scr = m_scroll;

  if( mv < 0 )
  {
    if( pos == UP_SCROLL_POS && scr != 0 )
    {
      scr += mv;
    }
    else
    {
      pos += mv;
      if( pos < 0 )
      {
        if( is_trg != false )
        {
          MoveCursorDirect( m_addCount-1 );
          return true;
        }
        else
        {
          pos = 0;
        }
      }
    }
  }
  else
  {
    if( pos == DOWN_SCROLL_POS && scr != (m_addCount-LIST_POS_MAX) )
    {
      scr += mv;
    }
    else
    {
      pos += mv;
      if( pos > (LIST_POS_MAX-1) )
      {
        if( is_trg != false )
        {
          MoveCursorDirect( 0 );
          return true;
        }
        else
        {
          pos = LIST_POS_MAX - 1;
        }
      }
    }
  }

  if( pos != m_curPos || scr != m_scroll ){
    if( m_scroll < scr ){
      u32 idx = ScrollPane( 1 );
      m_paneCount++;
      if( m_paneCount >= LIST_POS_MAX ){
        m_paneCount = 0;
      }
      m_scroll = scr;
      PutString( idx, m_itemArray[m_scroll+LIST_POS_MAX-1]->str );
      PutArrow();
    }else if( m_scroll > scr ){
      u32 idx = ScrollPane( -1 );
      m_paneCount--;
      if( m_paneCount < 0 ){
        m_paneCount = LIST_POS_MAX - 1;
      }
      m_scroll = scr;
      PutString( idx, m_itemArray[m_scroll]->str );
      PutArrow();
    }
    if( pos != m_curPos ){
      m_curPos = pos;
      PutCursor();
    }
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定位置へ移動
 *
 * @param		pos		カーソル位置
 *
 * @retval	"true = 移動"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::MoveCursorDirect( u32 pos )
{
  // 現在の位置と同じ
  if( pos == static_cast<u32>(m_scroll+m_curPos) ){
    return false;
  }

  // スクロールなし
  if( m_addCount <= LIST_POS_MAX ){
    m_curPos = pos;
    PutCursor();
    return true;
  }

  // カーソル移動だけでいけるか
  if( m_curPos == UP_SCROLL_POS && pos == static_cast<u32>(m_scroll+m_curPos+1) ){
    m_curPos += 1;
    PutCursor();
    return true;
  }
  if( m_curPos == DOWN_SCROLL_POS && pos == static_cast<u32>(m_scroll+m_curPos-1) ){
    m_curPos -= 1;
    PutCursor();
    return true;
  }

  {
    int	tmp;

    if( pos <= DOWN_SCROLL_POS ){
      m_curPos = pos;
      tmp = 0;
    }else if( pos == static_cast<u32>(m_addCount-1) ){
      m_curPos = LIST_POS_MAX-1;
      tmp = pos - m_curPos;
    }else{
      m_curPos = DOWN_SCROLL_POS;
      tmp = pos - m_curPos;
    }

    if( tmp == m_scroll ){
      PutCursor();
      return true;
    }

    m_scroll = tmp;
  }

  m_paneCount = 0;

  for( int i=0; i<LIST_POS_MAX; i++ ){
    PutString( i, m_itemArray[m_scroll+i]->str );
  }
  SetTextPane( LIST_POS_MAX );

  PutArrow();
  PutCursor();

  return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ウィンドウ配置
 *
 * @param		pos		カーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::Put( u32 pos )
{
  int	max;

  SetSize();
  PutWindowPos();

  m_paneCount = 0;

  if( m_addCount <= LIST_POS_MAX ){
    m_curPos = pos;
    m_scroll = 0;
    max = m_addCount;
  }else{
    if( pos <= DOWN_SCROLL_POS ){
      m_curPos = pos;
      m_scroll = 0;
    }else if( pos == static_cast<u32>(m_addCount-1) ){
      m_curPos = LIST_POS_MAX-1;
      m_scroll = pos - m_curPos;
    }else{
      m_curPos = DOWN_SCROLL_POS;
      m_scroll = pos - m_curPos;
    }
    max = LIST_POS_MAX;
  }

  for( int i=0; i<max; i++ ){
    PutString( i, m_itemArray[m_scroll+i]->str );
    SetVisiblePane( TextBoxPaneTable[i], true );
  }
  for( int i=max; i<LIST_POS_MAX; i++ ){
    SetVisiblePane( TextBoxPaneTable[i], false );
  }

  SetTextPane( max );

  PutArrow();
  //	PutCursor();
  SetVisiblePane( PANENAME_LIST_MENU_PANE_ARROW_00, false );

  SetVisible( m_param.visible );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ウィンドウ表示座標設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::PutWindowPos(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);
  gfl2::lyt::LytPane * win = pLytWk->GetPane( PANENAME_LIST_MENU_PANE_WIN_00 );

	nw::lyt::Size size = win->GetSize();
	gfl2::math::VEC3 pos( m_param.x, m_param.y, 0.0f );
	if( m_param.posTypeX == LIST_POS_LEFT ){
		pos.x += ( size.width / 2.0f );
	}else if( m_param.posTypeX == LIST_POS_RIGHT ){
		pos.x -= ( size.width / 2.0f );
	}
	if( m_param.posTypeY == LIST_POS_TOP ){
		pos.y -= ( size.height / 2.0f );
	}else if( m_param.posTypeY == LIST_POS_BOTTOM ){
		pos.y += ( size.height / 2.0f );
	}
  OHNO_PRINT(" PutWindowPos %d %d \n",pos.x, pos.y);
	win->SetTranslate( pos );
   
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ウィンドウとテキストのサイズ設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetSize(void)
{
  nw::lyt::Size size;
  u32	itemNum;

  size.width  = static_cast<f32>( GetTextPaneWidth() );
  size.height = TEXTBOX_PANE_HEIGHT;

  if( m_addCount <= LIST_POS_MAX ){
    itemNum = m_addCount;
  }else{
    itemNum = LIST_POS_MAX;
  }

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);


  for( u32 i=0; i<itemNum; i++ ){
    //		gfl::grp::g2d::LytTextBox * text = pLytWk->GetTextBoxPane( TextBoxPaneTable[i] );
    //		text->SetSize( size );
    gfl2::lyt::LytTextBox * text = pLytWk->GetTextBoxPane( TextBoxPaneTable[i] );
    text->SetSize(size);
  }

  size.height = ( size.height * itemNum ) + WINDOW_TEX_SIZE_V;
  size.width += WINDOW_TEX_SIZE_H;

  gfl2::lyt::LytWindow * win = pLytWk->GetWindowPane( PANENAME_LIST_MENU_PANE_WIN_00 );
  win->SetSize( size );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目からテキストボックスペインの幅を取得
 *
 * @param		none
 *
 * @return	テキストボックスペインの幅
 */
//--------------------------------------------------------------------------------------------
int ListMenu::GetTextPaneWidth(void)
{
  int	w = TEXT_WIDTH_MINIMUM;

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  gfl2::lyt::LytTextBox * text = pG2DUtil->GetLayoutWork(0)->GetTextBoxPane( TextBoxPaneTable[0] );

  for( int i=0; i<m_addCount; i++ ){
    int tmp = print::GetStringWidth( m_itemArray[i]->str, text );
    if( w < tmp ){
      w = tmp;
    }
  }

  return w;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目描画
 *
 * @param		pane	表示するペインのテーブルインデックス
 * @param		str		文字列
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::PutString( u32 pane, const gfl2::str::StrBuf * str )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytTextBox * text = pG2DUtil->GetLayoutWork(0)->GetTextBoxPane( TextBoxPaneTable[pane] );
  //	gfl::grp::g2d::LytTextBox * text = pLytWk->GetTextBoxPane( TextBoxPaneTable[pane] );
  print::SetStringForLytTextBoxEx( text, str , mLytTextWork );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		テキストボックスペイン配置
 *
 * @param		max		配置数
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetTextPane( u32 max )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);

  gfl2::lyt::LytPane * win = pLytWk->GetPane( PANENAME_LIST_MENU_PANE_WIN_00 );
  nw::lyt::Size size = win->GetSize();
  gfl2::math::VEC3 pos( 0.0f, (size.height-WINDOW_TEX_SIZE_V-TEXTBOX_PANE_HEIGHT)/2.0f, 0.0f );

  for( u32 i=0; i<max; i++ ){
    gfl2::lyt::LytTextBox * txt = pLytWk->GetTextBoxPane( TextBoxPaneTable[i] );
    txt->SetTranslate( pos );
    pos.y -= TEXTBOX_PANE_HEIGHT;

    nw::lyt::HorizontalPosition numbuf[]={
      nw::lyt::HORIZONTALPOSITION_CENTER,
      nw::lyt::HORIZONTALPOSITION_LEFT,
      nw::lyt::HORIZONTALPOSITION_RIGHT,
      nw::lyt::HORIZONTALPOSITION_MAX,
    };

    txt->SetTextPositionH( numbuf[m_param.strPosX] );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペインのスクロール処理
 *
 * @param		mv		移動方向
 *
 * @return	スクロールで配置されたペインのテーブルインデックス
 */
//--------------------------------------------------------------------------------------------
u32 ListMenu::ScrollPane( int mv )
{
  u32	idx;
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);

  if( mv < 0 ){
    gfl2::lyt::LytPane * txt = pLytWk->GetPane( TextBoxPaneTable[m_paneCount] );
    gfl2::math::VEC3 savePos = txt->GetTranslate();

    idx = m_paneCount;

    for( int i=0; i<LIST_POS_MAX-1; i++ ){
      txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
      gfl2::math::VEC3 pos = txt->GetTranslate();
      pos.y -= TEXTBOX_PANE_HEIGHT;
      txt->SetTranslate( pos );
      idx++;
      if( idx >= LIST_POS_MAX ){
        idx = 0;
      }
    }
    txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
    txt->SetTranslate( savePos );
  }else{
    idx = m_paneCount + LIST_POS_MAX - 1;
    if( idx >= LIST_POS_MAX ){
      idx -= LIST_POS_MAX;
    }
    gfl2::lyt::LytPane * txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
    gfl2::math::VEC3 savePos = txt->GetTranslate();

    idx = m_paneCount + 1;
    if( idx >= LIST_POS_MAX ){
      idx = 0;
    }

    for( int i=0; i<LIST_POS_MAX-1; i++ ){
      txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
      gfl2::math::VEC3 pos = txt->GetTranslate();
      pos.y += TEXTBOX_PANE_HEIGHT;
      txt->SetTranslate( pos );
      idx++;
      if( idx >= LIST_POS_MAX ){
        idx = 0;
      }
    }
    txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
    txt->SetTranslate( savePos );
  }

  return idx;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル配置
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::PutCursor(void)
{
	u32	idx = m_paneCount + m_curPos;
	if( idx >= LIST_POS_MAX ){
		idx -= LIST_POS_MAX;
	}
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);

	gfl2::lyt::LytPane * txt = pLytWk->GetPane( TextBoxPaneTable[idx] );
	gfl2::math::VEC3 pos = txt->GetTranslate();
	nw::lyt::Size txtSize = txt->GetSize();

//	gfl2::lyt::LytPane * cur = pLytWk->GetPane( PANENAME_LIST_MENU_PANE_ARROW_00 );
	gfl2::lyt::LytPane * cur = pLytWk->GetPane( PANENAME_LIST_MENU_PANE_NULL_ARROW );
	nw::lyt::Size curSize = cur->GetSize();
	pos.x -= ( ( txtSize.width + curSize.width ) / 2.0f );

	cur->SetTranslate( pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール矢印配置
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::PutArrow(void)
{
  if( m_addCount <= LIST_POS_MAX ){
		SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_UP, false );
		SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_DOWN, false );
	}else{
		if( m_scroll != 0 ){
			SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_UP, true );
		}else{
			SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_UP, false );
		}
		if( m_scroll != (m_addCount-LIST_POS_MAX) ){
			SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_DOWN, true );
		}else{
			SetVisiblePane( PANENAME_LIST_MENU_PANE_CURSOR_DOWN, false );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ペイン表示切り替え
 *
 * @param		index		ペインのインデックス
 * @param		flg			表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetVisiblePane( const gfl2::lyt::LytPaneIndex index, bool flg )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);
  gfl2::lyt::LytPane * pane = pLytWk->GetPane( index );
  pane->SetVisible( flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アニメセット
 *
 * @param		id		アニメID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::SetAnime( u32 id )
{
/*
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);
  if( pLytWk->IsBindAnime( id ) == true ){
    pLytWk->UnBindAnime( id );
  }
  pLytWk->BindAnime( id );
  pLytWk->SetAutoAnime( id, false );
  //	if( param.frameRate == gamesystem::FRAMEMODE_60 ){
  //		pLytWk->SetAutoAnimeSpeed( 0, 0.5f );
  //	}else{
  pLytWk->SetAutoAnimeSpeed( 0, 1.0f );
  //	}
  pLytWk->SetAnimeFrame( id, 0 );
*/
  GetG2DUtil()->StartAnime( 0, id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アニメ停止
 *
 * @param		id		アニメID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ListMenu::StopAnime( u32 id )
{
/*
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);
  pLytWk->UnBindAnime( id );
*/
  GetG2DUtil()->StopAnime( 0, id );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アニメ動作チェック
 *
 * @param		id		アニメID
 *
 * @retval	"true = 動作中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool ListMenu::CheckAnime( u32 id )
{
/*
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLytWk = pG2DUtil->GetLayoutWork(0);
  if( pLytWk->IsAnimeEnd( id ) == true ){
    return false;
  }
  return true;
*/
  return !GetG2DUtil()->IsAnimeEnd( 0, id );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  キー入力の検知
 *         ※タッチパネルの入力が優先される
 *
 * @param  pButton  ボタン ( A,B,X,Y,L,R... )
 * @param  pKey     十字キー
 * @param  pStick   アナログスティック
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//--------------------------------------------------------------------------------------------
ListMenu::ListenerResult ListMenu::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( m_mainseq != SEQ_SELECT )
  {
    return ENABLE_ACCESS;
  }

  {	// ボタン

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
     Sound::PlaySE( LISTMENU_SE_DECIDE );
      m_Sequence = SELECT_SUCCESS;
      return DISABLE_ACCESS;
    }
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
    {
      if( m_param.bMode == B_BUTTON_MODE_CANCEL )
      {
        Sound::PlaySE( LISTMENU_SE_CANCEL );
        m_Sequence = SELECT_CANCEL;
        return DISABLE_ACCESS;
      }
      else if( m_param.bMode != B_BUTTON_MODE_NONE )
      {
        if( MoveCursorDirect( m_param.bMode ) == true )
        {
          Sound::PlaySE( LISTMENU_SE_CURSOR_MOVE );
          return DISABLE_ACCESS;
        }
      }
    }
  }

  {	// 十字キー
    if( pKey->IsTrigger( gfl2::ui::DIR_UP ) )
    {
      if( MoveCursor( -1, true ) != false )
      {
        Sound::PlaySE( LISTMENU_SE_CURSOR_MOVE );
      }
      return DISABLE_ACCESS;
    }
    if( pKey->IsTrigger( gfl2::ui::DIR_DOWN ) )
    {
      if( MoveCursor( 1, true ) != false )
      {
        Sound::PlaySE( LISTMENU_SE_CURSOR_MOVE );
      }
      return DISABLE_ACCESS;
    }
    if( pKey->IsRepeat( gfl2::ui::DIR_UP ) )
    {
      if( MoveCursor( -1, false ) != false )
      {
        Sound::PlaySE( LISTMENU_SE_CURSOR_MOVE );
      }
      return DISABLE_ACCESS;
    }
    if( pKey->IsRepeat( gfl2::ui::DIR_DOWN ) )
    {
      if( MoveCursor( 1, false ) != false )
      {
        Sound::PlaySE( LISTMENU_SE_CURSOR_MOVE );
      }
      return DISABLE_ACCESS;
    }
  }
  return ENABLE_ACCESS;


}


GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )
