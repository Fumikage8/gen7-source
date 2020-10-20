//=================================================================================================
/**
 * @file   EventListDataManager.h
 * @brief  イベントリスト　データマネージャークラス
 * @author miyuki iwasawa
 * @date   2015.08.27
 */
//=================================================================================================

#ifndef __EVENTLISTDATAMANAGER_H__
#define __EVENTLISTDATAMANAGER_H__
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

//前方宣言
namespace GameSys {
  class GameManager;
  class GameData;
}
namespace Field{
  class EventWork;
}
namespace gfl2{
  namespace fs{
    class AsyncFileManager;  
  }
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(EventList)


/*
 * イベントリスト バイナリデータフォーマットヘッダ
 *
 * ---------------------------
 *  EventListBinHeader
 * ---------------------------
 *  EventListParam[]
 * ---------------------------
 *  EventListDetailInfo[]
 * ---------------------------
 *  EventListDetailField[]
 * ---------------------------
 */

class EventListData{
  GFL_FORBID_COPY_AND_ASSIGN( EventListData ); // コピーを禁止

public:
  struct BinHeader{
    u32 param_num;
    u32 param_offset;
    u32 detail_info_offset;
    u32 detail_field_offset;
  };

  struct Param{
    u8 no;
//    u8 category;
    u8 detail_num;
    u16 padding;
//    u16 msg_title;
//    u16 msg_info;
//    u32 work_id;
  };
  struct DetailInfo{
//    u16 msg_sub_title;
//    u16 msg_sub_info;
    u32 work_value;
  };
  struct DetailField{
    u16 iland_id;
    u16 msg_rotom_info;
    f32 pos_x;
    f32 pos_z;
  };

public:
  EventListData();
  ~EventListData();
  void SetupParam( EventWork* pEventWork, gfl2::heap::HeapBase* heap, EventListData::Param* p_param, EventListData::DetailInfo* p_info, EventListData::DetailField* p_field );

  //アクセサ
  const EventListData::Param* GetParam() const { return &m_Param; }
  const EventListData::DetailInfo* GetDetailInfoArray() const { return m_pDetailInfo; }
  const EventListData::DetailField* GetDetailFieldArray() const { return m_pDetailField; }
  const EventListData::DetailInfo* GetDetailInfo( u32 index ) const;
  const EventListData::DetailField* GetDetailField( u32 index ) const;
  const EventListData::DetailInfo* GetDetailInfoCurrent() const;
  const EventListData::DetailField* GetDetailFieldCurrent() const;

  //アクセサ 判定系

  //--------------------------------------------------------------
  /**
   * @brief このイベントがアクティブか？
   *
   * @retval  true   アクティブ（開始されている）
   * @retval  false  パッシブ（未開放）
   */
  //--------------------------------------------------------------
  bool IsActive() const;

  //--------------------------------------------------------------
  /**
   * @brief このイベントをクリア済みか？ 
   *
   * @retval  true   クリアしている
   * @retval  false  クリアしていない
   */
  //--------------------------------------------------------------
  bool IsClear() const;
  
  //--------------------------------------------------------------
  /**
   * @brief このイベントのカレントIndexを返す 
   */
  //--------------------------------------------------------------
  u32 GetCurrentIndex() const;

  //--------------------------------------------------------------
  /**
   * @brief このイベントの現在のワーク値を返す 
   */
  //--------------------------------------------------------------
  u32 GetWorkValue() const;
  
private:
  EventListData::Param  m_Param;
  EventListData::DetailInfo* m_pDetailInfo;
  EventListData::DetailField* m_pDetailField;

private:
  gfl2::heap::HeapBase* m_pHeap;
  EventWork* m_pEventWork;
};

class EventListDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN( EventListDataManager ); // コピーを禁止

public:
  enum EventListDataType{
    EVENT_LIST_DATA_TYPE_APP,
    EVENT_LIST_DATA_TYPE_FIELD,
  };
public:
  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap         workをとるためのメモリクラス
   * @param   type         データマネージャーの初期化タイプ
   */
  //--------------------------------------------------------------
  EventListDataManager( GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager* romAsyncFileManager );
  ~EventListDataManager();

public:
  //--------------------------------------------------------------
  /**
   * @brief   データロード開始
   */
  //--------------------------------------------------------------
  void Initialize( EventListDataType type );
  
  //--------------------------------------------------------------
  /**
   * @brief   データロード待ち
   */
  //--------------------------------------------------------------
  bool InitializeWait();
  
  //--------------------------------------------------------------
  /**
   * @brief   データ破棄
   */
  //--------------------------------------------------------------
  void Terminate(); 

  //--------------------------------------------------------------
  /**
   * @brief   データ格納
   */
  //--------------------------------------------------------------
  EventListData* SetupData( void* p_buffer, u32* p_num ); 

  //--------------------------------------------------------------
  /**
   * @brief   パラメータ取得
   */
  //--------------------------------------------------------------
  u32 GetMainListNum() const { return m_ListNumMain; }
  const EventListData* GetMainList( u32 index ) const; 
  const EventListData* GetMainListArray() const { return m_pListMain; }

  //--------------------------------------------------------------
  /**
   * @brief このイベントが有効か？（分岐用。falseのイベントはリストに表示しない）
   *
   * @retval  true   有効
   * @retval  false  無効
   */
  //--------------------------------------------------------------
  bool IsEnableParam( EventListData::Param* pParam ) const;

  //-------------------------------------------------------------- 
  /**
   * @brief 　カレントのメインイベントIndex
   *
   * @retval  -1    カレントがない（全クリア）
   * @retval  0-    MainListのIndex
   */
  //--------------------------------------------------------------
  s32 GetTargetIndexForMain( void ) const;

private:
  gfl2::heap::HeapBase* m_pHeap;
  gfl2::fs::AsyncFileManager* m_pAsyncFileManager;
  GameSys::GameManager* m_pGameManager;
  void* m_pBufferMain;

  EventListDataType m_Type; ///<データの初期化タイプ

  u32 m_ListNumMain;  ///<メインイベントリストの数
  EventListData* m_pListMain; ///<メインイベントリストデータ配列

  u32 m_SeqInitialize;  ///< 初期化用シーケンス
};


GFL_NAMESPACE_END(EventList)
GFL_NAMESPACE_END(Field)


#endif // __EVENTLISTDATAMANAGER_H__

