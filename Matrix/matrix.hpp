#pragma once
#include <algorithm>
#include <iostream>
#include <vector>

template <size_t N, size_t M, typename T = int64_t>

class Matrix {
  std::vector<std::vector<T>> myarr_;

 public:
  Matrix() : myarr_(N, std::vector<T>(M, 0)) {}
  T elem;
  Matrix(T elem) : myarr_(N, std::vector<T>(M, elem)) {}
  Matrix(std::vector<std::vector<T>> vec) : myarr_(vec) {}
  ~Matrix() = default;

  Matrix& operator=(const Matrix& k0ther) = default;

  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        (*this)(i, j) += k0ther(i, j);
      }
    }
    return *this;
  }

  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        (*this)(i, j) -= k0ther(i, j);
      }
    }
    return *this;
  }

  Matrix<N, M, T>& operator*=(T k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        myarr_[i][j] *= k0ther;
      }
    }
    return *this;
  }

  Matrix<N, M, T> operator*(T k0ther) const {
    Matrix<N, M, T> res;
    res = *this;
    res *= k0ther;
    return res;
  }

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T> k0ther) const {
    Matrix<N, K, T> res{};
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        for (size_t k = 0; k < M; ++k) {
          res(i, j) += (*this)(i, k) * k0ther(k, j);
        }
      }
    }
    return res;
  }

  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> trans;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        trans(j, i) = (*this)(i, j);
      }
    }
    return trans;
  }

  T& operator()(size_t row, size_t column) { return this->myarr_[row][column]; }

  T operator()(size_t row, size_t column) const {
    return this->myarr_[row][column];
  }

  Matrix<N, M, T> operator+(Matrix<N, M, T> k0ther) const {
    Matrix res;
    res = *this;
    res += k0ther;
    return res;
  }

  Matrix<N, M, T> operator-(Matrix<N, M, T> k0ther) const {
    Matrix res;
    res = *this;
    res -= k0ther;
    return res;
  }

  bool operator==(Matrix<N, M, T> k0ther) const {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        if ((*this)(i, j) != k0ther(i, j)) {
          return false;
        }
      }
    }
    return true;
  }
};

template <size_t N, typename T>
class Matrix<N, N, T> {
  std::vector<std::vector<T>> myarr_;

 public:
  Matrix() : myarr_(N, std::vector<T>(N, 0)) {}
  T elem;
  Matrix(T elem) : myarr_(N, std::vector<T>(N, elem)) {}
  Matrix(std::vector<std::vector<T>> vec) : myarr_(vec) {}
  ~Matrix() = default;
  Matrix& operator=(const Matrix& k0ther) = default;

  Matrix<N, N, T>& operator+=(const Matrix<N, N, T>& k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        (*this)(i, j) += k0ther(i, j);
      }
    }
    return *this;
  }

  Matrix<N, N, T>& operator-=(const Matrix<N, N, T>& k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; i < N; ++j) {
        (*this)(i, j) -= k0ther(i, j);
      }
    }
    return *this;
  }

  Matrix<N, N, T>& operator*=(T k0ther) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        myarr_[i][j] *= k0ther;
      }
    }
    return *this;
  }

  Matrix<N, N, T> operator*(T k0ther) const {
    Matrix<N, N, T> res(*this);
    res *= k0ther;
    return res;
  }

  Matrix<N, N, T> operator+(Matrix<N, N, T> k0ther) const {
    Matrix res;
    res = *this;
    res += k0ther;
    return res;
  }

  Matrix<N, N, T> operator-(Matrix<N, N, T> k0ther) const {
    Matrix res;
    res = *this;
    res -= k0ther;
    return res;
  }

  Matrix<N, N, T> operator*(const Matrix<N, N, T> k0ther) const {
    Matrix<N, N, T> res{};
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        for (size_t k = 0; k < N; ++k) {
          res(i, j) += (*this)(i, k) * k0ther(k, j);
        }
      }
    }
    return res;
  }

  Matrix<N, N, T> Transposed() const {
    Matrix<N, N, T> trans;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        trans(j, i) = (*this)(i, j);
      }
    }
    return trans;
  }

  T& operator()(size_t row, size_t column) { return this->myarr_[row][column]; }

  T operator()(size_t row, size_t column) const {
    return this->myarr_[row][column];
  }

  bool operator==(Matrix<N, N, T> k0ther) const {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (*this != k0ther(i, j)) {
          return false;
        }
      }
    }
    return true;
  }
  T Trace() const {
    T res;
    for (size_t i = 0; i < N; ++i) {
      res += (*this)(i, i);
    }
    return res;
  }
};

