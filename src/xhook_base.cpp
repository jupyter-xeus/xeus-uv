/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xeus-uv/xhook_base.hpp"

namespace xeus
{

    void xhook_base::pre_hook()
    {
        pre_hook_impl();
    }

    void xhook_base::post_hook()
    {
        post_hook_impl();
    }

    void xhook_base::run(std::shared_ptr<uvw::loop> loop)
    {
        run_impl(loop);
    }

} // namespace xeus
