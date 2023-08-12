# Apriorit Poland Summer 2023 Internship Program - Multithreading

This repository contains a simple multithreading example implemented using C++'s `std::thread`, `std::future`, and related synchronization primitives. The program demonstrates the usage of a basic thread pool called `Simple_Thread_Pool` to concurrently execute tasks.

## Description

The provided code demonstrates a basic implementation of a thread pool using C++ standard library features. The `Simple_Thread_Pool` class manages a fixed number of worker threads that can execute tasks concurrently. Tasks are added to the pool using the `post` method, and the results are collected using `std::future`.

## Building and Running

To compile and run the program, follow these steps:

1. Ensure you have a C++ compiler installed (e.g., g++, clang++).
2. Clone this repository to your local machine.
3. Open a terminal and navigate to the project directory.
4. Compile the source code:
   ```sh
   g++ -std=c++11 -o thread_pool_example thread_pool_example.cpp -lpthread
   ```
