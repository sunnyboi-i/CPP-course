#include "big_integer.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

const int kBase = 10;
const int kFive = 5;

int64_t BigInt::Counter(int64_t n) {
  if (n == 0) {
    return 1;
  }
  int64_t counter = 0;
  while (n != 0) {
    n /= kBase;
    counter++;
  }
  return counter;
}

BigInt::BigInt() {
  myvec_.push_back(0);
  sign_ = true;
}

BigInt::BigInt(int64_t n) {
  int64_t size = Counter(n);
  myvec_.resize(size);
  sign_ = (n >= 0);
  for (int64_t i = 0; i < size; i++) {
    myvec_[i] = std::abs(n % kBase);
    n /= kBase;
  }
}

bool BigInt::operator==(const BigInt& other) const {
  if (sign_ != other.sign_) {
    return false;
  }
  if (myvec_.size() != other.myvec_.size()) {
    return false;
  }
  for (size_t i = 0; i < myvec_.size(); ++i) {
    if (myvec_[i] != other.myvec_[i]) {
      return false;
    }
  }
  return true;
}

bool BigInt::operator!=(const BigInt& other) const { return !(*this == other); }

bool BigInt::operator<(const BigInt& other) const {
  if (!this->sign_ && other.sign_) {
    return true;
  }
  if (this->sign_ && !other.sign_) {
    return false;
  }
  int abs = 0;
  if (this->myvec_.size() < other.myvec_.size()) {
    abs = 1;
  } else if (this->myvec_.size() > other.myvec_.size()) {
    abs = -1;
  } else {
    int size = static_cast<int>(this->myvec_.size());
    for (int i = size - 1; i >= 0; --i) {
      if (this->myvec_[i] < other.myvec_[i]) {
        abs = 1;
        break;
      }
      if (this->myvec_[i] > other.myvec_[i]) {
        abs = -1;
        break;
      }
    }
  }
  if (!this->sign_) {
    abs = -abs;
  }
  return (abs == 1);
}

bool BigInt::operator>(const BigInt& other) const { return (other < *this); }

bool BigInt::operator<=(const BigInt& other) const { return !(*this > other); }
bool BigInt::operator>=(const BigInt& other) const { return !(*this < other); }

BigInt BigInt::operator-() const {
  BigInt tmp = *this;
  if (tmp == 0) {
    tmp.sign_ = true;
    return tmp;
  }
  tmp.sign_ = !tmp.sign_;
  return tmp;
}

void BigInt::Sum(const BigInt& one, const BigInt& two, BigInt& res) {
  if (one < two) {
    return Sum(two, one, res);
  }
  int trans = 0;
  for (size_t i = 0; i < one.myvec_.size(); ++i) {
    if (i < two.myvec_.size()) {
      res.myvec_[i] = one.myvec_[i] + two.myvec_[i] + trans;
      trans = res.myvec_[i] / kBase;
      res.myvec_[i] %= kBase;
    } else {
      if (res.myvec_.size() <= i) {
        res.myvec_.push_back(0);
      }
      if (i < two.myvec_.size()) {
        res.myvec_[i] = one.myvec_[i] + two.myvec_[i] + trans;
      } else {
        res.myvec_[i] = one.myvec_[i] + trans;
      }
      trans = res.myvec_[i] / kBase;
      res.myvec_[i] %= kBase;
    }
  }
  if (trans != 0) {
    res.myvec_.push_back(1);
  }
  if (res.myvec_.back() == 0) {
    res.myvec_.pop_back();
  }
}

BigInt BigInt::operator+(const BigInt& other) const {
  BigInt res = *this;
  res += other;
  return res;
}

BigInt& BigInt::operator+=(const BigInt& other) {
  if (*this <= 0 && other <= 0) {
    bool change = (*this == 0 && other < 0);
    Sum(-*this, -other, *this);
    if (change) {
      *this = -*this;
    }
    return *this;
  }
  if (other <= 0) {
    bool change = (*this < -other);
    Diff(*this, -other, *this);
    if (change) {
      *this = -*this;
    }
    return *this;
  }
  if (*this < 0) {
    bool change = (-*this <= other);
    Diff(other, -*this, *this);
    if (change) {
      *this = -*this;
    }
    return *this;
  }
  Sum(*this, other, *this);
  return *this;
}

BigInt& BigInt::operator-=(const BigInt& other) {
  *this = *this - other;
  return *this;
}

void BigInt::Diff(const BigInt& one, const BigInt& two, BigInt& res) {
  if (one < two) {
    Diff(two, one, res);
    return;
  }
  int trans = 0;
  for (size_t i = 0; i < one.myvec_.size(); ++i) {
    if (i < two.myvec_.size()) {
      res.myvec_[i] = one.myvec_[i] + trans - two.myvec_[i];
      trans = 0;
      if (res.myvec_[i] < 0) {
        res.myvec_[i] += kBase;
        trans = -1;
      }
    } else {
      if (res.myvec_.size() <= i) {
        res.myvec_.push_back(0);
      }
      res.myvec_[i] = one.myvec_[i] + trans;
      trans = 0;
      if (res.myvec_[i] < 0) {
        res.myvec_[i] += kBase;
        trans = -1;
      }
    }
  }
  while (res.myvec_.back() == 0 && res.myvec_.size() > 1) {
    res.myvec_.pop_back();
  }
}

BigInt BigInt::operator-(const BigInt& other) const { return *this + (-other); }

BigInt BigInt::Multiply(BigInt one, BigInt two) {
  BigInt pow = 1;
  BigInt res;
  for (size_t i = 0; i < two.myvec_.size(); ++i) {
    BigInt tmp;
    while (tmp.myvec_.size() < one.myvec_.size() + 1) {
      tmp.myvec_.push_back(0);
    }
    for (size_t j = 0; j < one.myvec_.size(); ++j) {
      tmp.myvec_[j] += one.myvec_[j] * two.myvec_[i];
      tmp.myvec_[j + 1] = tmp.myvec_[j] / kBase;
      tmp.myvec_[j] %= kBase;
    }
    for (size_t j = 0; j < i; ++j) {
      tmp.myvec_.insert(tmp.myvec_.begin(), 0);
    }
    res += tmp;
  }
  while (res.myvec_.back() == 0 && res.myvec_.size() > 1) {
    res.myvec_.pop_back();
  }
  return res;
}

BigInt BigInt::Divide2(BigInt one) {
  for (size_t i = 0; i < one.myvec_.size(); ++i) {
    if (one.myvec_[i] % 2 != 0 && i > 0) {
      one.myvec_[i - 1] += kFive;
    }
    one.myvec_[i] /= 2;
  }
  if (one.myvec_.back() == 0) {
    one.myvec_.pop_back();
  }
  return one;
}

BigInt BigInt::Division(BigInt one, BigInt other) {
  BigInt centre;
  BigInt first(0);
  BigInt last = one + 1;
  BigInt diff = last - first;
  while (last - first > BigInt(1)) {
    BigInt sum = (first + last);
    centre = Divide2(sum);
    if (centre * other > one) {
      last = centre;
    } else {
      first = centre;
    }
  }
  return first;
}

BigInt BigInt::operator/(const BigInt& other) {
  if (*this < 0 && other < 0) {
    return Division(-*this, -other);
  }
  if (*this < 0) {
    return -Division(-*this, other);
  }
  if (other < 0) {
    return -Division(*this, -other);
  }

  return Division(*this, other);
}

BigInt BigInt::Modulo(BigInt one, BigInt two) {
  BigInt res;
  res = one / two;
  if (one == res * two) {
    res = 0;
    return res;
  }

  BigInt tmp;
  tmp = one - two * res;
  return tmp;
}

BigInt BigInt::operator%(const BigInt& other) {
  if (*this < 0) {
    return -Modulo(-*this, other);
  }
  return Modulo(*this, other);
}

BigInt BigInt::operator*(const BigInt& other) {
  if (*this < 0 xor other < 0) {
    return -Multiply(*this, other);
  }
  return Multiply(*this, other);
}

BigInt& BigInt::operator++() {
  *this += 1;
  return *this;
}

BigInt BigInt::operator++(int) {
  BigInt tmp = *this;
  *this += 1;
  return tmp;
}

BigInt& BigInt::operator--() {
  *this -= 1;
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt tmp = *this;
  *this -= 1;
  return tmp;
}

BigInt::BigInt(std::string num) {
  std::reverse(num.begin(), num.end());
  if (num.back() == '-') {
    sign_ = false;
    num.pop_back();
  } else {
    sign_ = true;
  }
  int64_t size = num.size();
  myvec_.resize(size);
  for (int64_t i = 0; i < size; i++) {
    int mem = num[i] - '0';
    myvec_[i] = mem;
  }
  if (myvec_.back() == 0) {
    sign_ = true;
  }
}

std::ostream& operator<<(std::ostream& out, const BigInt& num) {
  if (!num.sign_) {
    out << '-';
  }
  int size = static_cast<int>(num.myvec_.size());
  for (int i = size - 1; i >= 0; --i) {
    out << static_cast<int>(num.myvec_[i]);
  }
  return out;
}

std::istream& operator>>(std::istream& inn, BigInt& num) {
  std::string input_num;
  inn >> input_num;
  num = BigInt(input_num);
  return inn;
}

