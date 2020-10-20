#pragma once


#include <string>


#include <types/include/gfl2_Typedef.h>


////////////////////////////////////////////////////////////////////////////////
//! @brief  EnvConverter呼び出し関数
//! @return  正常のとき0、異常のとき0以外
extern int EnvConverter_Main(int argc, _TCHAR* argv[]);


////////////////////////////////////////////////////////////////////////////////
/*
文字メモ

プロジェクトのプロパティの「文字セット」を
Unicode文字セットを使用する
    にすると、_TCHARはワイド文字になる。
マルチバイト文字セットを使用する
    にすると、_TCHARはマルチバイト文字になる。

std::stringはマルチバイト文字を扱うクラス。_TCHARがマルチバイト文字ならそのまま代入できる。


*/


////////////////////////////////////////////////////////////////////////////////
// 前方宣言
namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {
class GfEnvData;
} } } }


////////////////////////////////////////////////////////////////////////////////
//! @brief  EnvConverter
class EnvConverter
{
public:
  EnvConverter(void);
  virtual ~EnvConverter();

public:
  //! @brief  実行
  int Main(int argc, _TCHAR* argv[]);

private:
  //! @brief  引数解釈
  int readArg(int argc, _TCHAR* argv[]);
  
  //! @brief  gfenvファイルを読み込む
  int readGfEnvFile(void);
  
  //! @brief  gfbenvファイルを書き出す
  int writeGfBEnvFile(void) const;
  //! @brief  ファイルに0を書き出す
  static int writeFileZero(size_t byteNum, FILE* file, std::string fileName);
  //! @brief  ファイルに書き出す(エラーチェックあり)
  static int writeFile(const void* buf, size_t bufSize, size_t count, FILE* file, std::string fileName);

  //! @brief  数値をアラインメント調整する
  //! @param[in]  val    アラインメント調整する数値
  //! @param[in]  align  アラインメント値。2のべき乗であること。
  //! @return  valをアラインメント調整した数値を返す。
  static u32 alignVal(u32 val, u32 align);

  //! @brief  エラー出力(改行あり)
  static void printLnError(std::string str);
  //! @brief  デバッグ出力(改行あり)
  static void printLnDebug(std::string str);

private:
  std::string m_inFileName;
  std::string m_outFileName;

  gfl2::renderingengine::scenegraph::resource::GfEnvData* m_resourceData;
};
