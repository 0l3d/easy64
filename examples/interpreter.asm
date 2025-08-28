section bss 
  buffer reb 128

section data 
  filename ascii "interpreter_code.script" 0 
  prints ascii "print" 0 

section code 
entry bufreader
bufreader: 
  mov -100, r0 
  mov filename, r1 
  mov 0, r2 
  syscall 257
  mov r6, r15 # fd 

  mov r15, r0 
  mov buffer, r1 
  mov 128, r2 
  syscall 0 
  mov r6, r16 # ret   

  mov buffer, r8  # r8 = buffer
  mov prints, r9  # r9 = print 


string_compare:
  load r8, r11    # r11 = *r8
  load r9, r12   # r12 = *r9 

  cmp r11, 40
  je parent1

  cmp r12, 40
  je parent1

  cmp r11, r12
  je matchingnow

  jmp exit

matchingnow:
  inc r8              
  inc r9 
  jmp string_compare
  
parent1:
  inc r8 
  load r8, r11
  mov r8, r16
  mov 0, r17

  cmp r11, 34
  je in_string

  jmp exit

in_string:
  inc r8 
  load r8, r11

  cmp r11, 34 
  je out_string

  inc r17

  jmp in_string

out_string:
  inc r16
  mov 1, r0 
  mov r16, r1 
  mov r17, r2 
  syscall 1

exit:
  mov r15, r0 
  syscall 3

  mov 0, r0 
  syscall 60
