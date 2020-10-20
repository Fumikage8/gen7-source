//===========================================================================
/**
 * @file    gfl_Assert.cpp
 * @brief   アサート処理
 * @author  GAMEFREAK inc.
 * @date    2011.10.06
 */
//===========================================================================

#include <gflib.h>

GFL_NAMESPACE_BEGIN(gfl)
GFL_NAMESPACE_BEGIN(debug)

static GFL_ASSERT_LoopFunc				LoopFunc = NULL;
static char debugmessagebuff[512];

//------------------------------------------------------------------
/**
 * アサートループ時処理関数をユーザ設定する
 *
 * @param   loopFunc		ループ時に呼ばれるコールバック関数
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetLoopFunc( GFL_ASSERT_LoopFunc loopFunc )
{
	LoopFunc = loopFunc;
}


#if GFL_ENABLE_ASSERT


//==============================================================================================
// ユーザ定義の表示関数群を管理するためのポインタ
//==============================================================================================
static GFL_ASSERT_DispInitFunc		DispInitFunc = NULL;
static GFL_ASSERT_DispMainFunc		DispMainFunc = NULL;
static GFL_ASSERT_DispFinishFunc	DispFinishFunc = NULL;

static void InitAssertDisp( void );
static void FinishAssertDisp( const char* buff );


//------------------------------------------------------------------
/**
 * アサート情報出力処理関数をユーザ設定する
 *
 * @param   initFunc		表示初期化時に呼ばれるコールバック関数
 * @param   mainFunc		文字出力ごとに呼ばれるコールバック関数
 * @param   finishFunc		表示終了時に呼ばれるコールバック関数
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc )
{
	DispInitFunc = initFunc;
	DispMainFunc = mainFunc;
	DispFinishFunc = finishFunc;
}

//------------------------------------------------------------------
/**
 * アサート情報出力前処理
 *
 * @param   none		
 *
 */
//------------------------------------------------------------------
static void InitAssertDisp( void )
{
	if( DispInitFunc != NULL )
	{
		DispInitFunc();
	}
}

//------------------------------------------------------------------
/**
 * アサート情報出力終了処理
 */
//------------------------------------------------------------------
static void FinishAssertDisp( const char* buff )
{
	if( DispFinishFunc != NULL )
	{
		DispFinishFunc();
	}
	else
	{
    while(1){
			if( LoopFunc ){
        if( LoopFunc(buff) ){
          break;
        }
      }
		}
	}
}


void GFLassert( const char * filename, unsigned int line_no, const char * exp )
{
	InitAssertDisp();
	NN_LOG("  **** ASSERTION FAILED ! **** \n");
	NN_LOG("%s(%d)\n", filename, line_no);
  NN_LOG("%s\n",exp);
  if(DispMainFunc){
    DispMainFunc( exp );
  }
  sprintf(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s",filename, (int)line_no, exp);
	FinishAssertDisp( debugmessagebuff );

}

void GFLassertMsg( const char * filename, unsigned int line_no, const char * fmt, ... )
{
	InitAssertDisp();
	NN_LOG("  **** ASSERTION FAILED ! **** \n");
	NN_LOG("%s(%d)\n", filename, line_no);
  {
    char buff[256];
    va_list vlist;
    va_start(vlist, fmt);
    vsprintf(buff, fmt, vlist);
    va_end(vlist);
    NN_LOG("%s\n", buff);
    
    //NN_PANIC("");
    
    if(DispMainFunc){
      DispMainFunc( buff );
    }
    sprintf(debugmessagebuff,"ASSERTION FAILED !\n%s(%d)%s",filename, (int)line_no, buff);
    FinishAssertDisp(debugmessagebuff);
  }

}


void GFLassertStop( const char * filename, unsigned int line_no, const char * exp )
{
  GFLassert(filename, line_no, exp);
  while(1){
		if( LoopFunc ){ LoopFunc(exp); }
	}
}

void GFLassertMsgStop( const char * filename, unsigned int line_no, const char * fmt, ... )
{
  GFLassertMsg(filename, line_no, fmt);
  while(1){
		if( LoopFunc ){ LoopFunc(""); }
	}
}



#else //GFL_ENABLE_ASSERT


void GFLassert( const char * , unsigned int , const char *  )
{
  //製品版では処理をしない
}

void GFLassertMsg( const char * , unsigned int , const char * , ... )
{
  //製品版では処理をしない
}


void GFLassertStop( const char * , unsigned int , const char *  )
{
  while(1){
		if( LoopFunc ){ LoopFunc(""); }  //HOMEボタンを押せるようにするため
	}
}

void GFLassertMsgStop( const char * , unsigned int , const char * , ... )
{
  while(1){
		if( LoopFunc ){ LoopFunc(""); }  //HOMEボタンを押せるようにするため
	}
}

#endif //GFL_ENABLE_ASSERT

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl)


