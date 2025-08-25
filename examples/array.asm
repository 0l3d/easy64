section bss 
  myarray reb 100 # 5 * string[20]

section code 
entry test
write_str:
  mov 0, r19 # global string length count
  mov 0, r10 # store pointer 
  mov 0, r18 
  pop r1
  pop r0

  add 20, r18
  mul r1, r18

  mov r18, r19 
  sub 20, r19
  mov r19, r10

contin:
  store r0, myarray

  cmp r19, r18 
  je out

  inc r19
  inc r10 

  jmp contin

out:
  ret

test:
  mov 'A', r0    # string 
  push r0        # arg1 
  push 1         # arg2 -> index 
  call write_str # call function
  
  mov 'B', r0 
  push r0 
  push 2 
  call write_str

  mov 'C', r0 
  push r0 
  push 3 
  call write_str

  mov 'D', r0 
  push r0 
  push 4
  call write_str

  # write(1, buffer, 100);
  mov 0, r0 
  mov myarray, r1 
  mov 100, r2 
  syscall 1

# OUT : AAAAAAAAAAAAAAAABBBBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCCDDDDDDDDDDDDDDDDDDDDD


