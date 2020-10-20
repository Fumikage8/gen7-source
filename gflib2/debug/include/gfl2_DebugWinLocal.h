//======================================================================
/**
 * @file  gfl2_DebugwinLocal.h
 * @brief デバッグウィンドウ
 * @author  ariizumi
 * @date  11/03/29
 */
//======================================================================
#if !defined __GFL_DEBUGWIN_LOCAL_H__
#define __GFL_DEBUGWIN_LOCAL_H__
#pragma once
#include <nw/ut/ut_LinkList.h>

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_heap_manager.h>
#include <math/include/gfl2_Vec.h>
#include <fs/include/gfl2_FsArcFile.h>
#include <ro/include/gfl2_RoManager.h>

#include <debug/include/gfl2_DebugWinItem.h>
#include <debug/include/gfl2_DebugWinSystem.h>

//インクルード順の関係でgfl2_DebugWinItem.hで宣言
//#define GFL_DEBUGWIN_USE 1



GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)
class DrawUtilText;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

//クラス定義

GFL_NAMESPACE_BEGIN(ui)
  class DeviceManager;
  class Button;
  class VectorDevice;
GFL_NAMESPACE_END(ui)

GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
  class MsgData;
GFL_NAMESPACE_END(ui)
  
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
  class DebugWinItem;
  class DebugWinGroup;

#if GFL_DEBUGWIN_USE
  


  //-----------------------------------------------------------------------------
  /**
   * @class DebugWin
   * @brief デバッグウィンドウシステム
   */
  //-----------------------------------------------------------------------------
  class DebugWin :public gfl2::base::Singleton<DebugWin>
  {
    GFL_FORBID_COPY_AND_ASSIGN(DebugWin);

  public:
    //static関数郡
    //----------------------------------------------------------------------------
    /**
     *  @brief  更新
     */
    //-----------------------------------------------------------------------------
    void Update();

    //----------------------------------------------------------------------------
    /**
     *  @brief  描画
     *
     *  @param  gfl::grp::GraphicsSystem *grpSys グラフィックシステム
     *  @param  gfl::grp::DisplayType dispType   表示面
     *                                           複数面に出すときは複数回呼んで下さい
     */
    //-----------------------------------------------------------------------------
    void Draw( gfl2::gfx::CtrDisplayNo dispNo , gfl2::gfx::DisplayDeviceHandle handle );

    //----------------------------------------------------------------------------
    /**
     *  @brief  項目の追加
     *          基本的には用意されたセット関数を使う。
     *
     *  @param  DebugWinItem *item    追加する項目
     *  @param  DebugWinGroup *parent 親グループ(NULLならトップ)
     */
    //-----------------------------------------------------------------------------    
    void AddItem( DebugWinItem *item , DebugWinGroup *parent = NULL );

    //----------------------------------------------------------------------------
    /**
     *  @brief  グループの追加
     *          基本的には用意されたセット関数を使う。
     *
     *  @param  const wchar_t *name   追加する項目名
     *  @param  DebugWinGroup *parent 親グループ(NULLならトップ)
     *
     *  @return DebugWinGroup*  新しく作成されたグループ
     */
    //-----------------------------------------------------------------------------
    DebugWinGroup* AddGroup( heap::HeapBase *heap , const wchar_t *name , DebugWinGroup *parent = NULL , int itemNum = DEBUGWIN_GROUP_ITEM_NUM );
    DebugWinGroup* AddGroup( heap::HeapBase *heap , str::MsgData *msgData , u32 strId , DebugWinGroup *parent = NULL , int itemNum = DEBUGWIN_GROUP_ITEM_NUM );

    //----------------------------------------------------------------------------
    /**
     *  @brief  グループの削除
     *          子のグループ・項目もすべて削除されます。
     *
     *  @param  DebugWinGroup *group  グループのポインタ
     */
    //-----------------------------------------------------------------------------
    void RemoveGroup( DebugWinGroup *group );

    //----------------------------------------------------------------------------
    /**
     *  @brief  グループの削除
     *          子のグループ・項目もすべて削除されます。
     *
     *  @param  const wchar_t *name  グループの名前
     */
    //-----------------------------------------------------------------------------
    void RemoveGroup( const wchar_t *name );

    //----------------------------------------------------------------------------
    /**
     *  @brief  名前からグループの取得
     *
     *  @param  const wchar_t *name グループ名
     *
     *  @return DebugWinGroup* グループ(名前が一致しなかった場合トップグループが返ってくる)
     */
    //-----------------------------------------------------------------------------
    DebugWinGroup* GetGroupByName( const wchar_t *name );

    //----------------------------------------------------------------------------
    /**
     *  @brief  Topグループの取得
     *
     *  @return DebugWinGroup* グループ
     */
    //-----------------------------------------------------------------------------
    DebugWinGroup* GetTopGroup(void);

    //----------------------------------------------------------------------------
    /**
     *  @brief  グループの開閉チェック
     *
     *  @return bool グループが開いていればtrue
     */
    //-----------------------------------------------------------------------------
    bool IsOpen(void);

    //----------------------------------------------------------------------------
    /**
     *  @brief  DebugWinを閉じたフレームのみ取得
     *
     *  @return bool グループが開いていればtrue
     */
    //-----------------------------------------------------------------------------
    bool IsCloseTrigger(void);

    //----------------------------------------------------------------------------
    /**
     *  @brief  強制グループ閉じ
     */
    //-----------------------------------------------------------------------------
    void CloseWindow(void);

    //----------------------------------------------------------------------------
    /**
    *  @brief  操作ロック禁止フラグを設定
    */
    //-----------------------------------------------------------------------------
    void SetLockBanFlag( f32 flag );

    //----------------------------------------------------------------------------
    /**
    *  @brief  強制停止フラグを設定
    */
    //-----------------------------------------------------------------------------
    void SetFoceStopFlag( f32 flag ){ mIsFoceStop = flag; } 

    //----------------------------------------------------------------------------
    /**
    *  @brief  グループの操作ロックのチェック
    *
    *  @return bool ロックされていればtrue
    */
    //-----------------------------------------------------------------------------
    bool IsLock(void) const;

    //----------------------------------------------------------------------------
    /**
     *  @brief  効果音再生(コールバックを通じて各アプリに届きます。)
     */
    //-----------------------------------------------------------------------------
    void PlaySystemSound( const DEBUGWIN_SOUND_TYPE type );

    //----------------------------------------------------------------------------
    /**
     *  @brief  グループのリストから削除(内部用)
     *
     *  @param  DebugWinGroup *group  グループ
     */
    //-----------------------------------------------------------------------------
    void RemoveGroupList( DebugWinGroup *group );

    //----------------------------------------------------------------------------
    /**
     *  @brief  項目の削除がらみの処理
     *          実際の削除はしない(内部用)
     *
     *  @param  DebugWinItem *item  項目
     */
    //-----------------------------------------------------------------------------
    void RemoveItemFunc( DebugWinItem *item );

    //----------------------------------------------------------------------------
    /**
     *  @brief  デバッグウィンドウ用サウンド再生
     *
     *  @param  const DEBUGWIN_SOUND_TYPE サウンド種類
     */
    //-----------------------------------------------------------------------------
    void SoundCallBack( const DEBUGWIN_SOUND_TYPE type );

  public:
    //----------------------------------------------------------------------------
    /**
     *  @brief  コンストラクタ
     *
     *  @param  heap::HeapBase *heap  使用ヒープ
     */
    //-----------------------------------------------------------------------------
    DebugWin( heap::HeapBase *heap , gfl2::heap::HeapBase * devHeap , gfl2::gfx::IGLAllocator* allocator , gfl2::str::Font *font , int groupNum );

    //----------------------------------------------------------------------------
    /**
     *  @brief  デストラクタ
     */
    //-----------------------------------------------------------------------------
    virtual ~DebugWin();

    //----------------------------------------------------------------------------
    /**
    *  @brief  タッチ更新
    */
    //-----------------------------------------------------------------------------
    void InputUpdateTouch();
    
    //----------------------------------------------------------------------------
    /**
    *  @brief  キー入力更新
    */
    //-----------------------------------------------------------------------------
    void InputUpdateKeyborad();

    //----------------------------------------------------------------------------
    /**
     *  @brief  デバイスマネージャーの設定
     *
     *  @param  gfl::ui::DeviceManager *devMng  デバイスマネージャー
     *  @param  gfl::ui::Button *btn            操作に使用するボタン
     *  @param  gfl::ui::VectorDevice *key      操作に使用するキー
     */
    //-----------------------------------------------------------------------------
    void SetDeviceManager( gfl2::ui::DeviceManager *devMng , gfl2::ui::Button *btn = NULL , gfl2::ui::VectorDevice *key = NULL );

    //----------------------------------------------------------------------------
    /**
     *  @brief  コールバック受け取りクラスの設定
     *
     *  @param  gfl::debug::DebugWinSystemCallBack *cbClass      コールバックを受け取るクラス
     */
    //-----------------------------------------------------------------------------
    void SetCallbackClass( gfl2::debug::DebugWinSystemCallBack *cbClass );

    //----------------------------------------------------------------------------
    /**
     *  @brief  現在のカーソル位置の変更
     *
     *  @param  gfl::debug::DebugWinItem *item   現在地にしたいアイテム
     */
    //-----------------------------------------------------------------------------
    void SetCurrentItem( DebugWinItem *selItem );
    void SetCurrentItemByIndex( s32 index );

    //----------------------------------------------------------------------------
    /**
    *  @brief  現在のカーソル位置を更新(先頭にする)
    *  @note   開いているグループに項目を追加したときの対処
    */
    //-----------------------------------------------------------------------------
    void UpdateCurrent( void );

    //---------------------------------------------------
    //以下内部用
    //表示グループ設定
    void SetCurrentGroup( DebugWinGroup *group , DebugWinGroup *selGroup );


		gfl2::util::DrawUtilText* GetDrawUtilText(void){ return mpDrawUtilText; }

    gfl2::util::UtilTextRenderingPipeLine*	GetRenderingPipeLine(void){ return mRenderingPipeLine; }

    //各種操作デバイスの取得
    inline gfl2::ui::DeviceManager* GetDeviceManager(void)const{return mDeviceManager;}
    gfl2::ui::Button* GetUiButton(void)const;
    gfl2::ui::VectorDevice* GetUiCrossKey(void)const;
    
    //取得系
    inline gfl2::heap::HeapBase* GetHeap(void)const{return mHeap;}
    inline wchar_t* GetWorkStr(void){return mWorkStr;}
    inline str::StrBuf *GetWorkStrBuf(void){return mWorkStrbuf;}
    //取得系(DebugWinで操作できるようにポインタで返す)
    inline gfl2::math::Vector4* GetItemColor(void){return &mItemCol;}
    inline gfl2::math::Vector4* GetShadowColor(void){return &mShadowCol;}
    inline gfl2::math::Vector4* GetCurrentColor(void){return &mCurrentCol;}
    inline gfl2::math::Vector4* GetFillColor(void){return &mFillCol;}
    inline f32* GetFontScale(void){return &mFontScale;}
    inline f32* GetLineHeight(void){return &mLineHeight;}
    inline int* GetHoldKey(void){return &mHoldKey;}
    inline int* GetTriggerKey(void){return &mTrgKey;}

    // 表示位置関連
    void ItemViewBufferCalTouchiSelectAfter( void );
    void ItemViewBufferRecover( void );
    void ItemViewBufferClear( void );

  private:
    //#define offsetof(t, memb) ((__CLIBNS size_t)__INTADDR__(&(((t *)0)->memb)))
    //__CLIBNS(stdの名前空間がgfl2になる) と __INTADDR__(ARM系の定義。未定義になる)がおかしい！
    //@attention offsetof になおしたい。
    #define offsetof_(t, memb) ((size_t)(&(((t *)0)->memb)))

    typedef gfl2::util::List<DebugWinGroup*> DebugWinGroupList;
    typedef DebugWinGroupList::Iterator DebugWinGroupIt;
    
    //typedef ::std::list<DebugWinGroup*> DebugWinGroupList;
    //typedef DebugWinGroupList::iterator DebugWinGroupIt;

    gfl2::heap::HeapBase *mHeap;
    gfl2::heap::HeapBase *mDevHeap;
	  gfl2::heap::NwAllocator*		mHeapAllocator;
	  gfl2::heap::NwAllocator*		mDevAllocator;

  	gfl2::util::UtilTextRenderingPipeLine*		mRenderingPipeLine;

    gfl2::ui::DeviceManager *mDeviceManager;
    gfl2::ui::Button *mUiButton;
    gfl2::ui::VectorDevice *mUiCrossKey;
    
    int mHoldKey;
    int mTrgKey;
    int mLockTrgKey;
    
    bool mIsOpen;
    bool mIsCloseTrigger;
    bool mIsBanLock;
    bool mIsLock;
    bool mIsFoceStop;
    
    DebugWinGroup *mTopGroup;
    DebugWinGroup *mCurrentGroup;
    DebugWinGroupList mGroupList;
    
    DebugWinItemIt mCurrentItem;
    int mCurrentIdx;

    gfl2::math::Vector4 mItemCol;
    gfl2::math::Vector4 mShadowCol;
    gfl2::math::Vector4 mCurrentCol;
    gfl2::math::Vector4 mFillCol;
    
    f32 mFontScale;
    f32 mLineHeight;
    f32 mScrollOfs;

    wchar_t mWorkStr[DEBUGWIN_NAME_LEN];
    str::StrBuf *mWorkStrbuf;
    
    DebugWinSystemCallBack *mCallbackClass;

		gfl2::util::DrawUtilText* mpDrawUtilText;

    int  m_ItemViewBufferUpper;
    int  m_ItemViewBufferBottom;

  };


#endif //GFL_DEBUGWIN_USE
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
#if GFL_DEBUGWIN_USE
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DECLARATION( gfl2::debug::DebugWin );
#endif
#endif //GFL_DEBUGWIN_USE

#endif  //__GFL_DEBUGWIN_LOCAL_H__
