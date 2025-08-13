section data                                            # section data 
select ascii "Select an Operation [1|+ 2|- 3|* 4|/]: "  # string for print 
number1 ascii "Number 1: "                              # string for print again 
number2 ascii "Number 2: "                              # bla bla same 
result ascii "Result: "
exit_msg ascii "Thanks for usage!"



section code     # section code 

entry start

exit:
  mov exit_msg, r5
  printstr r5
  hlt 

start:           # start entry point 
  mov select, r5 # r5 = address of select label 
  printstr r5    # go r5 address and write string 
  input r0       # input for calculation 

  cmp r0, 1      # if r0 == 1 
  je add         # then jump add 

  cmp r0, 2      # if r0 == 2 
  je sub         # then jump sub 
  
  cmp r0, 3      # same 
  je mult        # same 

  cmp r0, 4      # same 
  je div         # are u still reading these comments? crazy man

  jmp exit       # i know u re not reading

add:           # label bla bla im bored
  mov number1, r5 
  printstr r5
  input r1     # input for num 1 
  mov number2, r5 
  printstr r5
  input r2     # input for num 2

  add r2, r1   # some math power yeaah (just num1 + num2)  

  mov result, r5 
  printstr r5
  print r1     # print result
  jmp exit     # jump to exit 

               # i dont explain other things cause they re all same.
sub:
  mov number1, r5 
  printstr r5
  input r1 
  mov number2, r5 
  printstr r5
  input r2 

  sub r2, r1 

  mov result, r5
  printstr r5
  print r1
  jmp exit


mult: 
  mov number1, r5 
  printstr r5
  input r1
  mov number2, r5 
  printstr r5
  input r2 

  mul r2, r1 

  mov result, r5 
  printstr r5
  print r1
  jmp exit 

div:
  mov number1, r5 
  printstr r5
  input r1
  mov number2, r5 
  printstr r5
  input r2 

  div r2, r1 

  mov result, r5 
  printstr r5
  print r1
  jmp exit


