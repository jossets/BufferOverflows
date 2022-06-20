#include <stdio.h>
#include <unistd.h>
#include <string.h>


char msg01[] = "Hello\n> ";
char msg02[] = "Bye\n> ";
void pwnme() 
{
    char buff[32];
    write(STDOUT_FILENO, msg01, strlen(msg01));
    read(STDIN_FILENO, buff, 60);
    write(STDOUT_FILENO, msg02, strlen(msg02));
}


char msg03[] = "Victory\n> ";
void callme() 
{
    write(STDOUT_FILENO, msg03, strlen(msg03));
}

/*
void x86_system_shell() 
{
    // x86 system(/bin/sh)
    asm("\
        needle0: jmp there\n\
        here:    pop %rdi\n\
                xor %rax, %rax\n\
                movb $0x3b, %al\n\
                xor %rsi, %rsi\n\
                xor %rdx, %rdx\n\
                syscall\n\
        there:   call here\n\
        .string \"/bin/sh\"\n\
        needle1: .octa 0xdeadbeef\n\
  ");
}
*/

void usefull_strings() 
{
    asm(".string \"/bin/sh\"");
    asm(".string \"flag.txt\"");
}

void usefull_gadgets() 
{
  // Gadgets
/*+---------+------+------+------+------+------+------+
  | syscall | arg0 | arg1 | arg2 | arg3 | arg4 | arg5 |
  +---------+------+------+------+------+------+------+
  |   %rax  | %rdi | %rsi | %rdx | %r10 | %r8  | %r9  |
  +---------+------+------+------+------+------+------+
  Little endian */
  asm(".byte 0x58, 0xc3");// "\x58\xc3" // pop rax, ret  
  asm(".byte 0x5f, 0xc3"); // "\x5f\xc3" // pop rdi, ret => 0x0000c35f
  asm(".byte 0x5e, 0xc3"); // "\x5e\xc3" // pop rsi, ret => 0x0000c35e
  asm(".byte 0x5a, 0xc3"); // "\x5a\xc3" // pop rdx, ret => 0x0000c35a
  asm(".byte 0x41, 0x58, 0xc3"); // "\x41\x58\xc3" // pop r8, ret  => 0x00c35c41
  //asm(".octa 0x00c35941"); // "\x41\x59\xc3" // pop r9, ret  => 0x00c35c41
  //asm(".octa 0x00c35a41"); // "\x41\x5a\xc3" // pop r10, ret => 0x00c35c41

  asm(".byte 0x5d, 0xc3"); // "\x5d\xc3" // pop rbp, ret => 0x0000c35d
  asm(".byte 0x5b, 0xc3"); // "\x5b\xc3" // pop rbx, ret => 0x0000c35b

  asm(".byte 0x41, 0x5b, 0xc3"); // "\x41\x5b\xc3" // pop r11, ret 
  asm(".byte 0x41, 0x5c, 0xc3"); // "\x41\x5c\xc3" // pop r12, ret 
  asm(".byte 0x41, 0x5d, 0xc3"); // "\x41\x5d\xc3" // pop r13, ret 
  asm(".byte 0x41, 0x5e, 0xc3"); // "\x41\x5e\xc3" // pop r14, ret  
  asm(".byte 0x41, 0x5f, 0xc3"); // "\x41\x5f\xc3" // pop r15, ret  

  asm(".byte 0x45, 0x30, 0x37, 0xc3"); // 45 30 37  xor %r14b,(%r15)      
  asm(".byte 0x4d, 0x89, 0x65, 0x00, 0xc3"); // 4d 89 65 00  mov  %r12,0x0(%r13)  
}


void tst(int a, int b, int c, int d, int e, int f) {
    pwnme();
}

int main()
{
    pwnme();
    return(0);
}

