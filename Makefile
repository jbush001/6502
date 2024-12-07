#
# Copyright 2024 Jeff Bush
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

CFLAGS=-W -Wall -O3 -Wno-unused-parameter

all: emulator program.bin

emulator: instructions.h emulator.c
	cc emulator.c -o emulator $(CFLAGS)

instructions.h: make_inst_tab.py
	python3 make_inst_tab.py > instructions.h

clean:
	rm instructions.h emulator

program.bin: program.asm
	dasm program.asm -f3 -lprogram.lst -oprogram.bin

