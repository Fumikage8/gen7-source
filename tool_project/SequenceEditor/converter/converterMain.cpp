
#include "convertSeqFile.h"

int main(int argc, char* argv[])
{
  if( argc == 1 )
  {
    return 0;
  }

  if( strcmp( argv[1] , "-header" ) == 0 )
  {
    char *commandName = argv[2];
    char *saveFileName = argv[3];
    gfl::core::Debug::PrintConsole("OutputHeader\n");

    gfl::xml::Document comDoc;
    bool ret;
    ret = comDoc.Load( commandName );
    if( !ret )
    {
      gfl::core::Debug::PrintConsole("Load error SequenceConverter.exe [%s]\n",saveFileName);
    }

    SeqConv::OutputHeaderFile( &comDoc , saveFileName );
    getchar();
    
  }
  else
  if( argc == 4 )
  {
    char *seqFileName = argv[1];
    char *commandName = argv[2];
    char *saveFileName = argv[3];
    
    gfl::xml::Document seqDoc;
    gfl::xml::Document comDoc;
    bool ret;
    ret = seqDoc.Load( seqFileName );
    if( !ret )
    {
      gfl::core::Debug::PrintConsole("Load error SequenceConverter.exe [%s]\n",seqFileName);
    }
    ret = comDoc.Load( commandName );
    if( !ret )
    {
      gfl::core::Debug::PrintConsole("Load error SequenceConverter.exe [%s]\n",saveFileName);
    }

    //gfl::core::Debug::PrintConsole("Start Seqfile convert [%s]->[%s]\n",seqFileName,saveFileName);
    ret = SeqConv::ConverSequenceFile( &seqDoc , &comDoc , saveFileName );
    //getchar();

    if( ret == false )
    {
      gfl::core::Debug::PrintConsole("ReturnError");
      return -1;
    }

    return 0;
  }
  else
  {
    gfl::core::Debug::PrintConsole("Command error SequenceConverter.exe [SequencetFile] [CommandFile] [SaveFile]\n");
    for( int i =0;i<argc;i++ )
    {
      gfl::core::Debug::PrintConsole("[%s]\n",argv[i]);
    }
    //getchar();
    return -1;
  }

}

