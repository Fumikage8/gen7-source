//============================================================================================
/**
 * @file    UIView.cpp
 * @brief    View関連基本クラス
 * @author  Hisanao Suzuki
 * @date    12.2.22
 *
 */
//============================================================================================
#include <AppLib/include/Util/app_util_AppLytObject.h>

#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIViewManager.h"

/* ビルドを通すため
#include "print/layout_default_font.h" //LAYOUT_DEFAULT_FONT
ビルドを通すため */

namespace app {
namespace ui {


//--------------------------------------------------------------------------------------------
/**
 * @brief    コンストラクタ
 *
 * @param   heap         ウインドウの親子関係管理や、ボタンマネージャ用のメモリとして利用される。
 * @param   pLytAlloc    レイアウトワーク生成時に使用されるアロケータ。NULLを指定するとデフォルトアロケータを使用する。
 * @param   pLytDevAlloc レイアウトワーク生成時に使用されるデバイスアロケータ。NULLを指定するとデフォルトアロケータを使用する。
 */
//--------------------------------------------------------------------------------------------
UIView::UIView( app::util::Heap * heap, gfl2::heap::NwAllocator *pLytAlloc, gfl2::heap::NwAllocator* pLytDevAlloc ) :
    app::ui::UIResponder( heap ),
    m_parentView(NULL),
    m_lytAllocator(pLytAlloc),
    m_lytDevAllocator(pLytDevAlloc),
    m_g2dUtil(NULL), m_textureSystem(NULL),
    m_bTexSysCreateSelf(false), m_bLytSysCreateSelf(false), m_bLytWrkCreateSelf(false),
    m_bMessageCreateSelf(false), m_isVisible(true), m_isModal(false), m_isInputEnable(true),
    m_isLayoutUpdating(true),m_isInitApplicationMemory(false),m_layout_transfe_area(gfl2::lyt::MEMORY_AREA_FCRAM),
    m_appLytObjArray(NULL), m_wordSetForAppLytObj(NULL)
{
  m_viewNode.m_view = this;
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetUIView(this);
}


//--------------------------------------------------------------------------------------------
/**
 * @brief  デストラクタ
 */
//--------------------------------------------------------------------------------------------
UIView::~UIView(void)
{
  // 親子関係解除
  // thisを親から取り除く
  if( GetSuperView() != NULL )
  {
    GFL_ASSERT_MSG( GetSuperView() == NULL , "~UIView: Parent exists. this=0x%p, parent=0x%p\n", this, GetSuperView() );
    RemoveFromSuperView();
  }
  // 子をthisから取り除く
  {
    u32 childNum = GetSubViewCount();
#if PM_DEBUG
    if( childNum > 0 )
    {
      GFL_ASSERT_MSG( childNum > 0 , "~UIView: %d Children exist. this=0x%p\n", childNum, this );
      UIViewList::iterator it = m_childViews.end();
      for( u32 i = 0; i < childNum; i++ )
      {
        it--;
        GFL_PRINT( "[%3d]    this=0x%p, child=0x%p\n", i, this, it->m_view );
      }
    }
#endif
    if(childNum > 0)
    {
      RemoveAllSubView();
    }
  }

  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->ResetUIView(this);
}


//--------------------------------------------------------------------------------------------
/**
* @brief  親ウインドウの取得
*
* @return 親ウインドウが存在するならそのポインタ、存在しないならNULLを返却
*/
//--------------------------------------------------------------------------------------------
UIView* UIView::GetSuperView(void)
{
  return m_parentView;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  子ウインドウ数を取得
*
* @return 子ウインドウの数
*/
//--------------------------------------------------------------------------------------------
u32 UIView::GetSubViewCount(void) const
{
  return m_childViews.size();
}


//--------------------------------------------------------------------------------------------
/**
* @brief  指定UIViewが子ウインドウとして登録されているか調査
*
* @param  view 検索対象となるUIViewポインタ
*
* @return 指定ウインドウが子ウインドウとして登録されているならtrueを返却する
*/
//--------------------------------------------------------------------------------------------
bool UIView::IsExistSubView( UIView *view )
{
  if( GetSubViewCount() > 0 )
  {
    for( UIViewList::iterator it = m_childViews.begin(); it != m_childViews.end(); ++it )
    {
      if( it->m_view == view ){
        return true;
      }
    }
  }
  return false;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  子ウインドウを登録する
*         親子関係はポインタ参照のみを管理するため、
*         親のインスタンスを解放したとしても、子のインスタンスは解放されない。
*         なお、子・親のインスタンスが削除される際に親子関係は解除される。
*
* @param  view 子ウインドウへのポインタ
*              親ウインドウへのポインタを引き渡した場合は無視する。
*/
//--------------------------------------------------------------------------------------------
void UIView::AddSubView( UIView* view )
{
  // 登録済み？
  if( (view!=NULL) && !IsExistSubView(view) )
  {
    // 親ウインドウ？
    if( GetSuperView() != view )
    {
      if( view->GetSuperView() == NULL )
      {
        view->m_parentView = this;
        m_childViews.push_back( &view->m_viewNode );
        OnAddChild();
        view->OnAddedToParent();
      }
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief  自身と親ウインドウの親子関係を解除する
*         親ウインドウが存在しない場合は何も行わない
*/
//--------------------------------------------------------------------------------------------
void UIView::RemoveFromSuperView( void )
{
  UIView *superView = GetSuperView();

  if( superView != NULL )
  {
    if( superView->IsExistSubView(this) ){
      superView->m_childViews.erase( &this->m_viewNode );
      superView->OnRemoveChild();
      OnRemovedFromParent();
    }
    m_parentView = NULL;
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief  全ての子ウインドウの登録を解除する
*         子ウインドウが存在しない場合は何も行わない
*/
//--------------------------------------------------------------------------------------------
void UIView::RemoveAllSubView( void )
{
  u32 childNum = GetSubViewCount();
  if( childNum > 0 )
  {
    UIViewList::iterator it = m_childViews.end();

    for( u32 i = 0; i < childNum; i++ )
    {
      it--;
      it->m_view->RemoveFromSuperView();

      if( m_childViews.size() <= 0 )
      {
        break;
      }

      //  内部でeraseして減らしているので常にiteratorは更新しないと複数登録時でやばい
      it = m_childViews.end();
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief  入力操作制御
*
* @return 以降の入力操作を継続するならtrueを返却
*/
//--------------------------------------------------------------------------------------------
bool UIView::DoInputAction( void )
{
  if( IsVisible() )
  {
    if(!GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->IsRespondUIView(this)){
      //優先主張状態の場合、ここはtrueを返す
      return true;  
    }
    
    // 自身の入力処理
    this->UpdateResponse( this, m_isInputEnable );

    // モーダルウインドウ時は以降の入力を禁止する
    if( this->IsModal() ){
      return false;
    }

    //アニメ中は以降の入力を禁止する
    if( this->IsButtonAnimation() && this->IsAnimationExclusive() )
    {
      return false;
    }

    // 入力の排他制御
    if( this->IsInputExclusive() && this->IsInputAction() ){
      return false;
    }
  }
  else
  {
    // 自身の入力処理 @note ボタンマネージャのアップデートだけする
    this->UpdateResponse( this, false );
  }

  return true;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  モーダルウインドウ判定
*
* @return モーダルウインドウであるならtrueを、モーダレスウインドウであればfalseを返却
*/
//--------------------------------------------------------------------------------------------
bool UIView::IsModal(void) const
{
  return m_isModal;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  自身をモーダルウインドウとして設定する
*         モーダルウインドウと設定した場合、自身以降のウインドウのキー入力を禁止する
*/
//--------------------------------------------------------------------------------------------
void UIView::SetModal(bool bmode)
{
  m_isModal = bmode;
}


//--------------------------------------------------------------------------------------------
/**
* @brief  定期更新処理
*         親ウインドウは本関数にて更新処理を実行することで、「子→親」の順に
*         Update()を実行する。
*/
//--------------------------------------------------------------------------------------------
void UIView::UpdateTree( void )
{
  m_isInputEnable = true;
  this->Update();
}


//--------------------------------------------------------------------------------------------
/**
* @brief  定期更新処理
*         子ウインドウについても自動的にコールする(子→親の順）
*/
//--------------------------------------------------------------------------------------------
void UIView::Update(void)
{
  // 親が存在しないなら、入力排他フラグをクリア
  if( this->GetSuperView() == NULL ){
    this->m_isInputEnable = true;
  }

  // 最下層のサブウインドウから実行
  u32 childNum = GetSubViewCount();
  if( childNum > 0 )
  {
    UIViewList::iterator it = m_childViews.end();
    for( u32 i = 0; i < childNum; i++ )
    {
      it--;
      it->m_view->m_isInputEnable = this->m_isInputEnable;
      it->m_view->Update();
      if( childNum != GetSubViewCount() )
      {//イベントリスナーの中で子の追加削除が行われた
        GFL_ASSERT( 0 );//@fix
        break;
      }
      this->m_isInputEnable = it->m_view->m_isInputEnable;
    }
  }

  if( !DoInputAction() ){
    this->m_isInputEnable = false;
  }

  // レイアウト更新
  if( m_isLayoutUpdating == true )
  {
    UpdateG2DUtil();
  }
}

//--------------------------------------------------------------------------------------------
/**
* @brief  G2DUtil定期更新処理
*/
//--------------------------------------------------------------------------------------------
void UIView::UpdateG2DUtil(void)
{
  if( IsVisible() )
  {
    if( m_g2dUtil != NULL ){
      m_g2dUtil->Update2D();
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief レイアウトの転送エリア指定
 *
 * @param area  MemoryArea  （デフォルトはFCRAM）
 * 
 * @note  create2Dの前に呼んでください
 */
//--------------------------------------------------------------------------------------------
void UIView::SetLayoutTransferArea( gfl2::lyt::MemoryArea area )
{
  m_layout_transfe_area = area;
}

//--------------------------------------------------------------------------------------------
/**
* @brief  ウインドウリストの表示。
*         親ウインドウは本関数にて描画を実行することで、「親→子」の順に
*         Draw()を実行する。
*/
//--------------------------------------------------------------------------------------------
void UIView::DrawTree( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( IsVisible() )
  {
    this->Draw( displayNo );

    if( GetSubViewCount() > 0 )
    {
      for( UIViewList::iterator it = m_childViews.begin(); it != m_childViews.end(); ++it )
      {
        it->m_view->DrawTree( displayNo );
      }
    }
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief   可視状態の設定
*          不可視状態時は描画、操作通知ともに無効となる。
*
* @param   isVisible  可視状態に設定するならtrueを指定
*/
//--------------------------------------------------------------------------------------------
void UIView::SetVisible( bool isVisible )
{
  m_isVisible = isVisible;
}

//--------------------------------------------------------------------------------------------
/**
* @brief   可視状態の取得
*
* @return  可視状態ならtrueを返却
*/
//--------------------------------------------------------------------------------------------
bool UIView::IsVisible( void ) const
{
  return m_isVisible;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク更新有無フラグ
 *
 * @param   isLayoutUpdating  更新するならtrueを指定
 */
//--------------------------------------------------------------------------------------------
void UIView::SetLayoutUpdating( bool isLayoutUpdating )
{
  m_isLayoutUpdating = isLayoutUpdating;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトワーク取得
 *
 * @param   layoutId 対象レイアウトＩＤ
 *
 * @return  レイアウトワーク
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytWk * UIView::GetLayoutWork( u32 layoutId ) const
{
  if( m_g2dUtil == NULL ){
    return NULL;
  }
  return m_g2dUtil->GetLayoutWork( layoutId );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   レイアウトシステム取得
 *
 * @return  レイアウトシステム
 */
//--------------------------------------------------------------------------------------------
gfl2::lyt::LytSys * UIView::GetLayoutSystem( void ) const
{
  if( m_g2dUtil == NULL ){
    return NULL;
  }
  return m_g2dUtil->GetLayoutSystem();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   G2DUtil取得
 *
 * @return  Viewシステム
 */
//--------------------------------------------------------------------------------------------
app::util::G2DUtil* UIView::GetG2DUtil( void ) const
{
  return m_g2dUtil;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief    Viewシステム作成
 *
 * @param    heap    ヒープ
 */
//--------------------------------------------------------------------------------------------
void UIView::CreateGraphicsSystem( app::util::Heap * heap, bool isInitApplicationMemory )
{
  /* ビルドを通すため
  m_graphicSystem = gm->GetGraphicsSystem();

  if( isInitApplicationMemory )
  {
    m_graphicSystem->InitializeApplicationMemory( heap->pDevHeapAllocator );
    gfl::grp::util::DrawUtil::PushAllocator( heap->pHeapAllocator, heap->pDevHeapAllocator );
    m_isInitApplicationMemory = isInitApplicationMemory;
  }
  ↓の処理に差し替え*/
  if( isInitApplicationMemory )
  {
  /*
    gfl::grp::util::DrawUtil::PushAllocator( heap->pHeapAllocator, heap->pDevHeapAllocator );
  */
    m_isInitApplicationMemory = isInitApplicationMemory;
  }

}


//--------------------------------------------------------------------------------------------
/**
 * @brief    Viewシステム削除
 */
//--------------------------------------------------------------------------------------------
void UIView::DeleteGraphicsSystem(void)
{
  if( m_isInitApplicationMemory )
  {
/* ビルドを通すため
    gfl::grp::util::DrawUtil::PopAllocator();
    m_graphicSystem->FinalizeApplicationMemory();
ビルドを通すため */
    m_isInitApplicationMemory = false;
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   Viewシステム取得
 *
 * @return  Viewシステム
 */
//--------------------------------------------------------------------------------------------
/* ビルドを通すため
gfl2::grp::GraphicsSystem* UIView::GetGraphicSystem(void) const
{
  return m_graphicSystem;
}
↓に差し替え*/
gfl2::lyt::LayoutRenderPath* UIView::GetLaoutRenderPath(void) const
{
  return m_pLytRenderPath;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    2D関連作成
 *
 * @param   heap                  ヒープ
 *          layout                レイアウトシステム
 *          layoutNum             レイアウト数
 *          lytResTbl             レイアウトリソースデータ
 *          lytRestblMax          上記配列の数
 *          layoutData            レイアウトデータ
 *          layoutNum             レイアウトデータ数
 *          message               メッセージデータID
 *          createPaneTbl         ペインテーブルを作り検索時間を短縮するかtrue or false
 *          createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り、生成時間を短縮するかtrue or false
 */
//--------------------------------------------------------------------------------------------
void UIView::Create2D( app::util::Heap* heap,
                       gfl2::lyt::LytSys* layout, u32 layoutNum,
                       const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
                       const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
                       u32 message,
                       b32 createPaneTbl, b32 createPaneTblWithHash,
                       s32 wordsetBufferNum )
{
  // レイアウトをつくる前の処理
  if( message == USE_APPLYT_MESSAGE_DATA )
  {
    m_appLytObjArray = GFL_NEW_ARRAY(heap->GetDeviceHeap()) app::util::AppLytObject[lytRestblMax];
    for(u32 i=0; i<lytRestblMax; ++i)
    {
      m_appLytObjArray[i].SyncCreate(lytResTbl[i].buf, heap->GetDeviceHeap());
    }
  }

  // レイアウトをつくる
  Initalize2D( heap, layout, layoutNum, lytResTbl, lytRestblMax, layoutData, layoutDataNum, createPaneTbl, createPaneTblWithHash );

  // レイアウトをつくった後の処理
  if( message == UNUSE_MESSAGE_DATA ){
    m_bMessageCreateSelf = false;
  }
  else if( message == USE_APPLYT_MESSAGE_DATA ){
    m_bMessageCreateSelf = false;

    m_wordSetForAppLytObj = GFL_NEW(heap->GetDeviceHeap()) print::WordSet( wordsetBufferNum, print::WordSet::DEFAULT_BUF_LEN, heap->GetDeviceHeap() );
    //app::util::AppLytObject::SetWordSetToG2DUtil(m_g2dUtil, m_wordSetForAppLytObj, m_pWordSetLoader);

    for(u32 i=0; i<layoutDataNum; ++i)
    {
      u32 res_index = layoutData[i].res_index;
      u32 lytres_id = layoutData[i].lytres_id;
      m_appLytObjArray[res_index].SetMessageToTextBoxPane(m_g2dUtil, m_wordSetForAppLytObj, i, lytres_id);
    }
  }
  else
  {
    m_g2dUtil->CreateMessageData( message, gfl2::str::MsgData::LOAD_FULL, heap , wordsetBufferNum );
    m_g2dUtil->SetWordSetLoader(m_pWordSetLoader);
    m_bMessageCreateSelf = true;
  }
}

//--------------------------------------------------------------------------------------------
/**
* @brief   2D関連作成
*
* @note    リソースデータからLayoutWork、メッセージデータを内部領域に生成する。
*
* @param   heap                  ヒープ
*          layout                レイアウトシステム
*          layoutNum             レイアウト数
*          lytResTbl             レイアウトリソースデータ
*          lytRestblMax          上記配列の数
*          layoutData            レイアウトデータ
*          layoutDataNum         レイアウトデータ数
*          messageData           メッセージデータインスタンス
*          wordset               ワードセットインスタンス
*          createPaneTbl         ペインテーブルを作り検索時間を短縮するかtrue or false
*          createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り、生成時間を短縮するかtrue or false
*/
//--------------------------------------------------------------------------------------------
void UIView::Create2D( app::util::Heap * heap,
                gfl2::lyt::LytSys *layout,  u32 layoutNum,
                const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
                const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
                gfl2::str::MsgData *messageData, print::WordSet *wordset,
                b32 createPaneTbl,
                b32 createPaneTblWithHash,
                s32 wordsetBufferNum )
{
  Initalize2D( heap, layout, layoutNum, lytResTbl, lytRestblMax, layoutData, layoutDataNum, createPaneTbl, createPaneTblWithHash );

  m_g2dUtil->SetMessageData( messageData, wordset );
  m_g2dUtil->SetWordSetLoader(m_pWordSetLoader);
  m_bMessageCreateSelf = false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 *
 * @param   heap        ヒープ
 *          layout      外部レイアウトシステム
 *          layoutNum   外部レイアウト数
 *          layoutWork  外部レイアウトワーク
 *          layoutNum   外部レイアウトデータ数
  *         layoutResID 外部レイアウトリソースID
 *          messageData 外部メッセージデータインスタンス
 *          messageData 外部ワードセットインスタンス
 *
 * @note    Updateの内部でレイアウトワークの更新を行いません。
 *          更新をしたい場合はSetLayoutUpdating( true )を処理してください。
 */
//--------------------------------------------------------------------------------------------
void UIView::Create2D( app::util::Heap* heap,
                       gfl2::lyt::LytSys* layout, const u32 layoutNum,
                       app::util::G2DUtil::LAYOUT_WORK *lytWork, const u32 lytWorkMax,
                       gfl2::lyt::LytMultiResID* layoutResID,
                       gfl2::str::MsgData *messageData, print::WordSet *wordset )

{
  Initalize2D( heap, layout, layoutNum, lytWork, lytWorkMax, layoutResID );
  m_g2dUtil->SetMessageData( messageData, wordset );
  m_g2dUtil->SetWordSetLoader(m_pWordSetLoader);

  m_bMessageCreateSelf = false;
  m_isLayoutUpdating   = false;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   2D関連の初期化
 *
 * @param   heap                  ヒープ
 *          layout                レイアウトシステム
 *          layoutNum             レイアウト数
 *          lytResTbl             レイアウトリソースデータ
 *          lytRestblMax          上記配列の数
 *          layoutData            レイアウトデータ
 *          layoutNum             レイアウトデータ数
 *          createPaneTbl         ペインテーブルを作り検索時間を短縮するかtrue or false
 *          createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り、生成時間を短縮するかtrue or false
 */
//--------------------------------------------------------------------------------------------
void UIView::Initalize2D( app::util::Heap* heap,
    gfl2::lyt::LytSys* layout, u32 layoutNum,
    const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
    const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
    b32 createPaneTbl, b32 createPaneTblWithHash )
{
  m_g2dUtil = GFL_NEW( heap->GetSystemHeap() ) app::util::G2DUtil( heap );


  m_g2dUtil->CreateAppLytAccessor(heap);

  if( layout == NULL )
  {
    m_g2dUtil->CreateLayoutSystem( heap );
    m_bLytSysCreateSelf = true;
  }
  else
  {
    m_g2dUtil->SetLayoutSystem( layout );
    m_bLytSysCreateSelf = false;
  }
  m_bLytWrkCreateSelf = true;

  //継承先で実装する
  OnPreLoadResource();

  m_g2dUtil->CreateLayoutEnv( heap, false );

  {
    m_g2dUtil->CreateLayoutWork( lytResTbl, lytRestblMax, layoutNum, heap, m_layout_transfe_area );
  }

  m_g2dUtil->AttachLayoutResourceReference( lytResTbl, lytRestblMax );
  m_g2dUtil->SetLayoutWork( 0, layoutData, layoutDataNum, m_lytAllocator, m_lytDevAllocator, createPaneTbl, createPaneTblWithHash );

  //@caution zukan_IViewにてvirtualで処理を足しているので、
  //ここに処理追加したらnagihashiまで連絡ください。
  //↑このnagihashiさんのコメントは別のUIView::Initalize2Dに書いてあったもの。
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   外部リソースを流用した2D関連の初期化
 *
 * @param   heap       ヒープ
 *          layout     外部レイアウトシステム
 *          layoutNum  外部レイアウト数
 *          layoutWork 外部レイアウトワーク
 *          layoutNum  外部レイアウトデータ数
 */
//--------------------------------------------------------------------------------------------
void UIView::Initalize2D( app::util::Heap* heap,
                          gfl2::lyt::LytSys* layout, const u32 layoutNum,
                          app::util::G2DUtil::LAYOUT_WORK *lytWork, const u32 lytWorkMax,
                          gfl2::lyt::LytMultiResID* layoutResID )
{
  m_g2dUtil = GFL_NEW( heap->GetSystemHeap() ) app::util::G2DUtil( heap );

  m_g2dUtil->CreateAppLytAccessor(heap);

  m_g2dUtil->SetLayoutSystem( layout );
  m_g2dUtil->SetLayoutWork( lytWork, lytWorkMax );
  m_g2dUtil->SetLayoutResourceID( layoutResID );
  m_bLytSysCreateSelf = false;
  m_bLytWrkCreateSelf = false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief    2D関連削除
 */
//--------------------------------------------------------------------------------------------
void UIView::Delete2D(void)
{
  GFL_SAFE_DELETE(m_wordSetForAppLytObj);
  GFL_SAFE_DELETE_ARRAY(m_appLytObjArray);

  if( m_g2dUtil != NULL)
  {
    m_g2dUtil->DeleteMsgCursor();
    m_g2dUtil->DeleteMessageWindow();

    if( m_bMessageCreateSelf )
    {
      m_g2dUtil->DeleteMessageData();
      m_bMessageCreateSelf = false;
    }
    if( m_bLytWrkCreateSelf )
    {
      m_g2dUtil->DeleteLayoutWork();
      m_g2dUtil->DeleteLayoutEnv();
      m_bLytWrkCreateSelf = false;
    }
    if( m_bLytSysCreateSelf )
    {
      m_g2dUtil->DeleteLayoutSystem();
      m_bLytSysCreateSelf = false;
    }

    m_g2dUtil->DeleteAppLytAccessor();
    GFL_SAFE_DELETE( m_g2dUtil );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   テクスチャシステム作成
 *
 * @param   heap       ヒープ
 * @param   devHeap    デバイスヒープ
 * @param   file_read_manager ファイル読み込み管理クラス
 * @param   arcId      ARCファイルID
 * @param   texNum     読み込みテクスチャー枚数
 * @param   bufferSize テクスチャシステム用バッファサイズ
 */
//--------------------------------------------------------------------------------------------
void UIView::CreateTextureSystem( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, gfl2::fs::AsyncFileManager *file_read_manager, u32 arcId, u32 texNum, u32 bufferSize )
{
  GFL_SAFE_DELETE( m_textureSystem );

  m_textureSystem     = GFL_NEW( heap ) app::tool::LytTexReplaceSystem( heap, devHeap, bufferSize, texNum );
  m_bTexSysCreateSelf = true;

  for( u32 i=0; i<texNum; i++ ) {
    m_textureSystem->LoadTexture( devHeap, i, arcId, i );
  }
}

//--------------------------------------------------------------------------------------------
/**
* @brief   テクスチャ単体ロード
*
* @param   heap       ヒープ
* @param   devHeap    デバイスヒープ
* @parma   file_read_manager ファイル読み込み制御クラス
* @param   arcId      ARCファイルID
* @param   arcDataId
*/
//--------------------------------------------------------------------------------------------
void UIView::LoadTexture( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, gfl2::fs::AsyncFileManager *file_read_manager, u32 arcId, u32 arcDataId, u32 texId, bool compressed )
{
  if( m_textureSystem == NULL )
  {
    m_textureSystem     = GFL_NEW( heap ) app::tool::LytTexReplaceSystem( heap, devHeap, 0, 1 );
    m_bTexSysCreateSelf = true;
  }

  m_textureSystem->LoadTexture( devHeap, texId, arcId, arcDataId, compressed );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    外部で準備されたテクスチャシステムを再利用する
 *
 * @param   textureSystem 再利用するテクスチャーシステム
 */
//--------------------------------------------------------------------------------------------
void UIView::ReUseTextureSystem( app::tool::LytTexReplaceSystem* textureSystem )
{
  m_textureSystem     = textureSystem;
  m_bTexSysCreateSelf = false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   テクスチャシステム削除
 */
//--------------------------------------------------------------------------------------------
void UIView::DeleteTextureSystem(void)
{
  if( m_bTexSysCreateSelf )
  {
    GFL_SAFE_DELETE( m_textureSystem );
    m_bTexSysCreateSelf = false;
  }
}


//--------------------------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャ差し替え
 *
 * @param   lytID  レイアウトID
 * @param   pane   ペインID
 * @param   texID  テクスチャID
 */
//--------------------------------------------------------------------------------------------
void UIView::ReplacePaneTexture( u32 lytID, u32 pane, u32 texID )
{
  m_textureSystem->ReplaceTexture( texID, m_g2dUtil->GetLayoutWork(lytID)->GetPicturePane( pane ), 0, 0 );
}


//--------------------------------------------------------------------------------------------
 /**
 * @brief ペインにメッセージをセットする
 * @param lytID      対象レイアウトID
 * @param paneId     対象ペインのインデックス
 * @param messageId  セットするメッセージ
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneMessage( u32 lytID, u32 paneId, s32 messageId )
{
  if( messageId >= 0 )
  {
    gfl2::str::StrBuf *buf = GetMessageStrBuf( messageId );

    if( buf != NULL ){
      m_g2dUtil->SetTextBoxPaneString( lytID, paneId, buf );
      return;
    }
  }

  SetTextboxPaneString( lytID, paneId, L"" );
}

//--------------------------------------------------------------------------------------------
  /**
  * @brief ペインにメッセージをセットする(ペインポインタ版)
  * @param paneAddr   テキストペインアドレス
  * @param messageId  セットするメッセージ
  */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneMessage( gfl2::lyt::LytTextBox* pTextBoxPane, s32 messageId )
{
  if( messageId >= 0 )
  {
    gfl2::str::StrBuf *buf = GetMessageStrBuf( messageId );

    if( buf != NULL )
    {
      print::SetStringForLytTextBox( pTextBoxPane, buf );
      return;
    }
  }

  print::SetStringForLytTextBox( pTextBoxPane, L"" );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ペインにメッセージをセットする(ペインポインタ/文字列バッファ版)
 * @param paneAddr   テキストペインアドレス
 * @param buf        文字列バッファ
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneMessage( gfl2::lyt::LytTextBox * pTextBoxPane, const gfl2::str::StrBuf * buf )
{
  print::SetStringForLytTextBox( pTextBoxPane, buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ペインに数字をセットする
 * @param   lytID       レイアウトのID
 * @param   pane        ペインのインデックス
 * @param   strID				文字列ID
 * @param   value       数値
 * @param   keta        桁
 * @param		bufID				バッファID　※デフォルト = 0
 * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
 * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneNumber(
  u32 lytID, u32 paneId, u32 messageId, u32 value, u32 keta, u32 bufID,
  print::NumberDispType dispType, print::NumberCodeType codeType
){
  m_g2dUtil->SetRegisterNumber( bufID, value, keta, dispType, codeType );

  SetTextboxPaneMessage( lytID, paneId, messageId );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ペインに数字をセットする(ペインポインタ版)
 * @param   pTextBox    テキストボックスペイン
 * @param   strID				文字列ID
 * @param   value       数値
 * @param   keta        桁
 * @param		bufID				バッファID　※デフォルト = 0
 * @param   dispType    整形タイプ　※デフォルト = NUM_DISP_LEFT
 * @param   codeType    文字コード　※デフォルト = NUM_CODE_DEFAULT
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneNumber(
    gfl2::lyt::LytTextBox* pTextBoxPane,
    u32 messageId, u32 value, u32 keta, u32 bufID,
    print::NumberDispType dispType, print::NumberCodeType codeType
){
  m_g2dUtil->SetRegisterNumber( bufID, value, keta, dispType, codeType );

  SetTextboxPaneMessage( pTextBoxPane, messageId );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   テキストボックスペインに文字列を描画(ペインポインタ版)
 *
 * @param   pTextBox    テキストボックスペイン
 * @param   str         文字列
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneString( gfl2::lyt::LytTextBox* pTextBox, gfl2::str::StrBuf* pStrBuf )
{
  if( pTextBox != NULL )
  {
    print::SetStringForLytTextBox( pTextBox, pStrBuf );
  }
  else {
    GFL_ASSERT(0);
  }
}

//--------------------------------------------------------------------------------------------
 /**
 * @brief ペインに文字列をセットする(wchar_t版）
 * @param lytID     対象レイアウトID
 * @param paneId    対象ペインのインデックス
 * @param string    セットする文字列
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneString( u32 lytID, u32 paneId, const wchar_t* string )
{
  m_g2dUtil->SetTextBoxPaneString( lytID, paneId, string );
}


//--------------------------------------------------------------------------------------------
 /**
 * @brief ペインに文字列をセットする (STRBUF版）
 * @param lytID     対象レイアウトID
 * @param paneId    対象ペインのインデックス
 * @param string    セットする文字列
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneString( u32 lytID, u32 paneId, gfl2::str::StrBuf *strbuf )
{
  m_g2dUtil->SetTextBoxPaneString( lytID, paneId, strbuf );
}

//--------------------------------------------------------------------------------------------
//@brief    タッチされているペインを取得(ペインポインタ指定)
//--------------------------------------------------------------------------------------------
bool UIView::GetToucHoldPane( const gfl2::ui::TouchPanel* tp, u32 lytID, gfl2::lyt::LytPane* pPane ) const
{
  gfl2::lyt::LytWk* pLayoutWork = GetG2DUtil()->GetLayoutWork( lytID );
  
  // 一度描画しないとチェック不可
  if( !pLayoutWork->IsHitCheckOK() ) return false;

  if( tp->IsTouch() != false )
  {
    return GetHitPane( lytID, pPane, tp->GetX(), tp->GetY());
  }

  return false;
}

//--------------------------------------------------------------------------------------------
//@brief    ペインのヒット判定
//--------------------------------------------------------------------------------------------
bool UIView::GetHitPane( u32 lytID, gfl2::lyt::LytPane* pPane, u16 x, u16 y, bool visible ) const
{
  gfl2::lyt::LytWk* pLayoutWork = GetG2DUtil()->GetLayoutWork( lytID );

  // 一度描画しないとチェック不可
  if( !pLayoutWork->IsHitCheckOK() ) return false;

  if( !visible )
  {
    if( !GetG2DUtil()->GetPaneVisible(pPane) )
    {
      return false;
    }
  }

  return pLayoutWork->GetHitPane( x, y, pPane );
}

//--------------------------------------------------------------------------------------------
 /**
  * @brief   指定ＩＤのメッセージを格納したバッファへのポインタを取得
  *
  * @param   messageId 取得したいメッセージＩＤ
  *
  * @return  メッセージバッファへのポインタ
  */
 //--------------------------------------------------------------------------------------------
gfl2::str::StrBuf * UIView::GetMessageStrBuf( u32 messageId )
{
  gfl2::str::StrBuf * buf = m_g2dUtil->GetTempStrBuf( 1 );
  m_g2dUtil->GetStringExpand( buf, messageId );
  return buf;
}


//--------------------------------------------------------------------------------------------
 /**
  * @brief   上画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   alpha        表示する矩形のアルファ値
  */
 //--------------------------------------------------------------------------------------------
void UIView::DrawUpperMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer, float alpha )
{
  DrawMaskRect( graphics, targetBuffer, 0, 0, DISPLAY_UPPER_WIDTH, DISPLAY_UPPER_HEIGHT, alpha );
}


//--------------------------------------------------------------------------------------------
 /**
  * @brief   上画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   alpha        表示する矩形のアルファ値
  */
 //--------------------------------------------------------------------------------------------
void UIView::DrawLowerMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer, float alpha )
{
  DrawMaskRect( graphics, targetBuffer, 0, 0, DISPLAY_LOWER_WIDTH, DISPLAY_LOWER_HEIGHT, alpha );
}


//--------------------------------------------------------------------------------------------
 /**
  * @brief   画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   x            マスク矩形のＸ座標
  * @param   y            マスク矩形のＹ座標
  * @param   width        マスク矩形の横幅
  * @param   height       マスク矩形の縦幅
  * @param   alpha        表示する矩形のアルファ値
  */
 //--------------------------------------------------------------------------------------------
void UIView::DrawMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer,
                           u32 x, u32 y, u32 width, u32 height, float alpha )
{
  DrawMaskRect( graphics, targetBuffer, x, y, width, height, 0.0f, 0.0f, 0.0f, alpha );
}

//--------------------------------------------------------------------------------------------
 /**
  * @brief   画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   x            マスク矩形のＸ座標
  * @param   y            マスク矩形のＹ座標
  * @param   width        マスク矩形の横幅
  * @param   height       マスク矩形の縦幅
  * @param   red          カラー値（R）
  * @param   green        カラー値（G）
  * @param   blue         カラー値（B）
  * @param   alpha        表示する矩形のアルファ値
  */
 //--------------------------------------------------------------------------------------------
void UIView::DrawMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer,
                           u32 x, u32 y, u32 width, u32 height,
                           float red, float green, float blue, float alpha )
{
/* ビルドを通すため( 下記の型が定義されていない )
  gfl2::grp::util::DrawUtil
  gfl2::grp::RenderTarget

  gfl2::grp::RenderTarget* targetRender = graphics->GetCurrentFrameBuffer( targetBuffer );
  if( graphics->BindCurrentCommandList( targetRender ) )
  {

    gfl2::grp::util::DrawUtil::BeginRender( targetRender ); // お約束[Begin]
    gfl2::grp::util::DrawUtil::MaterialInfo material;
    material.m_ShadingType = gfl2::grp::util::DrawUtil::MaterialInfo::COLOR_SHADER;
    gfl2::grp::util::DrawUtil::SetMaterial( material );

    gfl2::math::VEC4 color( red, green, blue, alpha );
    gfl2::grp::util::DrawUtil::DrawRect( x, y, width, height, color );  // ?頂点カラーで矩形を描画

    gfl2::grp::util::DrawUtil::EndRender();                  // お約束[End]
  }
  ビルドを通すため */
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 自分と子ビューすべての入力設定
 *
 * @param isEnable
 */
//--------------------------------------------------------------------------------------------
void UIView::SetInputEnabledAllView(  bool isEnable )
{
  this->SetInputEnabled( isEnable );

  if( GetSubViewCount() > 0 )
  {
    for( UIViewList::iterator it = m_childViews.begin(); it != m_childViews.end(); ++it )
    {
      it->m_view->SetInputEnabledAllView( isEnable ); //スミマセン再帰です。
    }
  }
}

//--------------------------------------------------------------------------------------------
 /**
 * @brief テキストボックスペインにタグプロセッサを登録する
 * @param lytID      対象レイアウトID
 * @param paneId     対象ペインのインデックス
 * @param tagProc    タグプロセッサへのポインタ
 */
//--------------------------------------------------------------------------------------------
void UIView::SetTextboxPaneTagProcessor( u32 lytID, u32 paneId, nw::lyt::TextBox::TagProcessor* tagProc )
{
  m_g2dUtil->SetTextboxPaneTagProcessor( lytID, paneId, tagProc );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief ペインの矩形を取得する
 * @param layout_work   ペインが所属するレイアウトワーク
 * @param pane_index    対象ペインのインデックス
 * @return ペインの矩形サイズ
 */
//--------------------------------------------------------------------------------------------
const nw::ut::Rect UIView::GetPaneRect( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  return m_g2dUtil->GetPaneRect( lytID, index );
}


//--------------------------------------------------------------------------------------------
 /**
  * @brief   ペイン座標の取得
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   paneId    取得対象ペインＩＤ
  *
  * @return  ペイン座標値
  */
 //--------------------------------------------------------------------------------------------
gfl2::math::VEC3 UIView::GetPanePos( u32 layoutId, u32 paneId ) const
{
  gfl2::lyt::LytPane * basePane = m_g2dUtil->GetLayoutWork( layoutId )->GetPane( paneId );
  return basePane->GetTranslate();
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペインの画面座標を取得(1度、描画されている必要がある）
 *
 * @param   layoutId ペインが属するレイアウトＩＤ
 * @param   paneId   取得対象ペインＩＤ
 *
 * @return  ペインの画面座標値
 */
//-----------------------------------------------------------------------------
gfl2::math::VEC3 UIView::GetPaneScreenPos( u32 layoutId, u32 paneId )
{
  return m_g2dUtil->GetPanePos( layoutId, paneId );
}

//--------------------------------------------------------------------------------------------
 /**
  * @brief   ペイン座標の設定
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 取得対象ペインＩＤ
  * @param   pos      設定座標
  */
 //--------------------------------------------------------------------------------------------
void UIView::SetPanePos( u32 layoutId, u32 paneId, gfl2::math::VEC3* pos )
{
  m_g2dUtil->SetPanePos( layoutId, paneId, pos );
}

//--------------------------------------------------------------------------------------------
 /**
  * @brief   ペインのスケール設定
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 設定対象ペインＩＤ
  * @param   scale    スケール値
  */
 //--------------------------------------------------------------------------------------------
void UIView::SetPaneScale( u32 layoutId, u32 paneId, gfl2::math::VEC2* scale )
{
  m_g2dUtil->SetScale( layoutId, paneId, *scale );
}


//--------------------------------------------------------------------------------------------
  /**
   * @brief   ペインのスケール取得
   *
   * @param   layoutId ペインが属するレイアウトＩＤ
   * @param   layoutId 設定対象ペインＩＤ
   *
   * @return  スケール値
   */
  //--------------------------------------------------------------------------------------------
void UIView::GetPaneScale( u32 layoutId, u32 paneId, gfl2::math::VEC2 * scale ) const
{
  m_g2dUtil->GetScale( layoutId, paneId, scale );
}


//--------------------------------------------------------------------------------------------
 /**
  * @brief   ペインサイズの設定
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 取得対象ペインＩＤ
  *
  * @return  ペインのサイズ
  */
 //--------------------------------------------------------------------------------------------
nw::lyt::Size UIView::GetPaneSize( u32 lytID, gfl2::lyt::LytPaneIndex index ) const
{
  nw::lyt::Size size = m_g2dUtil->GetSize( lytID, index );
  return size;
}


//--------------------------------------------------------------------------------------------
 /**
 * @brief テクスチャーシステムの取得
 *
 * @return テクスチャーシステムへのポインタ
 */
//--------------------------------------------------------------------------------------------
app::tool::LytTexReplaceSystem*  UIView::GetTextureSystem( void )
{
  return m_textureSystem;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief LytWkセットアップ構造体の取得
 *
 * @param disp 取得対象のディスプレイタイプ
 *
 * @return LytWkセットアップ構造体へのポインタ
 */
//--------------------------------------------------------------------------------------------
const gfl2::lyt::LytWkSetUp * UIView::GetLytSetup( gfl2::lyt::DisplayType disp )
{
  return m_g2dUtil->GetLayoutWorkSetup( disp );
}


//----------------------------------------------------------------------------
/**
 *  @brief  描画中か
 *  @return  UIViewのメンバのG2DUtil内で生成されたLytWkのうち１つでも描画中のものがあればtrue(LytWk１つずつ確認したい場合はGetLayoutWorkで取得して個別にLytWk::IsDrawingで確認して下さい)
 */
//----------------------------------------------------------------------------
bool UIView::IsDrawing(void) const
{
  bool isDrawing = false;
  if(m_g2dUtil)
  {
    isDrawing = m_g2dUtil->IsDrawing();
  }
  return isDrawing;
}


}  // ui
}  // app
