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
reset:              ldx #<str
print_str:          lda ,X
                    beq done
                    sta CONSOLE_OUT
                    inx
                    jmp print_str
done:               brk

str:                dc "Hello World"
                    dc.b 12, 13, 0

; CHECK: Hello World
