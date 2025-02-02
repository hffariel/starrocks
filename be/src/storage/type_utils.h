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

#include "storage/olap_common.h"

namespace starrocks {

using DataFormatVersion = int8_t;

const DataFormatVersion kDataFormatUnknown = 0;
const DataFormatVersion kDataFormatV1 = 1;
const DataFormatVersion kDataFormatV2 = 2;

class TypeUtils {
public:
    static inline bool specific_type_of_format_v1(LogicalType type) {
        return type == TYPE_DATE_V1 || type == TYPE_DATETIME_V1 || type == TYPE_DECIMAL;
    }

    static inline bool specific_type_of_format_v2(LogicalType type) {
        return type == TYPE_DATE || type == TYPE_DATETIME || type == TYPE_DECIMALV2;
    }

    static inline size_t estimate_field_size(LogicalType type, size_t variable_length) {
        switch (type) {
        case TYPE_UNKNOWN:
        case TYPE_DISCRETE_DOUBLE:
        case TYPE_STRUCT:
        case TYPE_MAP:
        case TYPE_NONE:
        case TYPE_MAX_VALUE:
        case TYPE_BOOLEAN:
        case TYPE_TINYINT:
        case TYPE_UNSIGNED_TINYINT:
            return 1;
        case TYPE_SMALLINT:
        case TYPE_UNSIGNED_SMALLINT:
            return 2;
        case TYPE_DATE_V1:
            return 3;
        case TYPE_INT:
        case TYPE_UNSIGNED_INT:
        case TYPE_FLOAT:
        case TYPE_DATE:
        case TYPE_DECIMAL32:
            return 4;
        case TYPE_BIGINT:
        case TYPE_UNSIGNED_BIGINT:
        case TYPE_DOUBLE:
        case TYPE_DATETIME_V1:
        case TYPE_DATETIME:
        case TYPE_DECIMAL64:
            return 8;
        case TYPE_DECIMAL:
            return 12;
        case TYPE_LARGEINT:
        case TYPE_DECIMALV2:
        case TYPE_DECIMAL128:
            return 16;
        default:
            // CHAR, VARCHAR, HLL, PERCENTILE, JSON, ARRAY, OBJECT
            return variable_length;
        }
    }

    static inline LogicalType convert_to_format(LogicalType type, DataFormatVersion format_version) {
        if (format_version == kDataFormatV2) {
            switch (type) {
            case TYPE_DATE_V1:
                return TYPE_DATE;
            case TYPE_DATETIME_V1:
                return TYPE_DATETIME;
            case TYPE_DECIMAL:
                return TYPE_DECIMALV2;
            case TYPE_UNKNOWN:
            case TYPE_TINYINT:
            case TYPE_UNSIGNED_TINYINT:
            case TYPE_SMALLINT:
            case TYPE_UNSIGNED_SMALLINT:
            case TYPE_INT:
            case TYPE_UNSIGNED_INT:
            case TYPE_BIGINT:
            case TYPE_UNSIGNED_BIGINT:
            case TYPE_LARGEINT:
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
            case TYPE_DISCRETE_DOUBLE:
            case TYPE_CHAR:
            case TYPE_VARCHAR:
            case TYPE_STRUCT:
            case TYPE_ARRAY:
            case TYPE_MAP:
            case TYPE_NONE:
            case TYPE_HLL:
            case TYPE_BOOLEAN:
            case TYPE_OBJECT:
            case TYPE_DECIMAL32:
            case TYPE_DECIMAL64:
            case TYPE_DECIMAL128:
            case TYPE_DATE:
            case TYPE_DATETIME:
            case TYPE_DECIMALV2:
            case TYPE_PERCENTILE:
            case TYPE_JSON:
            case TYPE_NULL:
            case TYPE_FUNCTION:
            case TYPE_TIME:
            case TYPE_BINARY:
            case TYPE_VARBINARY:
            case TYPE_MAX_VALUE:
                return type;
                // no default by intention.
            }
        } else if (format_version == kDataFormatV1) {
            switch (type) {
            case TYPE_DATE:
                return TYPE_DATE_V1;
            case TYPE_DATETIME:
                return TYPE_DATETIME_V1;
            case TYPE_DECIMALV2:
                return TYPE_DECIMAL;
            case TYPE_UNKNOWN:
            case TYPE_NONE:
            case TYPE_STRUCT:
            case TYPE_ARRAY:
            case TYPE_MAP:
            case TYPE_DECIMAL32:
            case TYPE_DECIMAL64:
            case TYPE_DECIMAL128:
            case TYPE_NULL:
            case TYPE_FUNCTION:
            case TYPE_TIME:
            case TYPE_BINARY:
            case TYPE_VARBINARY:
                return TYPE_UNKNOWN;
            case TYPE_TINYINT:
            case TYPE_UNSIGNED_TINYINT:
            case TYPE_SMALLINT:
            case TYPE_UNSIGNED_SMALLINT:
            case TYPE_INT:
            case TYPE_UNSIGNED_INT:
            case TYPE_BIGINT:
            case TYPE_UNSIGNED_BIGINT:
            case TYPE_LARGEINT:
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
            case TYPE_DISCRETE_DOUBLE:
            case TYPE_CHAR:
            case TYPE_DATE_V1:
            case TYPE_DATETIME_V1:
            case TYPE_DECIMAL:
            case TYPE_VARCHAR:
            case TYPE_HLL:
            case TYPE_BOOLEAN:
            case TYPE_PERCENTILE:
            case TYPE_JSON:
            case TYPE_MAX_VALUE:
            case TYPE_OBJECT:
                return type;
                // no default by intention.
            }
        }
        DCHECK(false) << "unknown format version " << format_version;
        return TYPE_UNKNOWN;
    }

    static inline LogicalType to_storage_format_v2(LogicalType type) { return convert_to_format(type, kDataFormatV2); }

    static inline LogicalType to_storage_format_v1(LogicalType type) { return convert_to_format(type, kDataFormatV1); }
};

} // namespace starrocks
