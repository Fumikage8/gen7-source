#ifndef __GFL_G3DUSERDATA_H__
#define __GFL_G3DUSERDATA_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUserData.h
 *	@brief  3D���[�U�f�[�^
 *	        @note ���[�U�f�[�^�̓|�C���^�̂悤�Ȃ��̂Ȃ̂ŁASet����Βl�����f����܂��B
 *	              ���[�U�f�[�^�A�N�Z�b�T�Ƃ����[�U�f�[�^�|�C���^�Ƃ����[�U�f�[�^���t�@�����X�Ƃ������O�ɕύX�������B
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
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D���[�U�f�[�^�N���X
//=====================================
class UserData
{
  GFL_FORBID_COPY_AND_ASSIGN(UserData);

  // ���L�N���X��private�����o�֐��ւ̃A�N�Z�X��������
  friend class Model;
  friend class ResAnim;
  friend class EdgeMapPreRenderCallbackSlot;


  // �萔
public:
  // �f�[�^�̌^��� 
  enum DataType
  {
    DATA_TYPE_FLOAT    = nw::ut::ResMetaData::DATATYPE_FLOAT_ARRAY,   // ����
    DATA_TYPE_INT      = nw::ut::ResMetaData::DATATYPE_INT_ARRAY,     // ����
    DATA_TYPE_STRING   = nw::ut::ResMetaData::DATATYPE_STRING_ARRAY,  // ������
    DATA_TYPE_UNKNOWN  = nw::ut::ResMetaData::DATATYPE_UNKOWN,        // �s��
    DATA_TYPE_INVALID  = -2                                           // ����
  };
  // ������̃G���R�[�h
  enum Encoding
  {
    ENCODING_ASCII    = nw::ut::ResStringArrayMetaData::ENCODING_ASCII,
    ENCODING_UTF8     = nw::ut::ResStringArrayMetaData::ENCODING_UTF8,
    ENCODING_UTF16LE  = nw::ut::ResStringArrayMetaData::ENCODING_UTF16LE,
    ENCODING_UTF16BE  = nw::ut::ResStringArrayMetaData::ENCODING_UTF16BE,
    ENCODING_UNKNOWN  = -1
  };


  // �����o�֐�
public:
  //-------------------------------------
  // ����
  //-------------------------------------
  UserData(void);
  //virtual ~UserData();
  virtual ~UserData();

  DataType  GetDataType(void) const;
  s32       GetValueCount(void) const;

  //-------------------------------------
  // ����
  //-------------------------------------
  // ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
  s32  GetIntValue(const s32 index) const;
  // ����ɐݒ�ł����Ƃ�true��Ԃ�
  b32  SetIntValue(const s32 index, const s32 value);
  
  //-------------------------------------
  // ����
  //-------------------------------------
  // ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
  f32  GetFloatValue(const s32 index) const;
  // ����ɐݒ�ł����Ƃ�true��Ԃ�
  b32  SetFloatValue(const s32 index, const f32 value);
  
  //-------------------------------------
  // ������
  //-------------------------------------
  // ����Ɏ擾�ł��Ȃ������ꍇ�AENCODING_UNKNOWN��Ԃ�
  Encoding        GetEncoding(void) const;
  // ����Ɏ擾�ł��Ȃ������ꍇ�ANULL��Ԃ�
  const char*     GetStringValue(const s32 index) const;
  // ����Ɏ擾�ł��Ȃ������ꍇ�ANULL��Ԃ�
  const wchar_t*  GetWStringValue(const s32 index) const;


  // friend�N���X����̂݃A�N�Z�X�ł��郁���o�֐�
private:
  //-------------------------------------
  // ����
  //-------------------------------------
  void  SetNwResMetaData(nw::ut::ResMetaData res_meta_data);


  // friend�N���X����A�N�Z�X���Ȃ��ŗ~���������o�֐�
private:
  //-------------------------------------
  // ����
  //-------------------------------------
  nw::ut::ResIntArrayMetaData    CastInt(void) const;
  nw::ut::ResFloatArrayMetaData  CastFloat(void) const;
  nw::ut::ResStringArrayMetaData CastString(void) const;

  // �����o�ϐ�
private:
  DataType             m_data_type;
  nw::ut::ResMetaData  m_res_meta_data;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DUSERDATA_H__
