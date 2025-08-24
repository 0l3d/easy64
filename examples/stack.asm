# PUSH FUNCTION EXAMPLE 
# PUSH - POP EXAMPLE
section bss 
  outbuf reb 2 

section data 
  data ascii "Hello, World!"

section code 
  mov data, r0 # ptr 
  
  load r0, r8 # H 
  inc r0 
  load r0, r7 # e 
  
  mov 0, r10 
  store r8, outbuf 
  inc r10 
  store r7, outbuf
  mov outbuf, r12 

  push 1            # first argument 
  push r12          # second 
  push 2            # third 
  push 1            # syscall id 
  call scall

  # HIGH LEVEL PRINT FUNCTION
  mov data, r0 
  push r0
  call print

  jmp exit 

scall:
  pop r3 
  pop r2
  pop r1 
  pop r0
  syscall r3 
  ret 

# high level functions 
# calculate length
print:
  pop r7
  mov r7, r4
  mov 0, r3 
loop:
  load r7, r1  
  cmp r1, 0 
  je syscalling 
  
  inc r3 
  inc r7
  jmp loop
# call write
syscalling:
  mov 1, r0 
  mov r4, r1 
  mov r3, r2 
  syscall 1
  ret 

exit:
  hlt 

