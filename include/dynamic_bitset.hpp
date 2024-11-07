#pragma once

#include <vector>
#include <cassert>

namespace ecs
{
    class dynamic_bitset {
    public:
        dynamic_bitset() = default;
        dynamic_bitset(size_t new_size, bool value = false) 
            { resize(new_size, value); }
        ~dynamic_bitset() = default;
        
        dynamic_bitset(dynamic_bitset&&) = default;
        dynamic_bitset& operator=(dynamic_bitset&&) = default;
        
        dynamic_bitset(const dynamic_bitset&) = default;
        dynamic_bitset& operator=(const dynamic_bitset&) = default;

        void set(const size_t& bit_index, const bool& value = true) {
            size_t byte_index = bit_index / 8;
		    std::uint8_t offset = static_cast<std::uint8_t>(bit_index % 8);
		    std::uint8_t bitfield = static_cast<std::uint8_t>(BIT_LEFT >> offset);

		    assert(bit_index < bit_size_ && "Bit index out of range");
		    assert(byte_index < data_.size() && "Byte index out of range");

		    if (value) // merge source and answer bytes with OR
		    	data_[byte_index] |= bitfield;
		    else // merge source and inverse answer bytes with AND
                data_[byte_index] &= (~bitfield);
        }
        [[nodiscard]] bool get(const size_t& bit_index) {
            size_t byte_index = bit_index / 8;
		    size_t offset = bit_index % 8;
            
		    assert(bit_index < bit_size_ && "Bit index out of range");
		    assert(byte_index < data_.size() && "Byte index out of range");

		    std::uint8_t answer = (data_[byte_index] << offset) & BIT_LEFT;
		    return answer;
        }

        [[nodiscard]] bool operator[](const std::size_t& bit_index) const {
            size_t byte_index = bit_index / 8;
		    size_t offset = bit_index % 8;
            
		    assert(bit_index < bit_size_ && "Bit index out of range");
		    assert(byte_index < data_.size() && "Byte index out of range");

		    std::uint8_t answer = (data_[byte_index] << offset) & BIT_LEFT;
		    return answer; // take first bit

            // also you can remove all bits except bit field and compare as byte
            // return (data[byte_index] & bitfield) == bitfield;
        }

        bool operator==(const dynamic_bitset& other) const
            { return bit_size_ == other.bit_size_ && data_ == other.data_; }

        [[nodiscard]] size_t size() const { return bit_size_; }
        [[nodiscard]] size_t capacity() const { return data_.capacity(); }
        [[nodiscard]] std::vector<std::uint8_t> data() const { return data_; }

        void reserve(size_t new_capacity) {
            size_t byte_count = GetByteCount(new_capacity);
            data_.reserve(byte_count);
        }

        void resize(size_t new_size, bool value = false) {
            size_t byte_count = GetByteCount(new_size);
            bit_size_ = new_size;
            data_.resize(byte_count, value);
        }

        bool any(size_t start_range, size_t end_range, bool any_value = true) {
		    assert(start_range <= end_range && "end_range can't be larger than start_range");
		    assert(end_range < bit_size_ && "end_range out of range");

            for (size_t i = start_range; i < end_range; i++) {
                if (get(i) == any_value) {
                    return true;
                }
            }
            return false;
        }
        bool all(size_t start_range, size_t end_range, bool all_value = true) {
		    assert(start_range <= end_range && "end_range can't be larger than start_range");
		    assert(end_range < bit_size_ && "end_range out of range");

            for (size_t i = start_range; i < end_range; i++) {
                if (get(i) != all_value) {
                    return false;
                }
            }
            return true;
        }

        void reset(bool value = false) {
            if (bit_size_ == 0) return;

            std::uint8_t byte_content = value ? ALL1 : ALL0;
            for (size_t i = 0; i < data_.size(); i++)
                data_[i] = byte_content;
            // all bits, which allocated and not in size() updated too
        }

        void clear() { bit_size_ = 0; data_.clear(); }
        void shrink_to_fit() { data_.shrink_to_fit(); }

    private:
        [[nodiscard]] size_t GetByteCount(size_t bit_count) {
            size_t byte_count = 1;
            if (bit_count >= 8) {
                assert(1 + (bit_count - 1) / 8 > 0 && "Byte count must be always > 0");
                byte_count = 1 + (bit_count - 1) / 8;
            }
            return byte_count;
        }

        size_t bit_size_;
        std::vector<std::uint8_t> data_;

        static const uint8_t BIT_LEFT = 128; // 10000000
        static const uint8_t BIT_RIGHT = 1; // 00000001
        static const uint8_t ALL0 = 0; // 00000000
        static const uint8_t ALL1 = 255; // 11111111
    };
}
