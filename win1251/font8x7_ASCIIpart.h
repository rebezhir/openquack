/* Copyright 2023 RebeZhir
 * https://github.com/rebezhir
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *     Unless required by applicable law or agreed to in writing, software
 *     distributed under the License is distributed on an "AS IS" BASIS,
 *     WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *     See the License for the specific language governing permissions and
 *     limitations under the License.
 */

#ifndef FONT8X7_ASCIIPART_H
#define FONT8X7_ASCIIPART_H

#include <stdint.h>
#include <stdbool.h>

bool is_ASCIIpart (char c);

extern const uint8_t gFont8x7_ASCIIpart[65][7];

#endif