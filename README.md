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
section data
  hello_world ascii "Hello, World!"
section code
  mov hello_world, r0
  printstr r0
```

### Compile and Interpret

```bash
./easy -c hello.asm # compiles to a.out
./easy -i a.out
```

# Planned

- Sections (data, bss)
- Entry point (for example: entry label)
- Sub Register Support (Assembler generates binaries for sub-registers like r0, e0, b0, p0; currently CPU does not support execution on these sub-registers)

# License

This project is licensed under the **GPL-3.0 License**.

# Author

Created By **0l3d**
