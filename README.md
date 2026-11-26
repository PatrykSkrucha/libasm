# libasm

> Classic libc functions and a small linked-list toolkit, rewritten in **x86-64 assembly (NASM)**.

![NASM](https://img.shields.io/badge/NASM-x86--64-555555?style=flat&logo=intel&logoColor=white)
![C](https://img.shields.io/badge/tests-C-00599C?style=flat&logo=c&logoColor=white)

## About

The goal of libasm is to learn how functions really work under the hood: the System V calling
convention, registers, the stack, syscalls and setting `errno` correctly. Every function is written
by hand in NASM, packed into a static library `libasm.a`, and can be called from regular C code.

## Functions

**Mandatory**

| Function | Behaves like |
|---|---|
| `ft_strlen` | `strlen(3)` |
| `ft_strcpy` | `strcpy(3)` |
| `ft_strcmp` | `strcmp(3)` |
| `ft_write` | `write(2)`, sets `errno` on failure |
| `ft_read` | `read(2)`, sets `errno` on failure |
| `ft_strdup` | `strdup(3)`, uses `malloc` |

**Bonus**

| Function | Description |
|---|---|
| `ft_atoi_base` | converts a string to an int in any given base |
| `ft_list_push_front` | adds a new node at the start of a list |
| `ft_list_size` | counts the nodes of a list |
| `ft_list_sort` | sorts a list with a comparison function |
| `ft_list_remove_if` | removes every node matching a reference value |

## Build & test

Requires Linux x86-64, `nasm` and a C compiler.

```bash
make          # builds libasm.a
make test     # builds and links the test program in main.c
./test
```

The test program prints each libasm result next to the libc one, so differences are easy to spot.

## Use in your own code

```c
size_t ft_strlen(const char *s);

int main(void) { return ft_strlen("hello") == 5 ? 0 : 1; }
```

```bash
cc main.c libasm.a -o app
```

---

<sub>Part of the <a href="https://www.codam.nl/">Codam</a> (42 network) curriculum.</sub>
