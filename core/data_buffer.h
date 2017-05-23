// Copyright 2016 The Draco Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#ifndef DRACO_CORE_DATA_BUFFER_H_
#define DRACO_CORE_DATA_BUFFER_H_

#include <cstring>
#include <ostream>
#include <vector>

#include "core/draco_types.h"

namespace draco {

// Buffer descriptor servers as a unique identifier of a buffer.
struct DataBufferDescriptor {
  DataBufferDescriptor() : buffer_id(0), buffer_update_count(0) {}
  // Id of the data buffer.
  int64_t buffer_id;
  // The number of times the buffer content was updated.
  int64_t buffer_update_count;
};

// Class used for storing raw buffer data.
class DataBuffer {
 public:
  DataBuffer();
  virtual ~DataBuffer();
  bool Update(const void *data, int64_t size);
  // TODO(zhafang): The two update functions should be combined. I will
  // leave for now in case it breaks any geometry compression tools.
  bool Update(const void *data, int64_t size, int64_t offset);
  void WriteDataToStream(std::ostream &stream);
  // Reads data from the buffer. Potentially unsafe, called needs to ensure
  // the accessed memory is valid.
  void Read(int64_t byte_pos, void *out_data, size_t data_size) const {
    memcpy(out_data, data() + byte_pos, data_size);
  }

  // Writes data to the buffer. Unsafe, caller must ensure the accessed memory
  // is valid.
  void Write(int64_t byte_pos, const void *in_data, size_t data_size) {
    memcpy(const_cast<uint8_t *>(data()) + byte_pos, in_data, data_size);
  }

  // Copies data from another buffer to this buffer.
  void Copy(int64_t dst_offset, const DataBuffer *src_buf, int64_t src_offset,
            int64_t size) {
    memcpy(const_cast<uint8_t *>(data()) + dst_offset,
           src_buf->data() + src_offset, size);
  }

  void set_update_count(int64_t buffer_update_count) {
    descriptor_.buffer_update_count = buffer_update_count;
  }
  int64_t update_count() const { return descriptor_.buffer_update_count; }
  size_t data_size() const { return data_size_;/*data_.size();*/ }
  const uint8_t *data() const { return data_;/* data_.data();*/ }
  int64_t buffer_id() const { return descriptor_.buffer_id; }
  void set_buffer_id(int64_t buffer_id) { descriptor_.buffer_id = buffer_id; }

protected:
  virtual bool resize(int64_t size) = 0;

 protected:
   uint8_t *data_;
   size_t data_size_;

  // Counter incremented by Update() calls.
  DataBufferDescriptor descriptor_;
};

class VectorDataBuffer : public DataBuffer
{
public:
  VectorDataBuffer();
  virtual ~VectorDataBuffer();

  bool resize(int64_t size) override;
  
private:
  std::vector<uint8_t> vector_;
};

class FixedDataBuffer : public DataBuffer
{
public:
  FixedDataBuffer(const uint8_t *data, size_t size);
  virtual ~FixedDataBuffer();

  bool resize(int64_t size) override;

};

}  // namespace draco

#endif  // DRACO_CORE_DATA_BUFFER_H_
