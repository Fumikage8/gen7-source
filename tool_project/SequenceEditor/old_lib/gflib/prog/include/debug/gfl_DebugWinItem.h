//======================================================================
/**
 * @file  gfl_DebugwinItem.h
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author  ariizumi
 * @date  11/03/29
 */
//======================================================================

#if !defined( __GFL_DEBUGWIN_ITEM_H__ )
#define __GFL_DEBUGWIN_ITEM_H__
#pragma once

#include <debug/gfl_DebugwinTypes.h>

#include <list>

//定義はgfl_DebugWinTypes.h にあります。
//#define GFL_DEBUGWIN_USE 1

namespace gfl
{

namespace ui
{
  class Button;
  class VectorDevice;
}

namespace debug
{

#if GFL_DEBUGWIN_USE

///項目の表示名前の最大サイズ
static const int DEBUGWIN_NAME_LEN = 64;

class DebugWin;
class DebugWinGroup;

//---------------------------------------------------------
/**
 * @class DebugWinItem
 * @brief デバッグウィンドウ項目基底クラス
 */
//---------------------------------------------------------
class DebugWinItem
{
public:
  //コンストラクタ
  DebugWinItem( DebugWinGroup *parent );
  DebugWinItem( DebugWinGroup *parent , const wchar_t *name );
  //デストラクタ
  virtual ~DebugWinItem();

  ///カーソルアクティブ時の更新
  virtual inline void UpdateFunc(DebugWin *debSys){GFL_UNUSED(debSys);}
  ///表示名取得
  virtual const wchar_t* GetDispName(DebugWin *debSys){GFL_UNUSED(debSys);return mName;}
  
  ///親グループの取得
  inline DebugWinGroup* GetParentGroup(void){return mParentGroup;}
  ///名前の設定
  void SetName( const wchar_t *name );
  void SetNameV( wchar_t *name , ... );
  ///名前の素の文字列取得(検索など用)
  inline wchar_t* GetName( void ) {return mName;}
  
  ///SEの再生
  void PlaySound( DEBUGWIN_SOUND_TYPE type );
  
  nw::ut::LinkListNode mItemNode;

protected:
  ///親の設定
  inline void SetParentGroup( DebugWinGroup *parent ){mParentGroup = parent;}

  /// デバッグウィンドウシステムから文字列処理用のワークを取得する
  wchar_t * GetSysWorkStr( void );
  /// デバッグウィンドウシステムからボタン情報を取得する
  gfl::ui::Button * GetSysUiButton();
  /// デバッグウィンドウシステムから十字キー情報を取得する
  gfl::ui::VectorDevice * GetSysUiCrossKey();

private:
  ///親グループへのポインタ
  DebugWinGroup* mParentGroup;
  ///名前文字列
  wchar_t mName[DEBUGWIN_NAME_LEN];
  
};

typedef nw::ut::LinkList<DebugWinItem,offsetof(DebugWinItem,mItemNode)> DebugWinItemList;
typedef DebugWinItemList::Iterator DebugWinItemIt;

//std::list用
//typedef ::std::list<DebugWinItem*> DebugWinItemList;
//typedef DebugWinItemList::iterator DebugWinItemIt;

//---------------------------------------------------------

//グループクラス
typedef void(*OpenGroupFunc)( void *work );

//---------------------------------------------------------
/**
 * @class DebugWinGroup
 * @brief デバッグウィンドウ項目グループクラス
 */
//---------------------------------------------------------
class DebugWinGroup : public DebugWinItem
{
public:
  //コンストラクタ
  DebugWinGroup( DebugWinGroup *parent , const wchar_t *name );
  //デストラクタ
  virtual ~DebugWinGroup();

  //カーソルアクティブ時の更新
  virtual void UpdateFunc(DebugWin *debSys);
  
  //子の追加
  void AddItem( DebugWinItem *item );
  
  //子の削除
  void RemoveItem( DebugWinItem *item );
  
  //子リストの取得
  inline DebugWinItemList* GetItemList(){return &mItemList;}
  
  //グループのワークの作成
  //グループ削除時に解放されます
  void* CreateGroupWork( const size_t size , heap::HeapBase *heap = NULL );
  
  void SetOpenGroupFunc( OpenGroupFunc func ){mOpenGroupFunc = func;}
  
  nw::ut::LinkListNode mGrpNode;

private:
  //子リスト
  DebugWinItemList mItemList;
  //ワーク
  void *mGroupWork;
  
  OpenGroupFunc mOpenGroupFunc;
};

//---------------------------------------------------------

//↓用の更新時・描画時クラス
typedef void (*DebugWinUpdateFunc)( void* userWork  , DebugWinItem *item );
typedef wchar_t* (*DebugWinDrawFunc)( void* userWork  , DebugWinItem *item );

//---------------------------------------------------------
/**
 * @class DebugWinFunc
 * @brief 更新処理独自設定用項目クラス
 */
//---------------------------------------------------------
class DebugWinFunc : public DebugWinItem
{
public:
  //コンストラクタ
  DebugWinFunc( DebugWinGroup *parent , const wchar_t *name , void *userWork , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc = NULL );
  //デストラクタ
  virtual ~DebugWinFunc(){};
  
  //更新(DebugWinUpdateFunc)
  virtual void UpdateFunc(DebugWin *debSys);
  //表示名の取得(DebugWinDrawFunc)
  virtual const wchar_t* GetDispName(DebugWin *debSys);
  
private:
  DebugWinUpdateFunc mUpdateFunc;
  DebugWinDrawFunc mDrawFunc;
  void *mUserWork;
};

#endif //GFL_DEBUGWIN_USE

}//namespace debug
}//namespace gfl




#endif  //__GFL_DEBUGWIN_ITEM_H__

