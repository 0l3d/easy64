#include "easy.h"
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv) {
  char *output_name = "a.out";
  int opt;
  while ((opt = getopt(argc, argv, "hc:i:")) != -1) {
    switch (opt) {
    case 'h':
      // help message not yet
      break;
    case 'c':
      parser(optarg, output_name);
      break;
    case 'i':
      interpret_easy64(output_name);
      break;
    }
  }
  return 0;
}
