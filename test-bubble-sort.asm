;
; Copyright 2024 Jeff Bush
;
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;

CONSOLE_OUT = $fffa

                    processor 6502

                    seg code
                    org $0000

                    ; Load array pointer
                    lda #<data_start
                    sta array_ptr
                    lda #>data_start
                    sta array_ptr+1
                    lda data_len
                    sta array_len
                    jsr sort_array
                    jsr print_array
                    brk


; When calling this, copy the base address into array_ptr and length into array_len
sort_array:         ldy #0
                    ldx array_len
                    dex
                    lda #0
                    sta swapped
loop0:              lda (array_ptr),Y
                    iny
                    cmp (array_ptr),Y
                    bcc noswap
                    beq noswap

                    ; Swap these two
                    pha               ; Save first slot on stack
                    lda (array_ptr),Y ; get second slot
                    dey
                    sta (array_ptr),Y ; store in first byte
                    pla               ; get the old first byte
                    iny
                    sta (array_ptr),Y ; save it in the second slot
                    lda #1
                    sta swapped

noswap:             dex
                    bne loop0

                    ; Done one pass, check if we are finished
                    lda swapped
                    bne sort_array

done:               rts

print_array:        ldy #0
                    ldx array_len
loop1:              lda (array_ptr),Y
                    sta CONSOLE_OUT
                    iny
                    dex
                    bne loop1
                    rts


array_ptr:          dc.s 0
array_len:          dc.b 0
swapped:            dc.b 0


data_start:         dc "ijdmrndefapqstbdchcggklo" ; CHECK: abccdddefgghijklmnopqrst
data_end:
data_len:           dc.b (data_end - data_start)

