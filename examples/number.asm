section bss 
  num reb 8 # 64 bit integer 


section code 
  # Write the number like this "13657"
  mov 13657, r0
  # THIS IS A NUMBER WE DONT KNOW IT 

  # n / 256 => result | rem 
  div 256, r0 
  # print r0 -> result 
  # print r1 -> rem
  # n * 256^0 = n
  # n * 256^1 = n * 256 

  # SET NUMBER
  mov 0, r10    # num[0] = rem 
  store r1, num 
  mov 1, r10    # num [1] = result
  store r0, num

  # How to get number?
  mov 0, r11 # THIS IS 'i'
  mov 1, r15 # POWER 
  mov 0, r14 # total 
  mov num, r19 # r19 = num

loop:
  load r19, r18  # *r19 = r18 
  print r18      # print steps (debug code)

  cmp r18, 0   # (null byte)
  je exit      # jump exit 
 
  mul r15, r18 # r18 *= r15 
  add r18, r14 # r14 += r18 
  mul 256, r15 # r15 *= 256 
  
  cmp r11, 8   # if r11 == 8 (end)
  je exit      # jump exit 

  inc r11      # r11++ 
  inc r19      # r19++ 
  jmp loop     # jump loop again 

exit: 
  print r14    # print result (debug code)
  hlt          # halt

