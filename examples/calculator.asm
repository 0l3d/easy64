input r0     # input for calculation

cmp r0, 1    # if r0 == 1 
je add       # then jump add 

cmp r0, 2    # if r0 == 2 
je sub       # then jump sub 

cmp r0, 3    # same 
je mult      # same 

cmp r0, 4    # same 
je div       # are u still reading these comments? crazy man

jmp exit     # i know u re not reading

add:         # label bla bla im bored
  input r1   # input for num 1 
  input r2   # input for num 2

  add r2, r1 # some math power yeaah (just num1 + num2)  

  print r2   # print result
  jmp exit   # jump to exit 

             # i dont explain other things cause they re all same.
sub: 
  input r1 
  input r2 

  sub r2, r1 

  print r1
  jmp exit


mult: 
  input r1 
  input r2 

  mul r2, r1 

  print r1
  jmp exit 

div:
  input r1 
  input r2 

  div r2, r1 

  print r1
  jmp exit


exit: 
  hlt 
