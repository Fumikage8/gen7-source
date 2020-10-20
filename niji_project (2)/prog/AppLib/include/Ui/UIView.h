//============================================================================================
/**
 * @file    UIView.h
 * @brief    View関連基本クラス
 * @author  Hisanao Suzuki
 */
//============================================================================================
#ifndef __APP_UI_VIEW_H__
#define __APP_UI_VIEW_H__
#pragma  once

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_Str.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>

#include "AppLib/include/Ui/UIResponder.h"
#include "AppLib/include/Ui/UIInputListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
class LayoutRenderPath;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
class AppLytObject;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




namespace app {

namespace ui {

/************************************************************************************************
*
*   @brief UIViewリスト用ノードクラス
*
************************************************************************************************/
class UIView;
struct UIViewNode
{
  UIView*              m_view;
  nw::ut::LinkListNode node;                 // Iteratorで使用されるノード(ユーザーの使用禁止）
};



/************************************************************************************************
*
*   @brief ウインドウ根底クラス
*
*         親子関係はポインタ参照のみを管理するため、
*         親のインスタンスを解放したとしても、子のインスタンスは解放されない。
*         RemoveFromSuperView()、RemoveAllSubView()を用いて、親子関係を解除すること。
*
*
************************************************************************************************/
class UIView : public UIResponder
{
public:
  typedef nw::ut::LinkList<UIViewNode, offsetof(UIViewNode,node)> UIViewList;

  static const u32 UNUSE_MESSAGE_DATA      = 0xFFFFFFFF;  // メッセージデータを使用しない場合のID
  static const u32 USE_APPLYT_MESSAGE_DATA = 0xFFFFFFFE;  // applytバイナリの情報からメッセージデータを作成する場合のID

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief    コンストラクタ
  *
  * @param   heap               ウインドウの親子関係管理や、ボタンマネージャ用のメモリとして利用される。
  * @param   pLayoutRenderPath  レイアウト描画パスを指定必ず指定する
  * @param   pLytAlloc          レイアウトワーク生成時に使用されるアロケータ。NULLを指定するとデフォルトアロケータを使用する。
  * @param   pLytDevAlloc       レイアウトワーク生成時に使用されるデバイスアロケータ。NULLを指定するとデフォルトアロケータを使用する。
  */
  //--------------------------------------------------------------------------------------------
  UIView( app::util::Heap * heap, gfl2::heap::NwAllocator *pLytAlloc=NULL, gfl2::heap::NwAllocator* pLytDevAlloc=NULL );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    デストラクタ
  */
  //--------------------------------------------------------------------------------------------
  virtual ~UIView(void);

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  親ウインドウの取得
  *
  * @return 親ウインドウが存在するならそのポインタ、存在しないならNULLを返却
  */
  //--------------------------------------------------------------------------------------------
  UIView* GetSuperView(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  子ウインドウ数を取得
  *
  * @return 子ウインドウの数
  */
  //--------------------------------------------------------------------------------------------
  u32 GetSubViewCount(void) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  指定UIViewが子ウインドウとして登録されているか調査
  *
  * @param  view 検索対象となるUIViewポインタ
  *
  * @return 指定ウインドウが子ウインドウとして登録されているならtrueを返却する
  */
  //--------------------------------------------------------------------------------------------
  bool IsExistSubView( UIView *view );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  子ウインドウを登録する。
  *          ※サブウインドウクラスは、必ずUpdate(), Draw()関数をvirtualで実装してください。
  *
  * @param  view 子ウインドウへのポインタ。
  *              親ウインドウへのポインタを引き渡した場合は無視する。
  */
  //--------------------------------------------------------------------------------------------
  void AddSubView( UIView* view );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  自身と親ウインドウの親子関係を解除する
  *         親ウインドウが存在しない場合は何も行わない
  */
  //--------------------------------------------------------------------------------------------
  void RemoveFromSuperView( void );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  全ての子ウインドウの登録を解除する
  *         子ウインドウが存在しない場合は何も行わない
  */
  //--------------------------------------------------------------------------------------------
  void RemoveAllSubView( void );

private:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  子ウィンドウが登録された時に呼び出されるハンドラです
  *         子が追加されたタイミングで何か処理したい場合はオーバーライドしてください
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnAddChild( void ){}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  親ウィンドウに登録された時に呼び出されるハンドラです
  *         親に追加されたタイミングで何か処理したい場合はオーバーライドしてください
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnAddedToParent( void ){}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  子ウィンドウが解除された時に呼び出されるハンドラです
  *         子が解除されたタイミングで何か処理したい場合はオーバーライドしてください
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnRemoveChild( void ){}

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  親ウィンドウから解除された時に呼び出されるハンドラです
  *         親から解除されたタイミングで何か処理したい場合はオーバーライドしてください
  */
  //--------------------------------------------------------------------------------------------
  virtual void OnRemovedFromParent( void ){}

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  モーダルウインドウ判定
  *
  * @return モーダルウインドウであるならtrueを、モーダレスウインドウであればfalseを返却
  */
  //--------------------------------------------------------------------------------------------
  bool IsModal(void) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  自身をモーダルウインドウとして設定する
  *         モーダルウインドウと設定した場合、自身以降のウインドウのキー入力を禁止する
    @param  OFF出来るように引数追加
  */
  //--------------------------------------------------------------------------------------------
  void SetModal(bool bmode=true);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    2D関連削除
  */
  //--------------------------------------------------------------------------------------------
  void Delete2D(void);

public:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  定期更新処理
  *         親ウインドウは本関数にて更新処理を実行することで、「子→親」の順に
  *         Update()を実行する。
  */
  //--------------------------------------------------------------------------------------------
  void UpdateTree( void );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  ウインドウリストの表示。
  *         親ウインドウは本関数にて描画を実行することで、「親→子」の順に
  *         Draw()を実行する。
  */
  //--------------------------------------------------------------------------------------------
  void DrawTree( gfl2::gfx::CtrDisplayNo displayNo );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ) = 0;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   レイアウトワーク取得
  *
  * @param   id レイアウトID
  */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytWk* GetLayoutWork( u32 layoutId ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   レイアウトシステム取得
  *
  * @return  レイアウトシステム
  */
  //--------------------------------------------------------------------------------------------
  gfl2::lyt::LytSys* GetLayoutSystem(void) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   グラフィックシステム取得
  *
  * @return  グラフィックシステム
  */
  //--------------------------------------------------------------------------------------------
/* ビルドを通すため
  gfl2::grp::GraphicsSystem* GetGraphicSystem(void) const;
*/
  gfl2::lyt::LayoutRenderPath* UIView::GetLaoutRenderPath(void) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   G2DUtil取得
  *
  * @return  グラフィックシステム
  */
  //--------------------------------------------------------------------------------------------
  app::util::G2DUtil* GetG2DUtil( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   テクスチャシステム作成
  *
  * @param   heap       ヒープ
  * @param   devHeap    デバイスヒープ
  * @parma   file_read_manager ファイル読み込み制御クラス
  * @param   arcId      ARCファイルID
  * @param   texNum     読み込みテクスチャー枚数
  * @param   bufferSize テクスチャシステム用バッファサイズ
  */
  //--------------------------------------------------------------------------------------------
  void CreateTextureSystem( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, gfl2::fs::AsyncFileManager *file_read_manager, u32 arcId, u32 texNum, u32 bufferSize );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   テクスチャ単体ロード
  *
  * @param   heap       ヒープ
  * @param   devHeap    デバイスヒープ
  * @parma   file_read_manager ファイル読み込み制御クラス
  * @param   arcId      ARCファイルID
  * @param   arcDataId  ARCファイルのデータID
  * @param   texId      設定するデータID
  * @param   compressed ロードするデータが圧縮しているかどうか
  */
  //--------------------------------------------------------------------------------------------
  void UIView::LoadTexture( gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, gfl2::fs::AsyncFileManager *file_read_manager, u32 arcId, u32 arcDataId, u32 texId, bool compressed );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    外部で準備されたテクスチャシステムを再利用する
  *
  * @param   textureSystem 再利用するテクスチャーシステム
  */
  //--------------------------------------------------------------------------------------------
  void ReUseTextureSystem( app::tool::LytTexReplaceSystem* textureSystem );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief テキストボックスペインにタグプロセッサを登録する
  * @param lytID      対象レイアウトID
  * @param paneId     対象ペインのインデックス
  * @param tagProc    タグプロセッサへのポインタ
  */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneTagProcessor( u32 lytID, u32 paneId, nw::lyt::TextBox::TagProcessor* tagProc );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインの矩形を取得する
  * @param layout_work   ペインが所属するレイアウトワーク
  * @param pane_index    対象ペインのインデックス
  * @return ペインの矩形サイズ
  */
  //--------------------------------------------------------------------------------------------
  const nw::ut::Rect GetPaneRect( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   ペイン座標の取得
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 取得対象ペインＩＤ
  *
  * @return  ペイン座標値
  */
  //--------------------------------------------------------------------------------------------
  gfl2::math::VEC3 GetPanePos( u32 layoutId, u32 paneId ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   ペイン座標の設定
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 設定対象ペインＩＤ
  * @param   pos      設定座標
  */
  //--------------------------------------------------------------------------------------------
  void SetPanePos( u32 layoutId, u32 paneId, gfl2::math::VEC3* pos );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   ペインのスケール設定
  *
  * @param   layoutId ペインが属するレイアウトＩＤ
  * @param   layoutId 設定対象ペインＩＤ
  * @param   scale    スケール値
  */
  //--------------------------------------------------------------------------------------------
  void SetPaneScale( u32 layoutId, u32 paneId, gfl2::math::VEC2* scale );

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
  void GetPaneScale( u32 layoutId, u32 paneId, gfl2::math::VEC2 * scale ) const;

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
  nw::lyt::Size GetPaneSize( u32 lytID, gfl2::lyt::LytPaneIndex index ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   主人公アイコンの初期位置をペインを使用して設定する
  *
  * @param   paneId 初期位置に使用するペインＩＤ
  */
  //--------------------------------------------------------------------------------------------
  void SetHeroPositionWithPane( u32 paneId );

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
  gfl2::math::VEC3 GetPaneScreenPos( u32 layoutId, u32 paneId );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインに文字列をセットする(wchar_t版)
  * @param lytID     対象レイアウトID
  * @param paneId    対象ペインのインデックス
  * @param string    セットする文字列
  */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneString( u32 lytID, u32 paneId, const wchar_t* string );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインに文字列をセットする (STRBUF版）
  * @param lytID     対象レイアウトID
  * @param paneId    対象ペインのインデックス
  * @param string    セットする文字列
  */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneString( u32 lytID, u32 paneId, gfl2::str::StrBuf *strbuf );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインにメッセージをセットする
  * @param lytID     対象レイアウトID
  * @param paneId    対象ペインのインデックス
  * @param messageId セットするメッセージ
  */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneMessage( u32 lytID, u32 paneId, s32 messageId );

  //--------------------------------------------------------------------------------------------
   /**
   * @brief ペインにメッセージをセットする(ペインポインタ版)
   * @param paneAddr   テキストペインアドレス
   * @param messageId  セットするメッセージ
   * @note
   *    messageIdを使わず文字列を直接設定する場合は下記の関数を使用するのがいい
   *      print::SetStringForLytTextBox( pTextBoxPane, L"" );
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneMessage( gfl2::lyt::LytTextBox* pTextBoxPane, s32 messageId );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ペインにメッセージをセットする(ペインポインタ/文字列バッファ版)
   * @param paneAddr   テキストペインアドレス
   * @param buf        文字列バッファ
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneMessage( gfl2::lyt::LytTextBox * pTextBoxPane, const gfl2::str::StrBuf * buf );
  
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
  void SetTextboxPaneNumber(
    u32 lytID, u32 paneId, u32 messageId,
    u32 value, u32 keta, u32 bufID = 0,
    print::NumberDispType dispType=print::NUM_DISP_LEFT,
    print::NumberCodeType codeType=print::NUM_CODE_DEFAULT );

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
  void SetTextboxPaneNumber(
    gfl2::lyt::LytTextBox* pTextBox,
    u32 messageId, u32 value, u32 keta, u32 bufID = 0,
    print::NumberDispType dispType=print::NUM_DISP_LEFT,
    print::NumberCodeType codeType=print::NUM_CODE_DEFAULT );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   テキストボックスペインに文字列を描画(ペインポインタ版)
   *
   * @param   pTextBox    テキストボックスペイン
   * @param   str         文字列
   */
  //--------------------------------------------------------------------------------------------
  void SetTextboxPaneString( gfl2::lyt::LytTextBox* pTextBox, gfl2::str::StrBuf* pStrBuf );

  //@brief    タッチされているペインを取得(ペインポインタ指定)
  bool GetToucHoldPane( const gfl2::ui::TouchPanel* tp, u32 lytID, gfl2::lyt::LytPane* pPane ) const;
  //@brief    ペインのヒット判定
  bool GetHitPane( u32 lytID, gfl2::lyt::LytPane* pPane, u16 x, u16 y, bool visible = true ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief テクスチャーシステムの取得
  *
  * @return テクスチャーシステムへのポインタ
  */
  //--------------------------------------------------------------------------------------------
  app::tool::LytTexReplaceSystem*  GetTextureSystem( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief LytWkセットアップ構造体の取得
   *
   * @param disp 取得対象のディスプレイタイプ
   *
   * @return LytWkセットアップ構造体へのポインタ
   */
  //--------------------------------------------------------------------------------------------
  const gfl2::lyt::LytWkSetUp * GetLytSetup( gfl2::lyt::DisplayType disp );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   ペインのテクスチャ差し替え
  *
  * @param   lytID  レイアウトID
  * @param   pane   ペインID
  * @param   texID  テクスチャID
  */
  //--------------------------------------------------------------------------------------------
  void ReplacePaneTexture( u32 lytID, u32 pane, u32 texID );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   可視状態の設定
  *          不可視状態時は描画、操作通知ともに無効となる。
  *
  * @param   isVisible  可視状態に設定するならtrueを指定
  */
  //--------------------------------------------------------------------------------------------
  virtual void SetVisible( bool isVisible );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   可視状態の取得
  *
  * @return  可視状態ならtrueを返却
  */
  //--------------------------------------------------------------------------------------------
  virtual bool IsVisible( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   レイアウトワーク更新有無フラグ
  *
  * @param   isLayoutUpdating  更新するならtrueを指定
  */
  //--------------------------------------------------------------------------------------------
  void SetLayoutUpdating( bool isLayoutUpdating );

   //--------------------------------------------------------------------------------------------
  /**
   * @brief 自分と子ビューすべての入力設定
   *
   * @param isEnable
   */
   //--------------------------------------------------------------------------------------------
  void SetInputEnabledAllView(  bool isEnable );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief		ワードセットローダークラスの設定
   * @param		WordSetLoader
   */
  //--------------------------------------------------------------------------------------------
  void SetWordSetLoader(WordSetLoader* ploader){  m_pWordSetLoader = ploader; };

  //----------------------------------------------------------------------------
  /**
   *  @brief  描画中か
   *  @return  UIViewのメンバのG2DUtil内で生成されたLytWkのうち１つでも描画中のものがあればtrue(LytWk１つずつ確認したい場合はGetLayoutWorkで取得して個別にLytWk::IsDrawingで確認して下さい)
   */
  //----------------------------------------------------------------------------
  virtual bool IsDrawing(void) const;


protected:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief    グラフィックシステム作成
  *
  * @param    heap                    ヒープ
  * @param    isInitApplicationMemory GraphicsSystem::InitializeApplicationMemory()の実行有無。
  *                                   trueであればapp::util::Heap::pDevHeapAllocatorが指定される。
  */
  //--------------------------------------------------------------------------------------------
  void CreateGraphicsSystem( app::util::Heap * heap, bool isInitApplicationMemory=true );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    グラフィックシステム削除
  */
  //--------------------------------------------------------------------------------------------
  void DeleteGraphicsSystem(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   テクスチャシステム削除
  */
  //--------------------------------------------------------------------------------------------
  void DeleteTextureSystem(void);

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
  *          message               メッセージデータID
  *          createPaneTbl         ペインテーブルを作り検索時間を短縮するかtrue or false
  *          createPaneTblWithHash ペインテーブル生成時にハッシュテーブルを作り、生成時間を短縮するかtrue or false
  */
  //--------------------------------------------------------------------------------------------
  void Create2D( app::util::Heap * heap,
                 gfl2::lyt::LytSys *layout,  u32 layoutNum,
                 const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
                 const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
                 u32 message,
                 b32 createPaneTbl = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_USE ,
                 b32 createPaneTblWithHash = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_HASH_USE,
                 s32 wordsetBufferNum = print::WordSet::DEFAULT_WORD_MAX );

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
  void Create2D( app::util::Heap * heap,
                 gfl2::lyt::LytSys *layout,  u32 layoutNum,
                 const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
                 const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
                 gfl2::str::MsgData *messageData, print::WordSet *wordset,
                 b32 createPaneTbl = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_USE ,
                 b32 createPaneTblWithHash = app::util::G2DUtil::DEFAULT_CREATE_PANETBL_HASH_USE,
                 s32 wordsetBufferNum = print::WordSet::DEFAULT_WORD_MAX );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   2D関連作成
  *
  * @note    LayoutWorkとメッセージデータを、引数で指定した外部データから流用する。
  *
  * @param   heap        ヒープ
  *          layout      外部レイアウトシステム
  *          layoutNum   外部レイアウト数
  *          layoutWork  外部レイアウトワーク
  *          layoutNum   外部レイアウトデータ数
  *          layoutResID 外部レイアウトリソースID
  *          messageData 外部メッセージデータインスタンス
  *          messageData 外部ワードセットインスタンス
  */
  //--------------------------------------------------------------------------------------------
  void Create2D( app::util::Heap* heap,
                 gfl2::lyt::LytSys* layout, const u32 layoutNum,
                 app::util::G2DUtil::LAYOUT_WORK *lytWork, const u32 lytWorkMax,
                 gfl2::lyt::LytMultiResID* layoutResID,
                 gfl2::str::MsgData *messageData, print::WordSet *wordset );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   指定ＩＤのメッセージを格納したバッファへのポインタを取得
  *
  * @param   messageId 取得したいメッセージＩＤ
  *
  * @return  メッセージバッファへのポインタ
  */
  //--------------------------------------------------------------------------------------------
  gfl2::str::StrBuf* GetMessageStrBuf( u32 messageId );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   上画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   alpha        表示する矩形のアルファ値
  */
  //--------------------------------------------------------------------------------------------
  void DrawUpperMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer, float alpha );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   下画面マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   alpha        表示する矩形のアルファ値
  */
  //--------------------------------------------------------------------------------------------
  void DrawLowerMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer, float alpha );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   マスク矩形の描画
  *
  * @param   graphics     対象GraphicsSystem
  * @param   targetBuffer 対象ディスプレイ
  * @param   x            表示する矩形のＸ座標
  * @param   y            表示する矩形のＹ座標
  * @param   width        表示する矩形の横幅
  * @param   height       表示する矩形の縦幅
  * @param   alpha        表示する矩形のアルファ値
  */
  //--------------------------------------------------------------------------------------------
  void DrawMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer,
                             u32 x, u32 y, u32 width, u32 height, float alpha );

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
  void DrawMaskRect( gfl2::lyt::LayoutRenderPath *graphics, gfl2::lyt::DisplayType targetBuffer,
                             u32 x, u32 y, u32 width, u32 height,
                             float red, float green, float blue, float alpha );
  

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  G2DUtil定期更新処理
  */
  //--------------------------------------------------------------------------------------------
  void UpdateG2DUtil(void);
  

  //--------------------------------------------------------------------------------------------
  /**
   * @brief レイアウトの転送エリア指定
   *
   * @param area  MemoryArea  （デフォルトはFCRAM）
   * 
   * @note  create2Dの前に呼んでください
   */
  //--------------------------------------------------------------------------------------------
  void SetLayoutTransferArea( gfl2::lyt::MemoryArea area );

protected:
   //--------------------------------------------------------------------------------------------
  /**
   * @brief レイアウトシステム生成後、レイアウトリソース読み込みの前に呼ばれる
   *      すでにレイアウトシステムは生成されています。
   * 
   *    用途としては、フォントの読み込みをするなどを想定しています
   */
   //--------------------------------------------------------------------------------------------
  virtual void OnPreLoadResource( void ){}

private:
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
  virtual void Initalize2D( app::util::Heap* heap,
                            gfl2::lyt::LytSys* layout, u32 layoutNum,
                            const app::util::G2DUtil::LytResData * lytResTbl, u32 lytRestblMax,
                            const app::util::G2DUtil::LytwkData *layoutData, u32 layoutDataNum,
                            b32 createPaneTbl, b32 createPaneTblWithHash );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief   外部リソースを流用した2D関連の初期化
  *
  * @param   heap       ヒープ
  *          layout     外部レイアウトシステム
  *          layoutNum  外部レイアウト数
  *          layoutWork 外部レイアウトワーク
  *          layoutNum  外部レイアウトデータ数
  *          layoutNum  外部レイアウトデータ数
  *          layoutResID 外部レイアウトリソースID
  */
  //--------------------------------------------------------------------------------------------
  void Initalize2D( app::util::Heap* heap,
                    gfl2::lyt::LytSys* layout, const u32 layoutNum,
                    app::util::G2DUtil::LAYOUT_WORK *lytWork, const u32 lytWorkMax,
                    gfl2::lyt::LytMultiResID* layoutResID );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  入力操作制御
  *
  * @return 以降の入力操作を継続するならtrueを返却
  */
  //--------------------------------------------------------------------------------------------
  virtual bool DoInputAction( void );


private:
  UIViewNode                       m_viewNode;                //!< 自身のノード
  UIView*                          m_parentView;              //!< 親View
  UIViewList                       m_childViews;              //!< 子View
  //

protected:
  gfl2::heap::NwAllocator*          m_lytAllocator;            //!< レイアウト用アロケータ
  gfl2::heap::NwAllocator*          m_lytDevAllocator;         //!< レイアウト用デバイスアロケータ


  WordSetLoader* m_pWordSetLoader;
  //
//  gfl2::grp::GraphicsSystem*        m_graphicSystem;           //!< グラフィックシステム
  gfl2::lyt::LayoutRenderPath*     m_pLytRenderPath;
  app::util::G2DUtil*              m_g2dUtil;                 //!< 2D関連
  app::tool::LytTexReplaceSystem*  m_textureSystem;           //!< テクスチャー置き換え
  bool                             m_bTexSysCreateSelf;       //!< テクスチャーシステム生成フラグ
  bool                             m_bLytSysCreateSelf;       //!< レイアウトデータ生成フラグ
  bool                             m_bLytWrkCreateSelf;       //!< レイアウトワーク生成フラグ
  bool                             m_bMessageCreateSelf;      //!< メッセージデータ生成フラグ
  bool                             m_isVisible;               //!< 描画設定
  bool                             m_isModal;                 //!< モーダルウインドウフラグ
  bool                             m_isInputEnable;           //!< 親子関係による入力排他用フラグ
  bool                             m_isLayoutUpdating;        //!< レイアウト更新フラグ
  bool                             m_isInitApplicationMemory; //!< アプリケーションメモリ初期化関数実行フラグ
  gfl2::lyt::MemoryArea            m_layout_transfe_area;


  app::util::AppLytObject*         m_appLytObjArray;          //!< AppLytからつくるオブジェクト郡
  print::WordSet*                  m_wordSetForAppLytObj;     //!< m_appLytObjArrayを使うときに使うワードセット
};



}  // ui
}  // app



#endif // __APP_UI_VIEW_H__


