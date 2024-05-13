/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SHELL_UV_HPP
#define XEUS_SHELL_UV_HPP

#include <memory>
#include <uvw.hpp>
#include <zmq.hpp>

#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

#include "xeus-zmq/xserver_zmq_split.hpp"

#include "xeus-uv/xhook_base.hpp"

namespace xeus
{


    class XEUS_UV_API xshell_uv final : public xserver_zmq_split
    {
    public:

        xshell_uv(xcontext& context,
                  const xconfiguration& config,
                  nl::json::error_handler_t eh,
                  xserver_zmq_split::control_runner_ptr control,
                  xserver_zmq_split::shell_runner_ptr shell,
                  std::shared_ptr<uvw::loop> loop_ptr,
                  std::unique_ptr<xhook_base> hook);

        virtual ~xshell_uv() = default;

    private:

        void start_impl(xpub_message message) override;
        void create_polls();

        std::shared_ptr<uvw::loop> p_loop{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_shell_poll{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_controller_poll{ nullptr };
        std::unique_ptr<xhook_base> p_hook{ nullptr };
    };

}

#endif
