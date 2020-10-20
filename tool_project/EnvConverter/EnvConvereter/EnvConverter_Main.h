#pragma once


#include <string>


#include <types/include/gfl2_Typedef.h>


////////////////////////////////////////////////////////////////////////////////
//! @brief  EnvConverter�Ăяo���֐�
//! @return  ����̂Ƃ�0�A�ُ�̂Ƃ�0�ȊO
extern int EnvConverter_Main(int argc, _TCHAR* argv[]);


////////////////////////////////////////////////////////////////////////////////
/*
��������

�v���W�F�N�g�̃v���p�e�B�́u�����Z�b�g�v��
Unicode�����Z�b�g���g�p����
    �ɂ���ƁA_TCHAR�̓��C�h�����ɂȂ�B
�}���`�o�C�g�����Z�b�g���g�p����
    �ɂ���ƁA_TCHAR�̓}���`�o�C�g�����ɂȂ�B

std::string�̓}���`�o�C�g�����������N���X�B_TCHAR���}���`�o�C�g�����Ȃ炻�̂܂ܑ���ł���B


*/


////////////////////////////////////////////////////////////////////////////////
// �O���錾
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
  //! @brief  ���s
  int Main(int argc, _TCHAR* argv[]);

private:
  //! @brief  ��������
  int readArg(int argc, _TCHAR* argv[]);
  
  //! @brief  gfenv�t�@�C����ǂݍ���
  int readGfEnvFile(void);
  
  //! @brief  gfbenv�t�@�C���������o��
  int writeGfBEnvFile(void) const;
  //! @brief  �t�@�C����0�������o��
  static int writeFileZero(size_t byteNum, FILE* file, std::string fileName);
  //! @brief  �t�@�C���ɏ����o��(�G���[�`�F�b�N����)
  static int writeFile(const void* buf, size_t bufSize, size_t count, FILE* file, std::string fileName);

  //! @brief  ���l���A���C�������g��������
  //! @param[in]  val    �A���C�������g�������鐔�l
  //! @param[in]  align  �A���C�������g�l�B2�ׂ̂���ł��邱�ƁB
  //! @return  val���A���C�������g�����������l��Ԃ��B
  static u32 alignVal(u32 val, u32 align);

  //! @brief  �G���[�o��(���s����)
  static void printLnError(std::string str);
  //! @brief  �f�o�b�O�o��(���s����)
  static void printLnDebug(std::string str);

private:
  std::string m_inFileName;
  std::string m_outFileName;

  gfl2::renderingengine::scenegraph::resource::GfEnvData* m_resourceData;
};
