#pragma once

//=================================================================================================
//
//  �V�X�e���R���g���[���pUtility
//
//=================================================================================================
#include <mmsystem.h>
#include <strsafe.h>

//=================================================================================================
//
//  ������\�^�C�}�[�R���g���[��
//
//=================================================================================================
class MMTimerControl
{
public:
  // �R���X�g���N�^
	MMTimerControl(){ valid = FALSE; }

  // �f�X�g���N�^
	~MMTimerControl(){ cleanup(); }

	bool activate(void)
  {
	  // �R���s���[�^�����݃T�|�[�g���Ă��鐸�x���擾
	  TIMECAPS timercaps;
	  ZeroMemory(&timercaps, sizeof(timercaps));

	  if(timeGetDevCaps(&timercaps, sizeof(TIMECAPS)) != TIMERR_NOERROR){ return FALSE; }
	  UPeriod = timercaps.wPeriodMin;	//�^�C�}�[����\�擾

    // �A�v���P�[�V�����܂��̓f�o�C�X�h���C�o�̍ŏ��^�C�}����\��ݒ肷��
	  if(timeBeginPeriod(UPeriod) != TIMERR_NOERROR){ return false; }

    prevTime = timeGetTime();
    valid = true;

		return true;
  }

	void cleanup(void)
	{
		if(valid == true){ 
			timeEndPeriod(UPeriod);
			valid = false;
		}
	}

  // ���݂̃^�C�}�[�擾
  DWORD getTime(){ return timeGetTime(); }

  // �t���[���J�E���g���Z�b�g
  void resetTimeFrame( void )
  {
    prevTime = timeGetTime();
  }

  // �t���[���J�E���g�擾
  DWORD getTimeFrame( DWORD interval )
  {
    DWORD time = timeGetTime();
		DWORD diff = time - prevTime;

    prevTime = time - (diff % interval);
    return diff / interval;
  }

  // �����ϐ�
public:
  bool  valid;

private:
  UINT  UPeriod;
  DWORD prevTime;
  DWORD interval;
};


//=================================================================================================
//
// �t�@�C������f�[�^�ǂݍ���
//
//=================================================================================================
inline void* ut_LoadFile(const wchar_t* filename, DWORD* size = NULL)
{
  char* srcBuffer;

  // �t�@�C���I�[�v��
  HANDLE file = CreateFile( filename, 
												    GENERIC_READ,
												    FILE_SHARE_READ,
												    NULL,
												    OPEN_EXISTING,
												    FILE_ATTRIBUTE_READONLY,
												    NULL);
  if(file == INVALID_HANDLE_VALUE){ return NULL; }

  // �ǂݍ��ݗp�o�b�t�@�쐬
  DWORD fileSize = GetFileSize(file, NULL);
  srcBuffer = new char[fileSize];

  // �ǂݍ���
  DWORD readSize;
  ReadFile(file, srcBuffer, fileSize, &readSize, NULL);
  CloseHandle(file);
  if(readSize != fileSize){
    delete[] srcBuffer;
    return NULL;
  }
  if(size){ *size = fileSize; }

  return (void*)srcBuffer;
}

