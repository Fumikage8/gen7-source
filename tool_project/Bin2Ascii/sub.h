#ifndef __sub_h_
#define __sub_h_

char* ReadFile( char* filename, int* p_size, int align );
void ChangeExtension( char *pSrc, char *pDst, char *pExt );

class Random
{
private:
	int Seed;					//	��

public:
	Random()					{	Seed = 0;	}		// �R���X�g���N�^

	void Init(int seed=0)		{	Seed = seed;	}	// ��ݒ菉����

	void SetSeed(int seed)	{	Seed = seed;	}	// ��ݒ�
	int GetSeed(void)			{	return Seed;	}	// ��擾

	int GetMax(void)					{	return 0x7fff;	}	// �ő�l�擾

	short Get(void)						{	return (((Seed = Seed * 214013L + 2531011L) >> 16) & 0x7fff);	}	// �l�擾
	short Get(int min, int max)		{	return (Get() % (max - min + 1)) + min;	}							// �l�擾(�ő�,�ŏ����w��)
};

#endif