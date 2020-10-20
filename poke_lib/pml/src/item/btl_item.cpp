//=============================================================================
/**
 * @file btl_item.h
 * @brief バトルで使用するアイテム情報にアクセスする関数群
 * @author obata_toshihiro
 * @date 2011.09.26
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>

#include <pml/include/system/pml_Library.h>
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include <pml/include/item/btl_item.h>


namespace item {


  // バトルポケットデータのバッファ
  static void* s_btlpocket_buffer = NULL;



  //---------------------------------------------------------------------------
  /**
   * @brief アイテムのバトルポケットデータを読み込む
   * @param heap  バッファ確保に使用するヒープ
   *
   * @caution
   * 読み込んだデータの使用が終了したらUnloadBtlPocketData()で破棄してください.
   */
  //---------------------------------------------------------------------------
  void LoadBtlPocketData( gfl2::heap::HeapBase* heap )
  {
    if( s_btlpocket_buffer == NULL )
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
      arcReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
      arcReq.heapForFile = heap->GetLowerHandle();
      arcReq.heapForReq = heap->GetLowerHandle();
      /*
        2016/08/25  Yu Muto
        GF_DEVELOP_HIO_RESOURCE!=0の場合は、
        全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
      */
#if GF_DEVELOP_HIO_RESOURCE
#else
      arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
      pml::Library::m_pmlFileManager->SyncArcFileOpen(arcReq);

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq readReq;
      readReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
      readReq.datId = pml::Library::m_setting->garcDataID_BattlePocket;
      readReq.ppBuf = &s_btlpocket_buffer;
      readReq.heapForReq = heap->GetLowerHandle();
      readReq.heapForBuf = heap;
      pml::Library::m_pmlFileManager->SyncArcFileLoadData( readReq );

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
      pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
    }
  }


   /**
   * @brief アイテムのバトルポケットデータを読み込む
   * @param heap  バッファ確保に使用するヒープ
   *
   * @caution
   * 読み込んだデータの使用が終了したらUnloadBtlPocketData()で破棄してください.
   */
  void AsyncLoadBtlPocketData( gfl2::heap::HeapBase* heap , gfl2::heap::HeapBase* workheap )
  {
    if( s_btlpocket_buffer == NULL )
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq arcReq;
      arcReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
      arcReq.heapForFile = heap->GetLowerHandle();
      arcReq.heapForReq = workheap;
      /*
        2016/08/25  Yu Muto
        GF_DEVELOP_HIO_RESOURCE!=0の場合は、
        全てのリソースをHIO経由で読み込むモードへの対応のためデフォルト設定（マネージャ設定に依存）とします
      */
#if GF_DEVELOP_HIO_RESOURCE
#else
      arcReq.hioUseSetting = gfl2::fs::AsyncFileManager::HioUseSetting::ALWAYS_MAIN;
#endif
      pml::Library::m_pmlFileManager->AddArcFileOpenReq(arcReq);

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq readReq;
      readReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
      readReq.datId = pml::Library::m_setting->garcDataID_BattlePocket;
      readReq.ppBuf = &s_btlpocket_buffer;
      readReq.heapForReq = workheap;
      readReq.heapForBuf = heap;
      pml::Library::m_pmlFileManager->AddArcFileLoadDataReq( readReq );
    }
  }
  bool IsFinishLoadBtlPocketData(void)
  {
    if( pml::Library::m_pmlFileManager->IsArcFileOpenFinished(pml::Library::m_setting->garcPath_BattlePocket) == false )
    {
      return false;
    }
    if( pml::Library::m_pmlFileManager->IsArcFileLoadDataFinished( &s_btlpocket_buffer ) == false )
    {
      return false;
    }
    return true;
  }

  //Asyncの読み込み後に1回だけ呼んでください。
  void FinishLoadBtlPocketData(void)
  {
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.fileName = pml::Library::m_setting->garcPath_BattlePocket;
    pml::Library::m_pmlFileManager->SyncArcFileClose( closeReq );
  }
  //---------------------------------------------------------------------------
  /**
   * @brief 読み込んだアイテムのバトルポケットデータを破棄する
   */
  //---------------------------------------------------------------------------
  void UnloadBtlPocketData( void )
  {
    if( s_btlpocket_buffer )
    {
      GflHeapFreeMemory( s_btlpocket_buffer );
      s_btlpocket_buffer = NULL;
    }
  }


  //---------------------------------------------------------------------------
  /**
   * @brief 指定したバトルポケットに所属するかどうかを調べる
   * @param pocket  チェックするポケット
   * @param item    チェックする道具
   * @retval true   item は pocket に所属する
   * @retval false  item は pocket に所属しない
   *
   * @caution
   * 必ず, LoadBtlPocketData() でデータを読み込んでから使用してください.
   * 未ロードの場合は false を返します.
   *
   * @note
   * ARCID_BATTLE_POCKET の GARC_item_battle_pocket_battle_pocket_DAT
   * の実体は reousrce/item/item_battle_pocket.dat
   * 全てのアイテムについて
   * どのバトルポケットに所属するのかが8bitずつ格納されている.
   * 0000 0001 のビットが立っているなら「ボール」      ポケットに所属.
   * 0000 0010 のビットが立っているなら「せんとうよう」ポケットに所属.
   * 0000 0100 のビットが立っているなら「HP/PP」       ポケットに所属.
   * 0000 1000 のビットが立っているなら「状態異常」    ポケットに所属.
   * 0001 0000 のビットが立っているなら「ロトポン」    ポケットに所属.
   */
  //---------------------------------------------------------------------------
  bool CheckBtlPocketInclude( BtlPocket pocket, u32 item )
  {
    GFL_ASSERT( s_btlpocket_buffer );  // @check データが読み込まれていない
    if( s_btlpocket_buffer == NULL ) {
      return false;
    }

    u8* btlpocket_data = static_cast<u8*>( s_btlpocket_buffer );
    return ( btlpocket_data[ item ] & GetBattlePocketID(pocket) ) == 0 ? false : true ;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief バトルポケットのIDを取得
   * @return バトルポケットのID
   */
  //---------------------------------------------------------------------------
  u8 GetBattlePocketID( BtlPocket pocket )
  {
    // バトルポケットID
    const u8 BTLPOCKET_ID[ BTLPOCKET_NUM ] =
    {
      1 << 4,  // ロトポン
      1 << 2,  // HP/PP
      1 << 3,  // 状態異常
      1 << 0,  // ボール
      1 << 1,  // せんとうよう
    };
    return BTLPOCKET_ID[pocket];
  }


}  // namespace item
