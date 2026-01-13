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

#include "xuv_runner.hpp"

#include "xeus-uv/xeus_uv.hpp"
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
            p_loop = uvw::loop::get_default();
        }

    }
    
    void xuv_runner::run_impl()
    {
        create_polls(); // create poll used to be called in the constructor
                        // but that caused get_shell_fd to crash
                        // likely because zmq sockets were not yet ready???

        p_shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
          
        if (p_hook)
        {
            p_hook->run(p_loop);
        }
        else
        {
            p_loop->run();
        }
    }

    void xuv_runner::create_polls()
    {
        // Get the file descriptor for the shell 
        // and create a (libuv) poll handle to bind it to the loop
        auto fd_shell = get_shell_fd();
    
        p_shell_poll = p_loop->resource<uvw::poll_handle>(fd_shell);

        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                int ZMQ_DONTWAIT{ 1 }; // from zmq.h 

                // Read **all** available messages
                while (auto msg = read_shell(ZMQ_DONTWAIT))
                {
                    notify_shell_listener(std::move(msg.value()));
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


    }

} // namespace xeus
