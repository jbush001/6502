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

CFLAGS=-W -Wall -Wno-unused-parameter -g

all: emulator instruction-test

test: instruction-test emulator
	./instruction-test
	gcov instruction-test-6502-core.c
	python3 run-test.py test-*.asm

emulator: instructions.h emulator-main.c 6502-core.c
	cc $(CFLAGS) emulator-main.c 6502-core.c -o emulator

instruction-test: instructions.h instruction-test.c 6502-core.c
	cc $(CFLAGS) -fprofile-arcs -ftest-coverage instruction-test.c 6502-core.c -o instruction-test

instructions.h: make_inst_tab.py
	python3 make_inst_tab.py

clean:
	rm -f instructions.h emulator instruction-test *.gcno *.gcda *.bin *.lst

