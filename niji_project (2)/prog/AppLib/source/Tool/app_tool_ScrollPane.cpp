//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_ScrollPane.cpp
 *	@brief  スクロールペイン
 *	@author	Toru=Nagihashi
 *	@date		2012.10.28
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "AppLib/include/Tool/app_tool_ScrollPane.h"

#include <stdio.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

//=============================================================================
// 生成・破棄
//=============================================================================
/**
 * @brief		コンストラクタ
 */
ScrollPane::ScrollPane( void )
{
  this->Reset();
}


/**
 * @brief		デストラクタ
 */
ScrollPane::~ScrollPane(void)
{
}


/**
 * @brief 作成
 *
 * @param dataID  データID（0〜MAXまでのスクロールペインがあったときのインデックス）
 * @param width   幅
 * @param height  高さ
 */
void ScrollPane::Create( u32 dataID, s16 width, s16 height )
{
  this->Reset();
  m_dataID     = dataID;
  m_isEnable   = true;
  m_width   = width; //cov_ctr10266
  m_height  = height;
  m_isInputEnable = true;
}

//=============================================================================
// 更新
//=============================================================================
/**
 * @brief		アップデート処理
 *
 * @return 生存中ならtrue、削除対象ならfalseを返却
 */
bool ScrollPane::Update(void)
{
  if( !IsEnable() ){
    return false;
  }

  if( m_isDelete )
  {
    if( EraseAction() )
    {
      m_isDelete = false;
      m_isEnable = false;
      return false;
    }
  }
  return true;
}

/**
 * @brief  ユーザーの削除アニメーションの開始
 */
void ScrollPane::StartDeletingAnime( void )
{
  m_isDelete = true;
}


/**
 * @brief   有効フラグの取得
 *
 * @return  有効ならtrueを返却
 */
bool ScrollPane::IsEnable( void ) const
{
  return m_isEnable;
}

/**
 * @brief   生存判定
 *
 * @return  生存状態ならtrueを返却
 */
bool ScrollPane::IsAlive( void ) const
{
  return m_isEnable && !m_isDelete;
}

/**
 * @brief   データＩＤの取得
 * @return  割り当てられているデータＩＤ
 */
u32 ScrollPane::GetDataID( void ) const
{
  return m_dataID;
}

/**
 * @brief  割り当てられているペインIDを取得する
 *
 * @return 割り当てられているペインID
 */
u32 ScrollPane::GetUsedPaneID( void ) const
{
  GFL_ASSERT( IsUsedPane() ); //@check
  return m_paneId;
}


/**
 * @brief ペインIDを確保
 */
void ScrollPane::UsePaneID( u32 paneID )
{
  m_isUseCharaPane = true;
  m_paneId         = paneID;
}


/**
 * @brief 使用しているペインIDをクリアする
 */
void ScrollPane::FreePane( void )
{
  m_isUseCharaPane = false;
  m_paneId    = 0;
}


/**
 * @brief ペインIDの使用状況を確認
 */
bool ScrollPane::IsUsedPane( void ) const
{
  return m_isUseCharaPane;
}

/**
 * @brief   画面内にペインが存在するか判定
 *
 * @param   scrollX Ｘスクロール量
 * @param   scrollY Ｙスクロール量
 * @param   are 表示エリア
 *
 * @return  画面内にペインが存在するならtrueを返却
 */
 bool ScrollPane::IsDisplayIn( s16 scrollX, s16 scrollY, const ScrollPaneDrawArea & area ) const
{
  s16 left    = area.GetX();
  s16 right   = left + area.GetWidth();
  s16 top     = area.GetY();
  s16 bottom  = top + area.GetHeight();
  
  //topを小さい方とする
  if( bottom < top )
  {
    s16 temp = bottom;
    bottom = top;
    top = temp;
  }
  //leftを小さい方とする
  if( right < left )
  {
    s16 temp = right;
    right = left;
    left = temp;
  }
  
  s16 x   = m_nowPos.GetX() + scrollX;
  s16 y   = m_nowPos.GetY() + scrollY;

  return ( (left-m_width/2) <= x ) && (x <= (right+m_width/2))
    && ((top-m_height/2) <= y) && ( y <= (bottom+m_height/2) );

    // && (gfl::math::Abs(m_nowPos.y+scrollY) < ((DISPLAY_LOWER_HEIGHT+m_height)>>1));
  
    

    //(gfl::math::Abs(m_nowPos.x+scrollX) < ((DISPLAY_LOWER_WIDTH+PANE_BOUND_SIZE)>>1))
}

/**
 * @brief 現在座標の設定
 *
 * @param pos 設定値となる座標
 */
void ScrollPane::SetPosition( const ScrollPanePosition& pos )
{
  m_nowPos = pos;
}

/**
 * @brief   現在の座標位置を取得
 *
 * @return  現在座標位置
 */
const ScrollPanePosition& ScrollPane::GetPosition( void ) const
{
  return m_nowPos;
}

/**
 * @brief 幅を取得
 *
 * @return  幅
 */
u16 ScrollPane::GetWidth( void ) const
{
  return m_width;
}

/**
 * @brief 高さを取得
 *
 * @return  高さ
 */
u16 ScrollPane::GetHeight( void ) const
{
  return m_height;
}

/**
 * @brief   現在のアルファ値を取得
 *
 * @return  アルファ値
 */
u8 ScrollPane::GetAlpha( void )
{
  return m_alpha;
}


/**
 * @brief   ローディング状態に設定する
 */
void ScrollPane::StartLoading( void )
{
  m_loading = LOAD_STATE_LOADING;
}

/**
 * @brief   ローディング状態を完了に設定する
 */
void ScrollPane::FinishLoading( void )
{
  if( m_loading != LOAD_STATE_LOADING )
  {
    GFL_ASSERT( 0 );
    return;
  }
  m_loading = LOAD_STATE_FINISH;
}

/**
 * @brief   ローディング状態を取得
 *
 * @return  ローディング中か判断
 */
ScrollPane::LoadState ScrollPane::IsLoading( void ) const
{
  return m_loading;
}

/**
 * @brief 入力検知
 *
 * @param is_enable true入力可 false入力できない
 */
void ScrollPane::SetInputEnable( bool is_enable )
{
  m_isInputEnable = is_enable;
}

/**
 * @brief 入力検知
 *
 * @return true入力可 false入力できない
 */
bool ScrollPane::GetInputEnable( void ) const
{
  return m_isInputEnable;
}

//=============================================================================
// PRIVATE
//=============================================================================
/**
 * @brief  メンバ変数の初期化
 */
void ScrollPane::Reset( void )
{
  m_isEnable        = false;
  m_isDelete        = false;
  m_isUseCharaPane  = false;
  m_loading         = LOAD_STATE_NONE;
  m_alpha          = 0xFF;
  m_width   = 0;
  m_height  = 0;
}


/**
 * @brief  削除アニメーション処理
 *
 * @return 削除アニメ完了ならtrueを返却
 */
bool ScrollPane::EraseAction( void )
{
  if( m_alpha > SUB_ALPHA )
  {
    m_alpha -= SUB_ALPHA;
    return false;
  }
  return true;
}

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )
