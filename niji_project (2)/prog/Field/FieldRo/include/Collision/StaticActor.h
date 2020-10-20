﻿#pragma once
//=================================================================================================
/**
 *  @file   StaticActor.h
 *  @brief  当たられる側アクター
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "BaseActor.h"

//----------------------------------------------------------------------------------------
//! @brief 当たられる側アクター
//----------------------------------------------------------------------------------------
class StaticActor : public BaseActor
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  StaticActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~StaticActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);
};
