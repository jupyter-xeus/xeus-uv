/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "xserver_uv_shell_main.hpp"
#include "xshell_uv_runner.hpp"

namespace xeus
{

    std::unique_ptr<xserver>
    make_xserver_uv(xcontext& context,
                    const xconfiguration& config,
                    nl::json::error_handler_t eh,
                    std::shared_ptr<uvw::loop> loop_ptr,
                    std::unique_ptr<xhook_base> hook)
    {
        return make_xserver_shell
        (
            context,
            config,
            eh,
            std::make_unique<xcontrol_default_runner>(),
            std::make_unique<xuv_runner>(std::move(loop), std::move(hook))
        );
    }

} // namespace xeus
