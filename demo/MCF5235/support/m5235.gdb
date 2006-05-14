define setup-and-load
  set $rambar=0x100001
  tbreak start
  tbreak main
end

define debug-printexception
  printf "vector: %d", *(unsigned short *)$sp >> 2 &0x1F
  printf "old pc: 0x%08x", *(unsigned long *)($sp + 4)
  printf "old sr: 0x%02x", *(unsigned short *)($sp + 2)
end

define execute
#  set $sp = *(long *)0x20000
  set $pc = *(long *)0x100004
  tk gdbtk_update
end
