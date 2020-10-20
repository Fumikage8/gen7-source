//===========================================================================
/**
 * @file    gfl_Assert.cpp
 * @brief   �A�T�[�g����
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
 * �A�T�[�g���[�v�������֐������[�U�ݒ肷��
 *
 * @param   loopFunc		���[�v���ɌĂ΂��R�[���o�b�N�֐�
 *
 */
//------------------------------------------------------------------
void GFL_ASSERT_SetLoopFunc( GFL_ASSERT_LoopFunc loopFunc )
{
	LoopFunc = loopFunc;
}


#if GFL_ENABLE_ASSERT


//==============================================================================================
// ���[�U��`�̕\���֐��Q���Ǘ����邽�߂̃|�C���^
//==============================================================================================
static GFL_ASSERT_DispInitFunc		DispInitFunc = NULL;
static GFL_ASSERT_DispMainFunc		DispMainFunc = NULL;
static GFL_ASSERT_DispFinishFunc	DispFinishFunc = NULL;

static void InitAssertDisp( void );
static void FinishAssertDisp( const char* buff );


//------------------------------------------------------------------
/**
 * �A�T�[�g���o�͏����֐������[�U�ݒ肷��
 *
 * @param   initFunc		�\�����������ɌĂ΂��R�[���o�b�N�֐�
 * @param   mainFunc		�����o�͂��ƂɌĂ΂��R�[���o�b�N�֐�
 * @param   finishFunc		�\���I�����ɌĂ΂��R�[���o�b�N�֐�
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
 * �A�T�[�g���o�͑O����
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
 * �A�T�[�g���o�͏I������
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
  //���i�łł͏��������Ȃ�
}

void GFLassertMsg( const char * , unsigned int , const char * , ... )
{
  //���i�łł͏��������Ȃ�
}


void GFLassertStop( const char * , unsigned int , const char *  )
{
  while(1){
		if( LoopFunc ){ LoopFunc(""); }  //HOME�{�^����������悤�ɂ��邽��
	}
}

void GFLassertMsgStop( const char * , unsigned int , const char * , ... )
{
  while(1){
		if( LoopFunc ){ LoopFunc(""); }  //HOME�{�^����������悤�ɂ��邽��
	}
}

#endif //GFL_ENABLE_ASSERT

GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl)


