﻿#pragma once
//=================================================================================================
/**
 *  @file   DynamicActor.h
 *  @brief  当たる側アクター
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include "BaseActor.h"

//----------------------------------------------------------------------------------------
//! @brief 当たる側アクター
//----------------------------------------------------------------------------------------
class DynamicActor : public BaseActor
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  DynamicActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~DynamicActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  対象のクッキングデータを利用する
  */
  //-----------------------------------------------------------------------------
  void EnableCookingData(void){ m_isUseCookingData = true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  対象のクッキングデータを利用しない
  */
  //-----------------------------------------------------------------------------
  void DisableCookingData(void){ m_isUseCookingData = false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  対象のクッキングデータを利用するか取得する
  *
  *  @return 利用するかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsUseCookingData(void){ return m_isUseCookingData; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  押し返しベクトルを利用する
  */
  //-----------------------------------------------------------------------------
  void EnableAdjustVector(void){ m_isUseAdjustVector = true; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  押し返しベクトルを利用しない
  */
  //-----------------------------------------------------------------------------
  void DisableAdjustVector(void){ m_isUseAdjustVector = false; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  対象が押し返しベクトルを利用するか取得する
  *
  *  @return 利用するかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsEnableAdjustVector(void){ return m_isUseAdjustVector; }

private:

  // 対象のクッキングデータを利用するかどうか
  b32 m_isUseCookingData;
  // 押し返しが有効かどうか
  b32 m_isUseAdjustVector;
};
