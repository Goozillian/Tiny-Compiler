#include <stdio.h>
#include <stdlib.h>
#include "符号表.h"
int ScanBucketList(BucketList L) //这里本来是int ScanBucketList(BucketList &L)之引用，影响到了后面对哈系表的调用，所以导致程序输出不符合预期，这个也困扰了己很久，值得谨记
{
  BucketList q;
  if(L==NULL)return 1;
  while(L)
  {
    q=(L)->next;//这样写是说明->的用法
    fprintf(asm_part[1],"  Variable%d dd 0\n",L->no); //这里不用l->name是因为汇编模块中已经定义有数据，这样可以避免重名，其实这样堪称绝妙
    L=q;
  }
  return 0;
}
int ScanSymtab(BucketList hashTable[])
{
  int i;
  for(i=0;i<SIZE;i++)ScanBucketList(hashTable[i]);
  return 0;
}
