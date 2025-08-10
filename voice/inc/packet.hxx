#pragma once

#include <cstring>
#include <memory>
#include <stdexcept>
template <typename T>
class Packet {

    public:
        Packet(T &data, size_t size) : data_(std::make_unique<T[]>(size)), size(size) {
            //TODO: std::move???
            memcpy(data_, data, size);
        };

        const T& operator[](size_t index) const {
            if (index >= size) {
                throw std::out_of_range("out of range");
            }

            return data_[index];
        };

    private:
        std::unique_ptr<T> data_;
        size_t size;

};
