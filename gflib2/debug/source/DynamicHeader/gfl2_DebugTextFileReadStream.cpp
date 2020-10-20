//=============================================================================
/**
* @brief  外部テキストファイルを読み込むクラス(デバッグ専用)の実装
* @file   gfl2_DebugTextFileReadStream.cpp
* @author Kanamaru Masanori
* @date   2015.2.18
*/
//=============================================================================

#include <debug/include/DynamicHeader/internal/gfl2_DebugTextFileReadStream.h>

#include <str/include/gfl2_Str.h>
#include <fs/include/gfl2_FsPcUtil.h>

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {  

      //--------------------------------------------------
      /**
      * @brief	初期化処理
      *
      * @param	path	    読み込むファイルパス
      * @param	pRootPath pathのルートとなるパスの設定
      * @param	pHeap	    使用するヒープへのポインタ
      */
      //--------------------------------------------------
      void TextFileReadStream::Initialize(const char* path, const RootPathConfig* pRootPath, gfl2::heap::HeapBase* pHeap)
      {
        // 指定されたルートパスを切り替えながらファイルを探す
        {
          bool exist;
          for (int i = 0 ; i < RootPathArrayMax; ++i)
          {
            fs::PcUtil::SetCurrentWorkDirectory(pRootPath->rootPathArray[i], NULL, 0);
            fs::PcUtil::CheckFileExist(&exist, path);
            if (exist)
            {
              break;
            }
          }
          GFL_ASSERT_MSG(exist, "TextFileReadStream Assert : Not Exist %s \n", path);
        }

        fs::PcUtil::GetFileSize(&(this->fileSize), path);

        this->contentBuffer = static_cast<char*>GflHeapAllocMemory(pHeap, this->fileSize);

        fs::Result res = fs::PcUtil::ReadFile(NULL, this->contentBuffer, this->fileSize, path);

        GFL_ASSERT_MSG(res.IsSuccess(), "TextFileReadStream Assert : Can Not Open %s \n", path);

        this->currentOffset = 0;
      }

      //--------------------------------------------------
      /**
      * @brief	終了処理(ファイルのクローズ)
      */
      //--------------------------------------------------
      void TextFileReadStream::Finalize()
      {
        GflHeapSafeFreeMemory(this->contentBuffer);
      }

      //--------------------------------------------------
      /**
      * @brief	1行取得する
      *			1行がmaxLengthを超えていた場合はアサートを発生させる
      *
      * @param	outBuffer	出力先
      * @param	maxLength	読み出す最大の長さ
      *
      * @return	実際に読み出した文字列の長さ
      */
      //--------------------------------------------------
      size_t TextFileReadStream::ReadLine(char* outBuffer, size_t maxLength)
      {
        size_t write_offset = 0;

        while(this->currentOffset + write_offset < this->fileSize)
        {        
          if (this->contentBuffer[this->currentOffset + write_offset] == gfl2::str::CR_CODE)
          {
            break;
          }

          GFL_ASSERT_MSG(write_offset < maxLength, "TextFileReadStream::Readline : Reading Line is longer to %d\n", maxLength);

          ++write_offset;
        }

        memcpy(outBuffer, this->contentBuffer + this->currentOffset, write_offset);

        outBuffer[write_offset] = gfl2::str::EOM_CODE;

        this->currentOffset += write_offset + 1;

        return write_offset;
      }
    }
  }
}
