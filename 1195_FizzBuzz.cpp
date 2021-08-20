#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>

class FizzBuzz {
private:
  int n;
  int mPos;
  std::mutex mMutex;
  std::condition_variable mConditionVariable;
  bool mFinished;

public:
  FizzBuzz(int n) {
    this->n = n;
    this->mPos = 1;
    this->mFinished = false;
  }

  // printFizz() outputs "fizz".
  void fizz(std::function<void()> printFizz) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mPos % 3 == 0) && (mPos % 5 != 0) || mFinished;
    });
    if (mFinished) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printFizz();
    if (mPos == n) {
      mFinished = true;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mPos += 1;
    lock.unlock();
    mConditionVariable.notify_all();
    fizz(printFizz);
  }

  // printBuzz() outputs "buzz".
  void buzz(std::function<void()> printBuzz) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mPos % 5 == 0) && (mPos % 3 != 0) || mFinished;
    });
    if (mFinished) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printBuzz();
    if (mPos == n) {
      mFinished = true;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mPos += 1;
    lock.unlock();
    mConditionVariable.notify_all();
    buzz(printBuzz);
  }

  // printFizzBuzz() outputs "fizzbuzz".
  void fizzbuzz(std::function<void()> printFizzBuzz) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mPos % 3 == 0) && (mPos % 5 == 0) || mFinished;
    });
    if (mFinished) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printFizzBuzz();
    if (mPos == n) {
      mFinished = true;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mPos += 1;
    lock.unlock();
    mConditionVariable.notify_all();
    fizzbuzz(printFizzBuzz);
  }

  // printNumber(x) outputs "x", where x is an integer.
  void number(std::function<void(int)> printNumber) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      return (mPos % 3 != 0) && (mPos % 5 != 0) || mFinished;
    });
    if (mFinished) {
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    printNumber(mPos);
    if (mPos == n) {
      mFinished = true;
      lock.unlock();
      mConditionVariable.notify_all();
      return;
    }
    mPos += 1;
    lock.unlock();
    mConditionVariable.notify_all();
    number(printNumber);
  }
};

int main(int argc, char *argv[]) {
  std::function<void()> printFizz = []() { std::cout << "Fizz"; };
  std::function<void()> printBuzz = []() { std::cout << "Buzz"; };

  std::function<void()> printFizzBuzz = []() { std::cout << "FizzBuzz"; };

  std::function<void(int)> printNumber = [](int num) { std::cout << num; };

  FizzBuzz fizzBuzz(17);
  auto fizz = std::async(&FizzBuzz::fizz, &fizzBuzz, printFizz);
  auto buzz = std::async(&FizzBuzz::buzz, &fizzBuzz, printBuzz);
  auto fizzbuzz = std::async(&FizzBuzz::fizzbuzz, &fizzBuzz, printFizzBuzz);
  auto number = std::async(&FizzBuzz::number, &fizzBuzz, printNumber);

  fizz.wait();
  buzz.wait();
  fizzbuzz.wait();
  number.wait();

  return 0;
}
