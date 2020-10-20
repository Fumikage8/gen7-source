#if defined(GF_PLATFORM_WIN32)
#pragma once
//=================================================================================================
/**
 @file SkeltonCode_player.h
 @brief		動作コード　処理管理　基底クラス
 @date	 2014.03.17
 @author takahashi_tomoya 2012.11.30
 @author tamada@gamefreak.co.jp

 */
//=================================================================================================

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>


#include "Test/Skelton/include/Field/SkeltonModelDefine.h"

#include "SkeltonCode.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


class SkeltonCodePlayer : public SkeltonCodeBase
{
	GFL_FORBID_COPY_AND_ASSIGN(SkeltonCodePlayer);

public:


public:

  //-----------------------------------------------------------------------------
  /**
   *    コンストラクタ
   */
  //-----------------------------------------------------------------------------
  SkeltonCodePlayer( void ){}

  //-----------------------------------------------------------------------------
  /**
   *    デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~SkeltonCodePlayer(){}


  //----------------------------------------------------------------------------
  /**
   *	@brief  対応している動作コードの取得
   *
   *	@return 動作コード
   */
  //-----------------------------------------------------------------------------
	virtual SKELTON_CODE GetMVCode(void) const { return SKELTON_CODE_PLAYER; }
  

  //----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理　初期化
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
  virtual void InitProcess( SkeltonCodeWork* p_wk ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理  メイン　
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
  virtual void MainProcess( SkeltonCodeWork* p_wk ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理  破棄
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
	virtual void DeleteProcess( SkeltonCodeWork* p_wk ) const{}

  //----------------------------------------------------------------------------
  /**
   *	@brief  動作コード処理  復帰
   *
   *	@param	p_wk  動作コードワーク
   */
  //-----------------------------------------------------------------------------
  virtual void ReturnProcess( SkeltonCodeWork* p_wk ) const{}
  
};	// end of class SkeltonCodePlayer;
	
GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
