﻿#pragma once
//=================================================================================================
/**
 *  @file   BaseSolver.h
 *  @brief  ソルバーベース
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector3.h>

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

  static const f32 MARJIN;     //< 頑健性のために余分に押し返す量

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

  //----------------------------------------------------------------------------
  /**
  *  @brief  引数のツリーに登録されているメッシュ形状を軽量化し保持する
  *
  *  @param  pActorRoot    軽量化対象のツリー
  *  @param  pos           ポリゴンを収集する中心値
  *  @param  collectionR   ポリゴンを収集する範囲
  */
  //-----------------------------------------------------------------------------
  virtual void MeshCooking(gfl2::renderingengine::scenegraph::instance::InstanceNode* pActorRoot,gfl2::math::Vector3 pos,f32 collectionR);

private:

};
