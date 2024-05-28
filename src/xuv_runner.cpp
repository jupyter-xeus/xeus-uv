/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include <uvw.hpp>
#include <iostream>

#include "xuv_runner.hpp"

#include "xeus-uv/xeus-uv.hpp"
#include "xeus-uv/xhook_base.hpp"


namespace xeus
{

    xuv_runner::xuv_runner(std::shared_ptr<uvw::loop> loop,
                           std::unique_ptr<xhook_base> hook)
        : xshell_runner()
        , p_loop{ loop }
        , p_hook{ std::move(hook) }
    {
        if (!p_loop)
        {
            std::cerr << "No loop provided, using default loop." << std::endl;
            p_loop = uvw::loop::get_default();
        }
        create_polls();
    }

    void xuv_runner::run_impl()
    {
        p_shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
        p_controller_poll->start(uvw::poll_handle::poll_event_flags::READABLE);

        if (p_hook)
        {
            p_hook->run(p_loop);
        }
        else
        {
            p_loop->run();
        }

        // TODO: handle stop
    }

    void xuv_runner::create_polls()
    {
        // Get the file descriptor for the shell and controller sockets
        // and create (libuv) poll handles to bind them to the loop
        p_shell_poll = p_loop->resource<uvw::poll_handle>(get_shell_fd());
        p_controller_poll = p_loop->resource<uvw::poll_handle>(get_shell_controller_fd());

        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                int ZMQ_DONTWAIT{ 1 }; // from zmq.h
                if (auto msg = read_shell(ZMQ_DONTWAIT)) // xmessage
                {
                    notify_shell_listener(std::move(msg.value()));
                }

                if (this->p_hook)
                {
                    this->p_hook->post_hook();
                }
            }
        );

        p_controller_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                int ZMQ_DONTWAIT{ 1 }; // from zmq.h
                if (auto msg = read_controller(ZMQ_DONTWAIT))
                {
                    std::string val{ msg.value() };
                    if (val == "stop")
                    {
                        send_controller(std::move(val));
                    }
                    else
                    {
                        std::string rep = notify_internal_listener(std::move(val));
                        send_controller(std::move(rep));
                    }
                }

                if (this->p_hook)
                {
                    this->p_hook->post_hook();
                }
            }
        );

        p_shell_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cerr << e.what() << std::endl;
            }
        );

        p_controller_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cerr << e.what() << std::endl;
            }
        );

    }

} // namespace xeus
