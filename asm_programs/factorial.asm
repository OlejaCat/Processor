in
pop ax
call FACTORIAL
out
hlt

FACTORIAL proc
    push 0
    push ax

    jne ML
        push 1
        ret
    ML:
    push ax

    push 1
    push ax
    sub
    pop ax

    call FACTORIAL

    mul

    ret
FACTORIAL endp

