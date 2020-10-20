//=============================================================================
/**
 * @file    app_tool_MenuCursor.h
 * @brief   汎用メニューカーソル
 * @author  Hiroyuki Nakamura
 * @date    12.05.29
 */
//=============================================================================
#if !defined( APP_TOOL_MENUCURSOR_H_INCLUDED )
#define APP_TOOL_MENUCURSOR_H_INCLUDED

#pragma once

#include <GameSys/include/GameProcManager.h>
#include <Layout/include/gfl2_Layout.h>
#include <Heap/include/gfl2_Heap.h>

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_AppLytAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//=============================================================================
/**
 * @class MenuCursor
 * @brief 汎用メニューカーソル
 * @date  12.05.29
 */
//=============================================================================
class MenuCursor
{
private:
  //! カーソルID
  enum CursorID
  {
    ID_LEFT = 0,    //!< 左
    ID_RIGHT,       //!< 右

    CURSOR_ID_MAX
  };


public:
  //! カーソルサイズ（大小）
  enum CursorSize
  {
    SIZE_SMALL = 0,   //!< 小
    SIZE_LARGE,       //!< 大
    SIZE_BTL,         //!< バトルUI用サイズ

    CURSOR_SIZE_MAX
  };

  static const u32 CURSOR_LEFT    = ( 1 << ID_LEFT );
  static const u32 CURSOR_RIGHT   = ( 1 << ID_RIGHT );
  static const u32 CURSOR_ALL     = ( CURSOR_LEFT | CURSOR_RIGHT );


private:
  static const f32 S_WIDTH_MIN;   //!< 小カーソルの最小幅
  static const f32 L_WIDTH_MIN;   //!< 大カーソルの最小幅

  //! サイズ別データ
  struct CURSOR_DATA
  {
    gfl2::lyt::LytArcIndex bflyt;
    gfl2::lyt::LytPaneIndex base;
    gfl2::lyt::LytPaneIndex cursor[CURSOR_ID_MAX];
    f32 min_width;
  };

  static const CURSOR_DATA CursorData[CURSOR_SIZE_MAX];   //!< カーソルデータ


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap  ヒープ
   * @param   buf   リソースの確保場所
   */
  //-----------------------------------------------------------------------------
  MenuCursor( app::util::Heap * heap, void ** buf );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MenuCursor();

  //-----------------------------------------------------------------------------
  /**
   * @brief   アップデート
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   描画
   *
   * @param   render_manager  描画マネージャ
   * @param   disp            描画先ディスプレイ
   */
  //-----------------------------------------------------------------------------
  void Draw( app::util::AppRenderingManager * render_manager, gfl2::gfx::CtrDisplayNo displayNo, gfl2::lyt::DisplayType disp, u8 order = gfl2::lyt::ORDER_NORMAL, u32 layerNo = 0 );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsModuleFree
   * @brief   クラスを破棄できるか
   * @date    2015.03.25
   *
   * @retval  true  = 可
   * @retval  false = 負荷
   */
  //-----------------------------------------------------------------------------
  bool IsModuleFree(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadResourceSync
   * @brief   リスース読み込み（同期版）
   * @date    2015.03.25
   */
  //-----------------------------------------------------------------------------
  void LoadResourceSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadResource
   * @brief   リスース読み込み（非同期版）
   * @date    2015.03.25
   *
   * @retval  true  = 読み込み中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool LoadResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteResource
   * @brief   リスース削除
   * @date    2015.03.25
   */
  //-----------------------------------------------------------------------------
  void DeleteResource(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateLayoutWork
   * @brief   レイアウトワーク作成
   * @date    2015.03.25
   *
   * @param   lytsys      レイアウトシステム
   * @param   setup       セットアップデータ
   * @param   accessor    applytファイルアクセサ
   * @param   size        カーソルサイズ ( 大小 )
   * @param   auto_draw   オート描画フラグ (true = 有効) ※デフォルト = false
   */
  //-----------------------------------------------------------------------------
  void CreateLayoutWork( gfl2::lyt::LytSys * lytsys, const gfl2::lyt::LytWkSetUp * setup, app::util::AppLytAccessor * accessor, CursorSize size, bool auto_draw=false );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteLayoutWork
   * @brief   レイアウトワーク削除
   * @date    2015.03.25
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool DeleteLayoutWork(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   レイアウトワーク取得
   *
   * @return  レイアウトワーク
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytWk * GetLayoutWork(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   アニメ開始
   */
  //-----------------------------------------------------------------------------
  void StartAnime(void);

  //-----------------------------------------------------------------------------
  /**
    * @brief    押すアニメが終了しているかどうか
    * @return   押すアニメが終わっている -> true / 押すアニメ再生中 -> false
    */
  //-----------------------------------------------------------------------------
  bool IsEndPushAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief   座標設定
   *
   * @param   pos   座標
   */
  //-----------------------------------------------------------------------------
  void SetPos( gfl2::math::VEC3 * pos );

  //-----------------------------------------------------------------------------
  /**
   * @brief   座標設定（ペイン指定）
   *
   * @param   pane  基準となるペイン
   */
  //-----------------------------------------------------------------------------
  void SetPos( gfl2::lyt::LytPane * pane );

  //-----------------------------------------------------------------------------
  /**
   * @brief   サイズ設定
   *
   * @param   w   幅
   */
  //-----------------------------------------------------------------------------
  void SetSize( f32 w );

  //-----------------------------------------------------------------------------
  /**
   * @brief   サイズ設定（ペイン指定）
   *
   * @param   pane  基準となるペイン
   */
  //-----------------------------------------------------------------------------
  void SetSize( gfl2::lyt::LytPane * pane );

  //-----------------------------------------------------------------------------
  /**
   * @brief   表示切り替え
   *
   * @param   flg   true = 表示, false = 非表示
   * @param   id    カーソルID　※デフォルト = 全体
   *
   * @li  id = CURSOR_LEFT  : 左カーソル
   * @li  id = CURSOR_RIGHT : 右カーソル
   * @li  id = CURSOR_ALL   : 全体
   */
  //-----------------------------------------------------------------------------
  void SetVisible( bool flg, u32 id = CURSOR_ALL );

  //-----------------------------------------------------------------------------
  /**
   * @brief   カーソルが表示されているか
   *
   * @param   id    カーソルID　※デフォルト = 全体
   *
   * @retval  true  = 表示されている
   * @retval  false = それ以外
   *
   * @li  id = CURSOR_LEFT  : 左カーソル
   * @li  id = CURSOR_RIGHT : 右カーソル
   * @li  id = CURSOR_ALL   : 全体
   */
  //-----------------------------------------------------------------------------
  bool IsVisible( u32 id = CURSOR_ALL );

  //-----------------------------------------------------------------------------
  /**
   * @brief   カーソル配置
   *
   * @param   lytwk   基準ペインのレイアウトワーク
   * @param   base    基準ペイン
   */
  //-----------------------------------------------------------------------------
  void Put( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * base );

  //-----------------------------------------------------------------------------
  /**
   * @brief   カーソル配置  (内部のvisibleがない版)
   *
   * @param   lytwk   基準ペインのレイアウトワーク
   * @param   base    基準ペイン
   * @note
   *    baseのペインの回転値をカーソルに設定している
   */
  //-----------------------------------------------------------------------------
  void PutNonVisible( gfl2::lyt::LytWk * lytwk, gfl2::lyt::LytPane * base );

  //-----------------------------------------------------------------------------
  /**
   * @brief   オート描画フラグ設定
   *
   * @param   flg   描画フラグ
   */
  //-----------------------------------------------------------------------------
  void SetAutoDrawFlag( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @brief   表示プライオリティ設定
   *
   * @param   priority  プライオリティ
   *
   * @li	オート描画時のみ有効
   */
  //-----------------------------------------------------------------------------
  void SetPriority( u32 priority );


private:
  //  再生するアニメid
  enum PlayAnimeId
  {
    PLAY_ANIME_ID_IDLE = 0,
  };

  //-----------------------------------------------------------------------------
  /**
   * @func    GetPane
   * @brief   カーソルペイン取得
   * @date    2015.03.25
   *
   * @return  カーソルのペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPane * GetPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    _StartAnime
   * @brief   アニメ再生
   * @date    2015.06.02
   *
   * @param   animeIdx    : 再生アニメidx指定
   * @parma   bAutoUnbind : アニメのバインドを自動で外すか
   */
  //-----------------------------------------------------------------------------
  void _StartAnime( const u32 animeIdx, const bool bAutoUnbind );

private:
  app::util::Heap * m_heap;           //!< ヒープ
  void ** m_pLytResBuff;              //!< リソース確保場所

  gfl2::lyt::LytSys * m_lytsys;       //!< レイアウトシステム
  gfl2::lyt::LytWk * m_pLytWk;        //!< レイアウトワーク
  gfl2::lyt::LytMultiResID m_resID;   //!< リソースID

  const CURSOR_DATA * m_cursorData;   //!< 参照するカーソルデータ

  u32 m_subSequence;      //!< サブシーケンス

  bool m_isCreate;        //!< リソース生成フラグ
  bool m_isModuleFree;    //!< モジュールを破棄できるか

  bool m_isDrawEnable;    //!< 描画するか
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // APP_TOOL_MENUCURSOR_H_INCLUDED
