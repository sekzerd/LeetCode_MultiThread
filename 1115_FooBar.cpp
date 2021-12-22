#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>

class FooBar {
private:
    int n;
    std::mutex mMutex;
    std::condition_variable mConditionVariable;
    bool flag;

public:
    FooBar(int n) {
        this->n = n;
        flag = true;
    }

    void foo(std::function<void()> printFoo) {

        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lock(mMutex);
            mConditionVariable.wait(lock, [=]() { return flag; });
            // printFoo() outputs "foo". Do not change or remove this line.
            printFoo();
            flag = false;
            lock.unlock();
            mConditionVariable.notify_one();
        }
    }

    void bar(std::function<void()> printBar) {

        for (int i = 0; i < n; i++) {
            std::unique_lock<std::mutex> lock(mMutex);
            mConditionVariable.wait(lock, [=]() { return (!flag); });
            // printBar() outputs "bar". Do not change or remove this line.
            printBar();
            flag = true;
            lock.unlock();
            mConditionVariable.notify_one();
        }
    }
};

int main(int argc, char *argv[]) {

    std::function<void()> printFoo = []() { std::cout << "foo\t"; };
    std::function<void()> printBar = []() { std::cout << "bar\t"; };

    FooBar foobar(2);

    auto threadOne = std::async(&FooBar::foo, &foobar, printFoo);
    auto threadTwo = std::async(&FooBar::bar, &foobar, printBar);

    threadOne.wait();
    threadTwo.wait();

    return 0;
}
