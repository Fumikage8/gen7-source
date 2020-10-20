//======================================================================
/**
 * @file  gfl2_DebugwinSystem.h
 * @brief デバッグウィンドウ
 * @author  ariizumi
 * @data  11/03/29
 * 
 * 
  DebugWinについて
  
  DebugWinにはグループ、項目を登録し、ツリー状にデバッグメニューを
  作成することができます。
  グループ、項目の作成は親グループを指定し、その下に作成されます。
  削除はグループ単位でのみ行え、グループの子のグループ、項目は
  自動的に削除されます。
  項目は基本的に更新関数を渡し、カーソルが"あっている間"更新関数が
  呼ばれ続けます。
  この関数の中で、「ボタンが押された時値を変更する」などの処理を
  実行させます。
  使いやすくする為、整数・小数・VEC3の値変更の項目の追加や、
  g3d::Cameta・g3d::Modelの項目変更の追加を一発で行えるような
  関数を用意しています。
  
  動かす為には、UpdateとDrawの関数を呼んでもらう必要があります。
  
  使う為には簡易描画の初期化と、簡易描画へのフォントの登録が必要です。

 * 
 * 
 */
//======================================================================
#ifndef __GFL_DEBUGWIN_SYSTEM_H__
#define __GFL_DEBUGWIN_SYSTEM_H__
#pragma once

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_heap_manager.h>
#include <math/include/gfl2_Vec.h>
#include <fs/include/gfl2_FsArcFile.h>
#include <str/include/gfl2_Str.h>

#include <util/include/gfl2_DrawUtilText.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

#include <debug/include/gfl2_DebugWinItem.h>

//定義はgfl2_DebugWinTypes.h にあります。
//#define GFL_DEBUGWIN_USE 1

GFL_NAMESPACE_BEGIN(gfl2)
  //クラス定義
GFL_NAMESPACE_BEGIN(grp)
class GraphicsSystem;
GFL_NAMESPACE_END(grp)

GFL_NAMESPACE_BEGIN(ui)
  class DeviceManager;
  class Button;
  class VectorDevice;
GFL_NAMESPACE_END(ui)
  
GFL_NAMESPACE_BEGIN(debug)
class DebugWinItem;
class DebugWinGroup;

#if GFL_DEBUGWIN_USE

class DebugWinSystemCallBack
{
public:
  //----------------------------------------------------------------------------
  /**
    *  @brief  デバッグウィンドウ用サウンド再生
    *          gflibにサウンドを持たせるわけにもいかないので、アプリにお任せ。
    *
    *  @param  const DEBUGWIN_SOUND_TYPE サウンド種類
    */
  //-----------------------------------------------------------------------------
  virtual void SoundCallBack( const DEBUGWIN_SOUND_TYPE type ){};
};
  
//----------------------------------------------------------------------------
/**
  * @brief デバッグウィンドウの初期化処理
  */
//----------------------------------------------------------------------------
extern void DebugWin_Initialize( gfl2::heap::HeapBase * heap , gfl2::heap::HeapBase * devHeap ,  gfl2::gfx::IGLAllocator* allocator , gfl2::str::Font *font , int groupNum );

//----------------------------------------------------------------------------
/**
  * @brief デバッグウィンドウの終了処理
  */
//----------------------------------------------------------------------------
extern void DebugWin_Finalize( void );

//----------------------------------------------------------------------------
/**
  *  @brief  更新
  */
//-----------------------------------------------------------------------------
extern void DebugWin_Update(void);
//----------------------------------------------------------------------------
/**
  *  @brief  描画
  *
  *  @param  gfl::grp::GraphicsSystem *grpSys グラフィックシステム
  *  @param  gfl::grp::DisplayType dispType   表示面
  *                                           複数面に出すときは複数回呼んで下さい
  */
//-----------------------------------------------------------------------------
extern void DebugWin_Draw( gfl2::gfx::CtrDisplayNo dispNo , gfl2::gfx::DisplayDeviceHandle handle );
//----------------------------------------------------------------------------
/**
  *  @brief  グループの開閉チェック
  *
  *  @return bool グループが開いていればtrue
  */
//-----------------------------------------------------------------------------
extern bool DebugWin_IsOpen(void);

//----------------------------------------------------------------------------
/**
  *  @brief  DebugWinを閉じたフレームのみ取得
  *
  *  @return bool グループが開いていればtrue
  */
//-----------------------------------------------------------------------------
bool DebugWin_IsCloseTrigger(void);

//----------------------------------------------------------------------------
/**
  *  @brief  強制グループ閉じ
  */
//-----------------------------------------------------------------------------
extern void DebugWin_CloseWindow(void);

//----------------------------------------------------------------------------
/**
  *  @brief  グループ追加
  *
  *  @param  const wchar_t *name   表示名
  *  @param  DebugWinGroup *parent 親グループ(NULLならTopグループ)
  *
  *  @return DebugWinGroup* 戻り値を使って子を追加していってください。
  */
//-----------------------------------------------------------------------------
extern DebugWinGroup* DebugWin_AddGroup( heap::HeapBase *heap , const wchar_t *name , DebugWinGroup *parent = NULL , int itemNum = DEBUGWIN_GROUP_ITEM_NUM );
extern DebugWinGroup* DebugWin_AddGroup( heap::HeapBase *heap , str::MsgData *msgData , u32 strId , DebugWinGroup *parent = NULL , int itemNum = DEBUGWIN_GROUP_ITEM_NUM );
//----------------------------------------------------------------------------
/**
  *  @brief  グループ削除
  *
  *  @param  DebugWinGroup *group 削除するグループ
  */
//-----------------------------------------------------------------------------
extern void DebugWin_RemoveGroup( DebugWinGroup *parent );
//----------------------------------------------------------------------------
/**
  *  @brief  名前からグループ削除
  *
  *  @param  const wchar_t *name  削除するグループの名前
  */
//-----------------------------------------------------------------------------
extern void DebugWin_RemoveGroupByName( const wchar_t *name );


//----------------------------------------------------------------------------
/**
  *  @brief  デバイスマネージャーの設定
  *
  *  @param  gfl::ui::DeviceManager *devMng  デバイスマネージャー
  *  @param  gfl::ui::Button *btn            操作に使用するボタン
  *  @param  gfl::ui::VectorDevice *key      操作に使用するキー
  */
//-----------------------------------------------------------------------------
extern void DebugWin_SetDeviceManager( gfl2::ui::DeviceManager *devMng , gfl2::ui::Button *btn = NULL , gfl2::ui::VectorDevice *key = NULL );

//----------------------------------------------------------------------------
/**
  *  @brief  コールバック受け取りクラスの設定
  *
  *  @param  gfl::debug::DebugWinSystemCallBack *cbClass      コールバックを受け取るクラス
  */
//-----------------------------------------------------------------------------
extern void DebugWin_SetCallbackClass( gfl2::debug::DebugWinSystemCallBack *cbClass );


//-----------------------------------------------------------------------------
/**
  * @brief
  * @return
  */
//-----------------------------------------------------------------------------
extern wchar_t * DebugWin_GetWorkStr( void );

//-----------------------------------------------------------------------------
/**
  * @brief
  * @return
  */
//-----------------------------------------------------------------------------
extern str::StrBuf * DebugWin_GetWorkStrBuf(void);

//-----------------------------------------------------------------------------
/**
  * @brief  名前からGroupを取得する
  * @param name  指定する名前
  * @return Groupへのポインタ
  */
//-----------------------------------------------------------------------------
extern DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name );

//-----------------------------------------------------------------------------
/**
  * @brief  TopGroupを取得する
  * @return Groupへのポインタ
  */
//-----------------------------------------------------------------------------
extern DebugWinGroup* DebugWin_GetTopGroup(void);

//----------------------------------------------------------------------------
/**
  *  @brief  現在のカーソル位置の変更
  *
  *  @param  gfl::debug::DebugWinItem *item   現在位置にしたいアイテム
  *         別のグループに入っていた場合などは何も処理しません。
  */
//-----------------------------------------------------------------------------
extern void DebugWin_SetCurrentItem( DebugWinItem *selItem );
extern void DebugWin_SetCurrentItemByIndex( s32 index );

//----------------------------------------------------------------------------
/**
  *  @brief  フォントのセットしなおし
  *
  */
//-----------------------------------------------------------------------------
extern void DebugWin_ReSetFont(gfl2::str::Font *font);


//以下はシステム的な操作を行うためのアクセス関数

///非選択位置文字色指定ワークのポインタ取得
extern gfl2::math::Vector4* DebugWin_GetItemColor(void);
///影色指定ワークのポインタ取得
extern gfl2::math::Vector4* DebugWin_GetShadowColor(void);
///選択位置文字色指定ワークのポインタ取得
extern gfl2::math::Vector4* DebugWin_GetCurrentColor(void);
///塗潰し色指定ワークのポインタ取得
extern gfl2::math::Vector4* DebugWin_GetFillColor(void);
///フォントスケール指定ワークのポインタ取得
extern f32* DebugWin_GetFontScale(void);
///行高さ指定ワークのポインタ取得
extern f32* DebugWin_GetLineHeight(void);
///起動用ホールドキー指定ワークのポインタ取得
extern int* DebugWin_GetHoldKey(void);
///起動用トリガーキー指定ワークのポインタ取得
extern int* DebugWin_GetTriggerKey(void);
///保持しているUIボタン情報へのポインタを取得
extern gfl2::ui::Button* DebugWin_GetUiButton(void);

  

#else  //GFL_DEBUGWIN_USE

/*
#define DebugWin_Update() ((void)0)
#define DebugWin_Draw(grpSys,dispType) ((void)0)
#define DebugWin_IsOpen() (false)
#define DebugWin_AddGroup(name,parent) (NULL)
#define DebugWin_RemoveGroup(parent) ((void)0)
#define DebugWin_RemoveGroupByName(name) ((void)0)
*/

extern void DebugWin_Initialize(...);
extern void DebugWin_Finalize(void);
extern void DebugWin_Update(void);
extern void DebugWin_Draw(...);
extern bool DebugWin_IsOpen(void);
extern bool DebugWin_IsCloseTrigger(void);
extern void DebugWin_CloseWindow(void);
extern DebugWinGroup* DebugWin_AddGroup(...);
extern DebugWinGroup* DebugWin_AddGroupMsg(...);
extern void DebugWin_RemoveGroup(...);
extern void DebugWin_RemoveGroupByName(...);
extern void DebugWin_SetCurrentItem( ... );

#endif //GFL_DEBUGWIN_USE

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

#endif  //__GFL_DEBUGWIN_SYSTEM_H__
