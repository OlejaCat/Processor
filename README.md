
# Processor

This project was written to better understand the operation of the processor, the idea is to transfer some of the commands from the assembler to "my assembler" and learn how to compile into machine code, then run this code.


## Run Locally

Clone the project

```bash
  git clone --recurse-submodules https://github.com/OlejaCat/Processor
```

Go to the project directory

```bash
  cd Processor
```

Building a project

```bash
  make
```

## Usage/Examples

You can watch examples of code in `asm_programs` directory.

Firstly you need to compile your code using

```bash
./compiler your_program.asm your_program_code.bin
```

For running assembled code run

```bash
./processor your_program_code.bin
```

