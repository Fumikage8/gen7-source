/* Pawn disassembler  - crude, but (perhaps) useful
 *
 *  Copyright (c) ITB CompuPhase, 2007-2009
 *
 *  This software is provided "as-is", without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1.  The origin of this software must not be misrepresented; you must not
 *      claim that you wrote the original software. If you use this software in
 *      a product, an acknowledgment in the product documentation would be
 *      appreciated but is not required.
 *  2.  Altered source versions must be plainly marked as such, and must not be
 *      misrepresented as being the original software.
 *  3.  This notice may not be removed or altered from any source distribution.
 *
 *  Version: $Id: pawndisasm.c 4097 2009-03-27 16:35:50Z thiadmer $
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../amx/osdefs.h"
#include "../amx/amx.h"

static FILE *fpamx;
static AMX_HEADER amxhdr;

typedef cell (*OPCODE_PROC)(FILE *ftxt,const cell *params,cell opcode,cell cip);

cell parm0(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm1(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm2(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm3(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm4(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm5(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell parm1_p(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_proc(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_call(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_jump(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell do_switch(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell casetbl(FILE *ftxt,const cell *params,cell opcode,cell cip);
cell icasetbl(FILE *ftxt,const cell *params,cell opcode,cell cip);


typedef struct {
  cell opcode;
  char *name;
  OPCODE_PROC func;
} OPCODE;

static OPCODE opcodelist[] = {
  {  0, "???",        parm0 },
  {  1, "load.pri",   parm1 },
  {  2, "load.alt",   parm1 },
  {  3, "load.s.pri", parm1 },
  {  4, "load.s.alt", parm1 },
  {  5, "lref.pri",   parm1 },
  {  6, "lref.alt",   parm1 },
  {  7, "lref.s.pri", parm1 },
  {  8, "lref.s.alt", parm1 },
  {  9, "load.i",     parm0 },
  { 10, "lodb.i",     parm1 },
  { 11, "const.pri",  parm1 },
  { 12, "const.alt",  parm1 },
  { 13, "addr.pri",   parm1 },
  { 14, "addr.alt",   parm1 },
  { 15, "stor.pri",   parm1 },
  { 16, "stor.alt",   parm1 },
  { 17, "stor.s.pri", parm1 },
  { 18, "stor.s.alt", parm1 },
  { 19, "sref.pri",   parm1 },
  { 20, "sref.alt",   parm1 },
  { 21, "sref.s.pri", parm1 },
  { 22, "sref.s.alt", parm1 },
  { 23, "stor.i",     parm0 },
  { 24, "strb.i",     parm1 },
  { 25, "lidx",       parm0 },
  { 26, "lidx.b",     parm1 },
  { 27, "idxaddr",    parm0 },
  { 28, "idxaddr.b",  parm1 },
  { 29, "align.pri",  parm1 },
  { 30, "align.alt",  parm1 },
  { 31, "lctrl",      parm1 },
  { 32, "sctrl",      parm1 },
  { 33, "move.pri",   parm0 },
  { 34, "move.alt",   parm0 },
  { 35, "xchg",       parm0 },
  { 36, "push.pri",   parm0 },
  { 37, "push.alt",   parm0 },
  { 38, "pick",       parm1 },        /* version 10 */
  { 39, "push.c",     parm1 },
  { 40, "push",       parm1 },
  { 41, "push.s",     parm1 },
  { 42, "pop.pri",    parm0 },
  { 43, "pop.alt",    parm0 },
  { 44, "stack",      parm1 },
  { 45, "heap",       parm1 },
  { 46, "proc",       do_proc },
  { 47, "ret",        parm0 },
  { 48, "retn",       parm0 },
  { 49, "call",       do_call },
  { 50, "call.pri",   parm0 },
  { 51, "jump",       do_jump },
  { 52, "jrel",       parm1 },  /* same as jump, since version 10 */
  { 53, "jzer",       do_jump },
  { 54, "jnz",        do_jump },
  { 55, "jeq",        do_jump },
  { 56, "jneq",       do_jump },
  { 57, "jless",      do_jump },
  { 58, "jleq",       do_jump },
  { 59, "jgrtr",      do_jump },
  { 60, "jgeq",       do_jump },
  { 61, "jsless",     do_jump },
  { 62, "jsleq",      do_jump },
  { 63, "jsgrtr",     do_jump },
  { 64, "jsgeq",      do_jump },
  { 65, "shl",        parm0 },
  { 66, "shr",        parm0 },
  { 67, "sshr",       parm0 },
  { 68, "shl.c.pri",  parm1 },
  { 69, "shl.c.alt",  parm1 },
  { 70, "shr.c.pri",  parm1 },
  { 71, "shr.c.alt",  parm1 },
  { 72, "smul",       parm0 },
  { 73, "sdiv",       parm0 },
  { 74, "sdiv.alt",   parm0 },
  { 75, "umul",       parm0 },
  { 76, "udiv",       parm0 },
  { 77, "udiv.alt",   parm0 },
  { 78, "add",        parm0 },
  { 79, "sub",        parm0 },
  { 80, "sub.alt",    parm0 },
  { 81, "and",        parm0 },
  { 82, "or",         parm0 },
  { 83, "xor",        parm0 },
  { 84, "not",        parm0 },
  { 85, "neg",        parm0 },
  { 86, "invert",     parm0 },
  { 87, "add.c",      parm1 },
  { 88, "smul.c",     parm1 },
  { 89, "zero.pri",   parm0 },
  { 90, "zero.alt",   parm0 },
  { 91, "zero",       parm1 },
  { 92, "zero.s",     parm1 },
  { 93, "sign.pri",   parm0 },
  { 94, "sign.alt",   parm0 },
  { 95, "eq",         parm0 },
  { 96, "neq",        parm0 },
  { 97, "less",       parm0 },
  { 98, "leq",        parm0 },
  { 99, "grtr",       parm0 },
  {100, "geq",        parm0 },
  {101, "sless",      parm0 },
  {102, "sleq",       parm0 },
  {103, "sgrtr",      parm0 },
  {104, "sgeq",       parm0 },
  {105, "eq.c.pri",   parm1 },
  {106, "eq.c.alt",   parm1 },
  {107, "inc.pri",    parm0 },
  {108, "inc.alt",    parm0 },
  {109, "inc",        parm1 },
  {110, "inc.s",      parm1 },
  {111, "inc.i",      parm0 },
  {112, "dec.pri",    parm0 },
  {113, "dec.alt",    parm0 },
  {114, "dec",        parm1 },
  {115, "dec.s",      parm1 },
  {116, "dec.i",      parm0 },
  {117, "movs",       parm1 },
  {118, "cmps",       parm1 },
  {119, "fill",       parm1 },
  {120, "halt",       parm1 },
  {121, "bounds",     parm1 },
  {122, "sysreq.pri", parm0 },
  {123, "sysreq.c",   parm1 },
  {124, "pushr.pri",  parm0 },        /* version 11 (replaces FILE from versions 0..7) */
  {125, "pushr.c",    parm1 },        /* version 11 (replaces LINE from versions 0..7) */
  {126, "pushr.s",    parm1 },        /* version 11 (replaces SYMBOL from versions 0..7) */
  {127, "pushr.adr",  parm1 },        /* version 11 (replaces SRANGE from versions 1..7) */
  {128, "jump.pri",   parm0 },        /* version 1 (now obsolete) */
  {129, "switch",     do_switch },    /* version 1 */
  {130, "casetbl",    casetbl },      /* version 1 */
  {131, "swap.pri",   parm0 },        /* version 4 */
  {132, "swap.alt",   parm0 },        /* version 4 */
  {133, "push.adr",   parm1 },        /* version 4 */
  {134, "nop",        parm0 },        /* version 6 */
  {135, "sysreq.n",   parm2 },        /* version 9 (replaces SYSREQ.d from versions 7 & 8) */
  {136, "symtag",     parm1 },        /* version 7 (now obsolete) */
  {137, "break",      parm0 },        /* version 8 */
  {138, "push2.c",    parm2 },        /* version 9 */
  {139, "push2",      parm2 },        /* version 9 */
  {140, "push2.s",    parm2 },        /* version 9 */
  {141, "push2.adr",  parm2 },        /* version 9 */
  {142, "push3.c",    parm3 },        /* version 9 */
  {143, "push3",      parm3 },        /* version 9 */
  {144, "push3.s",    parm3 },        /* version 9 */
  {145, "push3.adr",  parm3 },        /* version 9 */
  {146, "push4.c",    parm4 },        /* version 9 */
  {147, "push4",      parm4 },        /* version 9 */
  {148, "push4.s",    parm4 },        /* version 9 */
  {149, "push4.adr",  parm4 },        /* version 9 */
  {150, "push5.c",    parm5 },        /* version 9 */
  {151, "push5",      parm5 },        /* version 9 */
  {152, "push5.s",    parm5 },        /* version 9 */
  {153, "push5.adr",  parm5 },        /* version 9 */
  {154, "load.both",  parm2 },        /* version 9 */
  {155, "load.s.both",parm2 },        /* version 9 */
  {156, "const",      parm2 },        /* version 9 */
  {157, "const.s",    parm2 },        /* version 9 */
  {158, "icall",      parm1 },        /* version 10 */
  {159, "iretn",      parm0 },
  {160, "iswitch",    do_switch },    /* version 10 */
  {161, "icasetbl",   icasetbl },     /* version 10 */
  {162, "load.p.pri", parm1_p },
  {163, "load.p.alt", parm1_p },
  {164, "load.p.s.pri",parm1_p },
  {165, "load.p.s.alt",parm1_p },
  {166, "lref.p.pri", parm1_p },
  {167, "lref.p.alt", parm1_p },
  {168, "lref.p.s.pri",parm1_p },
  {169, "lref.p.s.alt",parm1_p },
  {170, "lodb.p.i",   parm1_p },
  {171, "const.p.pri",parm1_p },
  {172, "const.p.alt",parm1_p },
  {173, "addr.p.pri", parm1_p },
  {174, "addr.p.alt", parm1_p },
  {175, "stor.p.pri", parm1_p },
  {176, "stor.p.alt", parm1_p },
  {177, "stor.p.s.pri",parm1_p },
  {178, "stor.p.s.alt",parm1_p },
  {179, "sref.p.pri", parm1_p },
  {180, "sref.p.alt", parm1_p },
  {181, "sref.p.s.pri",parm1_p },
  {182, "sref.p.s.alt",parm1_p },
  {183, "strb.p.i",   parm1_p },
  {184, "lidx.p.b",   parm1_p },
  {185, "idxaddr.p.b",parm1_p },
  {186, "align.p.pri",parm1_p },
  {187, "align.p.alt",parm1_p },
  {188, "push.p.c",   parm1_p },
  {189, "push.p",     parm1_p },
  {190, "push.p.s",   parm1_p },
  {191, "stack.p",    parm1_p },
  {192, "heap.p",     parm1_p },
  {193, "shl.p.c.pri",parm1_p },
  {194, "shl.p.c.alt",parm1_p },
  {195, "shr.p.c.pri",parm1_p },
  {196, "shr.p.c.alt",parm1_p },
  {197, "add.p.c",    parm1_p },
  {198, "smul.p.c",   parm1_p },
  {199, "zero.p",     parm1_p },
  {200, "zero.p.s",   parm1_p },
  {201, "eq.p.c.pri", parm1_p },
  {202, "eq.p.c.alt", parm1_p },
  {203, "inc.p",      parm1_p },
  {204, "inc.p.s",    parm1_p },
  {205, "dec.p",      parm1_p },
  {206, "dec.p.s",    parm1_p },
  {207, "movs.p",     parm1_p },
  {208, "cmps.p",     parm1_p },
  {209, "fill.p",     parm1_p },
  {210, "halt.p",     parm1_p },
  {211, "bounds.p",   parm1_p },
  {212, "push.p.adr", parm1_p },
  {213, "pushr.p.c",  parm1_p },      /* version 11 */
  {214, "pushr.p.s",  parm1_p },      /* version 11 */
  {215, "pushr.p.adr",parm1_p },      /* version 11 */
};

void print_opcode(FILE *ftxt,cell opcode,cell cip)
{
  assert(opcodelist[(int)(opcode & 0x0000ffff)].opcode==(opcode & 0x0000ffff));
  fprintf(ftxt,"%08lx  %s ",(long)cip,opcodelist[(int)(opcode &0x0000ffff)].name);
}

cell parm0(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"\n");
  return 1;
}

cell parm1(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx\n",(long)*params);
  return 2;
}

cell parm2(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx %08lx\n",(long)params[0],(long)params[1]);
  return 3;
}

cell parm3(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx %08lx %08lx\n",(long)params[0],(long)params[1],(long)params[2]);
  return 4;
}

cell parm4(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx %08lx %08lx %08lx\n",(long)params[0],(long)params[1],(long)params[2],(long)params[3]);
  return 5;
}

cell parm5(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx %08lx %08lx %08lx %08lx\n",(long)params[0],(long)params[1],(long)params[2],(long)params[3],(long)params[4]);
  return 6;
}

cell parm1_p(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx\n",(long)(opcode>>16));
  return 1;
}

cell do_proc(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  int idx,numpublics,nameoffset;
  AMX_FUNCSTUBNT func;
  char name[sNAMEMAX+1];

  fprintf(ftxt,"\n");

  /* find the procedure in the table (only works for a public function) */
  nameoffset=-1;
  name[0]='\0';
  if ((amxhdr.flags & AMX_FLAG_OVERLAY)!=0) {
    //??? first find the address in the overlay table
    //??? find the overlay index in the public function table
  } else {
    /* find the address in the public function table */
    numpublics=(amxhdr.natives-amxhdr.publics)/sizeof(AMX_FUNCSTUBNT);
    fseek(fpamx,amxhdr.publics,SEEK_SET);
    for (idx=0; idx<numpublics && nameoffset<0; idx++) {
      fread(&func,sizeof func,1,fpamx);
      if (func.address==cip)
        nameoffset=func.nameofs;
    } /* for */
  } /* if */
  if (nameoffset>=0) {
    fseek(fpamx,nameoffset,SEEK_SET);
    fread(name,1,sNAMEMAX+1,fpamx);
  } /* if */

  print_opcode(ftxt,opcode,cip);
  if (strlen(name)>0)
    fprintf(ftxt,"\t; %s",name);
  fprintf(ftxt,"\n");
  return 1;
}

cell do_call(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx\n",(long)(*params+cip));
  return 2;
}

cell do_jump(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx\n",(long)(*params+cip));
  return 2;
}

cell do_switch(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  print_opcode(ftxt,opcode,cip);
  fprintf(ftxt,"%08lx\n",(long)(*params+cip));
  return 2;
}

cell casetbl(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  cell num;
  int idx;

  print_opcode(ftxt,opcode,cip);
  num=params[0]+1;
  fprintf(ftxt,"%08lx %08lx\n",(long)params[0],(long)(params[1]+cip+sizeof(cell)));
  for (idx=1; idx<num; idx++)
    fprintf(ftxt,"                  %08lx %08lx\n",(long)params[2*idx],(long)(params[2*idx+1]+cip+(2*idx+1)*sizeof(cell)));
  return 2*num+1;
}

cell icasetbl(FILE *ftxt,const cell *params,cell opcode,cell cip)
{
  cell num;
  int idx;

  print_opcode(ftxt,opcode,cip);
  num=params[0]+1;
  fprintf(ftxt,"%08lx %08lx\n",(long)params[0],(long)params[1]);
  for (idx=1; idx<num; idx++)
    fprintf(ftxt,"                  %08lx %08lx\n",(long)params[2*idx],(long)params[2*idx+1]);
  return 2*num+1;
}

static void expand(unsigned char *code, long codesize, long memsize)
{
  ucell c;
  struct {
    long memloc;
    ucell c;
  } spare[AMX_COMPACTMARGIN];
  int sh=0,st=0,sc=0;
  int shift;

  /* for in-place expansion, move from the end backward */
  assert(memsize % sizeof(cell) == 0);
  while (codesize>0) {
    c=0;
    shift=0;
    do {
      codesize--;
      /* no input byte should be shifted out completely */
      assert(shift<8*sizeof(cell));
      /* we work from the end of a sequence backwards; the final code in
       * a sequence may not have the continuation bit set */
      assert(shift>0 || (code[(size_t)codesize] & 0x80)==0);
      c|=(ucell)(code[(size_t)codesize] & 0x7f) << shift;
      shift+=7;
    } while (codesize>0 && (code[(size_t)codesize-1] & 0x80)!=0);
    /* sign expand */
    if ((code[(size_t)codesize] & 0x40)!=0) {
      while (shift < (int)(8*sizeof(cell))) {
        c|=(ucell)0xff << shift;
        shift+=8;
      } /* while */
    } /* if */
    /* store */
    while (sc && (spare[sh].memloc>codesize)) {
      *(ucell *)(code+(int)spare[sh].memloc)=spare[sh].c;
      sh=(sh+1)%AMX_COMPACTMARGIN;
      sc--;
    } /* while */
    memsize -= sizeof(cell);
    assert(memsize>=0);
    if ((memsize>codesize)||((memsize==codesize)&&(memsize==0))) {
      *(ucell *)(code+(size_t)memsize)=c;
    } else {
      assert(sc<AMX_COMPACTMARGIN);
      spare[st].memloc=memsize;
      spare[st].c=c;
      st=(st+1)%AMX_COMPACTMARGIN;
      sc++;
    } /* if */
  } /* while */
  /* when all bytes have been expanded, the complete memory block should be done */
  assert(memsize==0);
}

static void addchars(char *str,cell value,int pos)
{
  int v,i;

  str+=pos*sizeof(cell);
  for (i=0; i<sizeof(cell); i++) {
    v=(value >> 8*(sizeof(cell)-1)) & 0xff;
    value <<= 8;
    *str++= (v>=32) ? (char)v : ' ';
  } /* for */
  *str='\0';
}

int main(int argc,char *argv[])
{
  char name[_MAX_PATH];
  FILE *fplist;
  int codesize,count;
  cell *code,*cip;
  OPCODE_PROC func;

  if (argc<2 || argc>3) {
    printf("Usage: pawndisasm <input> [output]\n");
    return 1;
  } /* if */
  if (argc==2) {
    char *ptr;
    strcpy(name,argv[1]);
    if ((ptr=strrchr(name,'.'))!=NULL && strpbrk(ptr,"\\/:")==NULL)
      *ptr='\0';          /* erase existing extension */
    strcat(name,".lst");  /* append new extension */
  } else {
    strcpy(name,argv[2]);
  } /* if */
  if ((fpamx=fopen(argv[1],"rb"))==NULL) {
    printf("Unable to open input file \"%s\"\n",argv[1]);
    return 1;
  } /* if */
  if ((fplist=fopen(name,"wt"))==NULL) {
    printf("Unable to create output file \"%s\"\n",name);
    return 1;
  } /* if */

  /* load header */
  fread(&amxhdr,sizeof amxhdr,1,fpamx);
  if (amxhdr.magic!=AMX_MAGIC) {
    printf("Not a valid AMX file\n");
    return 1;
  } /* if */
  codesize=amxhdr.hea-amxhdr.cod; /* size for both code and data */
  fprintf(fplist,";File version: %d\n",amxhdr.file_version);
  fprintf(fplist,";Flags:        ");
  if ((amxhdr.flags & AMX_FLAG_COMPACT)!=0)
    fprintf(fplist,"compact-encoding ");
  if ((amxhdr.flags & AMX_FLAG_OVERLAY)!=0)
    fprintf(fplist,"overlays ");
  if ((amxhdr.flags & AMX_FLAG_DEBUG)!=0)
    fprintf(fplist,"debug-info ");
  fprintf(fplist,"\n\n");
  /* load the code block */
  if ((code=malloc(codesize))==NULL) {
    printf("Insufficient memory: need %d bytes\n",codesize);
    return 1;
  } /* if */

  /* read and expand the file */
  fseek(fpamx,amxhdr.cod,SEEK_SET);
  fread(code,1,codesize,fpamx);
  if ((amxhdr.flags & AMX_FLAG_COMPACT)!=0)
     expand((unsigned char *)code, amxhdr.size - amxhdr.cod, amxhdr.hea - amxhdr.cod);

  /* do a first run through the code to get jump targets (for labels) */
  //???

  /* browse through the code */
  cip=code;
  codesize=amxhdr.dat-amxhdr.cod;
  while (((unsigned char*)cip-(unsigned char*)code)<codesize) {
    func=opcodelist[(int)(*cip&0x0000ffff)].func;
    cip+=func(fplist,cip+1,*cip,(cell)(cip-code)*sizeof(cell));
  } /* while */

  /* dump the data section too */
  fprintf(fplist,"\n\n;DATA");
  cip=(cell*)((unsigned char*)code+(amxhdr.dat-amxhdr.cod));
  codesize=amxhdr.hea-amxhdr.cod;
  count=0;
  name[0]='\0';
  while (((unsigned char*)cip-(unsigned char*)code)<codesize) {
    if (count==0) {
      if (strlen(name)>0) {
        fprintf(fplist," %s",name);
        name[0]='\0';
      } /* if */
      fprintf(fplist,"\n%08lx  ",(long)((cip-code)*sizeof(cell)-(amxhdr.dat-amxhdr.cod)));
    } /* if */
    fprintf(fplist,"%08lx ",(long)*cip);
    addchars(name,*cip,count);
    count=(count+1) % 4;
    cip++;
  } /* while */
  if (strlen(name)>0) {
    fprintf(fplist," %s",name);
    name[0]='\0';
  } /* if */

  free(code);
  fclose(fpamx);
  fclose(fplist);
  return 0;
}
