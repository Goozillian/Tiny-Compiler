  OutKernel end,1,0
  mov eax,1
  int 80h
  
In:
  mov ecx,10
  mov edx,0
input: ;应该跳转到_start也是可以的，毕竟前面没有指令的话的话_start跟input是等价的
  push ecx
  push edx
retry:
  mov eax,3
  mov ebx,0
  mov ecx,x
  mov edx,1
  int 80h
  
  mov al,[x] ;这里只能用al之说明寄存器的选择是跟数据片段截取相关的
  
  cmp al,0ah ;以回车结束输入
  je ok
  cmp al,'-'
  jne go_on1
  mov edx,-1 ;之前用al，但由于后面是要imul eax,[base]所以存储大小不一致的话-1就会变成255则结果错误，这里用edx当然是即兴的之也可以用其他寄存器
  mov [base],edx
  jmp retry
go_on1:
  cmp al,'+'
  jne go_on2
  jmp retry
go_on2:
  cmp al,'0' ;只允许输入0~9
  jb retry
  cmp al,'9'
  ja retry
  
  sub eax,30h 
  pop edx
  pop ecx
;.mul:
;  imul eax,10
;  loop .mul
  add edx,eax
  imul edx,10
  
  loop input
  push edx
ok:
  pop edx
  pop ecx ;之前是pop得到想要的寄存器备份在栈里的值后就不pop其他寄存器的了，但由于子程序地址也存在栈里，所以不pop完在这里就会把ecx的数据作为子程序返回地址自然段错误
  mov eax,edx
  imul eax,[base]
  
  mov edx,+1
  mov [base],edx
  
  cdq ;没有这个会：浮点数例外 (核心已转储)
  mov ebx,10
  idiv ebx
  ret
  
Out:
  cmp eax,0
  jge N
  OutKernel minus,1,0 ;不能Out "-"
N:
  call toString
  OutKernel z,12,1
  mov eax,0
  mov ebx,0
  mov ecx,12
.reset: ;输出后要将z清零，否则会污染之后的使用
  mov [z+ebx],eax
  inc ebx
  loop .reset
  ret
  
;参数ax
;输出ax的10进制
toString:
  ;mov eax,[base]
  
  ;mov eax,[a]
  ;mov ebx,[b]
  ;add eax,ebx
  
  mov ecx,0
  ;处理负数
  cmp eax,0
  jge K
  mov ebx,0
  sub ebx,eax
  mov eax,ebx
K:
  mov ebx,10 ;之前y定义为db类型而ebx是32位的所以这里用mov ebx,[y]会由于执行后ebx为0则除0错误，貌似也不会，可能当时是编译到了另一个文件吧，之存储大小不匹配在特殊情况下才会导致程序出错，总之就是有时会有时不会，所以与其这样不如直接这样
L:
  cdq ;这个很有必要放在循环之中处理eax
  idiv ebx
  ;除法
  
  add edx,48
  push edx ;堆栈系统是虚拟增加寄存器数降低技巧要求的一个实用模型
  add ecx,1 ;这里inc ecx也可以
  cmp eax,0
  jne L
  mov ebx,0
M:
  pop edx
  mov [z+ebx],edx
  add ebx,1
  loop M
  ;余数
  ret
