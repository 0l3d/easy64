# The Collatz Conjecture 
# An unsolvable problem in math 

section data                           # DATA SECTION 
  select_msg ascii "Pick an number: "  # SELECTION MESSAGE 

section code                           # CODE SECTION 
entry start                            # ENTRY POINT 
.exit:                                 # EXIT POINT 
  hlt                                  # END 

start:                                 # START 
  mov select_msg, r10                  # R10 = adress of data 
  printstr r10                         # write string 
   
  input r0;                            # Get an input 

loop:                                  # MAIN LOOP 
  print r0                             # Print current 
  cmp r0, 1                            # if r0 == 1 
  je .exit                             # then jump exit 

  mov r0, r2                           # r2 = r0 
                                       # empty 
  div 2, r2                            # div r2 by 2 (rem always rn + 1, like: r[2 + 1] = r3 is our rem)

  cmp r3, 0                            # if r3 == 0
  je .even                             # then jump even 

  cmp r3, 1                            # if r3 == 1 
  je .odd                              # then jump odd 

.even:                                 # basically if r0 is even then n/2 like: r0 / 2 
  div 2, r0 
  jmp loop 

.odd:                                  # basically if r0 is odd then 3n + 1 like : (r0 * 3) + 1
  mul 3, r0 
  add 1, r0 
  jmp loop




