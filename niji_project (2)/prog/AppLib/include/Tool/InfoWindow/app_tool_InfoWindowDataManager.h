//======================================================================
/**
 * @file app_tool_InfoWindow.h
 * @date 2015/07/02 15:47:18
 * @author kawada_koji
 * @brief インフォウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDATAMANAGER_H_INCLUDED__
#define __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDATAMANAGER_H_INCLUDED__
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <util/include/gfl2_std_string.h>

// nijiのインクルード
#include "AppLib/include/ui/UIView.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)


//==============================================================================
/**
 @class  InfoWindowDataManager
 @brief  インフォウィンドウデータマネージャ
 */
class InfoWindowDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(InfoWindowDataManager);

  friend class InfoWindowBase;
  friend class InfoWindowSheetType;
  friend class InfoWindowBookType;

public:
  //! @brief  生成タイプ
  enum CreateType
  {
    CREATE_TYPE_SHEET,        //!< SheetTypeのリソースのみ生成する
    CREATE_TYPE_BOOK,         //!< BookTypeのリソースのみ生成する
    CREATE_TYPE_SHEET_BOOK,   //!< SheetTypeとBookTypeのリソースを生成する
  };

public:
  InfoWindowDataManager(void);
  virtual ~InfoWindowDataManager();

  //! @brief  非同期生成　開始
  //! @param[in]  appHeap     ヒープ。m_pDevHeapしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  createType  生成タイプ。
  //! @note  UpdateFuncが毎フレーム回っていないと非同期処理は進みません。
  void StartAsyncCreate(
      app::util::Heap* appHeap,
      CreateType       createType
  );
  //! @brief  同期生成 
  //! @param[in]  appHeap     ヒープ。m_pDevHeapしか使わない。完了するまでに随時メモリを確保する。一時的に後方確保することもある。
  //! @param[in]  createType  生成タイプ。
  void SyncCreate(
      app::util::Heap* appHeap,
      CreateType       createType
  );
  //! @brief  生成が完了しているか
  //! @return  生成が完了したらtrueを返す。
  //!          生成を開始していないときはfalseを返す。生成中はfalseを返す。
  //! @note  UpdateFuncが毎フレーム回っていないと非同期処理は進みません。
  bool IsCreated(void) const;

  //! @brief  更新処理
  //! @note  毎フレーム呼んで下さい。呼ぶのをいつ止めてもいいし全く呼ばなくてもいい。ただしこれを呼ばないと回らない処理もあるので注意。
  void UpdateFunc(void);
  //! @brief  終了処理
  //! @param[in]  appHeap  ヒープ。m_pDevHeapしか使わない。一時的な後方確保しかしない。
  //!                      ファイルクローズのために使う。クローズ済みなら使われない。NULLの場合は同期クローズを行う。
  //! @note  trueを返すまで必ず呼んで下さい。毎フレーム呼んで下さい。trueを返したら破棄できます。
  bool EndFunc(
      app::util::Heap* appHeap = NULL
  );


  //! @name  friendクラスからアクセスするメンバ
  //@{
  
private:
  enum DataId
  {
    DATA_ID_SHEET_TYPE,
    DATA_ID_BOOK_TYPE,
    DATA_ID_STRBUF_SLASH,   // GetDataBufの戻り値を(gfl2::str::StrBuf*)にキャストして使って下さい。
    DATA_ID_STRBUF_NUMBER,  // GetDataBufの戻り値を(gfl2::str::StrBuf*)にキャストして使って下さい。
    DATA_ID_NUM
  };

  enum DataType
  {
    DATA_TYPE_VOID,
    DATA_TYPE_STRBUF,
  };
  static const DataType DATA_TYPE[DATA_ID_NUM];

  //! @brief  データバッファを取得する
  //! @return  読み込みが完了していないときはNULLを返す。
  //! @note  データバッファの破棄はこのクラスで行うので、呼び出し元では破棄してはならない。
  //! @note  UpdateFuncが毎フレーム回っていないと非同期処理は進みません。
  void* GetDataBuf(
      DataId dataId
  ) const;

  //@}


private:
  enum CreateState
  {
    CREATE_STATE_NONE,    
    CREATE_STATE_CREATING,
    CREATE_STATE_CREATED,
  };
  static const u32 STR_BUF_CHAR_NUM_MAX = 32;  // 「/」と「99」用

  void localUpdate(void);

  void destroyDataBuf(void);
  void createStrBufData(void);

private:
  bool         m_isEndFuncCalled;  // EndFuncが呼び出されていたらtrue
  CreateState  m_createState;
 
  // Createの引数
  CreateType   m_createType;
 
  // 内部変数
  void*        m_dataBuf[DATA_ID_NUM];

  app::util::Heap*          m_appHeap;         // 「StartAsyncCreateが呼ばれたとき」から「IsCreatedがtrueを返すようになるとき」まで保持しておく。その後はNULLにしておく。
  void*                     m_tempMsgDataBuf;  // 生成過程で一時的に確保するメッセージデータのバッファ
  gfl2::fs::ArcFile::ARCID  m_msgArcId;        // 生成過程で一時的に覚えておくメッセージのARCID
};


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)


#endif // __NIJI_PROJECT_APPLIB_APP_TOOL_INFOWINDOWDATAMANAGER_H_INCLUDED__

