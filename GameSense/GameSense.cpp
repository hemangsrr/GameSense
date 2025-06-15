// ---------------------------------------------------------------------
// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// ---------------------------------------------------------------------

#include "GameSense.hpp"
#include "PromptHandler.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using namespace App;

namespace
{
    constexpr const int c_chat_separater_length = 80;

    void ChatSplit(bool end_line = true)
    {
        std::string split_line(c_chat_separater_length, '-');
        std::cout << "\n" << split_line;
        if (end_line)
        {
            std::cout << "\n";
        }
    }

    std::wstring Utf8ToUtf16(const std::string& utf8)
    {
        if (utf8.empty()) return L"";

        int wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
        if (wide_size == 0) return L"";

        std::wstring utf16(wide_size, L'\0');
        MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], wide_size);
        return utf16;
    }

    std::string Utf16ToUtf8(const std::wstring& utf16)
    {
        if (utf16.empty()) return "";

        int utf8_size = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8_size == 0) return "";

        std::string utf8(utf8_size, '\0');
        WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], utf8_size, nullptr, nullptr);
        return utf8;
    }

    struct SocketUserData {
        tcp::socket* client_socket;
        std::string response_accumulator;
    };

    void GenieCallBack(const char* response_back, const GenieDialog_SentenceCode_t sentence_code, const void* user_data)
    {
        SocketUserData* socket_data = static_cast<SocketUserData*>(const_cast<void*>(user_data));
        socket_data->response_accumulator.append(response_back);

        // Convert UTF-8 response to UTF-16 for proper console output
        std::wstring wide_response = Utf8ToUtf16(response_back);

        // Output the converted UTF-16 string
        std::wcout << wide_response << std::endl;

        // Send response_back to the client socket
        if (socket_data->client_socket && socket_data->client_socket->is_open()) {
            boost::system::error_code ignored_ec;
            boost::asio::write(*(socket_data->client_socket),
                boost::asio::buffer(response_back, strlen(response_back)),
                ignored_ec);
        }

        if (sentence_code == GenieDialog_SentenceCode_t::GENIE_DIALOG_SENTENCE_END)
        {
            ChatSplit(false);
        }
    }

} // namespace

ChatApp::ChatApp(const std::string& config)
{
    if (GENIE_STATUS_SUCCESS != GenieDialogConfig_createFromJson(config.c_str(), &m_config_handle))
    {
        throw std::runtime_error("Failed to create the Genie Dialog config. Please check config.");
    }

    if (GENIE_STATUS_SUCCESS != GenieDialog_create(m_config_handle, &m_dialog_handle))
    {
        throw std::runtime_error("Failed to create the Genie Dialog.");
    }
}

ChatApp::~ChatApp()
{
    if (m_config_handle != nullptr)
    {
        if (GENIE_STATUS_SUCCESS != GenieDialogConfig_free(m_config_handle))
        {
            std::cerr << "Failed to free the Genie Dialog config.";
        }
    }

    if (m_dialog_handle != nullptr)
    {
        if (GENIE_STATUS_SUCCESS != GenieDialog_free(m_dialog_handle))
        {
            std::cerr << "Failed to free the Genie Dialog.";
        }
    }
}

void ChatApp::RunServer()
{
    AppUtils::PromptHandler prompt_handler;

    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

    std::cout << "Socket server (Boost.Asio) listening on port 8080..." << std::endl;

    while (true) {
        tcp::socket socket(io_context);
        acceptor.accept(socket);

        char recvbuf[4096];
        boost::system::error_code error;
        size_t bytes_received = socket.read_some(boost::asio::buffer(recvbuf, sizeof(recvbuf) - 1), error);

        if (!error && bytes_received > 0) {
            recvbuf[bytes_received] = '\0';
            std::string user_prompt(recvbuf);

            std::cout << "[DEBUG] Raw input bytes: " << user_prompt << std::endl;

            std::wstring wide_user_prompt = Utf8ToUtf16(user_prompt);

            std::string tagged_prompt = App::c_bot_name.data() + std::string(": ") + prompt_handler.GetPromptWithTag(user_prompt);

            std::cout << "[LOG] Received query: " << user_prompt << std::endl;

            SocketUserData socket_data;
            socket_data.client_socket = &socket;
            socket_data.response_accumulator.clear();

            if (GENIE_STATUS_SUCCESS != GenieDialog_query(m_dialog_handle, tagged_prompt.c_str(),
                GenieDialog_SentenceCode_t::GENIE_DIALOG_SENTENCE_COMPLETE,
                GenieCallBack, &socket_data))
            {
                std::string error_msg = "Failed to get response from GenieDialog. Please restart the ChatApp.";
                boost::asio::write(socket, boost::asio::buffer(error_msg), error);
            }

            if (socket_data.response_accumulator.empty())
            {
                if (GENIE_STATUS_SUCCESS != GenieDialog_reset(m_dialog_handle))
                {
                    std::string error_msg = "Failed to reset Genie Dialog.";
                    boost::asio::write(socket, boost::asio::buffer(error_msg), error);
                }
                if (GENIE_STATUS_SUCCESS != GenieDialog_query(m_dialog_handle, tagged_prompt.c_str(),
                    GenieDialog_SentenceCode_t::GENIE_DIALOG_SENTENCE_COMPLETE,
                    GenieCallBack, &socket_data))
                {
                    std::string error_msg = "Failed to get response from GenieDialog. Please restart the ChatApp.";
                    boost::asio::write(socket, boost::asio::buffer(error_msg), error);
                }
            }

            std::cout << "[LOG] Generated response: " << socket_data.response_accumulator << std::endl;
        }

        socket.close();
    }
}