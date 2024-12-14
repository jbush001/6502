                    processor 6502

                    seg code
                    org $0000
reset:              ldx #<str
                    jsr print_str
                    brk

print_str:          lda ,X
                    beq done
                    sta $fffa
                    inx
                    jmp print_str
done:               rts

str:                dc "Hello World"
                    dc.b 12, 13, 0

; CHECK: Hello World
