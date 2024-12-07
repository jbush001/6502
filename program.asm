                    processor 6502

                    seg code
                    org $0000
reset               jmp (jumpptr)
                    brk
mylabel             lda #$58
                    brk

                    seg variables
                    org 100
jumpptr             dc.w mylabel
