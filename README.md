# EASY64

Simple cpu architecture in C.

## Features

- Simple and fast CPU architecture
- Easy-to-understand assembly language
- Label handling and jump instructions
- Basic stack operations (push, pop, call, ret)
- Arithmetic, logic, and bitwise operations
- Input/output instructions (print, input)

## Usage

```bash
git clone https://github.com/0l3d/easy64.git
cd easy64/
make

./easy -h
```

## Examples

**Hello World**

```
section data # data section
  hello_world ascii "Hello, World!" # helloworld
section code # code section
entry start  # entry selection
start:       # start label
  mov 1, r0
  mov hello_world, r1  # write syscall -> write(r0 = 1, r1 = adress of hello_world, r2 = 14 (size));
  mov 14, r2
  syscall 1 # syscall and id (0 = read, 1 = write, references -> https://filippo.io/linux-syscall-table/)
```

### Compile and Interpret

```bash
./easy -c hello.asm # compiles to a.out
./easy -i a.out
```

# Planned

- import support
- Sub Register Support (Assembler generates binaries for sub-registers like r0, e0, b0, p0; currently CPU does not support execution on these sub-registers)

# License

This project is licensed under the **GPL-3.0 License**.

# Author

Created By **0l3d**
