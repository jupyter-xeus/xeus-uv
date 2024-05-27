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
        , p_hook{ hook }
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
        // zmq::fd_t shell_fd = m_shell.get(zmq::sockopt::fd);
        zmq::fd_t shell_fd = get_shell_fd();
        // zmq::fd_t controller_fd = m_controller.get(zmq::sockopt::fd); // TODO

        // Create (libuv) poll handles and bind them to the loop
        p_shell_poll = p_loop->resource<uvw::poll_handle>(shell_fd);
        p_controller_poll = p_loop->resource<uvw::poll_handle>(controller_fd);

        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }
                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_shell, ZMQ_DONTWAIT)) // non-blocking
                {
                    xmessage msg = p_server->deserialize(wire_msg);
                    p_server->notify_shell_listener(std::move(msg));
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

                zmq::multipart_t wire_msg;
                if (wire_msg.recv(m_controller, ZMQ_DONTWAIT))
                {
                    zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                    wire_reply.send(m_controller);
                    std::string msg = wire_reply.peekstr(0);
                    if (msg == "stop")
                    {
                        wire_msg.send(m_controller);
                        p_loop->stop();
                    }
                    else
                    {
                        zmq::multipart_t wire_reply = p_server->notify_internal_listener(wire_msg);
                        wire_reply.send(m_controller);
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
            });

        p_controller_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cerr << e.what() << std::endl;
            });
    }

} // namespace xeus
