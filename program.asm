                    processor 6502

                    seg code
                    org $0000
reset               cmp #$77
                    sbc #$23
                    brk
                    seg variables
                    org 100
val                 dc.b $1
