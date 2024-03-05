#include "string.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

String::String() : size_(0), capacity_(0), str_(nullptr) {}

void String::Nullter() { str_[size_] = '\0'; }

String::String(size_t size, char character)
    : size_(size), capacity_(size), str_(new char[capacity_ + 1]) {
  for (size_t i = 0; i < size; i++) {
    str_[i] = character;
  }
  this->Nullter();
}

String::String(const char* string_1) {
  size_ = strlen(string_1);
  capacity_ = size_;
  str_ = new char[size_ + 1];
  for (size_t i = 0; i < size_; i++) {
    str_[i] = string_1[i];
  }
  this->Nullter();
}

String::String(const String& other) {
  size_ = other.size_;
  capacity_ = other.capacity_;
  str_ = new char[capacity_ + 1];
  memcpy(str_, other.str_, size_);
  this->Nullter();
}

String& String::operator=(const String& string) {
  if (string == *this) {
    return *this;
  }
  delete[] str_;
  size_ = string.size_;
  capacity_ = string.capacity_;
  str_ = new char[capacity_ + 1];
  memcpy(str_, string.str_, size_);
  this->Nullter();
  return *this;
}

String::~String() { delete[] str_; }

void String::Clear() { size_ = 0; }

void String::PushBack(char character) {
  if (capacity_ == 0) {
    size_ = 1;
    capacity_ = 1;
    str_ = new char[capacity_ + 1];
    str_[0] = character;
    this->Nullter();
    return;
  }
  if (size_ == capacity_) {
    char* new_str = str_;
    capacity_ *= 2;
    str_ = new char[capacity_ + 1];
    for (size_t i = 0; i < size_; i++) {
      str_[i] = new_str[i];
    }
    delete[] new_str;
  }
  size_++;
  str_[size_ - 1] = character;
  this->Nullter();
}

void String::PopBack() {
  if (size_ != 0) {
    size_--;
    this->Nullter();
  }
}

void String::Resize(size_t new_size, char character) {
  if (new_size > capacity_) {
    char* tmp = str_;
    capacity_ = new_size;
    str_ = new char[new_size + 1];
    memcpy(str_, tmp, size_);
    for (size_t i = size_; i < new_size; ++i) {
      str_[i] = character;
    }
    size_ = new_size;
    delete[] tmp;
  } else {
    size_ = new_size;
  }
  this->Nullter();
}

void String::Resize(size_t new_size) { this->Resize(new_size, '\0'); }

void String::Reserve(size_t new_capacity) {
  if (new_capacity > capacity_) {
    char* tmp = str_;
    capacity_ = new_capacity;
    str_ = new char[capacity_];
    memcpy(str_, tmp, size_);
    delete[] tmp;
  }
}

void String::ShrinkToFit() {
  if (capacity_ > size_) {
    capacity_ = size_;
    char* tmp = new char[size_ + 1];
    memcpy(tmp, this->str_, size_);
    delete[] this->str_;
    this->str_ = tmp;
  }
}

void String::Swap(String& other) {
  std::swap(other.size_, size_);
  std::swap(other.capacity_, capacity_);
  std::swap(other.str_, str_);
}

const char& String::operator[](size_t index) const { return str_[index]; }

char& String::operator[](size_t index) { return str_[index]; }

const char& String::Front() const { return str_[0]; }
char& String::Front() { return str_[0]; }

const char& String::Back() const { return str_[size_ - 1]; }

char& String::Back() { return str_[size_ - 1]; }

bool String::Empty() const { return (size_ == 0); }

size_t String::Size() const { return this->size_; }

size_t String::Capacity() const { return capacity_; }

char* String::Data() { return str_; }

const char* String::Data() const { return str_; }

bool String::operator==(const String& another_str) const {
  String str = *this;
  if (str.Size() != another_str.Size()) {
    return false;
  }
  for (size_t i = 0; i < str.Size(); ++i) {
    if (another_str[i] != str[i]) {
      return false;
    }
  }
  return true;
}

bool String::operator<(const String& another_str) const {
  if (this->size_ < another_str.size_) {
    return true;
  }
  if (this->size_ > another_str.size_) {
    return false;
  }
  return memcmp(this->str_, another_str.str_, size_) < 0;
}

bool String::operator>(const String& another_str) const {
  return !(*this < another_str || *this == another_str);
}

bool String::operator<=(const String& another_str) const {
  return !(*this > another_str);
}

bool String::operator>=(const String& another_str) const {
  return !(*this < another_str);
}

bool String::operator!=(const String& another_str) const {
  return !(*this == another_str);
}

String& String::operator+=(const String& new_string) {
  String answer;
  answer.capacity_ = size_ + new_string.size_;
  answer.size_ = size_ + new_string.size_;
  answer.str_ = new char[answer.size_ + 1];
  memcpy(answer.str_, str_, size_);
  memcpy(answer.str_ + size_, new_string.str_, new_string.size_);
  answer.Nullter();
  *this = answer;
  return *this;
}

String String::operator+(const String& new_string) const {
  String resstring = *this;
  resstring += new_string;
  return resstring;
}

String& String::operator*=(int n) {
  if (n == 0) {
    *this = "";
    return *this;
  }
  size_t oldsize = size_;
  this->Resize(size_ * n);
  for (int i = 1; i < n; ++i) {
    memcpy(this->str_ + oldsize * i, this->str_, oldsize);
  }
  return *this;
}

String String::operator*(int n) const {
  String new_string = *this;
  new_string *= n;
  return new_string;
}

std::vector<String> String::Split(const String& delim) {
  std::vector<String> sliced_str;
  size_t before_slice = 0;
  for (size_t index = 0; index < size_ - delim.size_ + 1; ++index) {
    if (memcmp(str_ + index, delim.str_, delim.size_) == 0) {
      String tmp;
      tmp.Reserve(index - before_slice);
      memcpy(tmp.str_, str_ + before_slice, index - before_slice);
      tmp.size_ = index - before_slice;
      sliced_str.push_back(tmp);
      before_slice = index + delim.size_;
    }
  }
  String tmp;
  tmp.Reserve(size_ - before_slice);
  memcpy(tmp.str_, str_ + before_slice, size_ - before_slice);
  tmp.size_ = size_ - before_slice;
  sliced_str.push_back(tmp);
  return sliced_str;
}

String String::Join(const std::vector<String>& strs) const {
  String tmp = *this;
  if (strs.empty()) {
    return "";
  }
  String res = strs[0];
  for (size_t i = 1; i < strs.size(); ++i) {
    res += tmp + strs[i];
  }
  return res;
}

std::ostream& operator<<(std::ostream& out, const String& str) {
  out << str.str_;
  return out;
}

std::istream& operator>>(std::istream& input, String& str) {
  str.Clear();
  char tmp;
  while (input.get(tmp)) {
    if (std::isspace(tmp) != 0) {
      return input;
    }
    str.PushBack(tmp);
  }
  return input;
}

