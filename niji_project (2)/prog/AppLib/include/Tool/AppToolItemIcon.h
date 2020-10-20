#ifndef NIJI_PROJECT_APPLIB_TOOL_ITEMICON_H_INCLUDED
#define NIJI_PROJECT_APPLIB_TOOL_ITEMICON_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    AppToolItemIcon.h
 * @author  fukushima_yuya
 * @date    2015.04.03
 * @brief   アイテムアイコン共通処理
 */
//==============================================================================

// niji
#include <AppLib/include/Tool/ItemIcon.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)
class UIView;
GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_BEGIN(util)
class G2DUtil;
class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

//------------------------------------------------------------------------------
// @brief   ポケモンアイコン共通処理クラス
//------------------------------------------------------------------------------
class ItemIconToolEx
{
  GFL_FORBID_COPY_AND_ASSIGN( ItemIconToolEx );

public:
  // 生成パラメータ
  struct ItemIconToolParam
  {
    app::ui::UIView* pUIView;
    app::util::Heap* pHeap;
    
    u32 layout_id;  //!< レイアウトID
    u32 res_id;     //!< リソースID(LytMultiResID取得で使用)

    u32 iconMax;    //!< アイコンの最大数
    bool sync;      //!< 同期フラグ(true = 同期読み込み、false = 非同期読み込み)

    bool allocBuffer;  //!< あらかじめメモリを確保しておく場合true

    ItemIconToolParam( void )
      : pUIView( NULL )
      , pHeap( NULL )
      , layout_id( 0 )
      , res_id( 0 )
      , iconMax( 1 )
      , sync( false )
      , allocBuffer( false )
    { ; }
  };

private:
  // アイコン生成用パラメータ
  struct CreateIconParam
  {
    u32   id;
    u16   itemNo;
    gfl2::lyt::LytPicture* pic;

    CreateIconParam( void )
      : id( 0xFFFFFFFF)
      , itemNo( ITEM_DUMMY_DATA )
      , pic( NULL )
    { ; }

    // リセット
    void Reset( void ) { id = 0xFFFFFFFF; itemNo = ITEM_DUMMY_DATA; pic = NULL; }
    // 値が代入されているかどうか
    bool IsInvalid( void ) const {
      if( id == 0xFFFFFFFF ) return true;
      if( itemNo == ITEM_DUMMY_DATA ) return true;
      if( pic == NULL ) return true;
      return false;
    }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  ItemIconToolEx( ItemIconToolParam param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~ItemIconToolEx( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新処理
   *
   * @note    今のところ非同期くらいでしか使用しないかな・・・
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   準備完了か？
   * @return  "true" == 完了
   * @return  "false" == 準備中
   *
   * @note    非同期の時のみ使用
   */
  //------------------------------------------------------------------
  bool IsReady( void );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   * @return  "true" == 終了
   * @return  "false" == 終了中
   */
  //------------------------------------------------------------------
  bool EndFunc( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替え
   * @param   id      アイコンID
   * @param   itemNo  アイテムNo
   * @param   pane    差し替え先のピクチャペイン
   *
   * @note    アイコンの読み込みと差し替えを行う。
   *          読み込みと差し替えを別々に行うには"LoadIcon""ReplaceIcon"を使用する
   */
  //------------------------------------------------------------------
  void SetItemIcon( u32 id, u16 itemNo, gfl2::lyt::LytPicture* pane );

  //------------------------------------------------------------------
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
  //------------------------------------------------------------------
  void SetItemIcon( u32 id, u16 itemNo, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts = NULL );


public:
  //------------------------------------------------------------------
  /**
   * @brief   アイコンの読み込み
   * @param   id      アイコンID
   * @param   itemNo  アイテムNo
   */
  //------------------------------------------------------------------
  void LoadItemIcon( u32 id, u16 itemNo );

  //------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替え
   * @param   id      アイコンID
   * @param   pane    差し替え先のピクチャペイン
   * @return  "true" == 差し替え完了
   * @return  "false" == それ以外
   */
  //------------------------------------------------------------------
  bool ReplaceIcon( u32 id, gfl2::lyt::LytPicture* pane );

  //------------------------------------------------------------------
  /**
   * @brief   アイコンの差し替え
   * @param   id          アイコンID
   * @param   paneIndex   差し替え先のピクチャペインインデックス
   * @param   parts       差し替え先の部品ペイン
   * @return  "true" == 差し替え完了
   * @return  "false" == それ以外
   */
  //------------------------------------------------------------------
  bool ReplaceIcon( u32 id, gfl2::lyt::LytPaneIndex paneIndex, gfl2::lyt::LytParts* parts = NULL );

  //------------------------------------------------------------------
  /**
   * @brief   アイコンの読み込み終了確認
   * @return  "true" == 完了
   * @return  "false" == 読み込み中
   */
  //------------------------------------------------------------------
  bool IsLoadItemIcon( u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   指定したアイコン張替えが終っているか
   * @return  "true" == 終っている
   * @return  "false" == 終っていない or 張替えをしていない
   */
  //------------------------------------------------------------------
  bool IsReplaced( const u32 id );

  //------------------------------------------------------------------
  /**
   * @brief   全てのアイコン張替えが終っているか
   * @return  "true" == 終っている
   * @return  "false" == 終っていない or 張替えをしていない
   */
  //------------------------------------------------------------------
  bool IsAllReplaced( void );

public:   //!< アニメーション関連
  //------------------------------------------------------------------
  /**
   * @brief	  アニメーションの再生
   * @param   anmID         アニメーションID
   * @param   auto_anim     
   * @param   auto_unbind   
   */
  //------------------------------------------------------------------
  void StartAnim( u32 anmID, bool auto_anim = true, bool auto_unbind = true );
  
  //------------------------------------------------------------------
  /**
   * @brief	  アニメーションの停止
   * @param   anmID   アニメーションID
   * @param   clear   
   */
  //------------------------------------------------------------------
  void StopAnim( u32 anmID, bool clear = false );
  
  //------------------------------------------------------------------
  /**
   * @brief	  アニメーションの終了チェック
   * @param   anmID   アニメーションID
   * @return  true == 終了
   */
  //------------------------------------------------------------------
  bool IsAnimEnd( u32 anmID );

private:
  app::tool::ItemIcon*        m_pItemIcon;    //!< アイテムアイコン

  app::util::Heap*            m_pHeap;        //!< ヒープ
  
  app::util::G2DUtil*         m_pG2D;         //!< 2Dユーティリティ

  gfl2::lyt::LytWk*           m_pLytwk;       //!< レイアウトワーク

  gfl2::lyt::LytMultiResID    m_ResID;        //!< リソースID

  u32     m_IconMax;    //!< アイコンの最大数

  u32     m_LytID;      //!< レイアウトID

  bool    m_bSync;      //!< 同期フラグ

  u32     m_EndSeq;     //!< 終了シーケンス

  u32     m_MainSeq;    //!< メインシーケンス

  u8*     m_pLoadIcon;  //!< アイコンの読み込み状態配列

  CreateIconParam*      m_pIconParam;   //!< アイコン生成パラメータ
};

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif  // #ifndef NIJI_PROJECT_APPLIB_TOOL_ITEMICON_H_INCLUDED
