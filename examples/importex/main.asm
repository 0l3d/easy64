import "imported.asm"
import "stdlib.asm"

section code
entry start
start:
  mov 0, r0
  mov oke, r0
  call moveoke
  call movezero 
  print r0
  hlt
