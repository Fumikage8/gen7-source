//============================================================================================
/**
 * @file    UIViewManager.h
 * @brief   UIViewの中で統一で管理したい機能をもたせたクラス
            UIView内部で参照+使用されるものであり、外部からは参照しないで下さい。
            GameManagerのシングルトン登録のため公開においています。
 * @author  k.ohno
 * @date    15.3.25
 */
//============================================================================================
#ifndef __APP_UI_VIEW_MANAGER_H__
#define __APP_UI_VIEW_MANAGER_H__
#pragma  once

#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>
#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

class UIView;

class UIViewManager : public gfl2::base::Singleton<UIViewManager>
{
  GFL_FORBID_COPY_AND_ASSIGN( UIViewManager );


public:

  /**
   * @brief   コンストラクタ＆デストラクタ
   */
  UIViewManager(void);

  ~UIViewManager();

  /**
   * @brief 作ったUIViewの登録
   * @param  UIView
   */
  void SetUIView(UIView* pUIView);


  void ResetUIView(UIView* pUIView);

  /**
   * @brief  一番優先順位が高いUIViewのセット
   * @param  UIView
   */
  void SetTopPriorityUIView(UIView* pUIView);

  /**
   * @brief   レスポンスしていいかどうか確認する
   * @return  true=OK false=他のViewが優先している
   */
  bool IsRespondUIView(UIView* pUIView);


  /**
   * @brief  登録されているUIViewがボタンアニメしているかどうかを取得する
   */
  bool IsButtonAnimation(UIView* pUIView);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ボタンアニメ中にキー入力を効かなくするかどうかを取得する
   *
   * @return true  許可  false不許可（デフォルト）
   */
  //--------------------------------------------------------------------------------------------
  bool GetInputExclusiveKeyByButtonAnimation( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ボタンアニメ中にキー入力を効かなくするかどうかを設定する
   *
   * @param flg   true  許可  false不許可
   */
  //--------------------------------------------------------------------------------------------
  void SetInputExclusiveKeyByButtonAnimation( bool flg );


private:
  UIView* GetUIViewTreeParent(UIView* pUIView);

  
private:

  static const int LIST_MAX = 32;  //管理数はいつでも変更( 16 -> 32 に変更　)
  
  UIView*     m_pUIViewEntry[LIST_MAX];   //登録されるUiView
  UIView*     m_pUIView_TopPriority;      //優先権の高いUIView

  bool                       m_isKeyExclusiveByButtonAnime;    //!< ボタンアニメ中にキー入力を効かなくするかどうか
  

  

};



GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)


/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( app::ui::UIViewManager );

#endif // __APP_UI_VIEW_MANAGER_H__

