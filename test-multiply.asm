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

                    ldx #$3d
                    lda #0
                    jsr umul8x8
                    jsr print_hex16 ; CHECK: 0

                    ldx #$ab
                    lda #1
                    jsr umul8x8
                    jsr print_hex16 ; CHECK: AB

                    ldx #$27
                    lda #$13
                    jsr umul8x8
                    jsr print_hex16 ; CHECK: 2E5

                    ldx #$d3
                    lda #$f9
                    jsr umul8x8
                    jsr print_hex16 ; CHECK: CD3B

                    brk

; Unsigned multiply, 8 bit inputs, 16 bit output
; X - multiplicand
; A - multiplier
; X - result high
; A - result low
umul8x8:            stx multiplicand    ; Copy from register into our scratch var
                    sta multiplier      ; same
                    lda #0
                    sta multiplicand + 1 ; Clear high byte of multiplicand
                    sta product          ; Clear out the product
                    sta product + 1

mul_loop:           lda multiplier      ; Check if multiplier is zero
                    beq mul_done        ; if so, bail

                    lsr multiplier      ; Shift multiplier right, get low bit
                    bcc no_add          ; If not 1, we don't need to add

                    ; Add the multiplicand to the product
                    clc
                    lda product
                    adc multiplicand
                    sta product
                    lda product + 1
                    adc multiplicand + 1
                    sta product + 1

no_add:             asl multiplicand      ; Shift the multipicand left one
                    rol multiplicand + 1
                    jmp mul_loop

mul_done:           lda product            ; Reload value into registers
                    ldx product + 1
                    rts


multiplicand:       dc.s 0
multiplier:         dc.b 0
product:            dc.s 0


; X - high byte
; A - low byte
print_hex16:        pha
                    txa
                    jsr print_hex8
                    pla
                    jsr print_hex8
                    rts

; A contains byte 0-255
print_hex8:         pha
                    lsr
                    lsr
                    lsr
                    lsr
                    jsr print_digit
                    pla
                    and #$f
                    jsr print_digit
                    rts

; Number is in A (0-15)
; Clobbers: A
print_digit:        cmp #9
                    bcs letter
                    adc #48
                    sta CONSOLE_OUT
                    jmp pd_done
letter:             clc
                    adc #(65 - 10)
                    sta CONSOLE_OUT
pd_done:            rts


