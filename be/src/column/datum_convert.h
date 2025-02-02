// Copyright 2021-present StarRocks, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "column/datum.h"
#include "storage/types.h"
namespace starrocks::vectorized {

Status datum_from_string(Datum* dst, LogicalType type, const std::string& str, MemPool* mem_pool);
Status datum_from_string(TypeInfo* type_info, Datum* dst, const std::string& str, MemPool* mem_pool);

std::string datum_to_string(const Datum& datum, LogicalType type);
std::string datum_to_string(TypeInfo* type_info, const Datum& datum);

} // namespace starrocks::vectorized
