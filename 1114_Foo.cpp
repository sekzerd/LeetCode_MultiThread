#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>

class Foo {
private:
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    int mFlag;

public:
    Foo() { mFlag = 0; }

    void first(std::function<void()> printFirst) {
        std::unique_lock<std::mutex> lock(mMutex);
        mConditionVariable.wait(lock, [=]() { return (0 == mFlag); });
        // printFirst() outputs "first". Do not change or remove this line.
        printFirst();
        mFlag = 1;
        lock.unlock();
        mConditionVariable.notify_all();
    }

    void second(std::function<void()> printSecond) {
        std::unique_lock<std::mutex> lock(mMutex);
        mConditionVariable.wait(lock, [=]() { return (1 == mFlag); });
        // printSecond() outputs "second". Do not change or remove this line.
        printSecond();
        mFlag = 2;
        lock.unlock();
        mConditionVariable.notify_all();
    }

    void third(std::function<void()> printThird) {
        std::unique_lock<std::mutex> lock(mMutex);
        mConditionVariable.wait(lock, [=]() { return (2 == mFlag); });
        // printThird() outputs "third". Do not change or remove this line.
        printThird();
        mFlag = 0;
        lock.unlock();
        mConditionVariable.notify_all();
    }
};

int main(int argc, char *argv[]) {
    std::function<void()> printFirst = []() { std::cout << "first\t"; };
    std::function<void()> printSecond = []() { std::cout << "second\t"; };
    std::function<void()> printThird = []() { std::cout << "third\t"; };

    Foo foo;

    auto a1 = std::async(&Foo::first, &foo, printFirst);
    auto a2 = std::async(&Foo::second, &foo, printSecond);
    auto a3 = std::async(&Foo::third, &foo, printThird);

    a3.wait();
    a2.wait();
    a1.wait();

    return 0;
}