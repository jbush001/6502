                    processor 6502

                    seg code
                    org $0000
reset               ldx #0
loop                lda str,X
                    beq done
                    sta $fffa
                    inx
                    jmp loop

done                brk
                    lda #99
                    brk
str                 dc "Hello World"
                    dc.b 12, 13, 0