#ifndef __RESORT_FRAME_H__
#define __RESORT_FRAME_H__
//=============================================================================
/**
 * @file   ResortFrame.h
 * @brief   アイランドフレーム
 * @author k.ohno
 * @date   2014.1.16
 */
//=============================================================================
#pragma once

#include "macro/include/gfl2_Macros.h"
#include "heap/include/gfl2_Heap.h"
#include "fs/include/gfl2_FsAsyncFileManager.h"

#include "AppLib/include/Frame/AppFrameCellSuper.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "ResortDrawListener.h"

#include "App/Resort/include/ResortAppParam.h"
#include "AppLib/include/Util/AppRenderingManager.h"

#include "ResortSequenceListener.h"
#include "ResortChangeIslandListener.h"
#include "ResortTouchToFlowListener.h"
#include "ResortFlowListener.h"

//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortInitLogic;
class ResortDraw;
class ResortFlow;
class UpperBGDraw;
class UpperDraw;
class UpperHomeMenuDraw;
class LowerBeansSelectDraw;
class LowerUIDraw;
class LowerSelectDraw;
class LowerFarmDraw;
class LowerFarmListDraw;
class LowerAdventureResultDraw;
class LowerGimDraw;
class LowerCourseSelectDraw;
class LowerKinomiGetEffectDraw;
class TalkMessage;
class ResortCharaModel;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( lyt )
class LayoutRenderPath;
GFL_NAMESPACE_END( lyt )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )

class WordSetLoader;

GFL_NAMESPACE_BEGIN(Savedata)
class ResortSave;
class BoxPokemon;
class MyItem;
class ZukanData;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   フレーム
//------------------------------------------------------------------------------
class ResortFrame
  : public applib::frame::CellSuper
  , public ResortDrawListener
  , public ResortTouchToFlowListener
  , public ResortFlowListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortFrame);

protected:
  int m_subseq;                ///< プロセス関数の動作シーケンス
  bool mbEnd;  //Endフラグ

public:
  //------------------------------------------------------------------
  /**
   * @brief    コンストラクタ
   */
  //------------------------------------------------------------------
  ResortFrame(void);

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~ResortFrame();



public:


  //------------------------------------------------------------------
  /**
   * @brief    動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( APP_PARAM* pAppParam, gfl2::heap::HeapBase* rootHeap );


  //------------------------------------------------------------------
  /**
   * @brief    PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc(void);
  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::gfx::CtrDisplayNo no);
  //------------------------------------------------------------------
  /**
   * @brief    PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc(void);


public:    //!< ResortDrawListener
  //--------------------------------------------------------------------------
  /**
   * @brief   ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnButtonAction( int button_no );
  //--------------------------------------------------------------------------
  /**
   * @brief   キー操作通知
   * @param    dir  キー方向
   * @retval  true = 操作実行
   * @retval  false = 操作失敗
   */
  //--------------------------------------------------------------------------
  virtual bool OnKeyAction( u32 dir );

  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの終了通知
   */
  //--------------------------------------------------------------------------
  virtual void OnEndResort( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの変更
   * @param   dir   移動方向(0: 左、1: 右)
   */
  //--------------------------------------------------------------------------
  virtual bool FeedResort( u8 dir );

  //--------------------------------------------------------------------------
  /**
   * @brief   管理人メニューを開く
   */
  //--------------------------------------------------------------------------
  virtual void OpenHomeMenu( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   きのみをうえる
   * @param   fieldIdx    はたけインデックス
   */
  //--------------------------------------------------------------------------
  virtual void PlantKinomi( u8 fieldIdx, int itemkind );


  //--------------------------------------------------------------------------
  /**
   * @brief   上画面への反映
   */
  //--------------------------------------------------------------------------
  virtual void UpdateUpper( u8 resortID = 5 );


public:    //!< ResortTouchToFlowListener
  //--------------------------------------------------------------------------
  /**
   * @brief   タッチを通達  ResortTouchToFlowListener
   */
  //--------------------------------------------------------------------------
  virtual void OnTouchToFlow( TouchKind kind,int param );
  virtual void OnTouchToFlowXY(ResortTouchToFlowListener::TouchKind kind,int index, int x, int y);


  //--------------------------------------------------------------------------
  /**
   * @brief   抑制だけ行う
   */
  //--------------------------------------------------------------------------
  virtual void ResortInputEnabled( bool bflg );


  //--------------------------------------------------------------------------
  /**
   * @brief   ボタンのONOFF
   */
  //--------------------------------------------------------------------------

  virtual void ButtonDisplayTypeEnabled( bool bflg );

  
private:    //!< ResortFlowListener
  //--------------------------------------------------------------------------
  /**
   * @brief   うんどうフローからのボックス画面の呼び出しを通知
   * @param   nextSeq   次のシーケンス
   */
  //--------------------------------------------------------------------------
  virtual void OnCallBoxGim( u8 nextSeq );

  //--------------------------------------------------------------------------
  /**
   * @brief   おんせんフローからのボックス画面の呼び出しを通知
   */
  //--------------------------------------------------------------------------
  virtual void OnCallBoxHotSpa( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   図鑑の呼び出し
   */
  //--------------------------------------------------------------------------
  virtual void OnCallZukan( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   ステータスの呼び出し
   */
  //--------------------------------------------------------------------------
  virtual void OnCallStatus( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   タマゴ孵化デモの呼び出し
   */
  //--------------------------------------------------------------------------
  virtual void OnCallEggDemo( void );

  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの開発に伴うデータの更新を通知
   */
  //--------------------------------------------------------------------------
  virtual void OnUpdateResort( void );
  
  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの終了を通知
   */
  //--------------------------------------------------------------------------
  virtual void OnCallResortEnd( void );
  
  //--------------------------------------------------------------------------
  /**
   * @brief   リゾートの変更
   *
   * @param   resortID    リゾートID
   */
  //--------------------------------------------------------------------------
  virtual void ChangeResort( u8 resortID );
  
  //--------------------------------------------------------------------------
  /**
   * @brief   フェードインリクエスト
   */
  //--------------------------------------------------------------------------
  virtual void RequestFadeIn( void );
 
  //木の実畑の収穫数アニメ終了を得る
  virtual bool IsAnimEndGetEffect( void );


  //ボタンアニメ中かを得る
  virtual bool IsButtonIdleState( void );

  
 public:
  void ChageNearMode(void);
  void ChageFarMode(void);



  bool GetFarMode(void){ return  mbFarMode; }

  void ResetFarMode(void){ mbFarMode=false; }

  inline void SetResortChangeIslandListener(ResortChangeIslandListener* pListener){ mpResortChangeIslandListener = pListener;};

  inline void SetResortSequenceListener(ResortSequenceListener* pListener){ mpResortSequenceListener = pListener; };

  void SetLogic(ResortInitLogic* pLogic){ mpResortInitLogic = pLogic; };
  
  //--------------------------------------------------------------------------
  /**
   * @brief   表示中の施設番号を取得
   * @return  施設番号
   */
  //--------------------------------------------------------------------------
  u8 GetResortIndex(void);



  
private:
  //------------------------------------------------------------------
  /**
   * @brief    ファイル読み込みようテーブル初期化
   * @param  arcid   ARCID
   * @param  dataid  ＡＲＣの中の読み込むデータ
   * @param  pAppHeap  app::util::Heapクラス
   * @param  pReadBuffAddr  実際に読み込むアドレス
   */
  //------------------------------------------------------------------
  void fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);

  //------------------------------------------------------------------
  /**
   * @brief    読み込みの流れの管理
   */
  //------------------------------------------------------------------
  bool fileReadSequece(void);

  //------------------------------------------------------------------
  /**
   * @brief    メッセージデータの読み込みの流れの管理
   */
  //------------------------------------------------------------------
  bool msgDataReadSequece(void);

  //------------------------------------------------------------------
  /**
   * @brief    キャラモデルの読み込み
   */
  //------------------------------------------------------------------
  bool charaModelReadSequence(void);


  //------------------------------------------------------------------
  /**
   * @brief    リゾートの初期化
   */
  //------------------------------------------------------------------
  void InitResort( void );

  //------------------------------------------------------------------
  /**
   * @brief    最初のフローセット
   */
  //------------------------------------------------------------------
  void SetStartFlow(void);

  //------------------------------------------------------------------
  /**
   * @brief   UIの更新
   */
  //------------------------------------------------------------------
  void UpdateUI( void );

  //------------------------------------------------------------------
  /**
   * @brief   リゾートの設定
   *
   * @param   resortID    リゾートID
   * @param   dir         移動方向
   */
  //------------------------------------------------------------------
  bool SetResortID( u8 resortID, u8 dir = 0,bool bfade=false );
  
  //------------------------------------------------------------------
  /**
   * @brief   タッチ後の待機
   */
  //------------------------------------------------------------------
  void TouchToFlowWait( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   フェードシーケンス
   */
  //------------------------------------------------------------------
  void FadeSequence( void );
  
private:
  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：BG画面(上)
   */
  //------------------------------------------------------------------
  void CreateDraw_UppBG( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：メイン画面(上)
   */
  //------------------------------------------------------------------
  void CreateDraw_UppMain( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：管理人メニュー
   */
  //------------------------------------------------------------------
  void CreateDraw_UppHomeMenu( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：UI画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowUI( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：選択画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowSelect( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：ポケマメ選択画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowBeans( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：はたけ画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowFarm( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：植えるきのみのリスト画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowFarmList( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：アドベンチャー結果画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowAdvRes( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：うんどう状態画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowGim( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：コース選択画面(下)
   */
  //------------------------------------------------------------------
  void CreateDraw_LowCourse( void );

  //------------------------------------------------------------------
  /**
   * @brief   Drawの生成：きのみ取得エフェクト
   */
  //------------------------------------------------------------------
  void CreateDraw_LowKinomiEffect( void );

  //------------------------------------------------------------------
  /**
   * @brief   フローの生成
   */
  //------------------------------------------------------------------
  void CreateResortFlow( void );

  //------------------------------------------------------------------
  /**
   * @brief   会話メッセージの生成
   */
  //------------------------------------------------------------------
  void CreateTalkMessage( void );


private:
  //! @brief  Drawの生成を進める
  bool updateCreateDraw(void);
  //! @brief  Drawの生成が終わった後のセットアップ
  void setupAfterCreateDraw(void);

  //拾ったマメの設定
  void setPokeBeans(u8 mame);
  //すくないところにマメを入れる
  void setMinPokeBeans(int kindmin,int kindmax);
  
  
private:
  static const u32 UPDATE_UI_FRAME    = 15;   //!< UIの更新フレーム間隔
  static const u32 TOUCH_WAIT_FRAME   = 15;

private:
  app::util::Heap*                mpAppHeap;

  app::util::Heap*                mpLayoutHeap;

  app::util::Heap*                mpIcon2DHeap;

  app::util::AppRenderingManager* mpAppRenderingManager;

  gfl2::fs::AsyncFileManager*     mpAsyncFileManager;

private:
  APP_PARAM*  mpAppParam;

  int         mFileSeq;

  bool        mbFarMode;

  u8          mLytCount;

  u8          mLoadCount;

  u32         mUpdateFrameUI;

private:
  struct TouchToFlowParam
  {
    TouchKind kind;
    int       param;
    u32       frame;
  } mTouchParam;

private:
  Savedata::ResortSave*         mpResortSave;

  Savedata::BoxPokemon*         mpBoxPokeSave;

  Savedata::MyItem*             mpItemSave;

  Savedata::ZukanData*          mpZukanSave;

private:
  ResortDraw*                   mpResortDraw;

  UpperBGDraw*                  mpUpperBGDraw;

  UpperDraw*                    mpUpperDraw;

  UpperHomeMenuDraw*            mpUpperHomeMenuDraw;

  LowerUIDraw*                  mpLowerUIDraw;

  LowerSelectDraw*              mpLowerSelectDraw;

  LowerBeansSelectDraw*         mpLowerBeansSelectDraw;

  LowerFarmDraw*                mpLowerFarmDraw;

  LowerFarmListDraw*            mpLowerFarmListDraw;

  LowerAdventureResultDraw*     mpLowerAdventureResultDraw;

  LowerGimDraw*                 mpLowerGimDraw;

  LowerCourseSelectDraw*        mpLowerCourseSelectDraw;

  LowerKinomiGetEffectDraw*     mpLowerKinomiEffectDraw;

  ResortFlow*                   mpResortFlow;

  ResortSequenceListener*       mpResortSequenceListener;

  ResortInitLogic*              mpResortInitLogic;

  ResortChangeIslandListener*   mpResortChangeIslandListener;

  TalkMessage*                  mpTalkMessage;

  ResortCharaModel*             mpCharaModel;

private:
  bool mSetupAfterCreateDrawFinished;  // setupAfterCreateDrawが終了したらtrue

private:
  u8    mReadSeq;

  u8    mReadModelSeq;

  enum {
    ARC_RESORT,
    ARC_FIELD_WND,
    ARC_NUM,
  };
  void* mpArcReadBuff[ARC_NUM];

  enum {
    MSG_RESORT_SYS,
    MSG_RESORT_WND,
    MSG_RESORT_ITEM_INFO,
    MSG_RESORT_NUM,
  };
  void* mMsgBuff[MSG_RESORT_NUM];

private:
  gfl2::str::MsgData*   mMsgData[MSG_RESORT_NUM];

  print::WordSet*       mWordSet;

  WordSetLoader*        mWordSetLoader;

private:
  u8    mResortIndex;         //!< リゾートインデックス

  u8    mResortMax;           //!< 開放されているリゾートの最大数

  u8    mFadeSeq;             //!< フェードシーケンス


private:
  b8    mIsFadeInStart;       //!< フェードインを開始したか
  b8    mIsFadeOutStart;      //!< フェードアウトを開始したか

public:
  bool IsFadeInStart(void);
  bool IsFadeOutStart(void);
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif // __RESORT_FRAME_H__



