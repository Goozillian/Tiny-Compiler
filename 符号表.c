/*符号表存储的其实就是文法中的终结符和非终结符或者说相对终结符相对非终结符之也即文法中的产生式中的各符号的各种相关信息之所以也才被称为符号表，而文法分析被分成了词法分析和语法分析，那就是说符号表是跟词法分析和语法分析都相互联系之跟文法分析相联系的。*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "符号表.h"
#define OK 1
#define TRUE 1
#define FALSE 0
#define ERROR 0
FILE *source;
FILE *intermediate_code;
FILE *asm_main;
FILE *asm_part[4];
//FILE *fpw3;
//FILE *fpw4;
/*语法树衍生表*/

LinkList Track_Vector; //轨迹向量链表，也即非终结符链表
LinkList Terminal_Symbol; //终结符链表

Status InitList(LinkList &L)
{
  L=(LinkList)malloc(sizeof(LNode));
  L->next=NULL; //先建立一个带头结点的单链表
  return 0;
}
Status DestroyList(LinkList &L)
{ /* 初始条件：线性表L已存在。操作结果：销毁线性表L */
  LinkList q;
  if(L==NULL)return 1;
  while(L)
  {
    q=(L)->next;//这样写是说明->的用法
    free(L);
    L=q;
  }
  return 0;
}
Status ListInsert(int n,LinkList &L,ElemType e)
{
  //在带头结点的单链线性表L中第i个位置之前插入元素e
  static int i[5]={1,1,1,1,1}; //这样即可简单解决多个链表要调用该函数时的冲突问题
  int _i=i[n]++; //实现头插法插入位置自动增1之头插法非逆序
  LinkList p=L;int j=0;
  while(p&&j<_i-1){p=p->next;++j;} //寻找第i-1个结点
  if(!p||j>_i-1)return ERROR; //i小于1或大于表长
  LinkList s=(LinkList)malloc(sizeof(LNode)); //生成新结点
  s->data=e;s->next=p->next;
  p->next=s;
  return OK;
}
char *toString(char c)
{
  static char str[2];
  str[0]=c;
  str[1]=0;
  return str;
}
Status ListInsert_Pro(int n,LinkList &L,char *str) //与形参改为char str[]而实参传入"abcd"也是可以的
{
  int i;
  for(i=0;str[i]!=0;i++)ListInsert(n,L,str[i]);
  return 0;
}
Status ListDelete(LinkList &L)
{
  //在带头结点的单链线性表L中，删除第i个元素，并由e返回其值
  int i=1;
  LinkList p=L;int j=0;
  while(p->next&&j<i-1) //寻找第i个结点，并令p指向其前驱之指向第i-1个结点
  {
    p=p->next;++j;
  }
  if(!(p->next)||j>i-1)return ERROR; //删除位置不合理
  LinkList q=p->next;p->next=q->next;//p->next=p->next->next;//删除并释放结点
  free(q);
  return OK;
}
Status ListTraverse(LinkList L)
{
  if(L->next==NULL)
  {
    printf("当前线性表为空\n");
    fprintf(intermediate_code,"当前线性表为空\n");
  }
  else
  {
    printf("当前线性表中的元素为:\n");
    fprintf(intermediate_code,"当前线性表中的元素为:\n");
    LinkList p;
    p=L->next;
    while(p)
    {
      printf("%c",p->data);
      fprintf(intermediate_code,"%c",p->data);
	  p=p->next;
    }
    printf("\n");
    fprintf(intermediate_code,"\n");
  }
  return 0;
}
/*标识符散列表*/
#define SHIFT 4
static int hash (char * key)
{
  int temp = 0;
  int i = 0;
  while (key[i] != '\0') //其实这里也有一次遍历之也算顺序查找了，但这个比起对所有标识符的遍历之顺序查找开销是可以小很多之利用小规模遍历实现大规模遍历
  { 
    temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  //fprintf(intermediate_code,"hash=%d",temp);
  return temp;
}
static int loc=0; //变量名编号
BucketList hashTable[SIZE];
void st_insert(char *name) //其实这个就是buildSymtab()
{ 
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))l = l->next;
  if (l == NULL) /* variable not yet in table */
  {
    l = (BucketList) malloc(sizeof(struct BucketListRec));
    strcpy(l->name,name); //原来是l->name=name;这样的话当name不是固定的那l->name也就不是固定的了这样程序输出就会不合预期
    //fprintf(intermediate_code,"l->name=%s",l->name);
    l->no = loc++;
    l->next = hashTable[h]; //其实就是l->next=NULL;
    hashTable[h] = l; 
  }
}
Status DestroyBucketList(BucketList &L)
{ /* 初始条件：线性表L已存在。操作结果：销毁线性表L */
  BucketList q;
  if(L==NULL)return 1;
  while(L)
  {
    q=(L)->next;//这样写是说明->的用法
    free(L);
    L=q;
  }
  return 0;
}
Status DestroyHashTable( BucketList hashTable[]) //复杂数据结构由于是由基本数据结构组成则可以利用基本数据结构的销毁函数合成其销毁函数
{
  int i;
  for(i=0;i<SIZE;i++)DestroyBucketList(hashTable[i]);
  return 0;
}
int st_lookup (char *name)
{ 
  int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))l = l->next;
  if (l == NULL)return -1;
  else return l->no;
}
