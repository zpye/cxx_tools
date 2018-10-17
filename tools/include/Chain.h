#ifndef CXX_TOOLS_CHAIN_H
#define CXX_TOOLS_CHAIN_H

#include "ThreadPool.h"

#include <iostream>
#include <type_traits>
#include "type_traits.h"

template< typename T >
class Chain;

template< typename R, typename... Args >
class Chain< R(Args...) >
{
public:
    using TFunc = std::function< R(Args...) >;
    using MFunc = std::function< std::future< R >(Args...) >;

    Chain(const TFunc& f)
    {
        mPool = std::make_shared< ThreadPool >(1);
        mFunc = std::make_shared< MFunc >(
        [=](Args&&... args) {
            return mPool->execute(f, 
                                  std::forward< Args >(args)...);
        });
    }

    Chain(TFunc&& f)
    {
        mPool = std::make_shared< ThreadPool >(1);
        mFunc = std::make_shared< MFunc >(
        [=](Args&&... args) {
            return mPool->execute(std::move(f), 
                                  std::forward< Args >(args)...);
        });
    }

    Chain(const MFunc& f, std::shared_ptr< ThreadPool > pool)
        : mPool(pool)
    {
        mFunc = std::make_shared< MFunc >(f);
    }

    Chain(MFunc&& f, std::shared_ptr< ThreadPool > pool)
        : mPool(pool)
    {
        mFunc = std::make_shared< MFunc >(std::move(f));
    }

    Chain(Chain&& chain)
    {
        mFunc = std::move(chain.mFunc);
        mPool = std::move(chain.mPool);
    }

    Chain& operator=(Chain&& chain)
    {
        mFunc = std::move(chain.mFunc);
        mPool = std::move(chain.mPool);
        return *this;
    }

    Chain(const Chain&) = delete;

    Chain& operator=(const Chain&) = delete;

    std::future< R > Run(Args&&... args)
    {
        return (*mFunc)(std::forward< Args >(args)...);
    }

    template< typename F >
    auto Then(F&& f)
        -> Chain< typename std::result_of< F(R) >::type(Args...) >
    {
        using rtype = typename std::result_of< F(R) >::type;

        std::function< rtype(R) > f_(std::forward< F >(f));
        auto func = [f_](std::shared_future< R > p) {
            return f_(p.get());
        };

        std::shared_ptr< MFunc > mFunc_(mFunc);
        std::shared_ptr< ThreadPool > mPool_(mPool);

        return Chain< rtype(Args...) >(
        [func, mFunc_, mPool_](Args&&... args) {
            std::future< R > prev = (*mFunc_)(std::forward< Args >(args)...);
            return mPool_->execute(func, prev.share());
        }, mPool_);
    }

private:
    std::shared_ptr< MFunc > mFunc;
    std::shared_ptr< ThreadPool > mPool;
};

template< typename... Args >
class Chain< void(Args...) >
{
public:
    using TFunc = std::function< void(Args...) >;
    using MFunc = std::function< std::future< void >(Args...) >;

    Chain(const TFunc& f)
    {
        mPool = std::make_shared< ThreadPool >(1);
        mFunc = std::make_shared< MFunc >(
        [=](Args&&... args) {
            return mPool->execute(f,
                                  std::forward< Args >(args)...);
        });
    }

    Chain(TFunc&& f)
    {
        mPool = std::make_shared< ThreadPool >(1);
        mFunc = std::make_shared< MFunc >(
        [=](Args&&... args) {
            return mPool->execute(std::move(f),
                                  std::forward< Args >(args)...);
        });
    }

    Chain(const MFunc& f, std::shared_ptr< ThreadPool > pool)
        : mPool(pool)
    {
        mFunc = std::make_shared< MFunc >(f);
    }

    Chain(MFunc&& f, std::shared_ptr< ThreadPool > pool)
        : mPool(pool)
    {
        mFunc = std::make_shared< MFunc >(std::move(f));
    }

    Chain(Chain&& chain)
    {
        mFunc = std::move(chain.mFunc);
        mPool = std::move(chain.mPool);
    }

    Chain& operator=(Chain&& chain)
    {
        mFunc = std::move(chain.mFunc);
        mPool = std::move(chain.mPool);
    }

    Chain(const Chain&) = delete;
    Chain& operator=(const Chain&) = delete;

    std::future< void > Run(Args&&... args)
    {
        return (*mFunc)(std::forward< Args >(args)...);
    }

    template< typename F >
    auto Then(F&& f)
        -> Chain< typename std::result_of< F(void) >::type(Args...) >
    {
        using rtype = typename std::result_of< F(void) >::type;

        std::function< rtype(void) > f_(std::forward< F >(f));
        auto func = [f_](std::shared_future< void > p) {
            p.get();
            return f_();
        };

        std::shared_ptr< MFunc > mFunc_(mFunc);
        std::shared_ptr< ThreadPool > mPool_(mPool);

        return Chain< rtype(Args...) >(
        [func, mFunc_, mPool_](Args&&... args) {
            std::future< void > prev = (*mFunc_)(std::forward< Args >(args)...);
            return mPool_->execute(func, prev.share());
        }, mPool_);
    }

private:
    std::shared_ptr< MFunc > mFunc;
    std::shared_ptr< ThreadPool > mPool;
};

#endif // CXX_TOOLS_CHAIN_H
