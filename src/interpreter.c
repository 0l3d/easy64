#include "easy.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

Flags flags = {0};

Register64 regsc[20];

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
  uint64_t pc = 0;
  uint64_t push_stack[256];

  Instruction instrc;
  fseek(binfile, 0, SEEK_SET);
  while (fread(&instrc, sizeof(Instruction), 1, binfile)) {
    switch (instrc.opcode) {
    case OPCODE_MOV:
      if (instrc.src == 0xFF) {
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 = instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg] = regsc[src_reg];
      }
      break;
    case OPCODE_ADD:
      if (instrc.src == 0xff) {
        uint8_t dst_reg = instrc.dst & 0x3f;
        regsc[dst_reg].u64 += instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3f;
        uint8_t dst_reg = instrc.dst & 0x3f;
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
        regsc[dst_reg].u64 /= instrc.imm64;
      } else {
        uint8_t src_reg = instrc.src & 0x3F;
        uint8_t dst_reg = instrc.dst & 0x3F;
        regsc[dst_reg].u64 /= regsc[src_reg].u64;
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
        fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
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
        fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JNE:
      if (!flags.zero) {
        pc = instrc.imm64;
        fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JL:
      if (flags.sign) {
        pc = instrc.imm64;
        fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
        continue;
      }
      break;

    case OPCODE_JG:
      if (flags.greater) {
        pc = instrc.imm64;
        fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
        continue;
      }
      break;
    case OPCODE_JMP:
      pc = instrc.imm64;
      fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
      continue;

    case OPCODE_CALL:
      call_stack[spcall++] = pc + 1;
      pc = instrc.imm64;
      fseek(binfile, pc * sizeof(Instruction), SEEK_SET);
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
    case OPCODE_PRINT:
      printf("%lu\n", regsc[instrc.dst & 0x3F].u64);
      break;
    case OPCODE_INPUT:
      uint64_t out;
      scanf("%lu", &out);
      regsc[instrc.dst & 0x3F].u64 = out;
      break;
    }

    pc++;
  }
  fclose(binfile);
}
