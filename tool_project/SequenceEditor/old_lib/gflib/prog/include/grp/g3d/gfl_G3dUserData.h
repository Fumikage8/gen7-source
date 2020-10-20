#ifndef __GFL_G3DUSERDATA_H__
#define __GFL_G3DUSERDATA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUserData.h
 *	@brief  3Dユーザデータ
 *	        @note ユーザデータはポインタのようなものなので、Setすれば値が反映されます。
 *	              ユーザデータアクセッサとかユーザデータポインタとかユーザデータリファレンスという名前に変更したい。
 *	@author	Koji Kawada
 *	@date		2011.08.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>

// gflib grp g3d
//#include <.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dユーザデータクラス
//=====================================
class UserData
{
  GFL_FORBID_COPY_AND_ASSIGN(UserData);

  // 下記クラスにprivateメンバ関数へのアクセスを許可する
  friend class Model;
  friend class ResAnim;
  friend class EdgeMapPreRenderCallbackSlot;


  // 定数
public:
  // データの型情報 
  enum DataType
  {
    DATA_TYPE_FLOAT    = nw::ut::ResMetaData::DATATYPE_FLOAT_ARRAY,   // 実数
    DATA_TYPE_INT      = nw::ut::ResMetaData::DATATYPE_INT_ARRAY,     // 整数
    DATA_TYPE_STRING   = nw::ut::ResMetaData::DATATYPE_STRING_ARRAY,  // 文字列
    DATA_TYPE_UNKNOWN  = nw::ut::ResMetaData::DATATYPE_UNKOWN,        // 不明
    DATA_TYPE_INVALID  = -2                                           // 無効
  };
  // 文字列のエンコード
  enum Encoding
  {
    ENCODING_ASCII    = nw::ut::ResStringArrayMetaData::ENCODING_ASCII,
    ENCODING_UTF8     = nw::ut::ResStringArrayMetaData::ENCODING_UTF8,
    ENCODING_UTF16LE  = nw::ut::ResStringArrayMetaData::ENCODING_UTF16LE,
    ENCODING_UTF16BE  = nw::ut::ResStringArrayMetaData::ENCODING_UTF16BE,
    ENCODING_UNKNOWN  = -1
  };


  // メンバ関数
public:
  //-------------------------------------
  // 共通
  //-------------------------------------
  UserData(void);
  //virtual ~UserData();
  virtual ~UserData();

  DataType  GetDataType(void) const;
  s32       GetValueCount(void) const;

  //-------------------------------------
  // 整数
  //-------------------------------------
  // 正常に取得できなかった場合、戻り値は不定 
  s32  GetIntValue(const s32 index) const;
  // 正常に設定できたときtrueを返す
  b32  SetIntValue(const s32 index, const s32 value);
  
  //-------------------------------------
  // 実数
  //-------------------------------------
  // 正常に取得できなかった場合、戻り値は不定 
  f32  GetFloatValue(const s32 index) const;
  // 正常に設定できたときtrueを返す
  b32  SetFloatValue(const s32 index, const f32 value);
  
  //-------------------------------------
  // 文字列
  //-------------------------------------
  // 正常に取得できなかった場合、ENCODING_UNKNOWNを返す
  Encoding        GetEncoding(void) const;
  // 正常に取得できなかった場合、NULLを返す
  const char*     GetStringValue(const s32 index) const;
  // 正常に取得できなかった場合、NULLを返す
  const wchar_t*  GetWStringValue(const s32 index) const;


  // friendクラスからのみアクセスできるメンバ関数
private:
  //-------------------------------------
  // 共通
  //-------------------------------------
  void  SetNwResMetaData(nw::ut::ResMetaData res_meta_data);


  // friendクラスからアクセスしないで欲しいメンバ関数
private:
  //-------------------------------------
  // 共通
  //-------------------------------------
  nw::ut::ResIntArrayMetaData    CastInt(void) const;
  nw::ut::ResFloatArrayMetaData  CastFloat(void) const;
  nw::ut::ResStringArrayMetaData CastString(void) const;

  // メンバ変数
private:
  DataType             m_data_type;
  nw::ut::ResMetaData  m_res_meta_data;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DUSERDATA_H__
