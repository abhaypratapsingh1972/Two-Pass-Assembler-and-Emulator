# MIPS Assembly code for adding 31 numbers and storing result in $t0

# Initialize registers with numbers directly
    li $v0, 1   # Storing numbers in registers
    li $v1, 2
    li $a0, 3
    li $a1, 4
    li $a2, 5
    li $a3, 6
    li $t1, 8
    li $t2, 9
    li $t3, 10
    li $t4, 11
    li $t5, 12
    li $t6, 13
    li $t7, 14
    li $s0, 15
    li $s1, 16
    li $s2, 17
    li $s3, 18
    li $s4, 19
    li $s5, 20
    li $s6, 21
    li $s7, 22
    li $t8, 23
    li $t9, 24
    li $k0, 25
    li $k1, 26
    li $gp, 27
    li $sp, 28
    li $fp, 29
    li $ra, 30

# Initialize $t0 to 0 to store the result
    li $t0, 7

# Adding all the registers
    add $t0, $t0, $v0  # t0 = t0 + v0
    add $t0, $t0, $v1  # t0 = t0 + v1
    add $t0, $t0, $a0  # t0 = t0 + a0
    add $t0, $t0, $a1  # t0 = t0 + a1
    add $t0, $t0, $a2  # t0 = t0 + a2
    add $t0, $t0, $a3  # t0 = t0 + a3
    add $t0, $t0, $t1  # t0 = t0 + t1
    add $t0, $t0, $t2  # t0 = t0 + t2
    add $t0, $t0, $t3  # t0 = t0 + t3
    add $t0, $t0, $t4  # t0 = t0 + t4
    add $t0, $t0, $t5  # t0 = t0 + t5
    add $t0, $t0, $t6  # t0 = t0 + t6
    add $t0, $t0, $t7  # t0 = t0 + t7
    add $t0, $t0, $s0  # t0 = t0 + s0
    add $t0, $t0, $s1  # t0 = t0 + s1
    add $t0, $t0, $s2  # t0 = t0 + s2
    add $t0, $t0, $s3  # t0 = t0 + s3
    add $t0, $t0, $s4  # t0 = t0 + s4
    add $t0, $t0, $s5  # t0 = t0 + s5
    add $t0, $t0, $s6  # t0 = t0 + s6
    add $t0, $t0, $s7  # t0 = t0 + s7
    add $t0, $t0, $t8  # t0 = t0 + t8
    add $t0, $t0, $t9  # t0 = t0 + t9
    add $t0, $t0, $k0  # t0 = t0 + k0
    add $t0, $t0, $k1  # t0 = t0 + k1
    add $t0, $t0, $gp  # t0 = t0 + gp
    add $t0, $t0, $sp  # t0 = t0 + sp
    add $t0, $t0, $fp  # t0 = t0 + fp
    add $t0, $t0, $ra  # t0 = t0 + ra


