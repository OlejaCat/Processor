
# Processor

This project was written to better understand the operation of the processor, the idea is to transfer some of the commands from the assembler to "my assembler" and learn how to compile into machine code, then run this code. Work of processor is based on last project [Stack](https://github.com/OlejaCatStack). You can watch commands usage below in [Commands](#commands) section.


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

## Commands

## Commands

Below are the main commands available for use in your assembler:

- **HLT** — Halts program execution.
  - **Usage**: `HLT`
  - **Description**: Stops the program immediately.

- **PUSH** — Pushes an element onto the stack.
  - **Usage**: `PUSH <value>`/`PUSH [<value>]`
  - **Description**: Places the specified value at the top of the stack / pushes value to RAM

- **POP** — Removes an element from the stack.
  - **Usage**: `POP <register>` / `POP [<value>]`
  - **Description**: Removes the top value from the stack and stores it in the specified register / Pops an element to RAM to that index

- **ADD** — Adds the top two elements of the stack.
  - **Usage**: `ADD`
  - **Description**: Pops the top two elements from the stack, adds them, and pushes the result back onto the stack.

- **MUL** — Multiplies the top two elements of the stack.
  - **Usage**: `MUL`
  - **Description**: Pops the top two elements from the stack, multiplies them, and pushes the result back onto the stack.

- **SUB** — Subtracts the top stack element from the second element.
  - **Usage**: `SUB`
  - **Description**: Pops the top two elements from the stack, subtracts the top element from the second element, and pushes the result back onto the stack.

- **DIV** — Divides the second stack element by the top element.
  - **Usage**: `DIV`
  - **Description**: Pops the top two elements from the stack, divides the second element by the top element, and pushes the result back onto the stack.

- **SQRT** — Calculates the square root of the top stack element.
  - **Usage**: `SQRT`
  - **Description**: Pops the top element from the stack, calculates its square root, and pushes the result back onto the stack.

- **OUT** — Outputs the top stack element.
  - **Usage**: `OUT`
  - **Description**: Displays the top element of the stack. Element will be removed.

- **IN** — Takes input and pushes it onto the stack.
  - **Usage**: `IN`
  - **Description**: Reads a value from the user and pushes it onto the stack.

- **JMP** — Unconditional jump to a label.
  - **Usage**: `JMP <label>`
  - **Description**: Transfers program execution to the specified label.

- **JA** — Jumps if above.
  - **Usage**: `JA <label>`
  - **Description**: Jumps to the label if the second element on the stack is greater than the top element.

- **JAE** — Jumps if above or equal.
  - **Usage**: `JAE <label>`
  - **Description**: Jumps to the label if the second element on the stack is greater than or equal to the top element.

- **JB** — Jumps if below.
  - **Usage**: `JB <label>`
  - **Description**: Jumps to the label if the second element on the stack is less than the top element.

- **JBE** — Jumps if below or equal.
  - **Usage**: `JBE <label>`
  - **Description**: Jumps to the label if the second element on the stack is less than or equal to the top element.

- **JE** — Jumps if equal.
  - **Usage**: `JE <label>`
  - **Description**: Jumps to the label if the top two elements on the stack are equal.

- **JNE** — Jumps if not equal.
  - **Usage**: `JNE <label>`
  - **Description**: Jumps to the label if the top two elements on the stack are not equal.

- **CALL** — Calls a function.
  - **Usage**: `CALL <label>`
  - **Description**: Jumps to a specified label and stores the return address to resume after the function completes.

- **RET** — Returns from a function.
  - **Usage**: `RET`
  - **Description**: Returns to the address stored by the previous `CALL` command.

- **PROC** — Marks the start of a function.
  - **Usage**: `<label> PROC`
  - **Description**: Defines the beginning of a function, typically followed by the function's label.

- **ENDP** — Marks the end of a function.
  - **Usage**: `ENDP`
  - **Description**: Defines the end of a function block, paired with a `PROC` command.

- **DRAW** — Draws a visual representation or output.
  - **Usage**: `DRAW`
  - **Description**: Draws a RAM in screen 64x64

