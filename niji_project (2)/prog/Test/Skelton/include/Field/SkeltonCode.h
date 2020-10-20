#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
@file SkeltonCode.h
@brief		動作コード　処理管理　基底クラス
@date	 2014.03.13
@author takahashi_tomoya 2011.05.14
@author tamada@gamefreak.co.jp

*/
//=================================================================================================

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>


#include "Test/Skelton/include/SkeltonCharaDefine.h"

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(Skelton)

class SkeltonFieldTransform;
class SkeltonCamera;

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


class SkeltonCodeBase
{
  GFL_FORBID_COPY_AND_ASSIGN(SkeltonCodeBase);

public:



  // ワーク初期化
  //  static void InitializeSkeltonCodeWork( SkeltonCodeWork* p_wk, SkeltonModel* p_model, ActionCmdBase::ActionCmdWork* p_acmd );
  // static void PopSkeltonCodeWork( SkeltonCodeWork* p_wk, SkeltonModel* p_model, ActionCmdBase::ActionCmdWork* p_acmd, SKELTON_CODE code );

  // 共有ワークの取得
  //  static void* GetSkeltonCodeWork( SkeltonCodeWork* p_wk, u32 size );
  //----------------------------------------------------------------------------
  /**
  *	@brief  ワーク初期化
  *
  *	@param	p_wk      ワーク
  *	@param	p_model   動作モデルベース
  */
  //-----------------------------------------------------------------------------
  static void InitializeSkeltonCodeWork( SkeltonCodeWork* p_wk, SkeltonModel* p_model, ActionCmdBase::ActionCmdWork* p_acmd )
  {
    p_wk->p_model = p_model;
    p_wk->p_acmd  = p_acmd;
    p_wk->mv_code = SKELTON_CODE_NONE;
  }

  //----------------------------------------------------------------------------
  /**
  *	@brief  ワークの復元
  *
  *	@param	p_wk
  *	@param	p_model
  *	@param	p_acmd
  *	@param	code 
  */
  //-----------------------------------------------------------------------------
  static void PopSkeltonCodeWork( SkeltonCodeWork* p_wk, SkeltonModel* p_model, ActionCmdBase::ActionCmdWork* p_acmd, SKELTON_CODE code )
  {
    p_wk->p_model = p_model;
    p_wk->p_acmd  = p_acmd;
    p_wk->mv_code = code;
  }

  //----------------------------------------------------------------------------
  /**
  *	@brief  共有ワークを取得
  *    
  *	@param	p_wk    動作コードワーク
  *	@param	size    必要なサイズ
  *
  *	@return ワークポインタ
  */
  //-----------------------------------------------------------------------------
  static void* GetSkeltonCodeWork( SkeltonCodeWork* p_wk, u32 size )
  {
    // GFL_ASSERT( size <= SKELTON_CODE_WORK_SIZE );

    return reinterpret_cast<void*>(p_wk->work);
  }



public:

  //-----------------------------------------------------------------------------
  /**
  *    コンストラクタ
  */
  //-----------------------------------------------------------------------------
  SkeltonCodeBase( void ){}

  //-----------------------------------------------------------------------------
  /**
  *    デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~SkeltonCodeBase(){}


  //----------------------------------------------------------------------------
  /**
  *	@brief  対応している動作コードの取得
  *
  *	@return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual SKELTON_CODE GetMVCode(void) const = 0;


  //----------------------------------------------------------------------------
  /**
  *	@brief  動作コード処理　初期化
  *
  *	@param	p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess( SkeltonCodeWork* p_wk ) const = 0;


  //----------------------------------------------------------------------------
  /**
  *	@brief  動作コード処理  メイン　
  *
  *	@param	p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess( SkeltonCodeWork* p_wk ) const = 0;

  //----------------------------------------------------------------------------
  /**
  *	@brief  動作コード処理  破棄
  *
  *	@param	p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void DeleteProcess( SkeltonCodeWork* p_wk ) const = 0;

  //----------------------------------------------------------------------------
  /**
  *	@brief  動作コード処理  復帰
  *
  *	@param	p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void ReturnProcess( SkeltonCodeWork* p_wk ) const = 0;

};	// end of class SkeltonCodeBase;



GFL_NAMESPACE_END(Skelton)
  GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32