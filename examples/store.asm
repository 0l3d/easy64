section bss 
  value reb 4 # 32-bit data 

section code 
  mov 65, r9      # r9 = 65 
  mov 0, r10      # IMPORTANT!! Store pointer = r10
  store r9, value # (*value + r10) = r9
  mov 10, r9
  mov 1, r10
  store r9, value
  mov 50, r9 
  mov 2, r10 
  store r9, value 
  mov 51, r9 
  mov 3, r10 
  store r9, value

  mov value, r0    # r0 = *value

  # prints 65 
  load r0, r3
  print r3

  # prints 10 
  inc r0 
  load r0, r3 
  print r3 
