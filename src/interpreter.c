#include "easy.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

Flags flags = {0};

Register64 regsc[20];

uint8_t memory[MEMORY_SIZE] = {0};
uint8_t zmemory[MEMORY_SIZE] = {0};

void interpret_easy64(const char *binname) {
  FILE *binfile = fopen(binname, "rb");
  if (binfile == NULL) {
    perror("cpu is failed to open binary file");
    return;
  }

  memset(regsc, 0, sizeof(regsc));

  // CPU FEATURES
  uint64_t call_stack[256];
  int sppush = 0;
  int spcall = 0;

  uint64_t push_stack[256];

  BinaryHeader header;
  fseek(binfile, 0, SEEK_SET);
  fread(&header, sizeof(BinaryHeader), 1, binfile);

  fseek(binfile, header.section_data, SEEK_SET);
  fread(&memory, 1, sizeof(memory), binfile);
  fseek(binfile, header.section_code, SEEK_SET);

  uint64_t pc = 0;

  Instruction instrc;
  while (fread(&instrc, sizeof(Instruction), 1, binfile)) {
    switch (instrc.opcode) {
    case OPCODE_MOV:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3f;
        regsc[dst_reg].u64 = instrc.imm64;
        regsc[dst_reg].type = VAL;
      } else if (instrc.src == 0xAD) {
        uint8_t dst_reg = instrc.dst & 0x3f;
        regsc[dst_reg].u64 = instrc.imm64;
        regsc[dst_reg].type = PTR;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg] = regsc[src_reg];
        regsc[dst_reg].type = VAL;
      }
      break;
    case OPCODE_ADD:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 += instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 += regsc[src_reg].u64;
      }
      break;
    case OPCODE_SUB:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 -= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 -= regsc[src_reg].u64;
      }
      break;
    case OPCODE_MUL:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 *= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 *= regsc[src_reg].u64;
      }
      break;
    case OPCODE_DIV:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        uint64_t dividend = regsc[dst_reg].u64;
        uint64_t divisor = instrc.imm64;

        if (divisor == 0) {
          printf("Division by zero\n");
          fclose(binfile);
          return;
        }

        regsc[dst_reg].u64 = dividend / divisor;
        regsc[dst_reg + 1].u64 = dividend % divisor;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        uint64_t dividend = regsc[dst_reg].u64;
        uint64_t divisor = regsc[src_reg].u64;

        if (divisor == 0) {
          printf("Division by zero\n");
          fclose(binfile);
          return;
        }

        regsc[dst_reg].u64 = dividend / divisor;
        regsc[dst_reg + 1].u64 = dividend % divisor;
      }
      break;
    case OPCODE_AND:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 &= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 &= regsc[src_reg].u64;
      }
      break;
    case OPCODE_OR:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 |= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 |= regsc[src_reg].u64;
      }
      break;
    case OPCODE_XOR:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 ^= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 ^= regsc[src_reg].u64;
      }
      break;
    case OPCODE_NOT:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 = ~instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 = ~regsc[src_reg].u64;
      }
      break;

    case OPCODE_RET:
      if (spcall > 0) {
        pc = call_stack[--spcall];
        fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
              SEEK_SET);
        continue;
      } else {
        printf("CCPU READ ERROR: RET USAGE UNDEFINED");
        fclose(binfile);
        return;
      }
      break;
    case OPCODE_SHR:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 >>= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 >>= regsc[src_reg].u64;
      }
      break;
    case OPCODE_SHL:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 <<= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 <<= regsc[src_reg].u64;
      }
      break;
    case OPCODE_INC:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3f;
        regsc[dst_reg].u64 += 1;
      }
      break;
    case OPCODE_DEC:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3f;
        regsc[dst_reg].u64 -= 1;
      }
      break;
    case OPCODE_CMP:
      uint64_t val1, val2;
      if (instrc.src != 0xFF)
        val1 = regsc[instrc.src & 0x3F].u64;
      else
        val1 = instrc.imm64;

      if (instrc.dst != 0xFF)
        val2 = regsc[instrc.dst & 0x3F].u64;
      else
        val2 = instrc.imm64;

      int64_t result = (int64_t)val1 - (int64_t)val2;

      flags.zero = (result == 0);
      flags.sign = (result < 0);
      flags.greater = (result > 0);
      break;
    case OPCODE_JE:
      if (flags.zero) {
        pc = instrc.imm64;
        fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
              SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JNE:
      if (!flags.zero) {
        pc = instrc.imm64;
        fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
              SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JL:
      if (flags.sign) {
        pc = instrc.imm64;
        fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
              SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JG:
      if (flags.greater) {
        pc = instrc.imm64;
        fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
              SEEK_SET);
        continue;
      }
      break;
    case OPCODE_JMP:
      pc = instrc.imm64;
      fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
            SEEK_SET);
      continue;

    case OPCODE_CALL:
      call_stack[spcall++] = pc + 1;
      pc = instrc.imm64;
      fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
            SEEK_SET);
      continue;
    case OPCODE_PUSH:
      if (sppush >= 256) {
        printf("STACK OVERFLOW");
        fclose(binfile);
        return;
      }
      push_stack[sppush++] = regsc[instrc.dst & 0x3F].u64;
      break;
    case OPCODE_POP:
      if (sppush <= 0) {
        printf("STACK UNDERFLOW");
        fclose(binfile);
        return;
      }
      regsc[instrc.dst & 0x3F].u64 = push_stack[--sppush];
      break;
    case OPCODE_NOP:
      break;
    case OPCODE_HLT:
      fclose(binfile);
      return;
    case OPCODE_SYSCALL:
      uint64_t syscall_id = instrc.imm64;

      long current_pos = ftell(binfile);

      uint64_t args[6] = {0};
      for (int i = 0; i < 6; i++) {
        if (regsc[i].type == PTR) {
          args[i] = (uint64_t)(memory + regsc[i].u64 - sizeof(BinaryHeader));
        } else {
          args[i] = regsc[i].u64;
        }
      }

      long ret = syscall(syscall_id, args[0], args[1], args[2], args[3],
                         args[4], args[5]);

      regsc[6].u64 = ret;
      fseek(binfile, current_pos, SEEK_SET);
      break;
    case OPCODE_PRINT:
      // FOR DEBUG
      printf("%ld\n", regsc[instrc.dst & 0x3F].u64);
      break;
    case OPCODE_ENTRY:
      pc = instrc.imm64;
      fseek(binfile, header.section_code + (pc * sizeof(Instruction)),
            SEEK_SET);
      continue;
    default:
      continue;
    }

    pc++;
  }
  fclose(binfile);
}
