input r0        # 64 bit unsigned integer input 
mov 1, r1       # r1 = 1
mov 1, r2       # r2 = 1

factorial:
  cmp r1, r0    # if (r1 - r0) > 0
  jg exit       # then jump to exit
  mul r1, r2    # r2 = r2 * r1
  inc r1        # r1 = r1 + 1
  jmp factorial # jump factorial again 

exit:
  print r2      # print result 
  hlt           # the end


