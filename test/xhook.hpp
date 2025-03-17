/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_UV_HOOK_HPP
#define XEUS_UV_HOOK_HPP

#include <memory>
#include <uvw.hpp>

#include "xeus-uv/xhook_base.hpp"

namespace xeus
{
    class xhook final : public xhook_base
    {
    public:
        xhook() = default;
        xhook(int x);

        ~xhook();

        xhook(const xhook&) = delete;
        xhook& operator=(const xhook&) = delete;

        xhook(xhook&&) = delete;
        xhook& operator=(xhook&&) = delete;

    private:
        int counter{ 0 };

        void pre_hook_impl() override;
        void post_hook_impl() override;
        void run_impl(std::shared_ptr<uvw::loop> loop) override;
    };

} // namespace xeus

#endif // XEUS_UV_HOOK_HPP
