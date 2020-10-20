//=============================================================================
/**
* @brief  動的ヘッダシステム実装
* @file   gfl2_DebugHeaderHashManager.cpp
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

#include <debug/include/DynamicHeader/internal/gfl2_DebugDynamicHeaderDefine.h>

#include <debug/include/DynamicHeader/internal/gfl2_DebugHeaderHashManager.h>
#include <debug/include/DynamicHeader/internal/gfl2_DebugCppEnumHeaderParser.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {

      HeaderHashManager::MasterHash* HeaderHashManager::hashRoot = NULL;
      gfl2::heap::HeapBase* HeaderHashManager::pHeap = NULL;
      size_t HeaderHashManager::heapAlertLimit = 0;
      const RootPathConfig* HeaderHashManager::pRootPath = NULL;

      bool HeaderHashManager::isEnable = false;

      //--------------------------------------------------
      /**
      * @brief	初期化
      *
      * @param  pHeap               利用するヒープのポインタ
      * @param  maxLabelHashAmount  同時ロード可能なファイルの上限	
      * @param  heapAlertLimit      ヒープ使用量の警告を出す閾値
      * @param  pRootPath            ロードされるヘッダパスのルート設定
      */
      //--------------------------------------------------
      void HeaderHashManager::Initialize(gfl2::heap::HeapBase* pHeap, const MasterHash::IdxType maxLabelHashAmount, const size_t heapAlertLimit, const RootPathConfig* pRootPath)
      {
        HeaderHashManager::hashRoot = GFL_NEW(pHeap) HeaderHashManager::MasterHash(maxLabelHashAmount, pHeap);
        HeaderHashManager::pHeap = pHeap;

        HeaderHashManager::heapAlertLimit = heapAlertLimit;
        HeaderHashManager::pRootPath = pRootPath;
      }

      //--------------------------------------------------
      /**
      * @brief	終了処理
      */
      //--------------------------------------------------
      void HeaderHashManager::Finalize()
      {
        // UnLoadされていないヘッダテーブルを強制UnLoad
        MasterHash::Iterator* itr = HeaderHashManager::hashRoot->GetIterator();
        const char* path;
        LabelHash* table;
        while(itr->Next(&path, &(table)))
        {
          GFL_DELETE table;
        }

        GFL_DELETE HeaderHashManager::hashRoot;
        HeaderHashManager::pHeap = NULL;
      }

      //--------------------------------------------------
      /**
      * @brief	ヘッダファイルを読み込み、ハッシュテーブルとしてロードする
      *			同時ロード上限を超えてロードしようとするとアサートで止まる
      *			読み込み済みのファイルを指定した場合は参照カウントのみ進めます
      *
      * @param	filePath	読み込むファイルパス
      * @param	capacity	ファイルから読み出す定義数 (実際に定義されていた数がこの値を超えるとアサートを出します)
      */
      //--------------------------------------------------
      void HeaderHashManager::LoadHeaderHash(const char* filePath, const LabelHash::IdxType capacity)
      {
        if (HeaderHashManager::GetIsEnable() == false)
        {
          return;
        }

        //文字列長チェック
        size_t pathLen = strlen(filePath);
        GFL_ASSERT_MSG(pathLen <= KeyMaxLength, "DynamicHeader: %s \n Too long filePath!!! \n", filePath);

        // 要求分を一旦確保してしまう
        LabelHash* pLabelHash = GFL_NEW(HeaderHashManager::pHeap) LabelHash(capacity, HeaderHashManager::pHeap);

        // 追加
        // キーが重複していた場合はfalseが返るため確保したメモリを開放
        // 重複時も参照カウンタが更新される
        bool res = HeaderHashManager::hashRoot->Add(filePath, pLabelHash);
        if (res == false)
        {
          GFL_DELETE pLabelHash;
          return;
        }

        // ヘッダファイルの構文解析
        CppEnumHeaderParser parser;

        parser.Initialize(filePath, HeaderHashManager::pRootPath, HeaderHashManager::pHeap);

        char key[KeyMaxLength+1];
        int  value = 0;

        while (parser.ReadDefinition(key, &value))
        {
          pLabelHash->Add(key, value);
        }

        parser.Finalize();

        // ヒープ使用量が閾値を超えたら警告する
        size_t used_size = HeaderHashManager::pHeap->GetTotalSize() - HeaderHashManager::pHeap->GetTotalFreeSize();
        if (used_size > HeaderHashManager::heapAlertLimit)
        {
          GFL_PRINT("DynamicHeader: Use Over %d bytes (%d bytes) \n", HeaderHashManager::heapAlertLimit, used_size);
        } 
      }

      //--------------------------------------------------
      /**
      * @brief	ハッシュテーブルのアンロード
      *			参照カウントが残っている場合はアンロードしません
      *
      * @param	filePath	対象ファイルパス(const char*)
      */
      //--------------------------------------------------
      void HeaderHashManager::UnLoadHeaderHash(const char* filePath)
      {
        if (HeaderHashManager::GetIsEnable() == false)
        {
          return;
        }

        // 参照カウンタの値によっては実際には削除されない
        LabelHash* pTable = HeaderHashManager::hashRoot->Remove(filePath);
        GFL_DELETE pTable;
      }

      //--------------------------------------------------
      /**
      * @brief	値取得(filePathを指定すると高速に取得できます)
      *
      * @param	keyword		    定数名
      * @param defaultValue  無効時に返す値
      * @param	filePath	    対象ヘッダファイルのパス
      *
      * @return	keywordに対応した値
      */
      //--------------------------------------------------
      s32 HeaderHashManager::GetValue(const char* keyword, const s32 defaultValue, const char* filePath)
      {
        if (HeaderHashManager::GetIsEnable() == false)
        {
          return defaultValue;
        }

        bool is_success;
        int value;

        if (filePath != NULL)
        {
          LabelHash* table = HeaderHashManager::hashRoot->GetValue(filePath, &is_success);

          GFL_ASSERT_MSG(is_success, "HeaderHashManager : [%s] is not Loaded \n", filePath);

          value = table->GetValue(keyword, &is_success);

          GFL_ASSERT_MSG(is_success, "HeaderHashManager : [%s] is not Define in [%s] \n", keyword, filePath);

          return value;
        }
        else // ファイルが指定されていない場合は全テーブルを対象に検索
        {
          MasterHash::Iterator* itr = HeaderHashManager::hashRoot->GetIterator();

          const char* path;
          LabelHash* table;
          while(itr->Next(&path, &table))
          {
            value = table->GetValue(keyword, &is_success);
            if(is_success)
            {
              return value;
            }
          }
          GFL_ASSERT_MSG(false, "HeaderHashManager : [%s]'s Definition is not Load. \n", keyword);
        }

        // コンパイラを通すためで実際はここに到達しない。
        return NULL;
      }

      //--------------------------------------------------
      /**
      * @brief	現在ロードしているヘッダの内容を出力
      */
      //--------------------------------------------------
      void HeaderHashManager::Dump()
      {
        if (HeaderHashManager::GetIsEnable() == false)
        {
          GFL_PRINT("Caution!!! Now DyanamicHeaderSytem is Disabled.\n");
          return;
        }

        GFL_PRINT("Dump HeaderHashTables -- Start\n");

        MasterHash::Iterator* itr = HeaderHashManager::hashRoot->GetIterator();

        const char* path;
        LabelHash* table;
        while(itr->Next(&path, &table))
        {
          GFL_PRINT("-- %s ---- START \n", path);

          LabelHash::Iterator* c_itr = table->GetIterator();

          const char* key;
          int   value;
          while(c_itr->Next(&key, &value))
          {
            GFL_PRINT("%-70s => %d\n", key, value);
          }

          GFL_PRINT("-- %s ---- END \n", path);
        }

        GFL_PRINT("Dump HeaderHashTables -- End\n");
      }

      //--------------------------------------------------
      /**
      * @brief システムの有効/無効を変更する
      *
      * @param 変更する値
      */
      //--------------------------------------------------
      void HeaderHashManager::SetIsEnable(bool value)
      {
        HeaderHashManager::isEnable = value;         
      }

      //--------------------------------------------------
      /**
      * @brief システムの有効/無効を取得する
      *
      * @return システムが有効か
      */
      //--------------------------------------------------
      bool HeaderHashManager::GetIsEnable()
      {
        return HeaderHashManager::isEnable;
      }

    } // DynamicHeader
  } // debug
} // gfl
