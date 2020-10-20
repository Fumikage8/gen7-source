//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldActionCmdBase.cpp
 *	@brief  フィールドアクションコマンド　基底クラス
 *	@author	tomoya takahashi
 *	@date		2011.05.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "../include/FieldActionCmdBase.h"
#include "../include/movemodel.h"

namespace field{
namespace mmodel{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

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
 *	@brief  アクションコマンドワーク　初期化
 *
 *	@param	p_wk        ワーク
 *	@param	p_model     動作モデルワークベース
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::InitializeAcmdWork( ActionCmdWork* p_wk, fld::mmodel::MoveModel* p_model )
{
  p_wk->p_model   = p_model;
  p_wk->acmd      = static_cast<Acmd>(0);
  p_wk->seq       = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  共有ワーク取得
 *
 *	@param	p_wk      ワーク
 *	@param	size      サイズ
 *
 *	@return 共有ワークサイズ
 */
//-----------------------------------------------------------------------------
void* ActionCmdBase::GetAcmdWork( ActionCmdWork* p_wk, u32 size )
{
  GFL_ASSERT( size < WORK_SIZE ); //@check
  return reinterpret_cast<void*>(p_wk->work);
}

//----------------------------------------------------------------------------
/**
 *	@brief  アクションコマンドの設定
 *
 *	@param	p_wk
 *	@param	acmd 
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::SetAcmd( ActionCmdWork* p_wk, Acmd acmd, gfl2::math::Quaternion qua )
{
  GFL_ASSERT( acmd < AC_MAX ); // , "MMDL ACMD CODE ERROR" ); //@check
  p_wk->acmd  = acmd;
  p_wk->seq   = 0;
  p_wk->qua   = qua;
  p_wk->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD );
  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD_END );
}

//----------------------------------------------------------------------------
/**
 *	@brief  シーケンスを進める
 *
 *	@param	p_wk    ワーク
 *	@param	add     変動値
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::AddAcmdWorkSeq( ActionCmdWork* p_wk, s32 add )
{
  if( (static_cast<s32>(p_wk->seq) + add) >= 0 ){
    p_wk->seq += add;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  シーケンスを設定
 *
 *	@param	p_wk    ワーク
 *	@param	seq     シーケンス
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::SetAcmdWorkSeq( ActionCmdWork* p_wk, s32 seq )
{
  p_wk->seq = seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  シーケンスを取得
 *
 *	@param	p_wk    ワーク
 */
//-----------------------------------------------------------------------------
s32 ActionCmdBase::GetAcmdWorkSeq( ActionCmdWork* p_wk )
{
  return p_wk->seq;
}

//----------------------------------------------------------------------------
/**
 *	@brief  終了チェック
 *
 *	@param	p_wk  ワーク
 *
 *	@retval true    終了
 *	@retval false   途中
 */
//-----------------------------------------------------------------------------
bool ActionCmdBase::IsAcmdWorkEnd( const ActionCmdWork* cp_wk )
{
	if( (cp_wk->acmd == 0) || (cp_wk->p_model->CheckMoveBit(fld::mmodel::MOVEBIT_ACMD_END)) ){
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ワークリセット
 *
 *	@param	p_wk  ワーク
 */
//-----------------------------------------------------------------------------
void ActionCmdBase::ClearAcmdWork( ActionCmdWork* p_wk )
{
  p_wk->acmd  = static_cast<Acmd>(0);
  p_wk->seq   = 0;

  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD );
  p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD_END );

}

//----------------------------------------------------------------------------
/**
 *	@brief  アクションコマンドの更新
 *
 *	@retval ACMD_RET_ONE_TIME_END,      ///<続く
 *	@retval ACMD_RET_END,           ///<終わり
 */
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdBase::UpdateAcmd( ActionCmdWork* p_wk ) const
{
  AcmdRet ret = ACMD_RET_END;
  s32 roop_count = 0;

  GFL_ASSERT( ACMDFUNC_SEQ_MAX >= m_SeqNum ); //@check
  
  // シーケンスチェック
  if( m_SeqNum > p_wk->seq ){
  
    do{
      
      ret = (m_pFuncTbl[ p_wk->seq ])( p_wk );

      // シーケンスオーバーチェック
      GFL_ASSERT( m_SeqNum > p_wk->seq ); //@check

      ++roop_count;

      // コンティニューが1000回続いたら、明らかにおかしいためアサートストップ
      GFL_ASSERT_STOP( roop_count < 1000 );
      
    }while( ret == ACMD_RET_CONTINUE ); // CONTINUEの間動作  @whilecheck
  }

  if( ret == ACMD_RET_END ){
    // アクションコマンドの実行が完了したことを、
    // モデルに設定
    p_wk->p_model->offMoveBit( fld::mmodel::MOVEBIT_ACMD );
    p_wk->p_model->onMoveBit( fld::mmodel::MOVEBIT_ACMD_END );
  }


  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  共通終了処理
 *
 *	@param	p_work  ワーク
 *
 *	@retval ACMD_RET_END
 */
//-----------------------------------------------------------------------------
ActionCmdBase::AcmdRet ActionCmdBase::AcEnd( ActionCmdWork* /*p_work*/ )
{
  return ACMD_RET_END;
}

} // mmodel
} // field
