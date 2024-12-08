                    processor 6502

                    seg code
                    org $0000
reset               lda #$24
                    sbc #$23
                    brk
                    seg variables
                    org 100
val                 dc.b $1
