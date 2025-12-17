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
            std::cout << "No loop provided, using default loop." << std::endl;
            p_loop = uvw::loop::get_default();
        }

    }
    
    void xuv_runner::run_impl()
    {
        std::cout << "run_impl xuv_runner" << std::endl;
        create_polls(); // create poll used to be called in the constructor
                        // but that caused get_shell_fd to crash
                        // likely because zmq sockets were not yet ready???


        std::cout << "Starting polls for shell and controller sockets." << std::endl;
        p_shell_poll->start(uvw::poll_handle::poll_event_flags::READABLE);

        std::cout << "Starting polls for shell and controller sockets." << std::endl;
        p_controller_poll->start(uvw::poll_handle::poll_event_flags::READABLE);
        
  
        
        if (p_hook)
        {
            std::cout << "run hook." << std::endl;
            p_hook->run(p_loop);
            std::cout << "post run hook" << std::endl;
        }
        else
        {
            p_loop->run();
        }
    }

    void xuv_runner::create_polls()
    {
        // Get the file descriptor for the shell and controller sockets
        // and create (libuv) poll handles to bind them to the loop

        std::cout << "get filedescriptors" << std::endl;
        
        auto  fd_shell = get_shell_fd();
        std::cout << "Shell FD: " << fd_shell << std::endl;
        auto fd_controller = get_shell_controller_fd();
        std::cout << "Controller FD: " << fd_controller << std::endl;
        
        std::cout<< "Creating poll handles for shell and controller sockets." << std::endl;
        p_shell_poll = p_loop->resource<uvw::poll_handle>(fd_shell);
        p_controller_poll = p_loop->resource<uvw::poll_handle>(fd_controller);

        std::cout << "Setting up poll event handlers.(shell)" << std::endl;



        p_shell_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "INSIDE Poll event on shell." << std::endl; 
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                int ZMQ_DONTWAIT{ 1 }; // from zmq.h
                if (auto msg = read_shell(ZMQ_DONTWAIT))
                {
                    std::cout << "xuv_runner: received message on shell" << std::endl;
                    std::cout<<"content: "<< msg.value().content().dump(4)<<std::endl;
                    notify_shell_listener(std::move(msg.value()));
                    std::cout << "xuv_runner: message on shell processed" << std::endl;
                }
                else{
                    std::cout << "xuv_runner: no message on shell" << std::endl;
                    // No message received, could be interrupted system call
                }

                if (this->p_hook)
                {
                    std::cout << "post hook." << std::endl;
                    this->p_hook->post_hook();
                    std::cout << "post hook done." << std::endl;
                }

            }
        );
        std::cout << "Setting up poll event handlers. (controller)" << std::endl;
        p_controller_poll->on<uvw::poll_event>(
            [this](uvw::poll_event&, uvw::poll_handle&)
            {
                std::cout << "INSIDE Poll event on controller." << std::endl;
                if (this->p_hook)
                {
                    this->p_hook->pre_hook();
                }

                int ZMQ_DONTWAIT{ 1 }; // from zmq.h
                if (auto msg = read_controller(ZMQ_DONTWAIT))
                {
                    std::cout << "xuv_runner: received message on controller" << std::endl;
                    std::string val{ msg.value() };
                    if (val == "stop")
                    {
                        send_controller(std::move(val));
                        p_loop->stop();
                    }
                    else
                    {
                        std::string rep = notify_internal_listener(std::move(val));
                        send_controller(std::move(rep));
                    }
                }
                else{
                    std::cout << "xuv_runner: no message on controller" << std::endl;
                    // No message received, could be interrupted system call
                }

                if (this->p_hook)
                {
                    this->p_hook->post_hook();
                }
            }
        );
        std::cout << "Setting up poll error handlers. (shell)" << std::endl;

        p_shell_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cout << "Error event on shell poll." << std::endl;
                std::cerr << e.what() << std::endl;
            }
        );
        std::cout << "Setting up poll error handlers. (controller)" << std::endl;
        p_controller_poll->on<uvw::error_event>(
            [](const uvw::error_event& e, uvw::poll_handle&)
            {
                std::cout << "Error event on controller poll." << std::endl;
                std::cerr << e.what() << std::endl;
            }
        );

    }

} // namespace xeus
