#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
class H2O {
private:
  int mHcount;
  int mOcount;
  std::mutex mMutex;
  std::condition_variable mConditionVariable;

public:
  H2O() {
    mHcount = 0;
    mOcount = 0;
  }

  void hydrogen(std::function<void()> releaseHydrogen) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() { return (mHcount != 2); });
    // releaseHydrogen() outputs "H". Do not change or remove this line.
    releaseHydrogen();
    mHcount += 1;
    if (mOcount == 1 && mHcount == 2) {
      mOcount = 0;
      mHcount = 0;
    }
    lock.unlock();
    mConditionVariable.notify_all();
  }

  void oxygen(std::function<void()> releaseOxygen) {
    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() { return (mOcount != 1); });
    // releaseOxygen() outputs "O". Do not change or remove this line.
    releaseOxygen();
    mOcount += 1;
    if (mOcount == 1 && mHcount == 2) {
      mOcount = 0;
      mHcount = 0;
    }
    lock.unlock();
    mConditionVariable.notify_all();
  }
};
int main(int argc, char *argv[]) {

  std::function<void()> printO = []() { std::cout << "O"; };
  std::function<void()> printH = []() { std::cout << "H"; };

  std::string input("OOHHHHHHHHOHHOO");
  std::vector<std::thread> threads;

  H2O h2O;

  for (size_t i = 0; i < input.size(); ++i) {
    if (input[i] == 'H') {
      threads.emplace_back(std::thread(&H2O::hydrogen, &h2O, printH));
    }
    if (input[i] == 'O') {
      threads.emplace_back(std::thread(&H2O::oxygen, &h2O, printO));
    }
  }
  for (auto iter = threads.begin(); iter < threads.end(); ++iter) {
    iter->join();
  }
  return 0;
}
