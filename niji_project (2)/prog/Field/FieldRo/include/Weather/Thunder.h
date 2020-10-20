//======================================================================
/**
 * @file Thunder.h
 * @date 2015/10/27 11:59:56
 * @author saito_nozomu
 * @brief 雷
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __THUNDER_H_INCLUDED__
#define __THUNDER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
// poke_3d
#include <model/include/gfl2_BaseModel.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

  /**
   * @class Thunder
   * @brief 天候雨に付随する雷を制御するクラス
   */
  class Thunder
  {
      GFL_FORBID_COPY_AND_ASSIGN(Thunder);
  public:
    /**
     * @brief コンストラクタ
     */
    Thunder(){
      m_Alpha = 0;
      m_NextAlpha = 0;
      m_ShortFlash = false;
      m_FlashMargine = 0;
      m_SEMargine = 0;
      m_pModel = NULL;
      m_Active= false;
    }

    /**
     * @brief デストラクタ
     */
    ~Thunder(){}

    /**
     * @brief 初期化
     * @param pModel モデルへのポインタ
     */
    void Initialize(poke_3d::model::BaseModel *pModel);
  
    /**
     * @brief 更新関数
     */
    void Update(void);

    /**
     * @brief フラッシュ処理のアクティブ状態の制御
     * @param active true　でアクティブ
     */
    void SetActive(bool active);

    /**
     * @brief 表示状態取得
     * @retrun bool trueで表示
     */
    bool IsVisible(void);

    private:
 
    /**
     * @brief フラッシュ設定
     */
    void Flash(void);
    
    /**
     * @brief フラッシュ更新関数
     */
    void FlashFade(void);

    /**
     * @brief コンスタントカラーのα値セット
     */
    void SetAlpha(f32 alpha);

    /**
     * @brief モデルのフラッシュ処理
     */
    void ModelFlash(void);

    /**
     * @brief 雷の音制御
     */
    void SoundUpdate(void);

  private:
    f32 m_Alpha;
    f32 m_NextAlpha;
    bool m_ShortFlash;
    int m_FlashMargine;
    int m_SEMargine;
    poke_3d::model::BaseModel *m_pModel;
    bool m_Active;
  };

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)

#endif // __HEADER_H_INCLUDED__