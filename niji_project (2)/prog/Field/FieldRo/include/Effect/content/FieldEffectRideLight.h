//======================================================================
/**
 * @file FieldEffectRideLight.h
 * @date 2015/09/11 19:25:51
 * @author miyachi_soichi
 * @brief エフェクト：ライド光
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_RIDELIGHT_H_INCLUDED__
#define __FIELD_EFFECT_RIDELIGHT_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

class IEffectRideBase : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(IEffectRideBase);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  IEffectRideBase( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~IEffectRideBase( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  開始
   */
  void CreateEffect( gfl2::math::Vector3 pos );

  /**
   *  @brief  終了
   */
  void DeleteEffect( void );

  /**
   *  @brief  座標オフセットの設定
   */
  void setOffsetPosition( gfl2::math::Vector3  offsetPos );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief 再生フレーム数を取得
   */
  u32 GetFrame() const;

  /**
   *  @brief 生存の確認
   */
  b32 IsAlive() const;

protected:

  /**
   *  @brief リソースIDの取得
   */
  virtual u32 GetResourceID( void ) const = 0;

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;

  u32               m_PlayFrame;
  b32               m_IsAlive;
  u16               m_Seq;
};

/**
 *  @class  EffectRideApperLand
 *  @brief  エフェクト：ライド乗り降り陸用エフェクト
 */
class EffectRideApperLand : public IEffectRideBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRideApperLand);
public:
  /**
   *  @brief  コンストラクタ
   */
  EffectRideApperLand( void ) : IEffectRideBase(){}
  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRideApperLand( void ){}
private:
  /**
   *  @brief リソースIDの取得
   */
  virtual u32 GetResourceID( void ) const;
};


/**
 *  @class  EffectRideApperSea
 *  @brief  エフェクト：ライド乗り降り陸用エフェクト
 */
class EffectRideApperSea : public IEffectRideBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectRideApperSea);
public:
  /**
   *  @brief  コンストラクタ
   */
  EffectRideApperSea( void ) : IEffectRideBase(){}
  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectRideApperSea( void ){}
private:
  /**
   *  @brief リソースIDの取得
   */
  virtual u32 GetResourceID( void ) const;
};


GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_RIDELIGHT_H_INCLUDED__
