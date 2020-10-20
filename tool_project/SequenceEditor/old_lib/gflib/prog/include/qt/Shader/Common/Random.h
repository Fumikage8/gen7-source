#ifndef GFL_MAYA_COMMON_RANDOM_H_
#define GFL_MAYA_COMMON_RANDOM_H_

#include "Type.h"

namespace gfl { namespace maya { namespace Common {

class Random
{
private:
	Int32 Seed;					//	��

public:
	Random()					{	Seed = 0;	}		// �R���X�g���N�^

	void Init(Int32 seed=0)		{	Seed = seed;	}	// ��ݒ菉����

	void SetSeed(Int32 seed)	{	Seed = seed;	}	// ��ݒ�
	Int32 GetSeed(void)			{	return Seed;	}	// ��擾

	Int32 GetMax(void)					{	return 0x7fff;	}	// �ő�l�擾

	Int16 Get(void)						{	return (((Seed = Seed * 214013L + 2531011L) >> 16) & 0x7fff);	}	// �l�擾
	Int16 Get(Int32 min, Int32 max)		{	return (Get() % (max - min + 1)) + min;	}							// �l�擾(�ő�,�ŏ����w��)
};

}}}

#endif