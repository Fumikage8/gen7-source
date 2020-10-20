#ifndef __sub_h_
#define __sub_h_

char* ReadFile( char* filename, int* p_size, int align );
void ChangeExtension( char *pSrc, char *pDst, char *pExt );

class Random
{
private:
	int Seed;					//	種

public:
	Random()					{	Seed = 0;	}		// コンストラクタ

	void Init(int seed=0)		{	Seed = seed;	}	// 種設定初期化

	void SetSeed(int seed)	{	Seed = seed;	}	// 種設定
	int GetSeed(void)			{	return Seed;	}	// 種取得

	int GetMax(void)					{	return 0x7fff;	}	// 最大値取得

	short Get(void)						{	return (((Seed = Seed * 214013L + 2531011L) >> 16) & 0x7fff);	}	// 値取得
	short Get(int min, int max)		{	return (Get() % (max - min + 1)) + min;	}							// 値取得(最大,最小を指定)
};

#endif