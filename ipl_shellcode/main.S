	.set noreorder
	.global start
	.ent    start
#entrypoint is 0x40E0000
start:
#copy + start ipl

    li      $t0, 0x4000000   #ipl stored here
    li      $t3, 0x39800      #230kB, max size of ipl part2 in 2.6-3.30
    addu    $t3, $t3, $t0
    li      $t1, 0x40f0000
loop:
    lw      $t2, 0($t0)
    sw      $t2, 0($t1)
    addiu   $t0, 4
    bne     $t3, $t0, loop
    addiu   $t1, 4

    li      $t0, 0x440F0168 #2.60-3.03 ipl jump to main.gz location
#    li      $t0, 0x440F01C4 #3.10+3.11 ipl jump to main.gz location
#    li      $t0, 0x440F0200 #3.30 ipl jump to main.gz location

    la      $t1, patch2 #MIPS_J(patch2)
    li      $t2, 0x0ffffffc
    and     $t1, $t1, $t2
    srl     $t1, 2
    li      $t2, 0x08000000
    addu    $t1, $t1, $t2

    sw      $t1, 0($t0) #overwrite ins
    sw      $0,  4($t0) #nop

    jal     cache1
    nop
    jal     cache2
    nop
#run original ins
    li       $ra, 0x40f0000
    jr       $ra
    nop

#right from the ipl, its messy but it works
#(and my disasm doesn't have allegrex additions in it)
cache1:
  .word 0x40088000
  .word 0x24090800
  .word 0x7D081180
  .word 0x01094804
  .word 0x00004021
  .word 0xBD100000
  .word 0x400AE000
  .word 0x400BE800
  .word 0x7D4C0500
  .word 0x11800004
  .word 0x7D6C0500
  .word 0x000A5340
  .word 0x01485025
  .word 0xBD5A0000
  .word 0x11800003
  .word 0x000B5B40
  .word 0x01685825
  .word 0xBD7A0000
  .word 0x25080040
  .word 0x1509FFF1
  .word 0x00000000
  .word 0x03E00008
  .word 0x0000000F

cache2:
  .word 0x40088000
  .word 0x24091000
  .word 0x7D081240
  .word 0x01094804
  .word 0x4080E000
  .word 0x4080E800
  .word 0x00004021
  .word 0xBD010000
  .word 0xBD030000
  .word 0x25080040
  .word 0x1509FFFC
  .word 0x00000000
  .word 0x03E00008
  .word 0x00000000

#running right before main.gz jump
patch2:
#copy dec'd main.gz to kmem
#stored at 0x883f0000 (not overwritten in 1.5)
    b skip_maingz #comment this out to dump main.gz
    nop

    li      $t0, 0x440F0000
    li      $t3, 0x10000
    addu    $t3, $t3, $t0
    lui     $t1, 0x483F
copy_loop2:
    lw      $t2, 0($t0)
    sw      $t2, 0($t1)
    addiu   $t0, 4
    bne     $t3, $t0, copy_loop2
    addiu   $t1, 4
    break
   
skip_maingz:
    la      $t0, patch3
    addiu   $t3, $t0, 48 #should be enough

#for 2.60-3.03 this is addr right be jump to part 3
#for 3.10+ this is right before call to kirk decrpyt
#for 2.60
#    li     $t1, 0x4008458
#for 2.70+2.71
#    li     $t1, 0x40086A0
#for 2.80+2.81+2.82
#    li     $t1, 0x4009620
#for 3.00-3.03
    li     $t1, 0x4009EC0
#for 3.10,3.11
#    li     $t1, 0x40002A8
#for 3.30
#    li     $t1, 0x40002E0

copy_loop3:
    lw      $t2, 0($t0)
    sw      $t2, 0($t1)
    addiu   $t0, 4
    bne     $t3, $t0, copy_loop3
    addiu   $t1, 4

    jal     cache1
    nop
    jal     cache2
    nop

    lui     $25, 0x0400
    li      $29, 0x40fff00
    jr      $25
    nop

patch3:
#running right before jump to part3/before kirk decrypt

    li      $t0, 0x44100000 #try 0x44100000-0x44130000 for 2.60 to 3.03
#    li      $t0, 0x08600000 #try 0x08600000-0x08630000 for 3.10+
    li      $t3, 0x10000
    addu    $t3, $t3, $t0
    lui     $t1, 0x483F
copy_loop4:
    lw      $t2, 0($t0)
    sw      $t2, 0($t1)
    addiu   $t0, 4
    bne     $t3, $t0, copy_loop4
    addiu   $t1, 4
    break

   .end start
	.set reorder
