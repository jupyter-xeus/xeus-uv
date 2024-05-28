/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_UV_RUNNER_HPP
#define XEUS_UV_RUNNER_HPP

#include <memory>
#include <uvw.hpp>

#include "xeus-uv/xeus-uv.hpp"
#include "xeus-uv/xhook_base.hpp"

#include "xeus-zmq/xshell_runner.hpp"


namespace xeus
{

    class XEUS_UV_API xuv_runner final : public xshell_runner
    {
    public:

        xuv_runner(std::shared_ptr<uvw::loop> loop,
                   std::unique_ptr<xhook_base> hook);

        xuv_runner(const xuv_runner&) = delete;
        xuv_runner& operator=(const xuv_runner&) = delete;
        xuv_runner(xuv_runner&&) = delete;
        xuv_runner& operator=(xuv_runner&&) = delete;

        ~xuv_runner() override = default;

    private:

        void create_polls();

        void run_impl() override;

        std::shared_ptr<uvw::loop> p_loop{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_shell_poll{ nullptr };
        std::shared_ptr<uvw::poll_handle> p_controller_poll{ nullptr };
        std::unique_ptr<xhook_base> p_hook{ nullptr };
    };

} // namespace xeus

#endif // XEUS_UV_RUNNER_HPP
