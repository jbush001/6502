                    processor 6502

                    seg code
                    org $0000
reset               stx $ce
                    sty $ce
                    brk
                    seg variables
                    org 100
val                 dc.b $1
