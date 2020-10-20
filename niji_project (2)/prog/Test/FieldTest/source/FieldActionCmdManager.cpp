//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdManager.cpp
 *	@brief  フィールドアクションコマンドマネージャ
 *	@author	tomoya takahashi
 *	@date		2011.05.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include "../include/FieldActionCmdManager.h"
#include "../include/FieldActionCmdNone.h"
#include "../include/FieldActionCmdAnalogMove.h"
#include "../include/FieldActionCmdTsuriTest.h"

namespace field{
namespace mmodel{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

// アクションコマンドテーブル
ActionCmdBase* ActionCmdManager::m_pTbl[ AC_MAX ];

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief    初期化
 *
 *	@param	  p_heap    ヒープ
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::Initialize( gfl2::heap::HeapBase* p_heap )
{
  // m_pTblを生成
  m_pTbl[ AC_NONE ] = GFL_NEW( p_heap ) ActionCmdNone();

  // 振り向き系

  // 移動系
   m_pTbl[ AC_WALK_32F ] = GFL_NEW( p_heap ) ActionCmdWalk_32F();

  // その場移動系

  // ジャンプ系

  // ウェイト系

  // 描画フラグ系

  // ワープ系

  // アイコン表示
  
  // ローラースケート系

  // 忍び足系

  // 自転車系

  // 波乗り系

  // サイホーン系

  // やぎ系

  // アニメーションアクション
   m_pTbl[ AC_TSURI_TEST ] = GFL_NEW( p_heap ) ActionCmdTsuriTest();

#if PM_DEBUG
  // 整合性チェック
  for( u32 i=0; i<AC_MAX; ++i )
  {
    if( m_pTbl[i]->GetAcmdID() != i ){
      GFL_ASSERT( 0 );
      break;
    }
  }
#endif
}


//----------------------------------------------------------------------------
/**
 *	@brief    破棄
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::Finalize( void )
{
  for( u32 i=0; i<AC_MAX; ++i ){

    if( m_pTbl[i] ){
      GFL_DELETE m_pTbl[i];
      m_pTbl[i] = NULL;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    MoveModelWork側でのコマンドの実行
 *
 *	*コマンドの終了状態を保持している。
 *	*コマンドを実行した人が、ワークをクリアする必要がある。
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::ActionCmd( ActionCmdBase::ActionCmdWork* p_wk )
{
  ActionCmdBase* p_acmd;
  
  if( p_wk->acmd < AC_MAX ){
   
    p_acmd = m_pTbl[p_wk->acmd];
    
  }else{
    GFL_ASSERT( p_wk->acmd < AC_MAX ); //@check
    p_acmd = m_pTbl[AC_NONE];

    p_wk->acmd  = AC_NONE;
    p_wk->seq   = 0;
  }

  // 実行
  p_acmd->UpdateAcmd( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief    動作モデルPush処理による強制終了時の処理
 *	@note     ActionCmdWorkはPop時にクリアされる
 *
 *	*コマンドを実行した人が、ActionCmdWorkをクリアする必要がある。
 */
//-----------------------------------------------------------------------------
void ActionCmdManager::PushCmd( ActionCmdBase::ActionCmdWork* p_wk )
{
  if( p_wk->acmd >= AC_MAX )
  {
    return;
  }

  // 実行
  m_pTbl[p_wk->acmd]->PushCmd( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief    コマンド完了待ち
 *
 *	@retval true    完了
 *	@retval false   途中
 */
//-----------------------------------------------------------------------------
bool ActionCmdManager::IsEndCmd( const ActionCmdBase::ActionCmdWork* cp_wk )
{
  return ActionCmdBase::IsAcmdWorkEnd( cp_wk );
}

} // mmodel
} // field
