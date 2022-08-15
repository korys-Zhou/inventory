#include <coroutine>
#include <iostream>

using namespace std;

template <typename T>
struct Generator {
    class ExhaustedException : exception {};
    struct promise_type {
        T value;
        bool is_ready = false;
        suspend_always initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}
        Generator get_return_object() {
            return Generator{
                coroutine_handle<promise_type>::from_promise(*this)};
        }
        void return_void() {}
        suspend_always await_transform(T value) {
            this->value = value;
            is_ready = true;
            return {};
        }
        suspend_always yield_value(T value) {
            this->value = value;
            is_ready = true;
            return {};
        }
    };

    explicit Generator(coroutine_handle<promise_type> handle) noexcept
        : handle(handle) {}
    Generator(Generator&& generator) noexcept
        : handle(move(generator.handle)) {}
    Generator(Generator&) = delete;
    Generator& operator=(Generator&) = delete;
    ~Generator() {
        if (handle)
            handle.destroy();
    }

    coroutine_handle<promise_type> handle;

    bool has_next() {
        if (handle.done())
            return false;
        if (!handle.promise().is_ready)
            handle.resume();
        return !handle.done();
    }

    T next() {
        if (has_next()) {
            handle.promise().is_ready = false;
            return handle.promise().value;
        }
        throw ExhaustedException();
    }

    static Generator from_array(T array[], int n) {
        for (int i = 0; i < n; ++i) {
            co_yield array[i];
        }
    }

    template <typename... TArgs>
    static Generator from_fold(TArgs... args) {
        (co_yield args, ...);
    }

    template <typename DEST>
    Generator<invoke_result_t<DEST, T>> map(DEST f) {
        while (has_next()) {
            co_yield f(next());
        }
    }
};

Generator<int> sequence() {
    int i = 0;
    while (i++ < 5)
        co_await i;
}

Generator<int> return_generator() {
    auto g = sequence();
    if (g.has_next())
        cout << "From return_generator: " << g.next() << endl;
    return g;
}

Generator<int> fibonacci() {
    co_yield 0;
    co_yield 1;

    int a = 0, b = 1;
    while (true) {
        co_yield a + b;
        b = a + b;
        a = b - a;
    }
}

int main() {
    auto generator = return_generator();
    for (int i = 0; i < 10; ++i) {
        if (generator.has_next())
            cout << "From sequence: " << generator.next() << endl;
        else
            break;
    }

    auto gen_fibo = fibonacci();
    for (int i = 0; i < 10; ++i) {
        if (gen_fibo.has_next())
            cout << "From fibonacci: " << gen_fibo.next() << endl;
        else
            break;
    }

    int arr[] = {1, 2, 3, 4, 5};
    auto gen_arr = Generator<int>::from_array(arr, sizeof(arr) / sizeof(int));
    for (int i = 0; i < 10; ++i) {
        if (gen_arr.has_next())
            cout << "From from_array: " << gen_arr.next() << endl;
        else
            break;
    }

    auto gen_fold = Generator<int>::from_fold(1, 2, 3, 4, 5);
    for (int i = 0; i < 10; ++i) {
        if (gen_fold.has_next())
            cout << "From from_fold: " << gen_fold.next() << endl;
        else
            break;
    }

    Generator<string> gen_str =
        fibonacci().map([](int i) { return to_string(i); });
    for (int i = 0; i < 10; ++i) {
        if (gen_str.has_next())
            cout << "From fib_to_str: " << gen_str.next() << endl;
        else
            break;
    }

    return 0;
}