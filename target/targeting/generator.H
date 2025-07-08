#pragma once

#include <coroutine>
#include <exception>
#include <utility>
namespace TARGETING
{
// Note: switch to std::generator if compiled with c++23
template <typename T> class generator
{
    static_assert(std::is_move_constructible_v<T>,
                  "generator<T> requires T to be move-constructible");

  public:
    struct promise_type
    {
        T current_value;

        generator get_return_object()
        {
            return generator{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_always final_suspend() noexcept
        {
            return {};
        }

        std::suspend_always yield_value(T&& value) noexcept
        {
            current_value = std::move(value);
            return {};
        }

        void return_void() noexcept {}

        void unhandled_exception()
        {
            std::terminate(); // You can also store and rethrow
        }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit generator(handle_type h) : coro(h) {}
    generator(generator&& other) noexcept : coro(other.coro)
    {
        other.coro = nullptr;
    }

    generator(const generator&) = delete;
    generator& operator=(const generator&) = delete;

    ~generator()
    {
        if (coro)
            coro.destroy();
    }

    class iterator
    {
      public:
        void operator++()
        {
            coro.resume();
        }

        T operator*() const
        {
            return std::move(coro.promise().current_value);
        }

        bool operator==(std::default_sentinel_t) const
        {
            return !coro || coro.done();
        }

        explicit iterator(handle_type h) : coro(h) {}

      private:
        handle_type coro;
    };

    iterator begin()
    {
        if (coro)
            coro.resume();
        return iterator{coro};
    }

    std::default_sentinel_t end()
    {
        return {};
    }

  private:
    handle_type coro;
};
} // namespace TARGETING
