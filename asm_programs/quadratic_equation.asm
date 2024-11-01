in
pop ax

in
pop bx

in
pop cx

push 0
push ax
jne QUADRATIC
    call LINEAR_CASE
    jmp OUT_PROGRAM

QUADRATIC:
    call QUADRATIC_CASE

OUT_PROGRAM:
hlt

QUADRATIC_CASE proc
    push ax
    push cx

    mul

    push 4
    mul

    push bx
    push bx
    mul

    sub

    pop dx

    push 0
    push dx

    jae ROOTS
        push 888888
        jmp OUT_QUADRATIC

    ROOTS:
        push dx
        sqrt
        pop dx

        push 2
        push ax
        mul

        push -1
        push bx
        mul

        push dx
        add

        div

        push 0
        push dx
        je OUT_QUADRATIC

        push 2
        push ax
        mul

        push dx

        push -1
        push bx
        mul

        sub

        div
        out

    OUT_QUADRATIC:

    out
    ret

QUADRATIC_CASE endp


LINEAR_CASE proc
    push 0
    push bx
    jne B_IS_ZERO

        push 0
        push cx

        jne CZERO
            push 888888


        CZERO:
            push -66666

        jmp OUT_LINEAR

    B_IS_ZERO:
        push bx

        push -1
        push cx
        mul

        div

    OUT_LINEAR:

    out
    ret

LINEAR_CASE endp
