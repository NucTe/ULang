#ifndef ULANG_UTILITIES_H_
#define ULANG_UTILITIES_H_

#include <iostream>
#include <optional>
#include <memory>

namespace UraniumLang {

  template <typename T>
  using uptr = std::unique_ptr<T>;

  struct Error {
    enum ErrCode : int {
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

    static Error NotImpl() {
      Error err(ErrCode::NOT_IMPL);
      err.Description = "Not Implemented!";
      return err;
    }

    static Error Failed(const std::string &desc) {
      Error err(ErrCode::FAILED);
      err.Description = desc;
      return err;
    }
  };

  template <typename T>
  class Result {
  public:
    Result() = default;
    Result(T value) : m_Result(value) {}
    Result(Error err) : m_Error(err) {}
  
    inline void SetResult(T result) { m_Result = std::move(result); }
    inline void SetError(Error err) { m_Error = err; }

    inline Error operator()() const {
      return m_Error;
    }

    inline std::optional<T> GetResult() { 
      return std::move(m_Result);
    }
  private:
    std::optional<T> m_Result{};
    Error m_Error = Error(Error::ErrCode::SUCCESS);
  };

}

#endif