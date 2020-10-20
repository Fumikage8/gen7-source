// ============================================================================
/*
 * @file gflnet2_CTR_ICecControlObjectRegister.h
 * @brief すれ違い通信の制御オブジェクトを登録するクラスです
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_ICECCONTROLOBJECTREGISTER_H_INCLUDE )
#define GFLNET2_CTR_ICECCONTROLOBJECTREGISTER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)

class CTR_CecControlObject;

class CTR_ICecControlObjectRegister
{
public:
  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信の制御オブジェクトを登録します。
   * @param  pCecControlObject すれ違い通信の制御オブジェクト
   */
  // --------------------------------------------------------------------------
  virtual void RegisterControlObject( CTR_CecControlObject* pCecControlObject ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリのコマンドを即実行します。
   * @param  pCecControlObject すれ違い通信の制御オブジェクト
   */
  // --------------------------------------------------------------------------
  virtual void QuickExecute( CTR_CecControlObject* pCecControlObject ) = 0;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_ICECCONTROLOBJECTREGISTER_H_INCLUDE
#endif
