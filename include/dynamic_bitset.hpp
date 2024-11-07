#pragma once

#include <vector>
#include <cassert>

namespace ecs
{
    class dynamic_bitset {
        
        class const_iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;

            constexpr const_iterator(uint8_t* ptr, const uint8_t& offset)
                : _ptr(ptr), _offset{offset} {}

            bool operator*() const
            {
		        std::uint8_t answer = (*_ptr << _offset) & BIT_LEFT;
		        return answer;
            }
            
            const_iterator& operator++() { _ptr++; return *this; } // Prefix increment
            const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; } // Postfix increment

            friend bool operator== (const const_iterator& a, const const_iterator& b)
                { return a._ptr == b._ptr && a._offset == b._offset; };
            friend bool operator!= (const const_iterator& a, const const_iterator& b)
                { return a._ptr != b._ptr || a._offset != b._offset; };

        private:
            uint8_t* _ptr;
            uint8_t _offset;
        };

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

		    assert(bit_index < _bit_size && "Bit index out of range");
		    assert(byte_index < _data.size() && "Byte index out of range");

		    if (value) // merge source and answer bytes with OR
		    	_data[byte_index] |= bitfield;
		    else // merge source and inverse answer bytes with AND
                _data[byte_index] &= (~bitfield);
        }
        [[nodiscard]] bool get(const size_t& bit_index) {
            size_t byte_index = bit_index / 8;
		    size_t offset = bit_index % 8;
            
		    assert(bit_index < _bit_size && "Bit index out of range");
		    assert(byte_index < _data.size() && "Byte index out of range");

		    std::uint8_t answer = (_data[byte_index] << offset) & BIT_LEFT;
		    return answer;
        }

        [[nodiscard]] bool operator[](const std::size_t& bit_index) const {
            size_t byte_index = bit_index / 8;
		    size_t offset = bit_index % 8;
            
		    assert(bit_index < _bit_size && "Bit index out of range");
		    assert(byte_index < _data.size() && "Byte index out of range");

		    std::uint8_t answer = (_data[byte_index] << offset) & BIT_LEFT;
		    return answer; // take first bit

            // also you can remove all bits except bit field and compare as byte
            // return (data[byte_index] & bitfield) == bitfield;
        }

        bool operator==(const dynamic_bitset& other) const
            { return _bit_size == other._bit_size && _data == other._data; }

        [[nodiscard]] size_t size() const { return _bit_size; }
        [[nodiscard]] size_t capacity() const { return _data.capacity(); }
        [[nodiscard]] std::vector<std::uint8_t> data() const { return _data; }

        void reserve(size_t new_capacity) {
            size_t byte_count = GetByteCount(new_capacity);
            _data.reserve(byte_count);
        }

        void resize(size_t new_size, bool value = false) {
            size_t byte_count = GetByteCount(new_size);
            _bit_size = new_size;
            _data.resize(byte_count, value);
        }

        constexpr const_iterator begin() {
            assert(_data.size() > 0 && "Can't iterate empty data");
            return const_iterator(_data.data(), 0);
        }
        constexpr const_iterator end() {
            assert(_data.size() > 0 && "Can't iterate empty data");
		    uint8_t offset = _bit_size % 8;
            return const_iterator(&_data.back(), offset);
        }

        bool any(size_t start_range, size_t end_range, bool any_value = true) {
		    assert(start_range <= end_range && "end_range can't be larger than start_range");
		    assert(end_range < _bit_size && "end_range out of range");

            for (size_t i = start_range; i < end_range; i++) {
                if (get(i) == any_value) {
                    return true;
                }
            }
            return false;
        }
        bool all(size_t start_range, size_t end_range, bool all_value = true) {
		    assert(start_range <= end_range && "end_range can't be larger than start_range");
		    assert(end_range < _bit_size && "end_range out of range");

            for (size_t i = start_range; i < end_range; i++) {
                if (get(i) != all_value) {
                    return false;
                }
            }
            return true;
        }

        void reset(bool value = false) {
            if (_bit_size == 0) return;

            std::uint8_t byte_content = value ? ALL1 : ALL0;
            for (size_t i = 0; i < _data.size(); i++)
                _data[i] = byte_content;
            // all bits, which allocated and not in size() updated too
        }

        void clear() { _bit_size = 0; _data.clear(); }
        void shrink_to_fit() { _data.shrink_to_fit(); }

    private:
        [[nodiscard]] size_t GetByteCount(size_t bit_count) {
            size_t byte_count = 1;
            if (bit_count >= 8) {
                assert(1 + (bit_count - 1) / 8 > 0 && "Byte count must be always > 0");
                byte_count = 1 + (bit_count - 1) / 8;
            }
            return byte_count;
        }

        size_t _bit_size;
        std::vector<std::uint8_t> _data;

        static const uint8_t BIT_LEFT = 128; // 10000000
        static const uint8_t BIT_RIGHT = 1; // 00000001
        static const uint8_t ALL0 = 0; // 00000000
        static const uint8_t ALL1 = 255; // 11111111
    };
}
