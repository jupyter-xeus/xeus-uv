/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <iostream>
#include <memory>
#include <uvw.hpp>

#include "xhook.hpp"

#include "xeus-uv/xhook_base.hpp"


namespace xeus
{
    xhook::xhook(int x) : xhook_base(), counter{ x }
    {
    }

    xhook::~xhook()
    {
        std::cout << "[HOOK] Destructor" << counter << std::endl;
    }

    void xhook::pre_hook_impl()
    {
        std::cout << "[HOOK] pre_hook_impl " << counter << std::endl;
        ++counter;
    }

    void xhook::post_hook_impl()
    {
        std::cout << "[HOOK] post_hook_impl " << counter << std::endl;
        ++counter;
    }

    void xhook::run_impl(std::shared_ptr<uvw::loop> loop)
    {
        std::cout << "[HOOK] run_impl " << counter << std::endl;
        loop->run();
    }

} // namespace xeus
