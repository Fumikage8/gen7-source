﻿//======================================================================
/**
 * @file    SystemFont.h
 * @brief   システムフォントの初期化・取得
 * @author  ariizumi
 * @author  Hiroyuki Nakamura
 * @date    11/04/07
 *
 * @li  sangoのsystem_font.cppを移植
 */
//======================================================================
#if !defined( SYSTEM_FONT_H_INCLUDE )
#define SYSTEM_FONT_H_INCLUDE

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>
#include <ro/include/gfl2_RoManager.h>

#include <Print/include/LayoutDefaultFont.h>

#define KANTAIHANTAI_TEST 0

//#include <nw/font/font_PairFont.h>


GFL_NAMESPACE_BEGIN(print)

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
class SystemFont: public gfl2::base::Singleton<SystemFont>
{
  GFL_FORBID_COPY_AND_ASSIGN( SystemFont );
public:

  //----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
   *
   *  @param  heap::HeapBase *heapMem  使用ヒープ
   *  @param  heap::HeapBase *deviceMem  使用ヒープ
   *  @param  日本語が載っているリージョンかどうか
   */
  //-----------------------------------------------------------------------------
  SystemFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem ,bool bJpnFontLoadRegion);

  //----------------------------------------------------------------------------
  /**
   *  @brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~SystemFont();

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプリ毎の初期化
   *          ※要gx初期化
   *          下にグローバル関数版があります
   */
  //-----------------------------------------------------------------------------
  void Initialize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプリ毎の解放
   *          下にグローバル関数版があります
   */
  //-----------------------------------------------------------------------------
  void Terminate();

  //-----------------------------------------------------------------------------
  /**
   * @func    SyncOpen
   * @brief   アークファイルオープン（同期）
   * @date    2016.05.02
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void SyncOpen( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    SyncOpen
   * @brief   アークファイルクローズ（同期）
   * @date    2016.05.02
   */
  //-----------------------------------------------------------------------------
  void SyncClose(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  フォントの取得
   *          下にグローバル関数版があります
   *
   *  @return gfl::str::Font フォント
   */
  //-----------------------------------------------------------------------------
  inline gfl2::str::Font* GetFont(){return mMainFont;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  共有フォントバッファの取得
   *          ResFontじゃないと取得できないので、日米欧フォントのバッファ
   *          下にグローバル関数版があります
   *
   *  @return void* フォントバッファ
   */
  //-----------------------------------------------------------------------------
  inline void* GetFontBuffer(){return mStdFontBuffer;}
  inline gfl2::str::ResFont* GetResFont(){return mKujiraFont;}

  //内蔵フォントのみ取得(実験用)
  inline gfl2::str::Font* GetSharedFont(){return mStdFont;}

  //拡張フォント関連
  //----------------------------------------------------------------------------
  /**
   *  @brief  numフォントの作成
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @param  heap::HeapBase *heap  使用ヒープ
   */
  //-----------------------------------------------------------------------------
  void CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

  //----------------------------------------------------------------------------
  /**
   *  @brief  numフォントの破棄
   *          下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void DeleteNumFont(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  numフォントの取得
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @return gfl::str::Font フォント
   */
  //-----------------------------------------------------------------------------
  inline gfl2::str::Font* GetNumFont(){return mNumFont;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  num共有フォントバッファの取得
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @return void* フォントバッファ
   */
  //-----------------------------------------------------------------------------
  inline void* GetNumFontBuffer(){return mNumFontBuffer;}

  //拡張フォント関連
  //----------------------------------------------------------------------------
  /**
   *  @brief  バトルフォントの作成
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @param  heap::HeapBase *heap  使用ヒープ
   */
  //-----------------------------------------------------------------------------
  void CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

  //----------------------------------------------------------------------------
  /**
   *  @brief  バトルフォントの破棄
   *          下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void DeleteBattleFont(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  バトルフォントの取得
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @return gfl::str::Font フォント
   */
  //-----------------------------------------------------------------------------
  inline gfl2::str::Font* GetBattleFont(){return mBattleFont;}
  
  //----------------------------------------------------------------------------
  /**
   *  @brief  バトル共有フォントバッファの取得
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @return void* フォントバッファ
   */
  //-----------------------------------------------------------------------------
  inline void* GetBattleFontBuffer(){return mBattleFontBuffer;}

  //----------------------------------------------------------------------------
  /**
   *  @brief  Rectドローシェーダーの取得
   *  @return void* mpRectDrawerShader
   */
  //-----------------------------------------------------------------------------
  inline gfl2::lyt::LytShaderResource* GetRectDrawerShaderRes(void){ return mpRectDrawerShaderRes; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  点字フォントの作成
   *          下にグローバル関数版があります。そっちを使ってください。
   *
   *  @param  heap::HeapBase *heap  使用ヒープ
   */
  //-----------------------------------------------------------------------------
//  void CreateTenjiFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

  //----------------------------------------------------------------------------
  /**
   *  @brief  点字フォントの破棄
   *          下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
//  void DeleteTenjiFont(void);



  //-----------------------------------------------------------------------------
  /**
   * @func    LoadNumFont
   * @brief   NUMフォントの読み込み（非同期）
   * @date    2015.07.17
   *
   * @param   heapMem     作業用ヒープ
   * @param   deviceMem   バッファ生成先ヒープ
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void LoadNumFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsLoadNumFont
   * @brief   NUMフォントの読み込み終了チェック
   * @date    2015.07.17
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  bool IsLoadNumFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNumFontResource
   * @brief   NUMフォントのリソースをセット
   * @date    2015.07.18
   *
   * @param   heapMem   ResFont作成ヒープ
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void SetNumFontResource( gfl2::heap::HeapBase * heapMem );

  //-----------------------------------------------------------------------------
  /**
   * @func    LoadBattleFont
   * @brief   バトルフォントの読み込み（非同期）
   * @date    2015.07.18
   *
   * @param   heapMem     作業用ヒープ
   * @param   deviceMem   バッファ生成先ヒープ
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void LoadBattleFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsLoadBattleFont
   * @brief   バトルフォントの読み込み終了チェック
   * @date    2015.07.18
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  bool IsLoadBattleFont(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetBattleFontResource
   * @brief   バトルフォントのリソースをセット
   * @date    2015.07.18
   *
   * @param   heapMem   ResFont作成ヒープ
   *
   * @note  下にグローバル関数版があります。そっちを使ってください。
   */
  //-----------------------------------------------------------------------------
  void SetBattleFontResource( gfl2::heap::HeapBase * heapMem );


  void ChangeMainFont( int no, gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem );

private:
  //-----------------------------------------------------------------------------
  /**
   * @func    CreateFontBuffer
   * @brief   フォントのバッファを生成
   * @date    2015.07.17
   *
   * @param   heapMem     サイズ取得作業用ヒープ
   * @param   deviceMem   バッファ生成先ヒープ
   * @param   dat_id      フォントのデータID
   * @param   size        生成したバッファのサイズ格納場所
   *
   * @return  生成したバッファ
   */
  //-----------------------------------------------------------------------------
  void * CreateFontBuffer( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem, gfl2::fs::ArcFile::ARCDATID dat_id, size_t * size );

private:
  int mMainFontNo;
  
  gfl2::str::Font * mMainFont;

  gfl2::str::ResFont *mStdFont;  //内蔵フォント
  void *mStdFontBuffer;
  gfl2::str::ResFont *mKujiraFont; //kujiraフォント
  void *mKujiraFontBuffer;

  gfl2::lyt::LytShaderResource* mpRectDrawerShaderRes;    //Font用Shader

 
  gfl2::heap::HeapBase* mpHeapDev;
  
  //numフォント
  gfl2::str::ResFont *mNumFont;
  void *mNumFontBuffer;
  
  //battleフォント
  gfl2::str::ResFont *mBattleFont;
  void *mBattleFontBuffer;

};


//----------------------------------------------------------------------------
/**
 *  @brief  アプリ毎の初期化
 *          ※要gx初期化
 *
 */
//-----------------------------------------------------------------------------
extern void SystemFont_Initialize(void);

//----------------------------------------------------------------------------
/**
 *  @brief  アプリ毎の解放
 */
//-----------------------------------------------------------------------------
extern void SystemFont_Terminate();

//----------------------------------------------------------------------------
/**
 *  @brief  フォントの取得
 *
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
extern gfl2::str::Font* SystemFont_GetFont();

//----------------------------------------------------------------------------
/**
 *  @brief  共有フォントバッファの取得
 *          ResFontじゃないと取得できないので、日米欧フォントのバッファ
 *
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
extern void* SystemFont_GetFontBuffer();

//内蔵フォントのみ取得(実験用)
extern gfl2::str::Font* SystemFont_GetSharedFont();


//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_CreateNumFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの破棄
 */
//-----------------------------------------------------------------------------
extern void SystemFont_DeleteNumFont(void);

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_LoadNumFont
 * @brief   NUMフォントの読み込み（非同期）
 * @date    2015.07.18
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_LoadNumFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem );

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_IsLoadNumFont
 * @brief   NUMフォントの読み込み終了チェック
 * @date    2015.07.18
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
extern bool SystemFont_IsLoadNumFont(void);

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_SetLoadNumFont
 * @brief   NUMフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_SetLoadNumFont( gfl2::heap::HeapBase * heapMem );

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントの取得
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
extern gfl2::str::Font* SystemFont_GetNumFont(void);

//----------------------------------------------------------------------------
/**
 *  @brief  num共有フォントバッファの取得
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
extern void* SystemFont_GetNumFontBuffer(void);

//----------------------------------------------------------------------------
/**
 *  @brief  追加されたnumフォント情報を消す
 */
//-----------------------------------------------------------------------------
extern void SystemFont_ResetNumFontInfo(void);

//拡張フォント関連
//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_CreateBattleFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの破棄
 */
//-----------------------------------------------------------------------------
extern void SystemFont_DeleteBattleFont(void);

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_LoadBattleFont
 * @brief   バトルフォントの読み込み（非同期）
 * @date    2015.07.18
 *
 * @param   heapMem     作業用ヒープ
 * @param   deviceMem   バッファ生成先ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_LoadBattleFont( gfl2::heap::HeapBase * heapMem, gfl2::heap::HeapBase * deviceMem );

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_IsLoadBattleFont
 * @brief   バトルフォントの読み込み終了チェック
 * @date    2015.07.18
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
extern bool SystemFont_IsLoadBattleFont(void);

//-----------------------------------------------------------------------------
/**
 * @func    SystemFont_SetLoadBattleFont
 * @brief   バトルフォントのリソースをセット
 * @date    2015.07.18
 *
 * @param   heapMem   ResFont作成ヒープ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_SetLoadBattleFont( gfl2::heap::HeapBase * heapMem );

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントの取得
 *  @return gfl::str::Font フォント
 */
//-----------------------------------------------------------------------------
extern gfl2::str::Font* SystemFont_GetBattleFont(void);

//----------------------------------------------------------------------------
/**
 *  @brief  バトル共有フォントバッファの取得
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
extern void* SystemFont_GetBattleFontBuffer(void);

//----------------------------------------------------------------------------
/**
 *  @brief  追加されたバトルフォント情報を消す
 */
//-----------------------------------------------------------------------------
extern void SystemFont_ResetBattleFontInfo(void);

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの作成
 *  @param  heap::HeapBase *heap  使用ヒープ
 */
//-----------------------------------------------------------------------------
//extern void SystemFont_CreateTenjiFont(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);

//----------------------------------------------------------------------------
/**
 *  @brief  点字フォントの破棄
 */
//-----------------------------------------------------------------------------
//extern void SystemFont_DeleteTenjiFont(void);


//----------------------------------------------------------------------------
/**
 *  @brief  defaultフォントを０番目にセットする
 */
//-----------------------------------------------------------------------------

extern void SystemFont_SetLytSystemDefaultFont(void);

//----------------------------------------------------------------------------
/**
 *  @brief  numフォントを１番目にセットする
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_SetLytSystemNumFont(void);

//----------------------------------------------------------------------------
/**
 *  @brief  バトルフォントを２番目にセットする
 *  @return void* フォントバッファ
 */
//-----------------------------------------------------------------------------
extern void SystemFont_SetLytSystemBattleFont(void);



//----------------------------------------------------------------------------
/**
 *  @brief  追加されたフォント情報を消す
 */
//-----------------------------------------------------------------------------
extern void SystemFont_ResetDefaultFontInfo(void);

//----------------------------------------------------------------------------
/**
 *  @brief  追加された点字フォントを消す
 */
//-----------------------------------------------------------------------------
//extern void SystemFont_ResetTenjiFont(void);


GFL_NAMESPACE_END(print)
  
/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( print::SystemFont );

#endif  // SYSTEM_FONT_H_INCLUDE
