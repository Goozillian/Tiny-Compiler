#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "符号表.h"
enum Type{KEYWORD,IDENTIFIER,NUMBER,NATIVE};
int lineno=1; //行号可以不必取整行之fgets，只要扫描到\n就自增1就是
int count=0; //定位到一行中的某个单词
char scanner; //字符扫描器
char set[88];
int location=0; //字符搜集器
int type=0; //类型标志数，初始化为0是为了与其他标志规避
char *keyword[]={
"if","then","else","end","repeat","until",
"read","write"};//建立关键字指针数组
char *native[]={
"+","-","*","/","=","<","(",")",";",
":="};
/*其实上述两个表也是符号表*/
int is_keyword(char str[]) //标识符匹配
{ 
  int i;
  for(i=0;i<8;i++)if(strcmp(keyword[i],str)==0)return 1;	   
  return 0;
} 
int is_native(char str[])
{
  int i;
  for(i=0;i<10;i++)if(strcmp(native[i],str))return i+1;//这里本来是返回i，但当i=0则程序会出错
  return 0;
}

int go_on_0=1; //处理自动机状态结3和4接收到非字母非数字非空格者而掠过的bug
int jump=0; //自动机状态结选择器，本来是叫goto，但由于有goto关键字所以报错，而这种错往往比较迷惑人
int ZERO(FILE *source,FILE *intermediate_code)
{
  if(go_on_0)scanner=fgetc(source);
  go_on_0=1;
  if(scanner==' '||scanner=='\n')
  {
    if(scanner=='\n')
    {
      lineno++;
      count=0;
    }
    jump=0;
  }
  else if(scanner=='{')jump=1;
  else if(scanner==':'){jump=2;set[location++]=scanner;}
  else if(isalpha(scanner)){jump=3;set[location++]=scanner;type=1;} //本来是isalnum()之以为就是判字母的，其实是判字母或数字，所以出bug后百思不得其解，这种bug很难发现。也像双重循环之本来是for(i=1;i<=100;i++)for(j=1;j<=100;j++)而错写成了for(i=1;i<=100;i++)for(j=1;i<=100;j++)之这样的错误比逻辑错误更难发现
  else if(isdigit(scanner)){jump=4;set[location++]=scanner;type=2;} //虽然case不能直接接判断条件而只能是常量，但可以直接给case接一个常量，然后执行if语句之巧妙模拟case直接接判断条件
  else {jump=5;if(scanner!=EOF)set[location++]=scanner;}
  return 0;
}
int ONE(FILE *source,FILE *intermediate_code)
{
  scanner=fgetc(source);
  if(scanner=='\n')
  {
    lineno++;
    count=0;
  }
  if(scanner=='}')jump=0;
  else jump=1;
  return 0;
}
int TWO(FILE *source,FILE *intermediate_code)
{
  scanner=fgetc(source);
  if(scanner=='='){jump=5;set[location++]=scanner;}
  else jump=5;
  return 0;
}
int THREE(FILE *source,FILE *intermediate_code)
{
  scanner=fgetc(source);
  if(isalpha(scanner))
    if(location<88)
    {
      jump=3;
      set[location++]=scanner;
    }
    else 
    {
      fprintf(intermediate_code,"(标识符长度超限制！)");
      exit(1);
    }
  else 
  {
    go_on_0=0;
    jump=5;
  }
  return 0;
}
int FOUR(FILE *source,FILE *intermediate_code)
{
  scanner=fgetc(source);
  if(isdigit(scanner))
    if(location<88)
    {
      jump=4;
      set[location++]=scanner;
    }
    else
    {
      fprintf(intermediate_code,"(数字长度超限制！)");
      exit(1);
    }
  else 
  {
    go_on_0=0;
    jump=5;
  }
  return 0;
}
int FIVE(FILE *source,FILE *intermediate_code)
{
  set[location++]='\0'; //去掉多余空格简易方法而不需要实现不等长数组。这就像在不允许使用变量定义数组之int a[n];则可定义一个足够大的数组int a[MAX]，虽然空间浪费点但也可以接受了
  count++; //生成一个单词后当行当前单词序号自增1
  //jump=location=0;
  location=0;
  if(type==2)fprintf(intermediate_code,"(%d,%s)",NUMBER,set);
  else if(is_keyword(set))fprintf(intermediate_code,"(%d,%s)",KEYWORD,set);
  else if(type==1)
  {
    fprintf(intermediate_code,"(%d,%s)",IDENTIFIER,set);
    st_insert(set);
  }
  else fprintf(intermediate_code,"(%d,%s)",NATIVE,set);
  //type=0;
  //int i;
  //for(i=0;i<88;i++)set[i]=' ';
  return 0;
}
int (*state[])(FILE *source,FILE *intermediate_code)={ZERO,ONE,TWO,THREE,FOUR,FIVE}; //不用函数指针就是直接在状态结函数中调用其他状态结函数了，用函数指针更灵活之广义变量把离散元素编制化处理

char *next_word(FILE *source,FILE *intermediate_code)
{
  do{
    state[jump](source,intermediate_code);
  }while(jump!=5);
  state[jump](source,intermediate_code); //简单处理，当然可以加设标志位压缩之统一代码
  return set;
}
