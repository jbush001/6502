                    processor 6502

                    seg code
                    org $0000
reset               lda myvar
                    brk

                    seg variables
                    org $100
myvar               dc.b $78
