/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBase.h
 *  @brief  フィールドエフェクト規定クラス
 *  @author miyachi_soichi
 *  @date   2015.50.20
 */

#if !defined( __FIELD_EFFECT_BASE_H__ )
#define __FIELD_EFFECT_BASE_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <heap/include/gfl2_Heap.h>
// types
#include <niji_conv_header/field/FieldEffectTypes.h>

// 先行宣言
GFL_NAMESPACE_BEGIN( Field );
class IField3DObjectNode;
GFL_NAMESPACE_END( Field );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Effect );

class IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( IEffectBase );

private:
  enum State
  {
    STATE_EXEC,
    STATE_REQUEST_DELETE,
    STATE_DELETE,
  }; // enum State

public:
  /**
   *  @brief  コンストラクタ
   */
  IEffectBase( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~IEffectBase( void );

  /**
   *  @brief  初期化処理：モデル指定
   *  @param  in_pResource  リソース
   *  @param  in_pHeap      ヒープ
   *  @param  in_pParent    親モデル
   */
  virtual void Initialize( void *in_pResource, gfl2::heap::HeapBase *in_pHeap, Field::IField3DObjectNode *in_pParent );

  /**
   *  @brief  初期化処理：座標指定
   *  @param  in_pResource  リソース
   *  @param  in_pHeap      ヒープ
   *  @param  in_vPosition  座標
   */
  virtual void Initialize( void *in_pResource, gfl2::heap::HeapBase *in_pHeap, const gfl2::math::Vector3 &in_vPosition );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  更新処理
   */
  virtual void Update( void );
  
  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void ){}

  /**
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void ){  return true; }

  /**
   *  @brief  破棄リクエスト
   */
  void RequestDelete( void );

  /**
   *  @brief  実行中確認
   *  @retval true  実行中である
   *  @retval false 破棄予約がなされている
   */
  bool IsExec( void ) const { return m_eState == STATE_EXEC; }

  /**
   *  @brief  破棄許可確認
   *  @retval true  破棄して問題ない
   *  @retval false 破棄すると問題がある
   */
  bool IsSafeDelete( void ) const;

  /**
   *  @brief  親オブジェクトの比較
   *  @param  in_pParent    比較する親オブジェクト
   *  @retval true  自身の親と一致する
   *  @retval false 一致しない
   */
  bool IsSameParent( Field::IField3DObjectNode *in_pParent ) const;
  void SetParent(Field::IField3DObjectNode *in_pParent) { m_pParent = in_pParent; }

  /**
   *  @brief  SE再生の有無設定
   */
  inline void SetPlaySe( bool bPlaySe ){ m_bPlaySe = bPlaySe; }

  /**
   *  @brief  エフェクトの種類を設定
   */
  inline void SetType( Type eType ){ m_eType = eType; }
  /**
   *  @brief  エフェクトの種類を取得
   */
  inline Type GetType( void ){ return m_eType; }

protected:
  /**
   *  @brief  通常時更新
   */
  virtual void updateNormal( void ){}

  /**
   *  @brief  SEさいせいの有無
   */
  bool IsPlaySe( void ) const { return m_bPlaySe; }

private:
  Field::IField3DObjectNode   *m_pParent;   //!< 親オブジェクト
  State                       m_eState;     //!< ステート
  Type                        m_eType;      //!< エフェクトの種類
  bool                        m_bPlaySe;    //!< SE再生の有無

}; // class IEffectBase

GFL_NAMESPACE_END( Effect );
GFL_NAMESPACE_END( Field );

#endif // !defined( __FIELD_EFFECT_BASE_H__ )
