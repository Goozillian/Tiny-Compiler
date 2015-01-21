#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "词法分析.h"
#include "语法分析.h"
#include "符号表.h"
int go_on_1=1; //确定哪些情况不需要读取单词
int _type=0;
char word[88];
void Program(FILE *source,FILE *intermediate_code);
void stmt_sequence(FILE *source,FILE *intermediate_code);
void _stmt_sequence(FILE *source,FILE *intermediate_code);
void statement(FILE *source,FILE *intermediate_code);
void if_stmt(FILE *source,FILE *intermediate_code);
void _if_stmt(FILE *source,FILE *intermediate_code);
void repeat_stmt(FILE *source,FILE *intermediate_code);
void assign_stmt(FILE *source,FILE *intermediate_code);
void read_stmt(FILE *source,FILE *intermediate_code);
void write_stmt(FILE *source,FILE *intermediate_code);
void exp(FILE *source,FILE *intermediate_code);
void _exp(FILE *source,FILE *intermediate_code);
void simple_exp(FILE *source,FILE *intermediate_code);
void _simple_exp(FILE *source,FILE *intermediate_code);
void term(FILE *source,FILE *intermediate_code);
void _term(FILE *source,FILE *intermediate_code);
void factor(FILE *source,FILE *intermediate_code);
int get_word(FILE *source,FILE *intermediate_code)
{
  next_word(source,intermediate_code);
  int i;
  for(i=0;i<88;i++)word[i]=set[i];
  int _type=type;
  type=jump=0;
  printf("%s\n",word);
  for(i=0;i<88;i++)set[i]=' '; //本来是在词法分析.c里的，跟语法分析.c合成后就移动到这里来了
  return _type;
}
void error()//char *maybe(char *s) //可设立相似算法，之当关键字拼写错误则提示可能的本来要拼写的关键字
{
  printf("第%d行有语法错误！\n",lineno);
  printf("该行第%d个符号%s不符合预期！\n",count,word);
  exit(1);
}
void Program(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"01 "); //这里固化编码就好，使用变量自动的话似乎大材小用而且不能体现其紧密联系
  ListInsert_Pro(1,Terminal_Symbol,"><; ");
  stmt_sequence(source,intermediate_code);
  if(!strcmp(word,""))
  {
    printf("文法分析通过！\n");
    fprintf(intermediate_code,"(文法分析通过！)\n");
  }
  else 
  {
    printf("错误1：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void stmt_sequence(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"02 ");
  ListInsert_Pro(1,Terminal_Symbol,"><>< ");
  statement(source,intermediate_code);
  _stmt_sequence(source,intermediate_code);
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void _stmt_sequence(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"03 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  //printf("%s\n",word);
  if(!strcmp(word,"end")||!strcmp(word,"else")||!strcmp(word,"until")) //处理空规则
  {
    ListInsert_Pro(1,Terminal_Symbol,"NULL");
    ListInsert_Pro(1,Terminal_Symbol,"}"); //之前没有发现这些地方遗漏了这个导致二级中间代码生成失败，这种bug也算是高级bug之认识性bug之很难发现
    go_on_1=0;
    return;
  }
  else if(!strcmp(word,";")) //这里的";"并没有作用于_type之这也是为什么会有_type异常之bug，所以有go_on_1的地方都应该有_type。_type异常之bug困扰了己一天，这种bug是由于对go_on_1和_type控制机制掌握得不够全面而导致的，值得谨记
  {
    if(go_on_1)_type=get_word(source,intermediate_code);
    go_on_1=1;
    if(!strcmp(word,"")) //扫描完成才会出现word=""，所以可以注释掉里面的go_on_1=0;
    {
      //go_on_1=0;
      ListInsert_Pro(1,Terminal_Symbol,"}");
      return;
    }
    go_on_1=0;
    ListInsert_Pro(1,Terminal_Symbol,";><>< ");
    statement(source,intermediate_code);
    _stmt_sequence(source,intermediate_code);
  }
  else 
  {
    printf("错误2：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void statement(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"04 ");
  ListInsert_Pro(1,Terminal_Symbol,">< ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  //printf("%s\n",word);
  if(!strcmp(word,"if"))if_stmt(source,intermediate_code);
  else if(!strcmp(word,"repeat"))repeat_stmt(source,intermediate_code);
  else if(!strcmp(word,"read"))read_stmt(source,intermediate_code);
  else if(!strcmp(word,"write"))write_stmt(source,intermediate_code);
  else if(_type==1)assign_stmt(source,intermediate_code);
  else 
  {
    printf("错误3：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void if_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"05 ");
  ListInsert_Pro(1,Terminal_Symbol,"if><then><>< ");
  exp(source,intermediate_code);
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"then"))
  {
    stmt_sequence(source,intermediate_code);
    _if_stmt(source,intermediate_code);
  }
  else 
  {
    printf("错误4：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void _if_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"06 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"end"))
  {
    //ListInsert_Pro(1,Terminal_Symbol,"end ");
    ListInsert_Pro(1,Terminal_Symbol,"}"); //这里也会有{}之不只是扫描结束
    return;
  }
  else if(!strcmp(word,"else"))
  {
    ListInsert_Pro(1,Terminal_Symbol,"else><end ");
    stmt_sequence(source,intermediate_code);
    ListInsert_Pro(1,Terminal_Symbol,"}");
    if(go_on_1)_type=get_word(source,intermediate_code);
    go_on_1=1;
    if(!strcmp(word,"end"))return;
    else 
    {
      printf("错误5：\t");
      error();
    }
  }
  else 
  {
    printf("错误6：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void repeat_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"07 ");
  ListInsert_Pro(1,Terminal_Symbol,"repeat><until>< ");
  stmt_sequence(source,intermediate_code);
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"until"))exp(source,intermediate_code);
  else 
  {
    printf("错误7：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void assign_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"08 ");
  ListInsert_Pro(1,Terminal_Symbol,":=>< ");
  printf("=====\n");
  char _word[88];
  sprintf(_word,"%s",word); //记录当前word，因为后面要用到，而word是全局变量下面exp()会改变其中数据
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,":="))exp(source,intermediate_code);
  else 
  {
    printf("错误8：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(1,Terminal_Symbol,_word); //本来word是放在:=><前面跟:><直接通连作为一个整体函数名的，不过看来还是不妥要分开。放在这里是为了中间代码生成方便
  //ListInsert_Pro(1,Terminal_Symbol," ");
  ListInsert_Pro(1,Terminal_Symbol,"}");
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void read_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"09 ");
  ListInsert_Pro(1,Terminal_Symbol,"read>< ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(_type==1)
  {
    ListInsert_Pro(1,Terminal_Symbol,"{");
    ListInsert_Pro(1,Terminal_Symbol,word);
    //ListInsert_Pro(1,Terminal_Symbol," ");
    ListInsert_Pro(1,Terminal_Symbol,"}");
    //ListInsert_Pro(1,Terminal_Symbol," ");
    ListInsert_Pro(1,Terminal_Symbol,"}");
    return;
  }
  else 
  {
    printf("错误9：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void write_stmt(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"10 ");
  ListInsert_Pro(1,Terminal_Symbol,"write>< ");
  exp(source,intermediate_code);
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void exp(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"11 ");
  ListInsert_Pro(1,Terminal_Symbol,"><>< ");
  simple_exp(source,intermediate_code);
  _exp(source,intermediate_code);
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void _exp(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"12 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"<")||!strcmp(word,"="))
  {
    ListInsert_Pro(1,Terminal_Symbol,word);
    ListInsert_Pro(1,Terminal_Symbol,">< ");
    simple_exp(source,intermediate_code); //词法分析把底层文法分析透明化了。之前这里把simple_exp错写成_simple_exp，则产生了莫名其妙的错误，这种拼写错误其实比逻辑错误更棘手之毕竟下意识中并没有写错，而只有突破界限意识到不是算法问题而是拼写问题代码问题什么才能修复bug。
  }
  //else if(!strcmp(word,""))return; //不进行完一趟词法分析是不会出现""的
  else
  {
    ListInsert_Pro(1,Terminal_Symbol,"NULL");
    go_on_1=0;
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void simple_exp(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"13 ");
  ListInsert_Pro(1,Terminal_Symbol,"><>< ");
  term(source,intermediate_code);
  _simple_exp(source,intermediate_code);
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void _simple_exp(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"14 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"+")||!strcmp(word,"-"))
  {
    ListInsert_Pro(1,Terminal_Symbol,word);
    ListInsert_Pro(1,Terminal_Symbol,"><>< ");
    term(source,intermediate_code);
    _simple_exp(source,intermediate_code);
  }
  //else if(!strcmp(word,""))return;
  else 
  {
    ListInsert_Pro(1,Terminal_Symbol,"NULL");
    go_on_1=0;
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void term(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"15 ");
  ListInsert_Pro(1,Terminal_Symbol,"><>< ");
  factor(source,intermediate_code);
  _term(source,intermediate_code);
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void _term(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"16 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"*")||!strcmp(word,"/"))
  {
    ListInsert_Pro(1,Terminal_Symbol,word);
    ListInsert_Pro(1,Terminal_Symbol,"><>< ");
    factor(source,intermediate_code);
    _term(source,intermediate_code);
  }
  //else if(!strcmp(word,""))return;
  else
  {
    ListInsert_Pro(1,Terminal_Symbol,"NULL");
    go_on_1=0;
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void factor(FILE *source,FILE *intermediate_code)
{
  ListInsert_Pro(1,Terminal_Symbol,"{");
  ListInsert_Pro(0,Track_Vector,"17 ");
  printf("=====\n");
  if(go_on_1)_type=get_word(source,intermediate_code);
  go_on_1=1;
  if(!strcmp(word,"("))
  {
    ListInsert_Pro(1,Terminal_Symbol,"(><) ");
    exp(source,intermediate_code);
    ListInsert_Pro(1,Terminal_Symbol,"}");
    printf("=====\n");
    if(go_on_1)_type=get_word(source,intermediate_code);
    go_on_1=1;
    if(!strcmp(word,")"))return;
    else 
    {
      printf("错误13：\t");
      error();
    }
  }
  else if(_type==1||_type==2)
  {
    ListInsert_Pro(1,Terminal_Symbol,word);
    //ListInsert_Pro(1,Terminal_Symbol," ");
    ListInsert_Pro(1,Terminal_Symbol,"}");
    return;
  }
  else 
  {
    printf("错误14：\t");
    error();
  }
  ListInsert_Pro(1,Terminal_Symbol,"}");
}
void parse(FILE *source,FILE *intermediate_code)
{
  Program(source,intermediate_code);
}
