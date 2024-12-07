                    processor 6502

                    seg code
                    org $0000
reset               lda myvar2
                    sta myvar1
                    rol myvar1
                    lda myvar1
                    brk

                    seg variables
                    org 10
myvar1              dc.b $88
myvar2              dc.b $a5