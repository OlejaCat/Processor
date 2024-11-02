in
pop cx

push cx
push cx
mul

pop fx

push 0
pop ax

LOOP_I:

    push 0
    pop bx

    push ax
    push 31
    sub

    push ax
    push 31
    sub

    mul

    pop ex

    LOOP_J:
        push bx
        push 31
        sub

        push bx
        push 31
        sub

        mul

        push ex

        add

        push fx
        jbe NO_SQUARE
            push ax
            push 64
            mul

            push bx
            add

            pop dx
            push 42

            pop [dx]

        NO_SQUARE:

        push 1
        push bx
        add
        pop bx

        push bx
        push 64
        ja LOOP_J

    push 1
    push ax
    add
    pop ax

    push ax
    push 64
    ja LOOP_I

draw
hlt
