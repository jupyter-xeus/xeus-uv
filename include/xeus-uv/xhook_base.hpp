/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XHOOK_BASE_HPP
#define XHOOK_BASE_HPP

#include <memory>
#include <uvw.hpp>

#include "xeus_uv.hpp"


namespace xeus
{
    class XEUS_UV_API xhook_base
    {
    public:

        virtual ~xhook_base() = default;

        xhook_base(const xhook_base&) = delete;
        xhook_base& operator=(const xhook_base&) = delete;

        xhook_base(xhook_base&&) = delete;
        xhook_base& operator=(xhook_base&&) = delete;

        void pre_hook();
        void post_hook();
        void run(std::shared_ptr<uvw::loop> loop);

    protected:

        xhook_base() = default;

    private:

        virtual void pre_hook_impl() = 0;
        virtual void post_hook_impl() = 0;
        virtual void run_impl(std::shared_ptr<uvw::loop> loop) = 0;

    };
}

#endif // XHOOK_BASE_HPP
