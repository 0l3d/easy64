section bss      # initialize memory with zeros 
  entered reb 8  # 8 bytes 

section data     # data section 
  hello ascii "Hello, World!"  # hello message 
  password ascii "mypass"      # my password 
  match ascii "Unmatched!"     # error message 

section code                   # code section 
entry start                    # entry start 
start:            
  # read(0, entered, 6)
  mov 0, r0           
  mov entered, r1              
  mov 6, r2 
  syscall 0 

  # compare strings
  # compare_string(r1, r2)
  mov password, r1 
  mov entered, r2
  call compare_string
  
  # if r6 == 1 
  cmp r6, 1 
  je matchedfunction # then jump matchedfunction

  call unmatchedfunction # else call unmatchedfunction

compare_string: # args -> r1(string), r2(string) -> return value => r6
  load r1, r10  # r10 = *r1 
  load r2, r11  # r11 = *r2 

  cmp r10, 0    # if r10 == '\0'
  je .matched   # then jump .matched 

  cmp r11, 0    # if r11 == '\0'
  je .matched   # then jump .matched 

  cmp r10, r11  # if r10 == r11 -> if "m" == "m"
  je .matchednow  # then jump .matchednow

  jmp .unmatched  # else jump .unmatched

.matchednow:
  inc r1             # r1++ (r1 is pointer)
  inc r2             # r2++ (r2 is pointer)
  jmp compare_string # jump again compare_string (loop)

.matched:            # matched 
  mov 1, r6          # r6 => return value -> r6 = 1 (true)
  ret                # return to start 

.unmatched:          # unmatched
  mov 0, r6          # r6 => return value -> r6 = 0 (false)
  ret                # return to start

# r1 = 0 
# write(1, hello, 14)
matchedfunction:
  mov 0, r1
  mov 1, r0 
  mov hello, r1 
  mov 14, r2 
  syscall 1 
  jmp exit

# r1 = 0
# write(1, match, 10)
unmatchedfunction:
  mov 0, r1
  mov 1, r0 
  mov match, r1 
  mov 10, r2 
  syscall 1 

# exit(1)
exit: 
  mov 1, r0
  syscall 60
