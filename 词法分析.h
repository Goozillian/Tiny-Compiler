extern int lineno;
extern int count;
extern int type;
extern int jump;
extern char set[88];
extern char *next_word(FILE *source,FILE *intermediate_code); //词法分析.c里该函数要用到的全局变量要不要extern到词法分析.h里来呢？不用的话模块性就更强了吧；事实证明确实不用这也是为什么有的变量有的函数要static之隐藏吧。当然函数默认是外部函数，所以貌似不加extern也行。
