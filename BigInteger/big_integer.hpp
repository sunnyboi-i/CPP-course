#pragma once
#include <iostream>
#include <vector>

class BigInt {
  std::vector<int8_t> myvec_;
  bool sign_;
  static BigInt Division(BigInt one, BigInt other);

 public:
  int64_t static Counter(int64_t n);

  BigInt();

  BigInt(int64_t n);

  bool operator==(const BigInt& other) const;

  bool operator!=(const BigInt& other) const;

  bool operator<(const BigInt& other) const;

  bool operator>(const BigInt& other) const;

  bool operator<=(const BigInt& other) const;

  bool operator>=(const BigInt& other) const;

  BigInt operator-() const;

  static void Sum(const BigInt& one, const BigInt& two, BigInt& res);

  BigInt operator+(const BigInt& other) const;

  BigInt& operator+=(const BigInt& other);

  BigInt& operator-=(const BigInt& other);

  static void Diff(const BigInt& one, const BigInt& two, BigInt& res);

  BigInt operator-(const BigInt& other) const;

  static BigInt Multiply(BigInt one, BigInt two);

  static BigInt Divide2(BigInt one);

  static BigInt Modulo(BigInt one, BigInt two);

  BigInt operator%(const BigInt& other);

  BigInt operator/(const BigInt& other);

  BigInt operator*(const BigInt& other);

  BigInt& operator++();

  BigInt operator++(int);

  BigInt& operator--();

  BigInt operator--(int);

  BigInt(std::string num);

  friend std::ostream& operator<<(std::ostream& out, const BigInt& num);

  friend std::istream& operator>>(std::istream& inn, BigInt& num);
};

