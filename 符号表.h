extern FILE *source;
extern FILE *intermediate_code;
extern FILE *asm_main;
extern FILE *asm_part[4];
//extern FILE *fpw3;
//extern FILE *fpw4;

#define ElemType char
typedef struct LNode{
  ElemType data;
  struct LNode *next;
}LNode,*LinkList;
typedef int Status;
extern LinkList Track_Vector;
extern LinkList Terminal_Symbol;
extern Status InitList(LinkList &L);
extern Status DestroyList(LinkList &L);
extern char *toString(char c);
extern Status ListInsert_Pro(int n,LinkList &L,char *str);
extern Status ListDelete(LinkList &L);
extern Status ListTraverse(LinkList L);

#define SIZE 211
typedef struct BucketListRec{
  char name[88];
  int no;
  struct BucketListRec * next;
}*BucketList;
extern BucketList hashTable[SIZE];
extern void st_insert(char *name);
extern Status DestroyHashTable( BucketList hashTable[]);
extern int st_lookup (char *name);
