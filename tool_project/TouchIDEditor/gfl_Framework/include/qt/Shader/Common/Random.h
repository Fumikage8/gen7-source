#ifndef GFL_MAYA_COMMON_RANDOM_H_
#define GFL_MAYA_COMMON_RANDOM_H_

#include "Type.h"

namespace gfl { namespace maya { namespace Common {

class Random
{
private:
	Int32 Seed;					//	種

public:
	Random()					{	Seed = 0;	}		// コンストラクタ

	void Init(Int32 seed=0)		{	Seed = seed;	}	// 種設定初期化

	void SetSeed(Int32 seed)	{	Seed = seed;	}	// 種設定
	Int32 GetSeed(void)			{	return Seed;	}	// 種取得

	Int32 GetMax(void)					{	return 0x7fff;	}	// 最大値取得

	Int16 Get(void)						{	return (((Seed = Seed * 214013L + 2531011L) >> 16) & 0x7fff);	}	// 値取得
	Int16 Get(Int32 min, Int32 max)		{	return (Get() % (max - min + 1)) + min;	}							// 値取得(最大,最小を指定)
};

}}}

#endif