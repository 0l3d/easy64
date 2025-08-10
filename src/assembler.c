#include "easy.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int error_report = 0;
int linecounter = 0;

Label labels[300] = {0};
int label_count;

Register64 regs[20];

int tokenizer(char *bufin, char *bufout[], int max_count) {
  int token_count = 0;
  const char *p = bufin;
  while (*p != '\0') {
    if (*p == ',') {
      char *sub = malloc(2);
      sub[0] = ',';
      sub[1] = '\0';
      bufout[token_count++] = sub;
      p += 1;
    } else if (*p == '#') {
      break;
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
      while (*p && !isspace(*p) && *p != ',')
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

void section() {}
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
void bss() {}
void data() {}

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
    instrc->src = en_registers(tokenized[1], 0);
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
}

void nothing_operants(char **tokens, Instruction *instrc, uint8_t opcode) {
  instrc->opcode = opcode;
  instrc->dst = 0xFF;
  instrc->src = 0xFF;
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

  while (fgets(line, sizeof(line), as_file)) {
    linecounter++;
    char *linecopy = strdup(line);
    if (strchr(linecopy, ':')) {
      char *label = strtok(linecopy, ":\n");
      if (label) {
        while (isspace(*label))
          label++;
        labelsp(label, byte_offset / sizeof(Instruction), LABEL_TYPE_CODE);
      }
    } else {
      char *tokens[10];
      int count = tokenizer(line, tokens, 10);

      if (count >= 1) {
        byte_offset += sizeof(Instruction);
      }

      for (int i = 0; i < count; i++) {
        free(tokens[i]);
      }
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

  linecounter = 0;
  while (fgets(line, sizeof(line), as_file)) {
    linecounter++;
    if (strchr(line, ':'))
      continue;

    char *tokens[10];
    int count = tokenizer(line, tokens, 10);

    if (count < 1)
      continue;

    Instruction instrc = {0};
    if (opcode(tokens, count, &instrc)) {
      fwrite(&instrc, sizeof(instrc), 1, out);
    } else {
      printf("Got an error Line: %d, Situation: 'undefined token'. \nSTR: %s",
             linecounter, line);
      return;
    }

    for (int i = 0; i < count; i++) {
      free(tokens[i]);
    }
  }

  fclose(as_file);
  fclose(out);
}
