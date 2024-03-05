#pragma once

#include <iostream>
#include <vector>

class String {
  size_t size_;
  size_t capacity_;
  char* str_;

 public:
  String();

  String(size_t size, char character);

  String(const char* string_1);

  String(const String& other);

  String& operator=(const String& string);

  ~String();

  void Clear();

  void Nullter();

  void PushBack(char character);

  void PopBack();

  void Resize(size_t new_size);

  void Resize(size_t new_size, char character);

  void Reserve(size_t new_capacity);

  void ShrinkToFit();

  void Swap(String& other);

  const char& operator[](size_t index) const;

  char& operator[](size_t index);

  const char& Front() const;

  char& Front();

  const char& Back() const;

  char& Back();

  bool Empty() const;

  size_t Size() const;

  size_t Capacity() const;

  char* Data();

  const char* Data() const;

  bool operator==(const String& another_str) const;

  bool operator>(const String& another_str) const;

  bool operator<(const String& another_str) const;

  bool operator<=(const String& another_str) const;

  bool operator>=(const String& another_str) const;

  bool operator!=(const String& another_str) const;

  String operator+(const String& new_string) const;

  String& operator+=(const String& new_string);

  String& operator*=(int n);

  String operator*(int n) const;

  friend std::ostream& operator<<(std::ostream& out, const String& str);

  friend std::istream& operator>>(std::istream& input, String& str);

  std::vector<String> Split(const String& delim = " ");

  String Join(const std::vector<String>& strs) const;
};

