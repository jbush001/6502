                    processor 6502

                    seg code
                    org $0000
reset               ror val
                    lda val
                    brk
                    seg variables
                    org 100
val                 dc.b $1
