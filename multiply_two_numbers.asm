ldc 0x1000       ; Set the stack pointer start location at 0x1000
a2sp             ; Transfer this address to SP (stack pointer)

ldc a            ; Load address of a
ldnl 0
stl 0            ; Store base address of a in SP + 0

ldc b            ; Load address of b
ldnl 0
stl 1            ; Store base address of b in SP + 1

ldc 0            ; Initialize result = 0
stl 2            ; Store result in SP + 2

ldc 0            ; Initialize counter = 0 (this will count the number of additions)
stl 3            ; Store counter in SP + 3

mul_loop:
    ldl 3        ; Load counter
    ldl 1        ; Load value of b

    sub          ; Check if counter >= b
    brz done     ; If counter == b, we're done

    ldl 2        ; Load current result
    ldl 0        ; Load value of a
    add          ; Add value of a to the result
    stl 2        ; Store updated result

    ldc 1        ; Load 1
    ldl 3        ; Load current counter
    add          ; Increment counter
    stl 3        ; Store updated counter

    br mul_loop  ; Repeat the loop

done:
    ldc 0
    ldc 0
    ldl 2
    ldc result   ; Load address of result
    stnl 0       ; Store the result in the result address

HALT             ; End the program

; Data Section
a:  data 10       ; a = 3
b:  data 5       ; b = 2

result: data 0   ; result will store the product of a and b
