#ifndef __GFL_ERRADAPTER_H__
#define __GFL_ERRADAPTER_H__
//=============================================================================
/**
 * @file   gfl_ErrAdapter.h
 * @brief  エラーEULAアプレットを適切に処理するクラス
 * @author k.ohno
 * @date   2012.10.23
 */
//=============================================================================
#pragma once

#include <gflib.h>

#include <nn/Result.h>

#include <ro/gfl_RoManager.h>
#include <nn/erreula.h>


GFL_NAMESPACE_BEGIN(gfl)
GFL_NAMESPACE_BEGIN(err)



class ErrAdapter
{
  GFL_FORBID_COPY_AND_ASSIGN(ErrAdapter);
private:
  static nn::erreula::Parameter mEreParam;  //渡すパラメータ 1KB以上あるので定義
  static nn::erreula::CTR::ReturnCode _ErrorDispDirectMain(nn::erreula::Parameter* ere_param);

public:


  //------------------------------------------------------------------
  /**
   * @brief   アプレットが指定された番号のエラーを出す
   * @param   errorCode  エラーコード
   * @param   errorType  エラータイプ
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(u32 errorCode , nn::erreula::ErrorType errorType);
  //------------------------------------------------------------------
  /**
   * @brief   直接エラー文章を出す
   * @param   pBuf StrBufのポインタ
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(gfl::str::StrBuf* pBuf);
#if GFL_DEBUG
  //------------------------------------------------------------------
  /**
   * @brief   直接エラーメッセージを出す デバッグ用途
   * @param   ワイドキャラ型文字列
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const wchar_t* pMessage);
  //------------------------------------------------------------------
  /**
   * @brief   直接エラーメッセージを出す デバッグ用途
   *          mbstowcsには限界があり、正しく表示されない事があります。 wchar_tなら出ます
   * @param   文字列
   * @return  nn::erreula::CTR::ReturnCode 任天堂アプレットのリターンコード
   */
  //------------------------------------------------------------------
  static nn::erreula::CTR::ReturnCode ErrorDisp(const char* pMessage);
  static void ErrorDispChar(const char* pMessage);

#endif //GFL_DEBUG

};

/*
エラー処理
概要

CTR ではエラーを大きく以下のように分類しています。
継続不可能な致命的エラー

「致命的なエラー」とは、ハードウェア不具合や本来製品で発生し得ないエラーなど
システムが何をしても復帰できず、かつハンドリング不可能なエラーのことを指します。
このようなエラーが発生した場合は、原則アプリケーションにエラーが渡されることはなく、
システムが停止してFATALエラー画面が表示されます。
このエラー表示に遭遇したユーザーはメッセージに従ってマニュアルを確認し、
解決しない場合は最終的にユーザサポートに連絡することになります。
システム上エラー表示も不可能な場合はその場で停止します。

復旧可能なエラー

「復旧可能なエラー」とは以下のようなものを指します。

    設定の間違い
    通信エラー
    ファイルの容量オーバー、空き容量不足
    セーブデータの破損
    ソフトウェア的な不整合（再初期化等で復帰可能なもの）
    致命的ではあるが、当面のシステム実行継続が困難ではないもの（カメラ故障等）

これらのエラーに遭遇した場合は、
アプリケーションは適切にハンドリングを行ってエラー状態からの復旧を試みたり、
必要に応じてエラー表示アプレットを用いてユーザーに問題の発生を伝えなければなりません。
アプリケーションの処理は基本的に継続します。
エラーハンドリング

SDK の多くのインタフェースはエラーを返す可能性がある場合は、
nn::Result クラスの実体を返します。
詳細は、Result クラスを参照してください。
各ライブラリでのエラーハンドリング方法の詳細については、
それぞれのライブラリのリファレンスを参照してください。

ファイル操作に関連するエラー処理について
ファイルシステムが返すエラーは、アクセスするメディア、
対象のファイルに対する権限などにより多様なエラーが発生します。
特に、セーブデータが破損した場合などアプリケーションの
シーケンスまで含めた対応が必要な場合があります。
このため、ファイルシステムが返すエラーについては、
他のライブラリとは異なった処理方針を持っています。
詳しくはファイルシステムのドキュメントを参照してください。

 */


GFL_NAMESPACE_END(err)
GFL_NAMESPACE_END(gfl)

#endif // __GFL_ERRADAPTER_H__
