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
      NOT_IMPL = -1,
      SUCCESS = 0,
      FAILED = 1,
    };

    std::string Description{};
    int Code = SUCCESS;

    Error()
      : Code(ErrCode::FAILED) {}

    Error(ErrCode code) 
      : Code(code) {}

    bool operator ()() const { return Code == SUCCESS; }
    bool operator !() const { return Code != SUCCESS; }
  };

  template <typename T>
  class Result {
  public:
    Result() = default;
    Result(T value) : m_Result(value) {}
    Result(Error err) : m_Error(err) {}
  
    inline void SetResult(T result) { m_Result = result; }
    inline void SetError(Error err) { m_Error = err; }

    inline std::pair<Error, const T&> operator ()() const { return { m_Error, m_Result.has_value() ? m_Result.value() : (const T&)T() }; }
    inline Error GetErr() const { return m_Error; }
  private:
    std::optional<T> m_Result{};
    Error m_Error{};
  };

}

#endif