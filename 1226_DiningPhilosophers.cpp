#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
class DiningPhilosophers {

private:
  struct ForkWithID {
    int id;
    int ateCount;
    bool leftFork;
  };
  std::vector<ForkWithID> mPhilosophers;

private:
  std::mutex mMutex;
  std::condition_variable mConditionVariable;

public:
  DiningPhilosophers() {
    for (size_t i = 0; i < 5; ++i) {
      mPhilosophers.push_back(ForkWithID{0, 0, false});
    }
  }

  void wantsToEat(int philosopher, std::function<void()> pickLeftFork,
                  std::function<void()> pickRightFork,
                  std::function<void()> eat, std::function<void()> putLeftFork,
                  std::function<void()> putRightFork) {

    std::unique_lock<std::mutex> lock(mMutex);
    mConditionVariable.wait(lock, [=]() {
      int pos = (philosopher == 0 ? 4 : philosopher - 1);
      return (mPhilosophers[philosopher].leftFork == false) &&
             (mPhilosophers[pos].leftFork == false);
    });

    pickLeftFork();
    mPhilosophers[philosopher].leftFork = true;
    pickRightFork();
    int pos = (philosopher == 0 ? 4 : philosopher - 1);
    mPhilosophers[pos].leftFork = true;
    eat();
    mPhilosophers[philosopher].ateCount += 1;
    putLeftFork();
    mPhilosophers[philosopher].leftFork = false;
    putRightFork();
    mPhilosophers[pos].leftFork = false;
    lock.unlock();
    mConditionVariable.notify_all();
  }
};

int main(int argc, char *argv[]) {
  std::function<void()> pickLeftFork = []() { std::cout << "pickLeftFork\t"; };
  std::function<void()> pickRightFork = []() {
    std::cout << "pickRightFork\t";
  };
  std::function<void()> eat = []() { std::cout << "eat\t"; };
  std::function<void()> putLeftFork = []() { std::cout << "putLeftFork\t"; };
  std::function<void()> putRightFork = []() { std::cout << "putRightFork\t"; };

  DiningPhilosophers diningPhilosophers;

  std::vector<std::thread> threads;
  for (size_t i = 0; i < 5; ++i) {
    threads.push_back(std::thread(
        &DiningPhilosophers::wantsToEat, &diningPhilosophers, i, pickLeftFork,
        pickRightFork, eat, putLeftFork, putRightFork));
  }

  for (auto iter = threads.begin(); iter != threads.end(); ++iter) {
    iter->join();
  }

  return 0;
}
