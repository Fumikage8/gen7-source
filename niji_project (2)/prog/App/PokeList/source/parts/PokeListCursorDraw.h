#ifndef NIJI_PROJECT_APP_POKELIST_CURSOR_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_CURSOR_DRAW_H_INCLUDED
//==============================================================================
/**
 * @file    PokeListCursorDraw.h
 * @author  fukushima_yuya
 * @date    2015.03.12
 * @brief   ポケモンリスト画面カーソル描画クラス
 */
//==============================================================================


// gflib2
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/Tool/app_tool_MenuCursor.h>

// pml
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;
class PlateDraw;
class StaticWork;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32    sc_INPUT_DIR_MAX = 4;


//------------------------------------------------------------------------------
// @brief   カーソル
// @note    カーソル及び入れ替え時に表示する選択カーソルを管理
//------------------------------------------------------------------------------
class CursorDraw
{
  GFL_FORBID_COPY_AND_ASSIGN( CursorDraw );

private:
  enum {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
    DIR_NUM,
    DIR_NULL,
  };

public:
  struct MoveData
  {
    CursorPos pos[DIR_NUM];
  };
  struct CursorData
  {
    CursorPos cpos;
    bool      enable;
    MoveData  moveData;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   pBase               DrawerBaseクラスポインタ
   * @param   pRenderingManager   レンダリングマネージャ
   * @param   member_count        メンバー数
   */
  //------------------------------------------------------------------
  CursorDraw( DrawerBase* pBase,
    app::util::AppRenderingManager* pRenderingManager,
    u8 member_count = pml::PokeParty::MAX_MEMBERS );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~CursorDraw( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Init( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   * @return  true == 完了
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了可能か？
   * @return  true == 終了、false == 否
   */
  //------------------------------------------------------------------
  bool IsEnd( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   *
   * @return  "true  == 終了"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool EndFunc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの開始
   */
  //------------------------------------------------------------------
  void StartMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルの終了
   */
  //------------------------------------------------------------------
  void EndMenuCursor( void );

  //------------------------------------------------------------------
  /**
   * @brief   メニューカーソルのバッファの取得
   */
  //------------------------------------------------------------------
  void* GetMenuCursorBuffer( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルタイプの設定
   * @param   cid   カーソルタイプ
   * @param   cpos  カーソルの初期位置指定
   */
  //------------------------------------------------------------------
  void SetCursorID( CursorID cid, CursorPos cpos = CPOS_NULL );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の設定
   * @param   cpos  カーソルの位置
   */
  //------------------------------------------------------------------
  void SetCursorPos( CursorPos cpos );

  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の取得
   *
   * @param   cid   カーソルID
   *
   * @return  カーソルの位置
   */
  //------------------------------------------------------------------
  CursorPos GetCursorPos( CursorID cid = CID_NUM );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの表示切替
   * @param   visible   表示フラグ
   */
  //------------------------------------------------------------------
  void SetVisible( bool visible );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カーソル位置の取得：StaticWorkから取得
   *
   * @param   cid   カーソルID
   *
   * @return  カーソルの位置
   */
  //------------------------------------------------------------------
  CursorPos _GetCursorPos( CursorID cid = CID_NUM );  // @fix GFNMCat[1462]


public:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの移動(デフォルト)
   */
  //------------------------------------------------------------------
  void SetCursorMoveDefault( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルの移動(決定ボタンあり)
   */
  //------------------------------------------------------------------
  void SetCursorMoveDecide( void );

  //------------------------------------------------------------------
  /**
   * @brief   カーソルデータの入力設定を変更
   *
   * @param   index   インデックス
   * @param   enable  有効フラグ
   */
  //------------------------------------------------------------------
  void SetCursorEnable( u32 index, bool enable );


private:
  //------------------------------------------------------------------
  /**
   * @brief   カーソルの配置
   */
  //------------------------------------------------------------------
  void PutCursor( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの入力処理
   * @param   pButton   ボタン
   */
  //------------------------------------------------------------------
  void InputButton( gfl2::ui::Button* pButton );

private:
  //------------------------------------------------------------------
  /**
   * @brief   入力されたボタンの方向を返す
   * @param   pButton   ボタン
   * @return  入力されたボタンの方向(上下左右)
   */
  //------------------------------------------------------------------
  u8 GetInputDir( gfl2::ui::Button* pButton );


private:
  enum CursorMoveType {
    CUR_MOVE_DEFAULT,   //!< 通常
    CUR_MOVE_DECIDE,    //!< ６プレート＋決定ボタン
    CUR_MOVE_NUM
  };

private:
  DrawerBase*                       m_pBase;

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  app::tool::MenuCursor*            m_pMenuCursor;

  void*                             m_pMenuCursorBuffer;

private:
  u8  m_Seq;

  u8  m_DeleteSeq;

  u8  m_MemberCount;

private:
  CursorData*       m_pCursorData;

  u32               m_CursorDataNum;

private:
  gfl2::lyt::LytPane*   m_pCursorPane[CPOS_NUM];
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_POKELIST_CURSOR_DRAW_H_INCLUDED
