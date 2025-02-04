#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

class Deque {
   public:
    Deque() = default;

    Deque(const Deque& other) {
        for (int i = 0; i < other.size_; i++) {
            PushBack(other[i]);
        }
    }

    Deque(Deque&& other) noexcept
        : data_(other.data_)
        , size_(other.size_)
        , capacity_(other.capacity_)
        , begin_(other.begin_)
        , end_(other.end_)
        , pos_bgn_(other.pos_bgn_)
        , pos_end_(other.pos_end_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        other.pos_bgn_ = 0;
        other.pos_end_ = 0;
        other.begin_ = 0;
        other.end_ = 0;
    }

    explicit Deque(const size_t size) {
        for (size_t i = 0; i < size; i++) {
            PushBack(0);
        }
    }

    Deque(const std::initializer_list<int> list) {
        for (const auto& u : list) {
            PushBack(u);
        }
    }

    Deque& operator=(const Deque& other) {
        if (this == &other) {
            return *this;
        }
        Clear();
        for (size_t i = 0; i < other.Size(); i++) {
            PushBack(other[i]);
        }
        return *this;
    }

    Deque& operator=(Deque&& other) noexcept {
        Clear();
        data_ = (other.data_);
        size_ = (other.size_);
        capacity_ = (other.capacity_);
        begin_ = (other.begin_);
        end_ = (other.end_);
        pos_bgn_ = (other.pos_bgn_);
        pos_end_ = (other.pos_end_);
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        other.pos_bgn_ = 0;
        other.pos_end_ = 0;
        other.begin_ = 0;
        other.end_ = 0;
        return *this;
    }

    ~Deque() {
        Clear();
    }

    void Swap(Deque& other) {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(begin_, other.begin_);
        std::swap(end_, other.end_);
        std::swap(pos_bgn_, other.pos_bgn_);
        std::swap(pos_end_, other.pos_end_);
    }

    void Reallocation(const int start) {
        if (capacity_ == -1) {
            data_ = new int*[1];    // NOLINT(cppcoreguidelines-owning-memory)
            *data_ = new int[128];  // NOLINT(cppcoreguidelines-owning-memory)
            capacity_ = 1;
            if (start == 1) {
                pos_end_ = 127;
                pos_bgn_ = 128;
            } else {
                pos_bgn_ = 0;
                pos_end_ = -1;
            }
            return;
        }
        capacity_ *= 2;
        int** temp = data_;
        data_ = new int*[capacity_];  // NOLINT(cppcoreguidelines-owning-memory)
        int pos = 0;
        int pos1 = 0;
        for (int i = start; pos < capacity_ / 2; i = (i + 1) % (capacity_ / 2)) {
            if (i == end_) {
                pos1 = pos;
            }
            if (temp != nullptr) {
                *std::next(data_, pos) = *std::next(temp, i);  // NOLINT
            }
            pos++;
        }
        for (int i = (capacity_ / 2); i < capacity_; i++) {
            *std::next(data_, i) = new int[128];  // NOLINT(cppcoreguidelines-owning-memory)
        }
        begin_ = 0;
        end_ = pos1;
        delete[] temp;  // NOLINT(cppcoreguidelines-owning-memory)
    }

    void PushBack(const int value) {
        size_++;
        if (capacity_ == -1) {
            Reallocation(0);
        }
        if (Size() == Capacity()) {
            Reallocation(begin_);
        }
        if (const int end2 = (end_ + (pos_end_ + 1) / 128) % capacity_;
            begin_ == end2 && pos_end_ == 127) {
            Reallocation(begin_);
        }
        end_ = (end_ + (pos_end_ + 1) / 128) % capacity_;
        pos_end_ = (pos_end_ + 1) % 128;
        *std::next(*std::next(data_, end_), pos_end_) = value;  // NOLINT
    }

    void PopBack() {
        size_--;
        end_ = (end_ - (pos_end_ == 0 ? 1 : 0) + capacity_) % capacity_;
        pos_end_ = (pos_end_ + 127) % 128;
    }

    void PushFront(const int value) {
        size_++;
        if (capacity_ == -1) {
            Reallocation(1);
        }
        if (Size() == Capacity()) {
            Reallocation(begin_);
        }
        if (const int begin2 = (begin_ - (pos_bgn_ == 0 ? 1 : 0) + capacity_) % capacity_;
            begin2 == end_ && pos_bgn_ == 0) {
            Reallocation(begin_);
        }
        begin_ = (begin_ - (pos_bgn_ == 0 ? 1 : 0) + capacity_) % capacity_;
        pos_bgn_ = (pos_bgn_ + 127) % 128;
        if (data_ != nullptr) {
            *std::next(*std::next(data_, begin_), pos_bgn_) = value;  // NOLINT
        }
    }

    void PopFront() {
        size_--;
        begin_ = (begin_ + (pos_bgn_ + 1) / 128) % capacity_;
        pos_bgn_ = (pos_bgn_ + 1) % 128;
    }

    int& operator[](const size_t index) {
        const int pos = static_cast<int>(index);
        return *std::next(
            *std::next(data_, (begin_ + ((pos_bgn_ + pos) / 128)) % capacity_),
            (pos_bgn_ + pos) % 128);
    }

    const int& operator[](const size_t index) const {
        const int pos = static_cast<int>(index);
        return *std::next(
            *std::next(data_, (begin_ + ((pos_bgn_ + pos) / 128)) % capacity_),
            (pos_bgn_ + pos) % 128);
    }

    [[nodiscard]] size_t Size() const {
        return size_;
    }

    [[nodiscard]] size_t Capacity() const {
        return static_cast<size_t>(capacity_) * 128;
    }

    void Clear() {
        if (data_ != nullptr) {
            for (int i = 0; i < capacity_; i++) {
                delete[] *std::next(data_, i);  // NOLINT(cppcoreguidelines-owning-memory)
                *std::next(data_, i) = nullptr;
            }
        }
        delete[] data_;
        data_ = nullptr;
        size_ = 0;
        capacity_ = -1;
        begin_ = 0;
        end_ = 0;
        pos_bgn_ = 0;
        pos_end_ = 0;
    }

   private:
    int** data_ = nullptr;
    size_t size_ = 0;
    int capacity_ = -1;
    int begin_ = 0;
    int end_ = 0;
    int pos_bgn_ = 0;
    int pos_end_ = 0;
};
