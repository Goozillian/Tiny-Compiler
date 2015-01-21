%macro OutKernel 3 ;这里的3是参数个数而不是参数表中参数的代号，这个困扰了己一阵子。另外貌似不能用macro设置零参宏，define或许可以，那这样的零参宏就相当于子程序了吧
  push eax   ;必须把用到的寄存器压栈或作类似处理
  mov eax,4
  mov ebx,1
  mov ecx,%1
  mov edx,%2
  int 80h
  %if %3
  mov eax,4
  mov ebx,1
  mov ecx,_z
  mov edx,1
  int 80h
  %endif
  pop eax
%endmacro

%macro Compute 3
  mov eax,%2
  mov ebx,%3
  %if %1=0
  add eax,ebx
  %elif %1=1
  sub eax,ebx
  %elif %1=2
  imul eax,ebx
  %elif %1=3
  cdq
  idiv ebx
  %elif %1=4 ;<之比较操作
  cmp eax,ebx
  jb .b
  mov eax,0
  jmp .nb
.b:
  mov eax,1
.nb:
  %elif %1=5 ;=之比较操作
  cmp eax,ebx
  je .e
  mov eax,0
  jmp .ne
.e:
  mov eax,1
.ne:
  %endif
  mov %2,eax ;之前没有这一句则相当于该宏过程做了无用功。之前用//来注释导致莫名其妙的语法错误，让己紧张了一阵子
%endmacro

section .data
  x db 0
  ;else_if dd 0 ;确定是否掠过else_if之标志位
  minus db '-'
  base dd +1 ;保存+1和-1之输入负数由此实现
  z times 12 dd 0
  _z db 09h ;实际还可以把这两个存入z，之之所以times后面不能再单独定义数据也是因为没必要
  end db 0ah ;回车键
  exp dd 0 ;暂存exp的值
