                    processor 6502

                    seg code
                    org $0000
reset               lda #01
                    brk
                    lda #99
                    brk
                    seg variables
                    org 100
val                 dc.b $1
