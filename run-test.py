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

import subprocess
import os
import sys

def run_test(filename):
    binname = os.path.splitext(filename)[0] + '.bin'
    subprocess.run(f'dasm {filename} -f3 -o{binname}',
                    shell=True, check=True, timeout=10,
                    stdout=subprocess.PIPE)
    result = subprocess.run(f'./emulator {binname}', shell=True,
                            check=True,
                            timeout=10, stdout=subprocess.PIPE)
    output = str(result.stdout, encoding='ASCII')
    check_prefix = '; CHECK:'
    search_offset = 0
    with open(filename) as f:
        for line in f:
            check_offs = line.find(check_prefix)
            if check_offs != -1:
                check_pattern = line[check_offs + len(check_prefix) + 1:].strip()
                got = output.find(check_pattern, search_offset)
                if got == -1:
                    raise Exception('could not find check pattern ' + check_pattern)

                search_offset = got + len(check_pattern)


for name in sys.argv[1:]:
    print('running', name)
    run_test(name)
    print('PASS')


print('All tests passed')

