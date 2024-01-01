#ifndef ULANG_UTILITIES_H_
#define ULANG_UTILITIES_H_

#include <iostream>
#include <optional>
#include <memory>

namespace UraniumLang {

  template <typename T>
  using uptr = std::unique_ptr<T>;

  struct Error {
    enum ErrCode {
      SUCCESS = 0,
      FAILED = 1,
    };

    std::string Description{};
    int Code = SUCCESS;

    Error()
      : Code(ErrCode::FAILED) {}

    Error(ErrCode code) 
      : Code(code) {}
  };

  template <typename T>
  class Result {
  public:
    Result() = default;
    Result(T value) : m_Result(value) {}
    Result(Error err) : m_Error(err) {}
  
    inline std::pair<bool, const T&> operator()() const { return { m_Error.Code == 0 && m_Result.has_value(), m_Result.has_value() ? m_Result.value() : T() }; }
    inline Error GetErr() const { return m_Error; }
  private:
    std::optional<T> m_Result{};
    Error m_Error{};
  };

}

#endif