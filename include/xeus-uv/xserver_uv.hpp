/***************************************************************************
* Copyright (c) 2024, Isabel Paredes                                       *
* Copyright (c) 2024, QuantStack                                           *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XEUS_SERVER_UV_HPP
#define XEUS_SERVER_UV_HPP

#include <uvw.hpp>

#include "xeus-uv/xeus-uv.hpp"
#include "xeus-uv/xhook_base.hpp"

#include "xeus/xserver.hpp"
#include "xeus/xeus_context.hpp"
#include "xeus/xkernel_configuration.hpp"

// TODO: include json


namespace xeus
{

    XEUX_UV_API
    std::unique_ptr<xserver>
    make_xserver_uv(xcontext& context,
                    const xconfiguration& config,
                    nl::json::error_handler_t eh,
                    std::shared_ptr<uvw::loop> loop,
                    std::unique_ptr<xhook_base> hook);

} // namespace xeus

#endif // XEUS_SERVER_UV_HPP
