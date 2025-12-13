# 🔧 Self-Derived Language → Assembly Language Translator

A translator written in **C** that converts a **custom-designed programming language** into **Intel x86 (32-bit) assembly**.  
This project demonstrates the fundamentals of **compiler and assembler design**, including parsing, control flow translation, and assembly code generation.

---

## 🚀 Features

- Translates a **self-defined high-level language** into **x86 assembly (NASM-style)**  
- Supports:
  - Function definitions and calls  
  - Labels and branching  
  - Conditional and unconditional jumps  
  - Variable assignments and arithmetic operations  
- Implements core compiler components — parsing, symbol handling, and instruction generation

---

## 🛠️ Getting Started

### Prerequisites
- **GCC** or any C compiler  
- **NASM** for assembling the output  
- Linux environment (recommended)

### Compilation
```bash
gcc main.c -o translator
```

### Usage
```bash
./translator < inputfile >
```

- **inputfile:** Program written in the self-derived language (e.g., `input.txt`)  
- Output: Generated x86 assembly file (e.g., `output.asm`)

---

## 📚 Example Input

```text
( $start 
  ( $sum 2
    EAX <- #0
    :loop CMP EAX, EBX
    JE :done
    ADD EAX, #1
    JMP :loop
    :done RET
  )
  ( $main 1
    EBX <- #5
    CALL $sum
  )
)
```

---

## ⚙️ Example Output (x86 Assembly)

```asm
section .text
global _start

__sbr_sum:
    push ebp
    mov ebp, esp
    mov eax, 0
__sum_loop:
    cmp eax, ebx
    je __sum_done
    add eax, 1
    jmp __sum_loop
__sum_done:
    pop ebp
    ret

__sbr_main:
    push ebp
    mov ebp, esp
    mov ebx, 5
    call __sbr_sum
    pop ebp
    ret

_start:
    call __sbr_main
    mov eax, 1
    int 0x80
```

---

## 🧠 Concepts Demonstrated

| Concept | Custom Syntax | Generated Assembly |
|----------|----------------|--------------------|
| Function Definition | `$sum` | `__sbr_sum:` |
| Variable Assignment | `EAX <- #0` | `mov eax, 0` |
| Label | `:loop` | `__sum_loop:` |
| Condition | `CMP EAX, EBX` | `cmp eax, ebx` |
| Jump | `JMP :loop` | `jmp __sum_loop` |
| Function Call | `CALL $sum` | `call __sbr_sum` |

---

## 📂 Project Structure

```
translator/
├── main.c        # Main translator source
├── translator1.h     # Header file
├── README.md         # Documentation
└── .gitignore
```

---

## ⚙️ .gitignore
```gitignore
*.o
*.out
*.exe
```

---

## 📜 License

This project is **proprietary** and intended for educational and demonstration purposes only.  
Unauthorized reproduction or redistribution is not permitted.

---

## 📬 Contact

**Author:** Subodh Wagh  
**Email:** subodhwagh1122@gmail.com  
**LinkedIn:** [linkedin.com/in/subodh-wagh-81541021b](https://www.linkedin.com/in/subodh-wagh-81541021b)  
**GitHub:** [github.com/subodh2003](https://github.com/subodh2003)
