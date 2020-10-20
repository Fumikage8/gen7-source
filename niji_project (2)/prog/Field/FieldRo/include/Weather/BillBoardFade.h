//======================================================================
/**
 * @file BillboardFade.h
 * @date 2015/11/09 15:56:14
 * @author saito_nozomu
 * @brief ビルボードのフェード管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BILLBOARD_FADE_H_INCLUDED__
#define __BILLBOARD_FADE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
// poke_3d
#include <model/include/gfl2_BaseModel.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

/**
 * @class BillBoardFade
 * @brief ビルボードフェード管理クラス
 */
class BillBoardFade
{
  GFL_FORBID_COPY_AND_ASSIGN(BillBoardFade);

public:
  /**
   * @brief コンストラクタ
   */
  BillBoardFade(){
    m_State = IDLE;
    m_Alpha = 0;
    m_pModel = NULL;
  }
  /**
   * @brief デストラクタ
   */
  ~BillBoardFade(){}

  /**
   * @brief 初期化
   * @param pModel モデルへのポインタ
   * @param alpha  α値
   */
  void Initialize(poke_3d::model::BaseModel *pModel, f32 alpha);

  /**
   * @brief 更新処理
   */
  void Update(void);

  /**
   * @brief フェードアウトリクエスト
   */
  void RequestFadeOut(void){ m_State = FADE_OUT; }

  /**
   * @brief フェードインリクエスト
   */
  void RequestFadeIn(void){ m_State = FADE_IN; }

  /**
   * @brief 表示状態
   * @param visible true で表示
   */
  void SetVisible(bool visible);

  /**
   * @brief 表示状態取得
   * @retrun bool trueで表示
   */
  bool IsVisible(void);

private:
  /**
   * @brief コンスタントカラーのα値セット
   * @param alpha α値
   */
  void SetAlpha(f32 alpha);
  
  enum STATE{
    IDLE,
    FADE_OUT,
    FADE_IN,
  };

  STATE m_State;
  f32 m_Alpha;
  poke_3d::model::BaseModel *m_pModel;
};


GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)

#endif // __BILLBOARD_FADE_H_INCLUDED__