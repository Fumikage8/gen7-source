//======================================================================
/**
 * @file  gfl_DebugwinSystem.h
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

#include <base/gfl_Singleton.h>
#include <heap/gfl_HeapManager.h>
#include <grp/gfl_RenderTarget.h>
#include <math/gfl_Vec.h>
#include <fs/gfl_ArcFile.h>

#include <debug/gfl_DebugWinItem.h>

//定義はgfl_DebugWinTypes.h にあります。
//#define GFL_DEBUGWIN_USE 1

namespace gfl
{
  //クラス定義
namespace grp
{
  class GraphicsSystem;
}
namespace ui
{
  class DeviceManager;
  class Button;
  class VectorDevice;
}

namespace str
{
  class StrBuf;
  class MsgData;
}
  
namespace debug
{
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
  extern void DebugWin_Initialize( gfl::heap::HeapBase * heap);

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
  extern void DebugWin_Draw( gfl::grp::GraphicsSystem *grpSys , gfl::grp::DisplayType dispType );
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
  extern DebugWinGroup* DebugWin_AddGroup( const wchar_t *name , DebugWinGroup *parent = NULL , heap::HeapBase *heap = NULL );
  extern DebugWinGroup* DebugWin_AddGroup( str::MsgData *msgData , u32 strId , DebugWinGroup *parent = NULL , heap::HeapBase *heap = NULL );
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
  extern void DebugWin_SetDeviceManager( gfl::ui::DeviceManager *devMng , gfl::ui::Button *btn = NULL , gfl::ui::VectorDevice *key = NULL );

  //----------------------------------------------------------------------------
  /**
   *  @brief  コールバック受け取りクラスの設定
   *
   *  @param  gfl::debug::DebugWinSystemCallBack *cbClass      コールバックを受け取るクラス
   */
  //-----------------------------------------------------------------------------
  extern void DebugWin_SetCallbackClass( gfl::debug::DebugWinSystemCallBack *cbClass );


  //-----------------------------------------------------------------------------
  /**
   * @brief
   * @return
   */
  //-----------------------------------------------------------------------------
  extern wchar_t * DebugWin_GetWorkStr( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief  名前からGroupを取得する
   * @param name  指定する名前
   * @return Groupへのポインタ
   */
  //-----------------------------------------------------------------------------
  extern DebugWinGroup* DebugWin_GetGroupByName( const wchar_t *name );


  //以下はシステム的な操作を行うためのアクセス関数

  ///非選択位置文字色指定ワークのポインタ取得
  extern gfl::math::VEC4* DebugWin_GetItemColor(void);
  ///影色指定ワークのポインタ取得
  extern gfl::math::VEC4* DebugWin_GetShadowColor(void);
  ///選択位置文字色指定ワークのポインタ取得
  extern gfl::math::VEC4* DebugWin_GetCurrentColor(void);
  ///塗潰し色指定ワークのポインタ取得
  extern gfl::math::VEC4* DebugWin_GetFillColor(void);
  ///フォントスケール指定ワークのポインタ取得
  extern f32* DebugWin_GetFontScale(void);
  ///行高さ指定ワークのポインタ取得
  extern f32* DebugWin_GetLineHeight(void);
  ///起動用ホールドキー指定ワークのポインタ取得
  extern int* DebugWin_GetHoldKey(void);
  ///起動用トリガーキー指定ワークのポインタ取得
  extern int* DebugWin_GetTriggerKey(void);
  ///保持しているUIボタン情報へのポインタを取得
  extern gfl::ui::Button* DebugWin_GetUiButton(void);

  

#else  //GFL_DEBUGWIN_USE

/*
  #define DebugWin_Update() ((void)0)
  #define DebugWin_Draw(grpSys,dispType) ((void)0)
  #define DebugWin_IsOpen() (false)
  #define DebugWin_AddGroup(name,parent) (NULL)
  #define DebugWin_RemoveGroup(parent) ((void)0)
  #define DebugWin_RemoveGroupByName(name) ((void)0)
*/

  extern void DebugWin_Update(void);
  extern void DebugWin_Draw(...);
  extern bool DebugWin_IsOpen(void);
  extern bool DebugWin_IsCloseTrigger(void);
  extern void DebugWin_CloseWindow(void);
  extern DebugWinGroup* DebugWin_AddGroup(...);
  extern DebugWinGroup* DebugWin_AddGroupMsg(...);
  extern void DebugWin_RemoveGroup(...);
  extern void DebugWin_RemoveGroupByName(...);
#endif //GFL_DEBUGWIN_USE

}//namespace debug
}//namespace gfl




#endif  //__GFL_DEBUGWIN_SYSTEM_H__
