/*
 * Copyright 2014-present Alibaba Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __INDEXLIB_RANGE_INDEX_READER_H
#define __INDEXLIB_RANGE_INDEX_READER_H

#include <memory>

#include "indexlib/common/field_format/range/range_field_encoder.h"
#include "indexlib/common_define.h"
#include "indexlib/index/inverted_index/SegmentPostings.h"
#include "indexlib/index/normal/inverted_index/accessor/normal_index_reader.h"
#include "indexlib/indexlib.h"

DECLARE_REFERENCE_CLASS(config, IndexConfig);
DECLARE_REFERENCE_CLASS(index, AttributeReader);

namespace indexlib { namespace index {
class DocValueFilter;
}} // namespace indexlib::index

namespace indexlib { namespace index { namespace legacy {

class RangeIndexReader : public NormalIndexReader
{
public:
    RangeIndexReader();
    ~RangeIndexReader();

public:
    void Open(const config::IndexConfigPtr& indexConfig, const index_base::PartitionDataPtr& partitionData,
              const InvertedIndexReader* hintReader = nullptr) override;
    void AddAttributeReader(const AttributeReaderPtr& attrReader) { mAttrReader = attrReader; }
    index::Result<index::PostingIterator*> Lookup(const index::Term& term, uint32_t statePoolSize = 1000,
                                                  PostingType type = pt_default,
                                                  autil::mem_pool::Pool* sessionPool = NULL) override;

    future_lite::coro::Lazy<index::Result<index::PostingIterator*>>
    LookupAsync(const index::Term* term, uint32_t statePoolSize, PostingType type, autil::mem_pool::Pool* pool,
                file_system::ReadOption option) noexcept override;

    index::Result<index::PostingIterator*> PartialLookup(const index::Term& term, const DocIdRangeVector& ranges,
                                                         uint32_t statePoolSize, PostingType type,
                                                         autil::mem_pool::Pool* sessionPool) override
    {
        return Lookup(term, statePoolSize, type, sessionPool);
    }

protected:
    index::NormalIndexSegmentReaderPtr CreateSegmentReader() override;
    std::shared_ptr<BuildingIndexReader> CreateBuildingIndexReader() override;
    DocValueFilter* CreateDocValueFilter(const index::Term& term, const AttributeReaderPtr& attrReader,
                                         autil::mem_pool::Pool* sessionPool);

private:
    future_lite::coro::Lazy<index::Result<SegmentPostingsVec>>
    GetSegmentPostings(uint64_t leftTerm, uint64_t rightTerm, autil::mem_pool::Pool* sessionPool,
                       const DocIdRangeVector& ranges, file_system::ReadOption option) const noexcept;

private:
    indexlib::common::RangeFieldEncoder::RangeFieldType mRangeFieldType;
    AttributeReaderPtr mAttrReader;

private:
    IE_LOG_DECLARE();
};

DEFINE_SHARED_PTR(RangeIndexReader);
}}} // namespace indexlib::index::legacy

#endif //__INDEXLIB_RANGE_INDEX_READER_H
