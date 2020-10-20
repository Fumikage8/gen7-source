//=============================================================================
/**
* @brief  ヘッダファイルを解析しEnumの定義を取得するクラスの実装
* @file   gfl2_DebugCppEnumHeaderParser.cpp
* @author Kanamaru Masanori
* @date   2015.2.20
*/
//=============================================================================

#include <debug/include/DynamicHeader/internal/gfl2_DebugCppEnumHeaderParser.h>
#include <stdlib.h>  //use stdtol

namespace {
  // ヘッダ解析に必要な処理のみ実装した文字列クラス
  template<int TLength>
  class XString
  {
    char buffer[TLength];

  public:

    // 文字列前後の空白文字と制御文字を取り除く
    void Trim()
    {
      size_t length = 0;

      if (this->buffer[0] == '\0')
      {
        return;
      }

      length = strlen(this->buffer);
      // rigthTrim
      for (int i = strlen(this->buffer); i >=0; --i)
      {
        if (this->buffer[i] >= '!')
        {
          length = i+1;
          break;
        }
      }
      this->buffer[length] = '\0';

      // leftTrim
      s32 new_start = -1;
      for (u32 i = 0; i <= length; ++i)
      {
        if(new_start == -1 && this->buffer[i] >= '!')
        {
          new_start = static_cast<s32>(i);
        }

        if (new_start > -1)
        {
          this->buffer[i - new_start] = this->buffer[i];
        }
      }
    }

    // 特定の文字列で開始しているか判定
    bool StartWith(const char* query)
    {
      size_t t_length = strlen(this->buffer);
      size_t q_length = strlen(query);

      if (t_length < q_length)
      {
        return false;
      }

      for (u32 i=0; i < q_length; ++i)
      {
        if (this->buffer[i] != query[i])
        {
          return false;
        }
      }

      return true;
    }

    // 空白文字と制御文字で文字列を分割する
    int Split(char out[][TLength])
    {
      size_t length = strlen(this->buffer);

      int outIndex = 0;
      int outInnerIndex = 0;
      bool delimiter = true;

      for (u32 i = 0; i < length; ++i)
      {
        if (this->buffer[i] < '!')
        {
          if (delimiter == false)
          {
            out[outIndex][outInnerIndex] = '\0';

            ++outIndex;
            outInnerIndex = 0;
          }
          delimiter = true;
        }
        else
        {
          out[outIndex][outInnerIndex] = this->buffer[i];

          ++outInnerIndex;

          delimiter = false;
        }
      }

      if (delimiter == false)
      {
        out[outIndex][outInnerIndex] = '\0';
      }

      return outIndex + (delimiter ? 0 : 1);
    }

    // 生文字列へのポインタを取得する
    char* GetCStr()
    {
      return this->buffer;
    }
  };

}

namespace gfl2 {
  namespace debug {
    namespace DynamicHeader {

      //----------------------------------------
      /**
      * @brief	初期化処理
      *
      * @param	path      処理するファイルのパス
      * @param	pRootPath pathのルート設定
      * @param	pHeap	    使用するヒープへのポインタ
      */
      //----------------------------------------
      void CppEnumHeaderParser::Initialize(const char* path, const RootPathConfig* pRootPath, gfl2::heap::HeapBase* pHeap)
      {
        this->trs.Initialize(path, pRootPath, pHeap);

        this->stateStack.Push(Global);

        this->currentValue = -1;
      }

      //----------------------------------------
      /**
      * @brief	enumで定義された定数値のキーと値の組を1組取得する
      *
      * @param	outKey		キー文字列
      * @param	outValue	キーに対応する値
      *
      * @retval	true	定義を取得した
      * @retval	false	定義を取得できず、ファイルを末端まで読み終わった
      */
      //----------------------------------------
      bool CppEnumHeaderParser::ReadDefinition(char* outKey, int* outValue)
      {
        XString<LineMaxLength> line;

        while(this->trs.ReadLine(line.GetCStr(), LineMaxLength) != 0)				
        {
          // 前後の空白・制御文字削除
          line.Trim();

          if (strlen(line.GetCStr()) == 0)
          {
            continue;
          }

          State currentState;
          this->stateStack.Peek(&currentState);

          // 1行コメント
          if (line.StartWith("//"))
          {
            continue;
          }

          // ブロックコメント開始
          if (currentState != BlockComment && line.StartWith("/*")) 
          {
            this->stateStack.Push(BlockComment);
            continue;
          }

          switch (currentState)
          {
          case CppEnumHeaderParser::Global:
            if (line.StartWith("enum"))
            {
              char parts[3][LineMaxLength];
              if (line.Split(parts) > 1 && parts[1][0] == '{') // Enum開始
              {
                this->stateStack.Push(Enum);
                this->currentValue = -1;
              }
              else
              {
                this->stateStack.Push(EnumReady);
              }
            }
            break;
          case CppEnumHeaderParser::EnumReady:
            if (line.StartWith("{"))
            {
              this->stateStack.Push(Enum);
              this->currentValue = -1;
            }
            break;
          case CppEnumHeaderParser::BlockComment:
            if (line.StartWith("*/")) // ブロックコメントの終了
            {
              State dumy;
              this->stateStack.Pop(&dumy);
            }
            break;
          case CppEnumHeaderParser::Enum:						
            if (line.StartWith("};")) // Enum終了
            {
              State dumy;
              this->stateStack.Pop(&dumy);
              break;
            }

            // Enum内容
            char parts[4][256];
            int p_amount = line.Split(parts);
            if (p_amount > 0)
            {
              if (p_amount > 2 && strcmp(parts[1], "=") == 0) // 値が明示的に指定されている場合
              {
                this->currentValue = ::strtol(parts[2], NULL, 0);
              } 
              else // 値が指定されていない
              {
                // 前の定義から値を決定
                ++this->currentValue;

                int tail = strlen(parts[0]) - 1;
                // キー文字列末尾の「,」を取り除く
                if (parts[0][tail] == ',')
                {
                  parts[0][tail] = '\0';
                };
              }

              memcpy(outKey, parts[0], strlen(parts[0])+1);
              (*outValue) = this->currentValue;

              return true;
            }

            break;
          }
        }

        return false;

      }

      //----------------------------------------
      /**
      * @brief	終了処理
      */
      //----------------------------------------
      void CppEnumHeaderParser::Finalize()
      {
        this->trs.Finalize();
        this->stateStack.Clear();
      }

    } // DynamicHeader
  } // debug
} // gfl
