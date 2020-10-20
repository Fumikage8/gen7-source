//======================================================================
/**
 * @file  gfl2_DebugwinItem.h
 * @brief デバッグウィンドウ用アイテム(項目)
 * @author  ariizumi
 * @date  11/03/29
 */
//======================================================================

#if !defined( __GFL_DEBUGWIN_ITEM_H__ )
#define __GFL_DEBUGWIN_ITEM_H__
#pragma once

#include <util/include/gfl2_List.h>
#include <debug/include/gfl2_DebugwinTypes.h>


//定義はgfl2_DebugWinTypes.h にあります。
//#define GFL_DEBUGWIN_USE 1

GFL_NAMESPACE_BEGIN(gfl2)

GFL_NAMESPACE_BEGIN(ui)
  class Button;
  class VectorDevice;
GFL_NAMESPACE_END(ui)

GFL_NAMESPACE_BEGIN(debug)

#if GFL_DEBUGWIN_USE

///項目の表示名前の最大サイズ
static const int DEBUGWIN_NAME_LEN = 128;
static const int DEBUGWIN_GROUP_ITEM_NUM = 32;
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
  DebugWinItem( DebugWinGroup *parent , const wchar_t *name, const s32 uScrId = -1 );
  //デストラクタ
  virtual ~DebugWinItem();

  ///カーソルアクティブ時の更新
  virtual inline void UpdateFunc(DebugWin *debSys){GFL_UNUSED(debSys);}

  ///カーソルがタッチされたさいの更新
  virtual inline void UpdateTouchFunc(DebugWin *debSys){GFL_UNUSED(debSys);}

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
  void PlaySystemSound( DEBUGWIN_SOUND_TYPE type );

  
  //数値更新時に呼ばれる関数の型(DebugWinItemValueで有効)
  //DebugWinItemで共通して受け取れるようにこっちに移動
  typedef void(*ChangedValueFunc)( void *work );
  ///変更通知を受け取るコールバックの登録
  inline void SetChangedValueFunc( ChangedValueFunc func , void *work )
  {
    mFunc = func;
    mWork = work;
  }

  /// デバッグウィンドウシステムから文字列処理用のワークを取得する
  wchar_t * GetSysWorkStr( void );
  /// デバッグウィンドウシステムからボタン情報を取得する
  gfl2::ui::Button * GetSysUiButton();
  /// デバッグウィンドウシステムから十字キー情報を取得する
  gfl2::ui::VectorDevice * GetSysUiCrossKey();

  void  SetScrId( const s32 _uScrId ){  m_iScrId = _uScrId; }
  s32 GetScrId() const {  return m_iScrId;  }

  void SetIsSelectSkip(bool flg){mIsSelectSkip = flg;}
  bool IsSelectSkip(void)const{return mIsSelectSkip;}

protected:
  ///親の設定
  inline void SetParentGroup( DebugWinGroup *parent ){mParentGroup = parent;}

  ChangedValueFunc mFunc; ///<変更通知を受け取るコールバック関数
  void *mWork;            ///<変更通知を受け取るコールバック関数のワーク
  //変更を通知する
  virtual void ChangedValue(void);

private:
  ///親グループへのポインタ
  DebugWinGroup* mParentGroup;
  ///名前文字列
  wchar_t mName[DEBUGWIN_NAME_LEN];
  s32     m_iScrId;
  
  //選択時スキップするか
  bool mIsSelectSkip;
};
//#define offsetof(t, memb) ((__CLIBNS size_t)__INTADDR__(&(((t *)0)->memb)))
//__CLIBNS(stdの名前空間がgfl2になる) と __INTADDR__(ARM系の定義。未定義になる)がおかしい！
//@attention offsetof になおしたい。
#define offsetof_(t, memb) ((size_t)(&(((t *)0)->memb)))

typedef gfl2::util::List<DebugWinItem*> DebugWinItemList;
typedef DebugWinItemList::Iterator DebugWinItemIt;

//std::list用
//typedef ::std::list<DebugWinItem*> DebugWinItemList;
//typedef DebugWinItemList::iterator DebugWinItemIt;

//---------------------------------------------------------

//グループクラス
typedef void(*OpenGroupFunc)( void *work );
typedef void(*CreateGroupFunc)( void *work, DebugWinItem *item );
typedef void(*DestroyGroupFunc)( void *work, DebugWinItem *item );
typedef void(*WindowOpenGroupFunc)( void *work, DebugWinItem *item );
typedef void(*WindowCloseGroupFunc)( void *work, DebugWinItem *item );

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
  DebugWinGroup( DebugWinGroup *parent , const wchar_t *name , int itemNum , gfl2::heap::HeapBase *heap);
  //デストラクタ
  virtual ~DebugWinGroup();

  //カーソルアクティブ時の更新
  virtual void UpdateFunc(DebugWin *debSys);

  //カーソルがタッチされたさいの更新
  virtual void UpdateTouchFunc(DebugWin *debSys);

  
  //子の追加
  void AddItem( DebugWinItem *item );
  
  //子の削除
  void RemoveItem( DebugWinItem *item );

  //子の空き数を取得
  s32 GetItemFreeSize( void ) const;
  
  //子リストの取得
  inline DebugWinItemList* GetItemList(){return &mItemList;}
  
  //グループのワークの作成
  //グループ削除時に解放されます
  void* CreateGroupWork( const size_t size , heap::HeapBase *heap = NULL );
  
  void SetOpenGroupFunc( OpenGroupFunc func ){mOpenGroupFunc = func;}

  void SetCreateDestroyGroupFunc(void *userWork, CreateGroupFunc createFunc, DestroyGroupFunc destroyFunc, CreateGroupFunc createAfterFunc = NULL ){ muserWork = userWork; mCreateGroupFunc = createFunc; mDestroyGroupFunc = destroyFunc; mCreateGroupAfterFunc = createAfterFunc; }
  void CallCreateGroupFunc( void );
  void CallCreateGroupAfterFunc( void );
  void CallDestroyGroupFunc( void );
  void SetWindowOpenCloseGroupFunc( void *userWork, WindowOpenGroupFunc openFunc, WindowCloseGroupFunc closeFunc ){ mWindowUserWork = userWork; mWindowOpenGroupFunc= openFunc; mWindowCloseGroupFunc = closeFunc; }
  void CallWindowOpenGroupFunc( void );
  void CallWindowCloseGroupFunc( void );
  void ResizeItem( int itemNum , gfl2::heap::HeapBase *heap );    // @note 中でRemoveAllItemを行います
  void RemoveAllItem( void );

  nw::ut::LinkListNode mGrpNode;

private:
  //子リスト
  DebugWinItemList mItemList;
  //ワーク
  void *mGroupWork;
  
  OpenGroupFunc mOpenGroupFunc;

  CreateGroupFunc mCreateGroupFunc;
  CreateGroupFunc mCreateGroupAfterFunc;
  DestroyGroupFunc mDestroyGroupFunc;
  WindowOpenGroupFunc mWindowOpenGroupFunc;
  WindowCloseGroupFunc mWindowCloseGroupFunc;


  void *muserWork;
  void *mWindowUserWork;
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
  DebugWinFunc( DebugWinGroup *parent , const wchar_t *name , void *userWork , DebugWinUpdateFunc updateFunc , DebugWinDrawFunc drawFunc = NULL, const s32 iScrId = -1 );
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

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

#endif  //__GFL_DEBUGWIN_ITEM_H__

