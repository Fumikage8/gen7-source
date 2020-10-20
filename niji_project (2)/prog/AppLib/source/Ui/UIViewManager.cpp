//============================================================================================
/**
 * @file    UIViewManager.cpp
 * @brief   UIViewの中で管理したい機能をもたせたクラス
 * @author  k.ohno
 * @date    15.3.25
 *
 */
//============================================================================================
#include "AppLib/include/Ui/UIViewManager.h"
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIResponder.h"


SINGLETON_INSTANCE_DEFINITION(app::ui::UIViewManager);
template class gfl2::base::SingletonAccessor<app::ui::UIViewManager>;


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(ui)

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
//SINGLETON_INSTANCE_DEFINITION(UIViewManager);
//template class gfl2::base::SingletonAccessor<UIViewManager>;


UIViewManager::UIViewManager(void)
{
  m_isKeyExclusiveByButtonAnime = false;  //無効状態
	for(int i=0;i<LIST_MAX;i++){
    m_pUIViewEntry[i] = NULL;
  }
}


UIViewManager::~UIViewManager()
{

}



void UIViewManager::SetUIView(UIView* pUIView)
{
  for(int i=0;i<LIST_MAX;i++){
    if(m_pUIViewEntry[i] == NULL){
      m_pUIViewEntry[i] = pUIView;
      return;
    }
  }
  GFL_ASSERT(0);  //ここにきた場合はLIST_MAXを増やしてください
}

void UIViewManager::ResetUIView(UIView* pUIView)
{
  for(int i=0;i<LIST_MAX;i++){
    if(m_pUIViewEntry[i] == pUIView){
      if(m_pUIView_TopPriority==pUIView){
        m_pUIView_TopPriority = NULL;
      }
      m_pUIViewEntry[i] = NULL;
      return;
    }
  }
  GFL_ASSERT(0);  //無いものをリセットしているのは実装不具合と疑ってください
}

void UIViewManager::SetTopPriorityUIView(UIView* pUIView)
{
  m_pUIView_TopPriority = pUIView;
}

bool UIViewManager::IsRespondUIView(UIView* pUIView)
{
  if(m_pUIView_TopPriority==NULL){ //登録されて無い場合操作可能 kujira,sangoどおり
    return true;
  }
  if(m_pUIView_TopPriority==pUIView){ //優先主張Viewが在る場合 これしかゆるさない
    return true;
  }
  return false;

}


/**
 * @brief  登録されているUIViewがボタンアニメしているかどうかを取得する
 * @param  pUIView  処理を呼び出したUIView
 * @return trueアニメ中  falseアニメしていない
 */
bool UIViewManager::IsButtonAnimation(UIView* pUIView)
{
  UIView* parent = GetUIViewTreeParent(pUIView);  // 大元の親を取得

  for(int i=0;i<LIST_MAX;i++){
    if(m_pUIViewEntry[i] && m_pUIViewEntry[i] != pUIView && GetUIViewTreeParent(m_pUIViewEntry[i]) == parent ){
      if(m_pUIViewEntry[i]->IsButtonAnimation()){
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief   UIViewの大元の親を取得
 * @param   pUIView   子ビュー
 * @return  大元の親ビュー
 * @note    子ビューが大元の場合は自分を返す
 */
UIView* UIViewManager::GetUIViewTreeParent(UIView* pUIView)
{
  UIView* parent = pUIView;
  for ever{
    UIView* tmp = parent->GetSuperView();
    if( tmp == NULL )
    {
      break;
    }
    parent = tmp;
  }
  return parent;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ボタンアニメ中にキー入力を効かなくするかどうかを取得する
 *
 * @return true  許可  false不許可（デフォルト）
 */
//--------------------------------------------------------------------------------------------
bool UIViewManager::GetInputExclusiveKeyByButtonAnimation( void )
{
  return m_isKeyExclusiveByButtonAnime;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ボタンアニメ中にキー入力を効かなくするかどうかを設定する
 *
 * @param flg   true  許可  false不許可
 */
//--------------------------------------------------------------------------------------------
void UIViewManager::SetInputExclusiveKeyByButtonAnimation( bool flg )
{
  m_isKeyExclusiveByButtonAnime = flg;
}





GFL_NAMESPACE_END(ui)
GFL_NAMESPACE_END(app)
