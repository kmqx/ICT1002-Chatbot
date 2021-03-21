cmake_minimum_required(VERSION 3.17)
project(ICT1002_Chatbot C)

set(CMAKE_C_STANDARD 11)

include_directories(src)

add_executable(ICT1002_Chatbot
        src/chat1002.h
        src/chatbot.c
        src/knowledge.c
        src/main.c)