﻿// ---------------------------------------------------------------------
// Copyright (c) 2024 Qualcomm Innovation Center, Inc. All rights reserved.
// SPDX-License-Identifier: BSD-3-Clause
// ---------------------------------------------------------------------
#include "PromptHandler.hpp"
#include "ChatApp.hpp"

using namespace AppUtils;

// Llama3 prompt
constexpr const std::string_view c_first_prompt_prefix_part_1 =
    "<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\nYou are an expert Cricket Commentator. Only commentate on the current ball as given, do not add any extra details like bowler or batsman names";
constexpr const std::string_view c_first_prompt_prefix_part_2 =
    "Based on the current runs scored, you need to generate the commentary. Make sure to capture all the emotion and excitement of the moment properly into your words. You have to emulate Ravi Shastri's style of commentary. Make sure to not add any extra tags or emotion notes in your text. Break the commentary into small lines. Make sure to keep the entire thing within 3 lines<|eot_id|>\n\n";
constexpr const std::string_view c_prompt_prefix = "<|start_header_id|>user<|end_header_id|>\n\n";
constexpr const std::string_view c_end_of_prompt = "<|eot_id|>";

// Indus Prompt
//constexpr const std::string_view c_first_prompt_prefix_part_1 =
//"<|begin_of_text|><|start_header_id|>system<|end_header_id|>\n\nYour name is ";
//constexpr const std::string_view c_first_prompt_prefix_part_2 =
//"and you are a helpful AI assistant. Please keep answers consice and to the point. <|eot_id|>\n\n";
//constexpr const std::string_view c_prompt_prefix = "<s>उपयोगकर्ता</s>\n\n";
//constexpr const std::string_view c_end_of_prompt = "<s>सहायक</s>";

/*  Llama2 system prompt
constexpr const std::string_view c_first_prompt_prefix_part_1 = "[INST] <<SYS>>\nYour name is ";
constexpr const std::string_view c_first_prompt_prefix_part_2 =
    "and you are a helpful AI assistant. Please keep answers consice and to the point. \n<</SYS>>\n\n";
constexpr const std::string_view c_prompt_prefix = "[INST] ";
constexpr const std::string_view c_end_of_prompt = " [/INST] ";
*/

PromptHandler::PromptHandler()
    : m_is_first_prompt(true)
{
}

std::string PromptHandler::GetPromptWithTag(const std::string& user_prompt)
{
    // Ref: https://www.llama.com/docs/model-cards-and-prompt-formats/meta-llama-2/
    if (m_is_first_prompt)
    {
        m_is_first_prompt = false;
        return std::string(c_first_prompt_prefix_part_1) + 
               c_first_prompt_prefix_part_2.data() + user_prompt + c_end_of_prompt.data();
        //return  std::string(c_prompt_prefix) + user_prompt.data() + std::string(c_end_of_prompt);
    }
    return std::string(c_prompt_prefix) + user_prompt.data() + std::string(c_end_of_prompt);
}
