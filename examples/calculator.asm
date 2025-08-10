input r0

cmp r0, 1 
je add 

cmp r0, 2 
je sub 

cmp r0, 3
je mult 

cmp r0, 4 
je div 

jmp exit


add:
  input r1
  input r2 

  add r2, r1 

  print r2
  jmp exit

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
