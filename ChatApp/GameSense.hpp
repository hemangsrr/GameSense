// ---------------------------------------------------------------------
// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// ---------------------------------------------------------------------

#pragma once

#include <string>

#include "GenieCommon.h"
#include "GenieDialog.h"

namespace App
{
    constexpr const std::string_view c_exit_prompt = "exit";
    constexpr const std::string_view c_bot_name = "Qbot";

    class ChatApp
    {
    private:
        GenieDialogConfig_Handle_t m_config_handle = nullptr;
        GenieDialog_Handle_t m_dialog_handle = nullptr;

    public:
        ChatApp(const std::string& config);
        ChatApp() = delete;
        ChatApp(const ChatApp&) = delete;
        ChatApp(ChatApp&&) = delete;
        ChatApp& operator=(const ChatApp&) = delete;
        ChatApp& operator=(ChatApp&&) = delete;
        ~ChatApp();

        void RunServer();
    };
} // namespace App
