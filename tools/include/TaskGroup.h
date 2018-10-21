/**
 * refer to https://github.com/qicosmos/TaskCppV1.1
*/

#ifndef CXX_TOOLS_TASK_GROUP_H
#define CXX_TOOLS_TASK_GROUP_H

#include "Chain.h"

#include <utility>

#include <iostream>

template< typename R >
class TaskGroup
{
public:
    inline void Add(const std::shared_future< R >& sf)
    {
        group.emplace_back(sf);
    }

    inline void Add(std::shared_future< R >&& sf)
    {
        group.emplace_back(std::move(sf));
    }

    inline void Add(std::future< R >&& fut)
    {
        Add(fut.share());
    }

    template< typename F >
    inline void Add(F&& f)
    {
        Add(std::async(std::launch::async, 
                       std::forward< F >(f)));
    }

    template< typename F, typename... Funcs >
    inline void Add(F&& f, Funcs&&... funcs)
    {
        Add(std::forward< F >(f));
        Add(std::forward< Funcs >(funcs)...);
    }

    Chain< std::vector< R >() > All()
    {
        return Chain< std::vector< R >() >(
        [this]() {
            std::vector< R > values;
            for(auto& sf : group)
            {
                values.emplace_back(sf.get());
            }
            return values;
        });
    }

    Chain< std::pair< size_t, R >() > Any()
    {
        return Chain< std::pair< size_t, R >() >(
        [this]() {
            size_t size = group.size();
            while(true)
            {
                for(size_t i = 0; i < size; ++i)
                {
                    if(group[i].wait_for(std::chrono::milliseconds(1))
                       == std::future_status::ready)
                    {
                        return std::make_pair(i, group[i].get());
                    }
                }
            }
        });
    }

private:
    std::vector< std::shared_future< R > > group;
};

#endif // CXX_TOOLS_TASK_GROUP_H