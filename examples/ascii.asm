section bss 
  hello reb 2

section data 
  thanks ascii "Thanks!"

section code
  mov 0, r0 
  mov hello, r1 
  mov 2, r2 
  syscall 0 

  mov hello, r7 
  mov 0, r8 
  load r7, r8 
  inc r7 
  cmp 'H', r8   # now you can use def operants with ascii characters
  je Htyped 
  
  jmp exit 

Htyped:
  load r7, r8 
  cmp 'e', r8
  je Etyped 
  
  jmp exit 

Etyped:
  mov 1, r0 
  mov thanks, r1 
  mov 8, r2 
  syscall 1 


exit: 
  hlt
