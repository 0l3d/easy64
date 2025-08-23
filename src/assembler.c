#include "easy.h"
#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int error_report = 0;
int linecounter = 0;

Label labels[300] = {0};
int label_count = 0;

BSS bss[300] = {0};
int bss_count;

Data datas[300] = {0};
int data_count = 0;

Section sections;

Register64 regs[NUM_REGS];

int tokenizer(char *bufin, char *bufout[], int max_count) {
  int token_count = 0;
  const char *p = bufin;
  while (*p != '\0') {
    if (*p == '#') {
      break;
    } else if (*p == ',') {
      char *sub = malloc(2);
      sub[0] = ',';
      sub[1] = '\0';
      bufout[token_count++] = sub;
      p++;
    } else if (*p == ':') {
      char *sub = malloc(2);
      sub[0] = ':';
      sub[1] = '\0';
      bufout[token_count++] = sub;
      p++;
    } else if (isspace(*p)) {
      p++;
      continue;
    } else if (*p == '\0') {
      break;
    } else if (*p == '"' || *p == '\'') {
      char in_string = *p;
      const char *string_start = ++p;
      while (*p && *p != in_string)
        p++;
      int stringlen = p - string_start;
      char *in_string_tokens = malloc(stringlen + 1);
      strncpy(in_string_tokens, string_start, stringlen);
      in_string_tokens[stringlen] = '\0';
      bufout[token_count++] = in_string_tokens;
      if (*p)
        p++;
    } else {
      const char *word_start = p;
      while (*p && !isspace(*p) && *p != ',' && *p != ':')
        p++;
      int word_len = p - word_start;
      char *word = malloc(word_len + 1);
      strncpy(word, word_start, word_len);
      word[word_len] = '\0';
      bufout[token_count++] = word;
    }
    if (token_count >= max_count)
      break;
  }
  bufout[token_count] = NULL;
  return token_count;
}

int section(char **tokens) {
  if (strcmp(tokens[0], "section") == 0) {
    if (strcmp(tokens[1], "code") == 0) {
      sections = SECTION_CODE;
    } else if (strcmp(tokens[1], "data") == 0) {
      sections = SECTION_DATA;
    } else if (strcmp(tokens[1], "bss") == 0) {
      sections = SECTION_BSS;
    } else {
      printf("Got an error Line %d, Situation: 'undefined section'.",
             linecounter);
    }
  }
  return 0;
}

void labelsp(const char *label_name, int pos, LabelType type) {
  if (label_count == 300) {
    printf("MAXIMUM LABEL : 300 ");
    return;
  }
  strcpy(labels[label_count].name, label_name);
  labels[label_count].address = pos;
  labels[label_count].type = type;
  label_count++;
}

void datasp(const char *data_label_name, int pos, DataType type) {
  if (data_count == 300) {
    printf("MAXIMUM DATA : 300");
    return;
  }
  strcpy(datas[data_count].name, data_label_name);
  datas[data_count].addr = pos;
  datas[data_count].type = type;
  data_count++;
}

void bsssp(const char *bss_label_name, int pos, BssType type, int size) {
  if (bss_count == 300) {
    printf("MAXIMUM BSS : 300");
    return;
  }
  strcpy(bss[bss_count].name, bss_label_name);
  bss[bss_count].section.bss_id = bss_count;
  bss[bss_count].section.addr = pos;
  bss[bss_count].section.size = size;
  bss[bss_count].section.type = type;
  bss_count++;
}

uint8_t en_registers(const char *reg_name, int reg_adr) {
  int index = -1;
  uint8_t type = 0;

  if (reg_name[0] == 'r')
    type = 0;
  else if (reg_name[0] == 'e')
    type = 1;
  else if (reg_name[0] == 'b')
    type = 2;
  else if (reg_name[0] == 'p')
    type = 3;
  else
    return 0xFF;

  index = atoi(&reg_name[1]);

  return (type << 6) | (index & 0x3F);
}

int get_data_addr(const char *name) {
  for (int i = 0; i < data_count; i++) {
    if (strcmp(datas[i].name, name) == 0) {
      return datas[i].addr;
    }
  }
  return -1;
}

int get_bss_addr(const char *name) {
  for (int i = 0; i < bss_count; i++) {
    if (strcmp(bss[i].name, name) == 0) {
      return bss[i].section.addr;
    }
  }
  return -1;
}

void def_operants(char **tokenized, Instruction *instrc, uint8_t opcode) {

  if (tokenized[2][0] != ',') {
    printf("Got an error on Line %d, Situation: ',' undefined on %s\n",
           linecounter, tokenized[0]);
    error_report = 1;
  } else if (strcmp(tokenized[1], tokenized[3]) == 0) {
    printf("Warning: same register? Line: %d. Situation: %s -> %s on %s\n",
           linecounter, tokenized[1], tokenized[3], tokenized[0]);
  }

  instrc->opcode = opcode;
  if (isalpha(tokenized[1][0])) {
    if (isalpha(tokenized[1][1])) {
      int addr = 0;
      addr = get_data_addr(tokenized[1]);
      if (addr == -1) {
        addr = get_bss_addr(tokenized[1]);
        if (addr == -1) {
          printf("Got an error Line: %d, Situation: 'undefined label' on %s",
                 linecounter, tokenized[0]);
          return;
        }
      }
      instrc->src = 0xAD;
      instrc->imm64 = addr;
    } else {
      instrc->src = en_registers(tokenized[1], 0);
    }
  } else {
    instrc->src = 0xFF;
    instrc->imm64 = (uint64_t)strtol(tokenized[1], NULL, 0);
  }

  if (isalpha(tokenized[3][0])) {
    instrc->dst = en_registers(tokenized[3], 0);
  } else {
    instrc->dst = 0xFF;
    instrc->imm64 = (uint64_t)strtol(tokenized[3], NULL, 0);
  }
}

int get_label_addr(const char *name) {
  for (int i = 0; i < label_count; i++) {
    if (strcmp(labels[i].name, name) == 0) {
      return labels[i].address;
    }
  }
  return -1;
}

void lbl_operand(char **tokens, Instruction *instrc, uint8_t opcode) {
  instrc->opcode = opcode;
  const char *label = tokens[1];

  int label_addr = get_label_addr(label);
  if (label_addr == -1) {
    printf("Got an error Line: %d, Sitation: 'undefined label' %s\n",
           linecounter, label);
    error_report = 1;
  } else {
    instrc->imm64 = label_addr;
    instrc->src = 0xFF;
  }
}

void nlbl_operants(char **tokens, Instruction *instrc, uint8_t opcode) {
  instrc->opcode = opcode;
  const char *label = tokens[1];
  instrc->dst = en_registers(tokens[1], 0);
  instrc->src = 0xFF;
  instrc->imm64 = 0;
}

void nothing_operants(char **tokens, Instruction *instrc, uint8_t opcode) {
  instrc->opcode = opcode;
  instrc->dst = 0xFF;
  instrc->src = 0xFF;
}

void nlblnorg_operants(char **tokens, Instruction *instrc, uint8_t opcode) {
  instrc->opcode = opcode;
  instrc->imm64 = (uint64_t)strtol(tokens[1], NULL, 0);
  instrc->dst = 0xFF;
  instrc->src = 0xFF;
}

void revdef_operants(char **tokenized, Instruction *instrc, uint8_t opcode) {
  if (tokenized[2][0] != ',') {
    printf("Got an error on Line %d, Situation: ',' undefined on %s\n",
           linecounter, tokenized[0]);
    error_report = 1;
  } else if (strcmp(tokenized[1], tokenized[3]) == 0) {
    printf("Warning: same register? Line: %d. Situation: %s -> %s on %s\n",
           linecounter, tokenized[1], tokenized[3], tokenized[0]);
  }

  instrc->opcode = opcode;
  if (isalpha(tokenized[3][0])) {
    if (isalpha(tokenized[3][1])) {
      int addr = 0;
      addr = get_data_addr(tokenized[3]);
      if (addr == -1) {
        addr = get_bss_addr(tokenized[3]);
        if (addr == -1) {
          printf("Got an error Line: %d, Situation: 'undefined label' on %s",
                 linecounter, tokenized[0]);
          return;
        }
      }
      instrc->dst = 0xAD;
      instrc->imm64 = addr;
    } else {
      printf("Got an error on Line %d, Situation: 'undefined label' on %s",
             linecounter, tokenized[0]);
    }
  }
  if (isalpha(tokenized[1][0])) {
    instrc->src = en_registers(tokenized[1], 0);
  } else {
    instrc->src = 0xFF;
    instrc->imm64 = (uint64_t)strtol(tokenized[1], NULL, 0);
  }
}

int opcode(char *tokenized[], int count, Instruction *instrc) {
  if (strcmp(tokenized[0], "nop") == 0) {
    nothing_operants(tokenized, instrc, OPCODE_NOP);
    return 1;
  } else if (strcmp(tokenized[0], "mov") == 0) {
    def_operants(tokenized, instrc, OPCODE_MOV);
    return 1;
  } else if (strcmp(tokenized[0], "add") == 0) {
    def_operants(tokenized, instrc, OPCODE_ADD);
    return 1;
  } else if (strcmp(tokenized[0], "sub") == 0) {
    def_operants(tokenized, instrc, OPCODE_SUB);
    return 1;
  } else if (strcmp(tokenized[0], "mul") == 0) {
    def_operants(tokenized, instrc, OPCODE_MUL);
    return 1;
  } else if (strcmp(tokenized[0], "div") == 0) {
    def_operants(tokenized, instrc, OPCODE_DIV);
    return 1;
  } else if (strcmp(tokenized[0], "and") == 0) {
    def_operants(tokenized, instrc, OPCODE_AND);
    return 1;
  } else if (strcmp(tokenized[0], "or") == 0) {
    def_operants(tokenized, instrc, OPCODE_OR);
    return 1;
  } else if (strcmp(tokenized[0], "xor") == 0) {
    def_operants(tokenized, instrc, OPCODE_XOR);
    return 1;
  } else if (strcmp(tokenized[0], "not") == 0) {
    def_operants(tokenized, instrc, OPCODE_NOT);
    return 1;
  } else if (strcmp(tokenized[0], "shl") == 0) {
    def_operants(tokenized, instrc, OPCODE_SHL);
    return 1;
  } else if (strcmp(tokenized[0], "shr") == 0) {
    def_operants(tokenized, instrc, OPCODE_SHR);
    return 1;
  } else if (strcmp(tokenized[0], "jmp") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_JMP);
    return 1;
  } else if (strcmp(tokenized[0], "je") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_JE);
    return 1;
  } else if (strcmp(tokenized[0], "jne") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_JNE);
    return 1;
  } else if (strcmp(tokenized[0], "jl") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_JL);
    return 1;
  } else if (strcmp(tokenized[0], "jg") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_JG);
    return 1;
  } else if (strcmp(tokenized[0], "cmp") == 0) {
    def_operants(tokenized, instrc, OPCODE_CMP);
    return 1;
  } else if (strcmp(tokenized[0], "call") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_CALL);
    return 1;
  } else if (strcmp(tokenized[0], "ret") == 0) {
    nothing_operants(tokenized, instrc, OPCODE_RET);
    return 1;
  } else if (strcmp(tokenized[0], "push") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_PUSH);
    return 1;
  } else if (strcmp(tokenized[0], "pop") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_POP);
    return 1;
  } else if (strcmp(tokenized[0], "hlt") == 0) {
    nothing_operants(tokenized, instrc, OPCODE_HLT);
    return 1;
  } else if (strcmp(tokenized[0], "inc") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_INC);
    return 1;
  } else if (strcmp(tokenized[0], "dec") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_DEC);
    return 1;
  } else if (strcmp(tokenized[0], "print") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_PRINT);
    return 1;
  } else if (strcmp(tokenized[0], "input") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_INPUT);
    return 1;
  } else if (strcmp(tokenized[0], "printstr") == 0) {
    nlbl_operants(tokenized, instrc, OPCODE_PRINTSTR);
    return 1;
  } else if (strcmp(tokenized[0], "entry") == 0) {
    lbl_operand(tokenized, instrc, OPCODE_ENTRY);
    return 1;
  } else if (strcmp(tokenized[0], "syscall") == 0) {
    nlblnorg_operants(tokenized, instrc, OPCODE_SYSCALL);
    return 1;
  } else if (strcmp(tokenized[0], "load") == 0) {
    def_operants(tokenized, instrc, OPCODE_LOAD);
    return 1;
  } else if (strcmp(tokenized[0], "store") == 0) {
    revdef_operants(tokenized, instrc, OPCODE_STORE);
    return 1;
  }
  return 0;
}

void parser(const char *asm_file, const char *out_file) {
  FILE *as_file = fopen(asm_file, "r");
  if (as_file == NULL) {
    printf("assembler failed to open file first pass\n");
    return;
  }

  char line[256];
  int byte_offset = 0;
  linecounter = 0;

  int current_instruction_pos = 0;

  sections = SECTION_DATA;

  BinaryHeader header;
  memset(&header, 0, sizeof(header));
  while (fgets(line, sizeof(line), as_file)) {
    linecounter++;
    char *linecopy = strdup(line);

    char *tokens[1000];
    int count = tokenizer(line, tokens, 10);

    if (count >= 1) {
      section(tokens);
    }

    if (sections == SECTION_CODE) {
      if (!header.section_code) {
        header.section_code = byte_offset + sizeof(BinaryHeader);
      }

      if (count >= 2 && strcmp(tokens[1], ":") == 0) {
        labelsp(tokens[0], current_instruction_pos, LABEL_TYPE_CODE);
      }

      if (count >= 1 && strcmp(tokens[0], "section") != 0) {
        if (count == 1 || (count >= 2 && strcmp(tokens[1], ":") != 0)) {
          current_instruction_pos++;
        }
      }

    } else if (sections == SECTION_DATA) {
      if (!header.section_data) {
        header.section_data = byte_offset + sizeof(BinaryHeader);
      }
      if (count >= 3 && strcmp(tokens[1], "ascii") == 0) {
        datasp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_ASCII);
        size_t len = strlen(tokens[2]);
        byte_offset += len + 1;
      } else if (count >= 3 && strcmp(tokens[1], "byte") == 0) {
        datasp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_BYTE);
        byte_offset += 1;
      } else if (count >= 3 && strcmp(tokens[1], "hword") == 0) {
        datasp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_HWORD);
        byte_offset += 2;
      } else if (count >= 3 && strcmp(tokens[1], "word") == 0) {
        datasp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_WORD);
        byte_offset += 4;
      } else if (count >= 3 && strcmp(tokens[1], "dword") == 0) {
        datasp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_DWORD);
        byte_offset += 8;
      }
    } else if (sections == SECTION_BSS) {
      if (!header.section_bss) {
        header.section_bss = byte_offset + sizeof(BinaryHeader);
      }
      if (count >= 3 && strcmp(tokens[1], "reb") == 0) {
        int size = atoi(tokens[2]);
        bsssp(tokens[0], byte_offset + sizeof(BinaryHeader), DATA_TYPE_RB,
              size);
        byte_offset += sizeof(BSSSectionType);
      }
    }

    for (int i = 0; i < count; i++) {
      free(tokens[i]);
    }
    free(linecopy);
  }
  fclose(as_file);

  as_file = fopen(asm_file, "r");
  FILE *out = fopen(out_file, "wb");
  if (as_file == NULL || out == NULL) {
    printf("assembler failed to open file second pass\n");
    if (as_file)
      fclose(as_file);
    if (out)
      fclose(out);
    return;
  }

  header.bss_count = bss_count;
  fwrite(&header, sizeof(header), 1, out);

  linecounter = 0;
  int bss_i = 0;
  while (fgets(line, sizeof(line), as_file)) {
    linecounter++;

    char *tokens[10];
    int count = tokenizer(line, tokens, 10);

    if (count == 0)
      continue;

    if (count == 2 && strcmp(tokens[1], ":") == 0) {
      for (int i = 0; i < count; i++) {
        free(tokens[i]);
      }
      continue;
    }
    if (count >= 1 && strcmp(tokens[0], "section") == 0) {
      section(tokens);
      for (int i = 0; i < count; i++) {
        free(tokens[i]);
      }
      continue;
    }

    if (sections == SECTION_CODE) {
      Instruction instrc = {0};
      if (opcode(tokens, count, &instrc)) {
        fwrite(&instrc, sizeof(instrc), 1, out);
      }
    } else if (sections == SECTION_DATA) {
      if (count >= 3 && strcmp(tokens[1], "ascii") == 0) {
        const char *str = tokens[2];
        size_t len = strlen(str);
        fwrite(str, 1, len, out);
        fputc('\0', out);
      } else if (count >= 3 && strcmp(tokens[1], "byte") == 0) {
        uint8_t val = (uint8_t)strtol(tokens[2], NULL, 0);
        fputc(val, out);
      } else if (count >= 3 && strcmp(tokens[1], "hword") == 0) {
        uint16_t val = (uint16_t)strtol(tokens[2], NULL, 0);
        fwrite(&val, 1, 2, out);
      } else if (count >= 3 && strcmp(tokens[1], "word") == 0) {
        uint32_t val = (uint32_t)strtol(tokens[2], NULL, 0);
        fwrite(&val, 1, 4, out);
      } else if (count >= 3 && strcmp(tokens[1], "dword") == 0) {
        uint64_t val = (uint64_t)strtol(tokens[2], NULL, 0);
        fwrite(&val, 1, 4, out);
      }
    } else if (sections == SECTION_BSS) {
      if (count >= 3) {
        fwrite(&bss[bss_i++].section, sizeof(BSSSectionType), 1, out);
      }
    }

    for (int i = 0; i < count; i++) {
      free(tokens[i]);
    }
  }

  fclose(as_file);
  fclose(out);
}
