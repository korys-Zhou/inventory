#include <condition_variable>
#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <list>
#include <mutex>
#include <optional>
#include <thread>

using namespace std;

template <typename ResultType>
struct Task;

template <typename T>
struct Result {
    explicit Result() = default;

    explicit Result(T&& value) : _value(value) {}

    explicit Result(exception_ptr&& exception_ptr)
        : _exception_ptr(exception_ptr) {}

    T get_or_throw() {
        if (_exception_ptr) {
            rethrow_exception(_exception_ptr);
        }
        return _value;
    }

   private:
    T _value{};
    exception_ptr _exception_ptr;
};

template <>
struct Result<void> {
    explicit Result() = default;

    explicit Result(exception_ptr&& exception_ptr)
        : _exception_ptr(exception_ptr) {}

    void get_or_throw() {
        if (_exception_ptr) {
            rethrow_exception(_exception_ptr);
        }
    }

   private:
    exception_ptr _exception_ptr;
};

template <typename ResultType>
struct TaskAwaiter {
    explicit TaskAwaiter(Task<ResultType>&& task) noexcept : task(move(task)) {}

    TaskAwaiter(TaskAwaiter&& completion) noexcept
        : task(exchange(completion.task, {})) {}

    TaskAwaiter(TaskAwaiter&) = delete;

    TaskAwaiter& operator=(TaskAwaiter&) = delete;

    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(coroutine_handle<> handle) noexcept {
        task.finally([handle]() { handle.resume(); });
    }

    ResultType await_resume() noexcept { return task.get_result(); }

   private:
    Task<ResultType> task;
};

template <typename ResultType>
struct TaskPromise {
    suspend_never initial_suspend() { return {}; }

    suspend_always final_suspend() noexcept { return {}; }

    Task<ResultType> get_return_object() {
        return Task{coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    void unhandled_exception() {
        lock_guard<mutex> lock(completion_lock);
        result = Result<ResultType>(current_exception());
        completion.notify_all();
        notify_callbacks();
    }

    void return_value(ResultType value) {
        lock_guard<mutex> lock(completion_lock);
        result = Result<ResultType>(move(value));
        completion.notify_all();
        notify_callbacks();
    }

    void on_completed(function<void(Result<ResultType>)>&& func) {
        unique_lock lock(completion_lock);
        if (result.has_value()) {
            auto value = result.value();
            lock.unlock();
            func(value);
        } else {
            completion_callbacks.push_back(func);
        }
    }

    ResultType get_result() {
        unique_lock lock(completion_lock);
        if (!result.has_value()) {
            completion.wait(lock);
        }
        return result->get_or_throw();
    }

    template <typename _ResultType>
    TaskAwaiter<_ResultType> await_transform(Task<_ResultType>&& task) {
        return TaskAwaiter<_ResultType>(move(task));
    }

   private:
    optional<Result<ResultType>> result;
    mutex completion_lock;
    condition_variable completion;

    list<function<void(Result<ResultType>)>> completion_callbacks;

    void notify_callbacks() {
        auto value = result.value();
        for (auto& callback : completion_callbacks) {
            callback(value);
        }
        completion_callbacks.clear();
    }
};

template <>
struct TaskPromise<void> {
    suspend_never initial_suspend() { return {}; }

    suspend_always final_suspend() noexcept { return {}; }

    Task<void> get_return_object() {
        return Task{coroutine_handle<TaskPromise>::from_promise(*this)};
    }

    void unhandled_exception() {
        lock_guard<mutex> lock(completion_lock);
        result = Result<void>(current_exception());
        completion.notify_all();
        notify_callbacks();
    }

    void return_void() {
        lock_guard<mutex> lock(completion_lock);
        result = Result<void>();
        completion.notify_all();
        notify_callbacks();
    }

    void on_completed(function<void(Result<void>)>&& func) {
        unique_lock lock(completion_lock);
        if (result.has_value()) {
            auto value = result.value();
            lock.unlock();
            func(value);
        } else {
            completion_callbacks.push_back(func);
        }
    }

    void get_result() {
        unique_lock lock(completion_lock);
        if (!result.has_value()) {
            completion.wait(lock);
        }
        result->get_or_throw();
    }

    TaskAwaiter<void> await_transform(Task<void>&& task) {
        return TaskAwaiter<void>(move(task));
    }

   private:
    optional<Result<void>> result;
    mutex completion_lock;
    condition_variable completion;

    list<function<void(Result<void>)>> completion_callbacks;

    void notify_callbacks() {
        auto value = result.value();
        for (auto& callback : completion_callbacks) {
            callback(value);
        }
        completion_callbacks.clear();
    }
};

template <typename ResultType>
struct Task {
    using promise_type = TaskPromise<ResultType>;

    ResultType get_result() { return handle.promise().get_result(); }

    Task& then(function<void(ResultType)>&& func) {
        handle.promise().on_completed([func](auto result) {
            try {
                func(result.get_or_throw());
            } catch (exception& e) {
            }
        });
        return *this;
    }

    Task& catching(function<void(exception&)>&& func) {
        handle.promise().on_completed([func](auto result) {
            try {
                result.get_or_throw();
            } catch (exception& e) {
                func(e);
            }
        });
        return *this;
    }

    Task& finally(function<void()>&& func) {
        handle.promise().on_completed([func](auto result) { func(); });
        return *this;
    }

    explicit Task(coroutine_handle<promise_type> handle) noexcept
        : handle(handle) {}

    Task(Task&& task) noexcept : handle(exchange(task.handle, {})) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() {
        if (handle)
            handle.destroy();
    }

   private:
    coroutine_handle<promise_type> handle;
};

template <>
struct Task<void> {
    using promise_type = TaskPromise<void>;

    void get_result() { handle.promise().get_result(); }

    Task& then(function<void()>&& func) {
        handle.promise().on_completed([func](auto result) {
            try {
                result.get_or_throw();
                func();
            } catch (exception& e) {
            }
        });
        return *this;
    }

    Task& catching(function<void(exception&)>&& func) {
        handle.promise().on_completed([func](auto result) {
            try {
                result.get_or_throw();
            } catch (exception& e) {
                func(e);
            }
        });
        return *this;
    }

    Task& finally(function<void()>&& func) {
        handle.promise().on_completed([func](auto result) { func(); });
        return *this;
    }

    explicit Task(coroutine_handle<promise_type> handle) noexcept
        : handle(handle) {}

    Task(Task&& task) noexcept : handle(exchange(task.handle, {})) {}

    Task(Task&) = delete;

    Task& operator=(Task&) = delete;

    ~Task() {
        if (handle)
            handle.destroy();
    }

   private:
    coroutine_handle<promise_type> handle;
};

Task<int> simple_task2() {
    this_thread::sleep_for(1s);
    co_return 2;
}

Task<int> simple_task3() {
    this_thread::sleep_for(2s);
    co_return 3;
}

Task<int> simple_task() {
    auto result2 = co_await simple_task2();
    cout << result2 << endl;
    auto result3 = co_await simple_task3();
    cout << result3 << endl;
    co_return result2 + result3 + 1;
}

int main() {
    auto simpletask = simple_task();

    simpletask.then([](int i) { cout << i << endl; })
        .catching([](exception& e) { cout << e.what() << endl; });

    try {
        auto i = simpletask.get_result();
        cout << i << endl;
    } catch (exception& e) {
        cout << e.what() << endl;
    }
    return 0;
}