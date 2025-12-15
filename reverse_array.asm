    ; Assume 'array' is the starting address of the array in memory
    ; Assume 'length' holds the length of the array
    ; This program reverses the array in place

    ldc length        ; Load the array length
    stl 0             ; Store length in local variable (offset 0)
    ldc 0             ; Initialize start index
    stl 1             ; Store in local variable (offset 1)
    ldl 0             ; Load length
    ldc 1
    sub               ; Calculate length - 1 (last index)
    stl 2             ; Store in local variable (offset 2)

reverse_loop:
    ldl 2             ; Load end index
    ldl 1             ; Load start index
    sub               ; Check if start >= end
    brlz end_reverse  ; If start >= end, we are done

    
    ; Swap array[start] and array[end]
    ldl 1             ; Load start index
    ldc array         ; Load base address of the array
    add               ; Compute address of array[start]
    ldnl 0            ; Load value at array[start]
    stl 3             ; Store in temp variable (offset 3)

    ldl 1;Load start index
    ldc array
    add
    stl 4             ; temporarily store starting index
    ldl 2             ; Load end index
    ldc array         ; Load base address of the array
    add               ; Compute address of array[end]
    ldnl 0            ; Load value at array[end]
    ldl 4             ;Load start index
    stnl 0            ;store value at array[start]=array[end]

    ldl 2
    ldc array
    add
    stl 4             ;temporarily store end index
    ldl 3             ;load array[start]
    ldl 4             ;load end address
    stnl 0            ; array[end]=array[start]

    ; Increment start and decrement end
    ldl 1             ; Load start index
    ldc 1
    add               ; Increment start index
    stl 1             ; Store updated start index

    ldl 2             ; Load end index
    ldc 1
    sub               ; Decrement end index
    stl 2             ; Store updated end index

    br reverse_loop   ; Repeat loop

end_reverse:
    HALT              ; End program

length: SET 9     ; Set the length of the array to 9

array:
    data 12       ; First element of the array
    data -7       ; Second element
    data 25       ; Third element
    data 3        ; Fourth element
    data 0        ; Fifth element
    data -14      ; Sixth element
    data 8        ; Seventh element
    data 19       ; Eighth element
    data -4       ; Ninth element
