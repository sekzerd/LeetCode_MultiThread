#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>

class ZeroEvenOdd {
private:
  int n;
  int mPos;
  std::mutex mMutex;
  std::condition_variable mConditionVariable;
  enum class Status { ZERO, EVEN, ODD, NONE };
  Status mStatus;

public:
  ZeroEvenOdd(int n) {
    this->n = n;
    mStatus = Status::ZERO;
    mPos = 0;
  }

  // printNumber(x) outputs "x", where x is an integer.
  void zero(std::function<void(int)> printNumber) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mStatus == Status::ZERO) || (mStatus == Status::NONE);
    });
    if (mStatus == Status::NONE) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printNumber(0);
    mPos += 1;
    if (mPos % 2 == 0) {
      mStatus = Status::EVEN;
    } else {
      mStatus = Status::ODD;
    }
    lock.unlock();
    mConditionVariable.notify_all();
    return zero(printNumber);
  }

  void even(std::function<void(int)> printNumber) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mStatus == Status::EVEN) || (mStatus == Status::NONE);
    });
    if (mStatus == Status::NONE) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printNumber(mPos);
    if (mPos == n) {
      mStatus = Status::NONE;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mStatus = Status::ZERO;
    lock.unlock();
    mConditionVariable.notify_all();
    return even(printNumber);
  }

  void odd(std::function<void(int)> printNumber) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mStatus == Status::ODD) || (mStatus == Status::NONE);
    });
    if (mStatus == Status::NONE) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printNumber(mPos);
    if (mPos == n) {
      mStatus = Status::NONE;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mStatus = Status::ZERO;
    lock.unlock();
    mConditionVariable.notify_all();
    return odd(printNumber);
  }
};
int main(int argc, char *argv[]) {

  std::function<void(int)> printNumber = [](int num) {
    std::cout << num << "\t";
  };

  ZeroEvenOdd zeroEvenOdd(2);

  auto zero = std::async(&ZeroEvenOdd::zero, &zeroEvenOdd, printNumber);
  auto odd = std::async(&ZeroEvenOdd::odd, &zeroEvenOdd, printNumber);
  auto even = std::async(&ZeroEvenOdd::even, &zeroEvenOdd, printNumber);

  zero.wait();
  odd.wait();
  even.wait();

  return 0;
}