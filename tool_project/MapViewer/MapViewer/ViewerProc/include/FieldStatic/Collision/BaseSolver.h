#pragma once
//=================================================================================================
/**
 *  @file   BaseSolver.h
 *  @brief  ソルバーベース
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
class BaseActor;

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace instance {
  class InstanceNode;
} } } }

//----------------------------------------------------------------------------------------
//! @brief ソルバーベース
//----------------------------------------------------------------------------------------
class BaseSolver
{
public:

  static const s32 MARJIN = 0.5f;     //< 頑健性のために余分に押し返す量

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  BaseSolver();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~BaseSolver();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ツリー間の衝突を解決
  *
  *  @param  pRootStaticActorRoot  衝突されるツリー
  *  @param  pRootStaticActorRoot  衝突するツリー
  */
  //-----------------------------------------------------------------------------
  virtual void Solved(gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootStaticActorRoot,
                      gfl2::renderingengine::scenegraph::instance::InstanceNode* pRootDynamicActorRoot);

private:

};
