//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_ScrollPane.h
 *	@brief  スクロールペイン
 *	@author	Toru=Nagihashi
 *	@date		2012.10.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __APP_TOOL_SCROLLPANE_H__)
#define __APP_TOOL_SCROLLPANE_H__

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <Layout/include/gfl2_Layout.h>
#include <math/include/gfl2_Vector2.h>
#include <math/include/gfl2_Rect.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

typedef gfl2::math::Rect  ScrollPaneDrawArea;
typedef gfl2::math::Vector2 ScrollPanePosition;

class ScrollPane
{
  GFL_FORBID_COPY_AND_ASSIGN(ScrollPane);
  
public:
  static const s32 MOVE_SPEED = 8;         //!< 移動量


  enum LoadState
  {
    LOAD_STATE_NONE,
    LOAD_STATE_LOADING,
    LOAD_STATE_FINISH,
  };

public:
  //=============================================================================
  // 生成・破棄
  //=============================================================================
  /**
   * @brief    コンストラクタ
   */
  ScrollPane( void );
  /**
   * @brief    デストラクタ
   */
  virtual ~ScrollPane(void);

  /**
   * @brief 作成
   *
   * @param dataID  データID（0〜MAXまでのスクロールペインがあったときのインデックス）
   * @param width   幅
   * @param height  高さ
   */
  void Create( u32 dataID, s16 width, s16 height );

  //=============================================================================
  // 更新
  //=============================================================================
  /**
   * @brief    アップデート処理
   *
   * @return 生存中ならtrue、削除対象ならfalseを返却
   */
  bool Update(void);
  
  //=============================================================================
  // 操作・取得
  //=============================================================================
  /**
   * @brief  ユーザーの削除アニメーションの開始
   */
  void StartDeletingAnime( void );

  /**
   * @brief  割り当てられているペインIDを取得する
   *
   * @return 割り当てられているペインID
   */
  u32 GetUsedPaneID( void ) const;

  /**
   * @brief ペインIDを使用する
   *
   * @param 使用するペインID
   */
  void UsePaneID( u32 paneID );

  /**
   * @brief 使用しているペインIDをクリアする
   */
  void FreePane( void );

  /**
   * @brief ペインIDの使用状況を確認
   */
  bool IsUsedPane( void ) const;

  /**
   * @brief   現在のアルファ値を取得
   *
   * @return  アルファ値
   */
  u8 GetAlpha( void );

  /**
   * @brief 現在座標の設定
   *
   * @param pos 設定値となる座標
   */
  void SetPosition( const ScrollPanePosition& pos );

  /**
   * @brief   有効フラグの取得
   *
   * @return  有効ならtrueを返却
   */
  bool IsEnable( void ) const;

  /**
   * @brief   生存判定
   *
   * @return  生存状態ならtrueを返却
   */
  bool IsAlive( void ) const;

  /**
   * @brief   画面内にペインが存在するか判定
   *
   * @param   scrollX Ｘスクロール量
   * @param   scrollY Ｙスクロール量
   * @param   are 表示エリア
   *
   * @return  画面内にペインが存在するならtrueを返却
   */
  bool IsDisplayIn( s16 scrollX, s16 scrollY, const ScrollPaneDrawArea & area ) const;

  /**
   * @brief   データＩＤの取得
   * @return  割り当てられているデータＩＤ
   */
  u32 GetDataID( void ) const;

  /**
   * @brief   現在の座標位置を取得
   *
   * @return  現在座標位置
   */
  const ScrollPanePosition& GetPosition( void ) const;
  
  /**
   * @brief 幅を取得
   *
   * @return  幅
   */
  u16 GetWidth( void ) const;
  
  /**
   * @brief 高さを取得
   *
   * @return  高さ
   */
  u16 GetHeight( void ) const;

  /**
   * @brief   ローディング状態に設定する
   */
  void StartLoading( void );

  /**
   * @brief   ローディング状態を完了に設定する
   */
  void FinishLoading( void );

  /**
   * @brief   ローディング状態を取得
   *
   * @return  ローディング中か判断
   */
  ScrollPane::LoadState IsLoading( void ) const;

  /**
   * @brief 入力検知
   *
   * @param is_enable true入力可 false入力できない
   */
  void SetInputEnable( bool is_enable );
  
  /**
   * @brief 入力検知
   *
   * @return true入力可 false入力できない
   */
  bool GetInputEnable( void ) const;
private:
  void Reset( void );
  bool EraseAction( void );

private:
  static const u8   SUB_ALPHA  = 0x20; //!< アルファ値減算量

private:
  u32                             m_dataID;      // データID
  ScrollPanePosition   m_nowPos;          // 現在位置
  u16          m_width;           // サイズ
  u16          m_height;          // 高さ
  u16          m_paneId;          // 使用ペインID
  u8           m_alpha;           // アルファ値
  bool         m_isEnable;        // 有効フラグ
  bool         m_isUseCharaPane;  // キャラクター用ペイン使用フラグ
  bool         m_isDelete;        // 削除フラグ
  bool         m_isInputEnable;   // 入力有効フラグ
  LoadState    m_loading;       // ローディング状態

public:
  nw::ut::LinkListNode node;      // List Node
};
  


GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
#endif		// __APP_TOOL_SCROLLPANE_H__
