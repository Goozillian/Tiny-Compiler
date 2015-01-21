/*这里实际就是认为编译就是若干级中间代码生成*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "语法分析.h"
#include "符号表.h" //之前是#include "语义分析.h"放在这个前面导致int ScanSymtab(BucketList hashTable[])里的BucketList类型未定义，但编译时又只是提示ScanSymtab can not be used as function之莫名其妙让己困惑很久，而#include最好不要出现在头文件，一是为灵活二是防冲突，所以解决方法是换位置
#include "语义分析.h"
void toSet(LinkList ThirdStepCode,char *&fuc,int &Begin,int &End);
char *sfuc;
int sBegin,sEnd;
void genFirstStepCode(LinkList ZerothStepCode,LinkList &FirstStepCode)
{
  parse(source,intermediate_code); //语法分析或者说文法分析即第一步中间代码生成之词法分析生成的单词序列算是第零步之第零级中间代码
}
void genSecondStepCode(LinkList FirstStepCode,LinkList &SecondStepCode)
{
  LinkList p=FirstStepCode->next;
  int Begin,End,Match,No;
  Begin=End=Match=No=0;
  while(p)
  {
    if(p->data=='{')
    {
      if(Match==0)
      {
        Begin=No;
        ListInsert_Pro(2,SecondStepCode,toString('('));
        /*生成前缀式函数名*/
        do{
          if(p->next->data=='}')break; //处理语法分析结束标志之空字符串之在一级中间代码中就是"{}"。本来没有这句之这是导致段错误的一个bug，由于在修复语法分析设计上的bug前己就发现了这个bug，所以当修复完语法分析设计上的bug发现还有段错误后己就敏锐地把目光投向了这里，果然加了这句二级代码生成就大功告成了
          p=p->next;
          No++;
          ListInsert_Pro(2,SecondStepCode,toString(p->data));
        }while(p->next->data!=' ');
      }
      Match++;
    }
    else if(p->data=='}')
    {
      /*当扫描到之前生成的前缀式的右花括号则掠过*/
      if(Match==0)
      {
        p=p->next;
        No++;
        continue;
      }
      Match--;
      if(Match==0)
      {
        End=No;
        No=Begin;
        char a[88],b[88]; //这个当然限制了单个程序的大小
        sprintf(a,"%d",Begin);
        sprintf(b,"%d",End);
        ListInsert_Pro(2,SecondStepCode,toString(','));
        ListInsert_Pro(2,SecondStepCode,toString('['));
        ListInsert_Pro(2,SecondStepCode,a);
        ListInsert_Pro(2,SecondStepCode,toString(','));
        ListInsert_Pro(2,SecondStepCode,b);
        ListInsert_Pro(2,SecondStepCode,toString(']'));
        ListInsert_Pro(2,SecondStepCode,toString(')'));
        /*生成下一个前缀式*/
        int i;
        p=FirstStepCode->next; //本来是p=FirstStepCode;之由于下面for循环的机制则出现了bug
        for(i=0;i<Begin;i++)p=p->next;
      }
    }
    //fprintf(asm_main,"Match=%d\n",Match);
    //fprintf(asm_main,"p->data=%c\n",p->data);
    p=p->next;
    No++;
  }
}
void genThirdStepCode(LinkList SecondStepCode,LinkList &ThirdStepCode)
{
  char prefix[22];
  toSet(SecondStepCode,sfuc,sBegin,sEnd); //获取程序覆盖范围
  LinkList p=SecondStepCode->next;
  while(p)
  {
    int i=0;
    p=p->next; //掠过'('
    while(p->data!=',')
    {
      prefix[i++]=p->data;
      p=p->next;
    }
    prefix[i]='\0';
    if(strcmp(prefix,"><;")&&strcmp(prefix,"><><")&&strcmp(prefix,";><><")&&strcmp(prefix,"NULL")&&strcmp(prefix,"><")&&strcmp(prefix,"><;")&&strcmp(prefix,"")) //顺便在此删除空函数名前缀式
    {
      ListInsert_Pro(3,ThirdStepCode,toString('('));
      ListInsert_Pro(3,ThirdStepCode,prefix);
      while(p->data!='(')
      {
        ListInsert_Pro(3,ThirdStepCode,toString(p->data));
        p=p->next;
        //if(!p)break; //本来没有这一句也段错误，看来语法分析结束标志之空字符串带来的影响深远啊
      }
    }
    else 
    {
      while(p->data!='(')
      {
        p=p->next;
        if(!p)break; //这句本来是放在上一个分条件语句中，在if判别式中加入删除空函数名前缀式特性后就要移到这里来了
      }
    }
  }
}
void genFourthStepCode(LinkList ThirdStepCode,LinkList &FourthStepCode);
/*第一个要生成三元式的肯定是Stmt，之后要生成什么的三元式由Stmt唯一确定，事实上完整生成一个Stmt的三元式之后下一个要生成三元式的也是Stmt，这样只要提供起始位置就可以调用genExp()和genStmt()进行四级中间代码生成之进行三元式生成了，这里是以单个前缀式为基本单位进行三元式生成。本来是设置一个loc之loc随着单个前缀式的使用而递增来指示起始位置，现在直接把使用过的前缀式从三级中间代码中删除，这样loc就永远是0了也就可以省了这一步了*/
int No=0; //三元式编号
char _No[88]; //字符串形式三元式编号
char all[100]; //genExp()中的字符串存放器
int r[4]={}; //四个32位寄存器
void toSet(LinkList ThirdStepCode,char *&fuc,int &Begin,int &End) //从三级中间代码中的各前缀式提取数据，这里的fuc指针果然需要引用，其实参还不能被定义为数组
{
  static char prefix[100]; //这里本来是设为prefix[22]，现在认为不够大之应该比标识符长度之88大
  LinkList p=ThirdStepCode->next;
  int i=0;
  p=p->next; //掠过'('
  ListDelete(ThirdStepCode);
  while(p->data!=',')
  {
    prefix[i++]=p->data;
    p=p->next;
    ListDelete(ThirdStepCode);
  }
  prefix[i]='\0';
  fuc=prefix;
  p=p->next->next; //p指向作用域左边界
  ListDelete(ThirdStepCode);ListDelete(ThirdStepCode);
  char number[88];
  i=0;
  while(p->data!=',')
  {
    number[i++]=p->data;
    p=p->next;
    ListDelete(ThirdStepCode);
  }
  number[i]='\0';
  Begin=atoi(number);
  p=p->next;
  ListDelete(ThirdStepCode);
  i=0;
  while(p->data!=']')
  {
    number[i++]=p->data;
    p=p->next;
    ListDelete(ThirdStepCode);
  }
  number[i]='\0';
  End=atoi(number);
  p=p->next->next;
  ListDelete(ThirdStepCode);ListDelete(ThirdStepCode);
}
char *fuc;
int Begin,End;
//char s_result[16]; //本来是s_result[6]之数组大小不能满足程序需求则数组越界，或许正是由于数组越界才导致了程序莫名其妙的输出之覆盖了本程序的其余数据。这个bug值得谨记
int level=0; //表达式层级，记录genExp()使用到了哪些寄存器。0表示exp只有一个factor；1表示exp只有一个连乘；2表示exp只有一个连加（也即exp就是simple-exp）；3表示_exp非空。其中r[0]是专用factor采集寄存器，r[1]是专用term采集寄存器，r[2]是专用simple-exp采集寄存器，r[3]是专用exp采集寄存器。r[0..3]分别对应汇编模型的eax、ebx、ecx、edx
char *toRegister(char *r) //局部模型对接引擎或者说狭义模型对接模型。使用转换的原因是要用表达式层级之level选中寄存器，这样就要把寄存器存入广义变量之编制化
{
  static char Register[4];
  if(!strcmp(r,"r[0]"))strcpy(Register,"eax");
  else if(!strcmp(r,"r[1]"))strcpy(Register,"ebx");
  else if(!strcmp(r,"r[2]"))strcpy(Register,"ecx");
  else if(!strcmp(r,"r[3]"))strcpy(Register,"edx");
  return Register;
}
void genExp(LinkList ThirdStepCode,LinkList &FourthStepCode,int ExpEnd) //这里有一个难点，一般式需要存储三个Exp的值，而Exp的值是由同一个函数确定存储位置的，这似乎陷入绝路了，当然静态变量是一条活路，或者也可以增加参数之调用genExp()时可以分配不同的参数，这些都是建设性的解决方案。这里采用第三种方案，之r[0]、r[1]用来提取factor的值，r[2]、r[3]、r[4]分别用来存储三个Exp的值之作为参数用来存储公共Exp返回值之，r[5]用来存储第一个根Exp的值，之result用来指定genExp()用来存储Exp值的寄存器编号。不过现在认为选择另一种方案会更好，之采用汇编栈思想伪动态拓展寄存器数量，这样也能更自然地配合递归，这实在是无与伦比的绝妙之精妙绝伦。
{ //其实这里使用三级递归这是之前己也没有想到的
  //fprintf(asm_main,"%s,%d,%d\n",fuc,Begin,End);
  //while(Begin<ExpEnd) //这样设计就可以完美解决去除框架性前缀式后Stmt与Exp的边界划分问题。这里要注意之由于Exp跟Stmt不同之Exp在文法上是被分成若干形式化的部分的，所以不能像Stmt那样处理，至少要进行适当修改。与由于本函数的机制，这里用if甚至不用都可以，之毕竟里面是只运行一次的
  //{
    /*[No++](:=,r[0],0)*/ //这句初始化r[0]，但放在这里由于递归就可能会被执行多次，所以不能放这里。然而事实是寄存器不需要初始化，只不过本来是设计成[No++](+,r[0],0)之思维惰，所以才需要初始化。
    //ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(:=,r[0],0)");
    if(!strcmp(fuc,"(><)"))
    {
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      //else return;
      /*[No++](save,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(save,/,/)");
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      /*[No++](:=,[exp],r[level])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(:=,[exp],%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);
      /*[No++](restore,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(restore,/,/)");
      /*[No++](:=,r[0],[exp])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(all,"(:=,%s,[exp])",toRegister("r[0]"));ListInsert_Pro(4,FourthStepCode,all);
    }
    else 
    {
      if(isalpha(fuc[0]))
      {
        /*[No++](:=,r[0],[fuc])*/
        ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"Variable%d",st_lookup(fuc));sprintf(all,"(:=,%s,[%s])",toRegister("r[0]"),_No);ListInsert_Pro(4,FourthStepCode,all);
      }
      else if(isdigit(fuc[0]))
      {
        /*[No++](:=,r[0],fuc)*/
        ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(all,"(:=,%s,%s)",toRegister("r[0]"),fuc);ListInsert_Pro(4,FourthStepCode,all);
      }
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      //else return; //不需要return，而且会影响到level的更新
    }
    level=0;
    if((!strcmp(fuc,"*><><")||!strcmp(fuc,"/><><")||!strcmp(fuc,"+><><")||!strcmp(fuc,"-><><")||!strcmp(fuc,"<><")||!strcmp(fuc,"=><"))&&Begin<ExpEnd) //原来复制粘贴的时候多了些'('，这样就导致程序逻辑失控了，只是己还以为是正确的，这种错误是很难发现的
    {
      /*[No++](:=,r[1],r[0])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"(:=,%s,",toRegister("r[1]"));sprintf(all,"%s)",toRegister("r[0]"));ListInsert_Pro(4,FourthStepCode,_No); ListInsert_Pro(4,FourthStepCode,all); //本来是sprintf(all,"(:=,%s,%s)",toRegister("r[1]"),toRegister("r[0]"));之sprintf()里的两个toRegister()返回的字符串一样，这不可能，应该是函数处理参数是从右到左处理，先从左到右依次调用这里的两个toRegister()然后再统一取返回值，所以两者会一样
    }
    else return;
    
    while((!strcmp(fuc,"*><><")||!strcmp(fuc,"/><><"))&&Begin<ExpEnd)
    {
      char _fuc[22];
      sprintf(_fuc,"%s",fuc);
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      //else return;
      /*[No++](save,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(save,/,/)");
      genExp(ThirdStepCode,FourthStepCode,tempEnd); //不断压缩代码之多重递归之本来是把前面的代码直接复制过来的，然后逐渐转换成递归形式之简洁明了
      /*[No++](:=,[exp],r[level])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(:=,[exp],%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);
      /*[No++](restore,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(restore,/,/)");
      
      /*[No++](mulop,r[1],[exp])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"%s",toString(_fuc[0]));sprintf(all,"(%s,%s,[exp])",_No,toRegister("r[1]"));ListInsert_Pro(4,FourthStepCode,all);
    }
    level=1;
    if((!strcmp(fuc,"+><><")||!strcmp(fuc,"-><><")||!strcmp(fuc,"<><")||!strcmp(fuc,"=><"))&&Begin<ExpEnd)
    {
      /*[No++](:=,r[2],r[1])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"(:=,%s,",toRegister("r[2]"));sprintf(all,"%s)",toRegister("r[1]"));ListInsert_Pro(4,FourthStepCode,_No); ListInsert_Pro(4,FourthStepCode,all); //这里本来错写成R[1]则导致莫名其妙的错误，这种错误是很难发现的之要进行自我怀疑否定。类似的错误还有似修改程序后没有保存或没有编译或编译时有点小毛病没通过而保留了上次编译通过的可执行文件。
    }
    else return;
    
    while((!strcmp(fuc,"+><><")||!strcmp(fuc,"-><><"))&&Begin<ExpEnd)
    {
      char _fuc[22];
      sprintf(_fuc,"%s",fuc);
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      //else return;
      /*[No++](save,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(save,/,/)");
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      /*[No++](:=,[exp],r[level])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(:=,[exp],%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);
      /*[No++](restore,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(restore,/,/)");
      
      /*[No++](addop,r[2],[exp])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"%s",toString(_fuc[0]));sprintf(all,"(%s,%s,[exp])",_No,toRegister("r[2]"));ListInsert_Pro(4,FourthStepCode,all);
    }
    level=2;
    if((!strcmp(fuc,"<><")||!strcmp(fuc,"=><"))&&Begin<ExpEnd)
    {
      /*[No++](:=,r[3],r[2])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"(:=,%s,",toRegister("r[3]"));sprintf(all,"%s)",toRegister("r[2]"));ListInsert_Pro(4,FourthStepCode,_No); ListInsert_Pro(4,FourthStepCode,all); 
    }
    else return;
    
    if((!strcmp(fuc,"<><")||!strcmp(fuc,"=><"))&&Begin<ExpEnd) //这里原来是if((!strcmp(fuc,"<><><")||!strcmp(fuc,"=><><"))&&Begin<ExpEnd)之记错了语法设置，所以出错了让己好找。当然这里用while也可以
    {
      char _fuc[22];
      sprintf(_fuc,"%s",fuc);
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      //else return;
      /*[No++](save,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(save,/,/)");
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      /*[No++](:=,[exp],r[level])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(:=,[exp],%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);
      /*[No++](restore,/,/)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(restore,/,/)");
      /*[No++](comparison-op,r[3],[exp])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No++);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"%s",toString(_fuc[0]));sprintf(all,"(%s,%s,[exp])",_No,toRegister("r[3]"));ListInsert_Pro(4,FourthStepCode,all);
      //break;
    }
    level=3;
    
    //else break; //扫描到非上述七类之也即Exp系前缀式，也即扫描到Stmt系前缀式，说明扫描到边界，则退出循环
    //if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
    //else break;//else Begin=ExpEnd; //这个很有想法
  //}
}
void genStmt(LinkList ThirdStepCode,LinkList &FourthStepCode,int StmtEnd)
{
  //fprintf(asm_main,"%s,%d,%d\n",fuc,Begin,End);
  //int A,B;/*双指针所在三元式编号*/
  //int C,D;
  while(Begin<StmtEnd)
  {
    if(!strcmp(fuc,"if><then><><"))
    {
      int A=No++,B=No++;
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      int temp_level=level;
      
      /*[No++](jz,r[level],A)*/ //这种提高抽象程度之小公式到大公式的注释是最好的注释
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(jz,%s,",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);sprintf(_No,"%d)",A);ListInsert_Pro(4,FourthStepCode,_No);No++;
      
      while(strcmp(fuc,"else><end")) //这个简单化解了在撤除框架三元式后if><then><><中的genStmt()和else><end中的genStmt()的界限划分问题，如果用其他方法的话可能会很麻烦，甚至说一开始己甚至觉得这是不可能的之当然在计算机里没有什么不可能。与这里只有两个分支，这样的设计当然就是说二叉树跟多叉树的处理能力是一样的，而且更精致，像RISC与CISC之间的区别那样，像英语与汉语之间的区别那样，之树的模型或者说数据结构的模型跟集合的模型很接近，所以这些模型都具有极高的普适性。似genExp()那样，连加减连乘除这些也不是必需的，只允许单加减单乘除而factor允许是exp之递归这样可以实现一样的效果，只不过书写的时候就要多写几层括号了，这样实现起来也没那么困难，与实际可以实现单加减单乘除，然后把连加减连乘除编译之通过添加括号成为该函数合法输入，这样其实是更简明的编程思想，之广义编码思想，当然也是最简伪模型思想。此二者之所以如此相似是因为其模型是一样的
      {
        int tempEnd=End;
        genStmt(ThirdStepCode,FourthStepCode,tempEnd);
      }
      
      /*[No++](jmp,/,A)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",A);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,")");No++;
      
      /*[A](StackIn,/,r[temp_level])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",A);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(StackIn,/,");sprintf(_No,"r[%d]",temp_level);sprintf(all,"%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);
      
      /*[No++](jmp,/,B)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",B);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4, FourthStepCode,")");No++;
      
      /*[B](nop,/,/)*/ //这种提高抽象程度之小公式到大公式的注释是最好的注释
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",B);/*这里本来是sprintf(_No,"%d",No++);但由于sprintf()是先从右到左把所有作为参数的函数调用调用完毕再采集参数，所以这里不能No++*/ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(nop,/,/)");
    }
    else if(!strcmp(fuc,"else><end"))
    {
      int A=No++,B=No++;
      int tempEnd=End;
      
      /*[No++](StackOut,/,r[0])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",0);sprintf(all,"(StackOut,/,%s)",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);No++;
      
      /*[No++](jz,r[0],A)*/ //这种提高抽象程度之小公式到大公式的注释是最好的注释
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",0);sprintf(all,"(jz,%s,",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);sprintf(_No,"%d)",A);ListInsert_Pro(4,FourthStepCode,_No);No++;
      
      /*[No++](jmp,/,B)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",B);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,")");No++;
      
      /*[A](nop,/,/)*/ //原来是/*[A](jmp,/,No)之所谓指向下一个，其实由于genStmt()设计上的原因No往往并不是下一个而是下若干个*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",A);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(nop,/,/)");
      
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      genStmt(ThirdStepCode,FourthStepCode,tempEnd);
      
      /*[B](jmp,/,No)*/ //这里当然也可以设置成[B](nop,/,/)
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",B);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,")");
      
      /*[No++](nop,/,/)*/ //这种提高抽象程度之小公式到大公式的注释是最好的注释
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);/*这里本来是sprintf(_No,"%d",No++);但由于sprintf()是先从右到左把所有作为参数的函数调用调用完毕再采集参数，所以这里不能No++*/ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(nop,/,/)");No++;
    }
    else if(!strcmp(fuc,"repeat><until><"))
    { 
      //在上一个case使用三元式实现if><then><><后三元式构建语句就可以直接拿来用了之所谓复用
      int A=No++,B=No++,C=No++;
      /*[A](jmp,/,No)*/ //像这种指向下一条语句的空语句不用No++
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",A);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,")");
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      //printf("word=%s\n",fuc);
      genStmt(ThirdStepCode,FourthStepCode,tempEnd);
      //printf("word=%s\n",fuc);
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      //printf("word=%s\n",fuc);
      //printf("OK\n"); //这种人工断点是很有用的
      /*[B](jnz,r[level],No)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",B);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(_No,"r[%d]",level);sprintf(all,"(jnz,%s,",toRegister(_No));ListInsert_Pro(4,FourthStepCode,all);sprintf(_No,"%d)",No);/*printf("OK\n"); 之前是sprintf(_No,"%s)",No++);之段错误，之No是整型而不是指针，%s就是将之用作指针自然段错误，所以出现段错误时应注意下是否格式错误之尤其多次使用复制粘贴*/ListInsert_Pro(4,FourthStepCode,_No);
      /*[C](jmp,/,A)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",C);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(jmp,/,");sprintf(_No,"%d",A);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,  FourthStepCode,")");
      /*[No++](nop,/,/)*/ //这种提高抽象程度之小公式到大公式的注释是最好的注释
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);/*这里本来是sprintf(_No,"%d",No++);但由于sprintf()是先从右到左把所有作为参数的函数调用调用完毕再采集参数，所以这里不能No++*/ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(nop,/,/)");No++;
    }
    else if(!strcmp(fuc,":=><"))
    {
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      /*[No++](:=,[fuc],r[0])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(:=,[");sprintf(_No,"Variable%d",st_lookup(fuc));ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"],");ListInsert_Pro(4,FourthStepCode,toRegister("r[0])"));ListInsert_Pro(4,FourthStepCode,")");No++;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
    }
    else if(!strcmp(fuc,"read><"))
    {
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      /*[No++](in,/,fuc)*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");ListInsert_Pro(4,FourthStepCode,"(in,/,");ListInsert_Pro(4,FourthStepCode,fuc);ListInsert_Pro(4,  FourthStepCode,")");No++;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
    }
    else if(!strcmp(fuc,"write><"))
    {
      int tempEnd=End;
      if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
      else break;
      genExp(ThirdStepCode,FourthStepCode,tempEnd);
      /*[No++](out,/,r[0])*/
      ListInsert_Pro(4,FourthStepCode,"[");sprintf(_No,"%d",No);ListInsert_Pro(4,FourthStepCode,_No);ListInsert_Pro(4,FourthStepCode,"]");sprintf(all,"(out,/,%s)",toRegister("r[0]"));ListInsert_Pro(4,FourthStepCode,all);No++;
    }
    else break; //扫描到非上述五类之也即Stmt系前缀式，也即扫描到Exp系前缀式，说明扫描到边界，则退出循环
    //if(ThirdStepCode->next)toSet(ThirdStepCode,fuc,Begin,End);
    //else break;//else Begin=sEnd; //这个很有想法
    //Begin=sEnd;
  }
}
void genFourthStepCode(LinkList ThirdStepCode,LinkList &FourthStepCode)
{
  //fprintf(asm_main,"%s,%d,%d\n",sfuc,sBegin,sEnd);
  toSet(ThirdStepCode,fuc,Begin,End); //要先初始化么
  genStmt(ThirdStepCode,FourthStepCode,sEnd);
}
char *Address,*Parameter1,*Parameter2,*Parameter3;
//int AddrUsed[No]={}; //地址使用抽象数组或者说地址使用向量，用以记录该符号地址是否被使用，之跳转三元式中出现的符号地址该值为1，否则略去，这也算一层代码优化。如果放在genStmt()里虽然可以直截了当，但蛋疼的是这样数组大小就不能设定，之陷入绝境了只能放弃
void getParameters(LinkList &FourthStepCode)
{
  static char _Address[88];
  static char _Parameter1[88];
  static char _Parameter2[88];
  static char _Parameter3[88];
  /*以上四者用完必须清零，否则会污染后面者*/
  LinkList p=FourthStepCode->next;
  int i=0;
  p=p->next;
  ListDelete(FourthStepCode);
  while(p->data!=']')
  {
    _Address[i++]=p->data;
    p=p->next;
    ListDelete(FourthStepCode);
  }
  _Address[i]='\0';
  Address=_Address;
  p=p->next->next; //p指向作用域左边界
  ListDelete(FourthStepCode);ListDelete(FourthStepCode);
  i=0;
  while(p->data!=',')
  {
    _Parameter1[i++]=p->data;
    p=p->next;
    ListDelete(FourthStepCode);
  }
  _Parameter1[i]='\0';
  Parameter1=_Parameter1;
  p=p->next;
  ListDelete(FourthStepCode);
  i=0;
  while(p->data!=',')
  {
    _Parameter2[i++]=p->data;
    p=p->next;
    ListDelete(FourthStepCode);
  }
  _Parameter2[i]='\0';
  Parameter2=_Parameter2;
  p=p->next;
  ListDelete(FourthStepCode);
  i=0;
  while(p->data!=')')
  {
    _Parameter3[i++]=p->data;
    p=p->next;
    ListDelete(FourthStepCode);
  }
  _Parameter3[i]='\0';
  Parameter3=_Parameter3;
  p=p->next;
  ListDelete(FourthStepCode);
}
void genAssemblyCode(LinkList FourthStepCode,FILE *asm_main,FILE *asm_part[3]) //就像高级语言可直接有解释器，之这样高级语言就是相对的目标代码，所以确实中间代码和目标代码都是相对的就像小公式和大公式之间的相对关系。这里其实就是第五步中间代码生成
{
  char ch=fgetc(asm_part[0]);
  while(ch!=EOF)
  {
    fprintf(asm_main,"%c",ch);
    ch=fgetc(asm_part[0]);
  }
  ScanSymtab(hashTable);
  fclose(asm_part[1]);
  if((asm_part[1]=fopen("1.asm","r"))==NULL)exit(0);
  ch=fgetc(asm_part[1]);
  while(ch!=EOF)
  {
    fprintf(asm_main,"%c",ch);
    ch=fgetc(asm_part[1]);
  }
  ch=fgetc(asm_part[2]);
  while(ch!=EOF)
  {
    fprintf(asm_main,"%c",ch);
    ch=fgetc(asm_part[2]);
  }
  while(1)
  {
    if(FourthStepCode->next)getParameters(FourthStepCode);
    else break;
    fprintf(asm_main,"Address%s:\n",Address);
    if(!strcmp(Parameter1,"in"))fprintf(asm_main,"  call In\n  mov [Variable%d],eax\n",st_lookup(Parameter3));
    else if(!strcmp(Parameter1,"out"))fprintf(asm_main,"  call Out\n");
    else if(!strcmp(Parameter1,"StackIn"))fprintf(asm_main,"  push %s\n",Parameter3);
    else if(!strcmp(Parameter1,"StackOut"))fprintf(asm_main,"  pop %s\n",Parameter3);
    else if(!strcmp(Parameter1,":="))fprintf(asm_main,"  mov %s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"jmp"))fprintf(asm_main,"  jmp Address%s\n",Parameter3);
    else if(!strcmp(Parameter1,"jz"))fprintf(asm_main,"  cmp %s,0\n  jz Address%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"jnz"))fprintf(asm_main,"  cmp %s,0\n  jnz Address%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"nop"))fprintf(asm_main,"  nop\n");
    else if(!strcmp(Parameter1,"+"))fprintf(asm_main,"  Compute 0,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"-"))fprintf(asm_main,"  Compute 1,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"*"))fprintf(asm_main,"  Compute 2,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"/"))fprintf(asm_main,"  Compute 3,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"<"))fprintf(asm_main,"  Compute 4,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"="))fprintf(asm_main,"  Compute 5,%s,%s\n",Parameter2,Parameter3);
    else if(!strcmp(Parameter1,"save"))fprintf(asm_main,"  push eax\n  push ebx\n  push ecx\n  push edx\n");
    else if(!strcmp(Parameter1,"restore"))fprintf(asm_main,"  pop edx\n  pop ecx\n  pop ebx\n  pop eax\n");
    //printf("%s\n",Parameter3);
    memset(Address,0,88);
    memset(Parameter1,0,88);
    memset(Parameter2,0,88);
    memset(Parameter3,0,88);
    /*这应该比使用循环清零效率高点吧*/
  }
  ch=fgetc(asm_part[3]);
  while(ch!=EOF)
  {
    fprintf(asm_main,"%c",ch);
    ch=fgetc(asm_part[3]);
  }
}
int main(int argc,char *argv[]) //当然最后main()还是要加入参数以操作不同的文件的
{
  //if((source=fopen("sample.01","r"))==NULL)exit(0);
  char pgm[120]; /* source code file name */
  if(argc!=2)
  { 
    fprintf(stderr,"usage: %s <filename>\n",argv[0]);
    exit(1);
  }
  strcpy(pgm,argv[1]) ;
  source=fopen(pgm,"r");
  if(source==NULL)
  { 
    fprintf(stderr,"File %s not found\n",pgm);
    exit(1);
  }
  if((intermediate_code=fopen("sample.1","w"))==NULL)exit(0);
  if((asm_main=fopen("sample.asm","w"))==NULL)exit(0); 
  if((asm_part[0]=fopen("0.asm","r"))==NULL)exit(0); 
  if((asm_part[1]=fopen("1.asm","w"))==NULL)exit(0);
  if((asm_part[2]=fopen("2.asm","r"))==NULL)exit(0);  
  if((asm_part[3]=fopen("3.asm","r"))==NULL)exit(0);
  //if((fpw3=fopen("sample.3","w"))==NULL)exit(0);
  //if((fpw4=fopen("sample.4","w"))==NULL)exit(0);
  //int i;for(i=0;i<39;i++)get_word(source,intermediate_code); //说明改造后的词法分析没有问题
  InitList(Track_Vector);
  InitList(Terminal_Symbol);
  LinkList FirstStepCode=Terminal_Symbol;
  LinkList SecondStepCode,ThirdStepCode,FourthStepCode;
  InitList(SecondStepCode);
  InitList(ThirdStepCode);
  InitList(FourthStepCode);
  genFirstStepCode(NULL,FirstStepCode);
  ListTraverse(Track_Vector);
  ListTraverse(Terminal_Symbol);
  genSecondStepCode(FirstStepCode,SecondStepCode);
  ListTraverse(SecondStepCode);
  genThirdStepCode(SecondStepCode,ThirdStepCode);
  ListTraverse(ThirdStepCode);
  genFourthStepCode(ThirdStepCode,FourthStepCode);
  ListTraverse(FourthStepCode);
  genAssemblyCode(FourthStepCode,asm_main,asm_part);
  DestroyList(Track_Vector);
  DestroyList(Terminal_Symbol);
  DestroyList(FirstStepCode);
  DestroyList(SecondStepCode);
  DestroyList(ThirdStepCode);
  DestroyList(FourthStepCode);
  DestroyHashTable(hashTable);
  fclose(source);
  fclose(intermediate_code);
  fclose(asm_part[0]);
  fclose(asm_part[1]);
  fclose(asm_part[2]);
  fclose(asm_part[3]);
  //fclose(fpw3);
  //fclose(fpw4);
  return 0;
}
