// Apriorit Poland Summer 2023 Internship Program
// Multithreading
// Szymon Siąkała

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <future>
#include <random>

class Simple_Thread_Pool
{
private:
	bool stop;
	size_t thread_count;
	std::vector <std::thread> threads;
	std::queue <std::function <void()>> tasks;
	std::condition_variable condition;
	std::mutex mut;

public:
    explicit Simple_Thread_Pool(const std::size_t thread_count): thread_count(thread_count), stop(false)
    {
        for (std::size_t i = 0; i < thread_count; ++i)   // Add all threads to the vector.
        {
			threads.emplace_back([this] { work(); });
		}
	}

    Simple_Thread_Pool(const Simple_Thread_Pool&) = delete;   // Delete copy constructor.

    ~Simple_Thread_Pool()
    {
        destroy();
    }

	Simple_Thread_Pool& operator = (const Simple_Thread_Pool&) = delete;   // Delete copy assignment operator.

    template <typename Fnc_T>
    auto post(Fnc_T task) -> std::future <decltype(task())>   // Add task to the queue.
    {
        const auto wrapper = std::make_shared <std::packaged_task <decltype(task())()>> (std::move(task));
        tasks.emplace([=] { (*wrapper)(); });
        condition.notify_one();                               // Notify one thread that task is ready.
        return wrapper->get_future();                         // Return future object.
    }

    void work()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mut);							   // Lock mutex.
            condition.wait(lock, [this] { return stop || !tasks.empty(); });   // Wait for stop flag TRUE or tasks in the queue.
            if (stop && tasks.empty())                                         // If stop flag is TRUE and queue is empty, then finish work.
            {
                lock.unlock();												   // Unlock mutex.
                return;
            }
            const std::function<void()> task = std::move(tasks.front());       // Get task from the queue.
            tasks.pop();
            lock.unlock(); 												       // Unlock mutex.

            try
            {
                task();														   // Execute task.
            }
            catch (const std::exception& e)
            {
				std::cout << "ERROR IN WORKER THREAD: " << e.what() << std::endl;
			}
        }
    }

    void destroy()
    {
        std::unique_lock<std::mutex> lock(mut);   // Lock mutex.
        stop = true;
        condition.notify_all();                   // Notify all threads that stop flag is TRUE.
        lock.unlock();
        for (std::thread& thread : threads)       // Join all threads to ensures that all worker threads have completed their tasks and exited before the destroy function returns.
        {
            thread.join();
        }
    }
};



int main()
{
    try
    {
        const size_t thread_count = 4;
        Simple_Thread_Pool thread_pool(thread_count);
        std::vector <std::future <int>> results;

        const auto start = std::chrono::high_resolution_clock::now();

        std::cout << "Work started with " << thread_count << " threads active." << std::endl;

        for (int i = 0; i < thread_count; i++)
        {
            auto task = []() -> int                                                  // Lambda function that returns random number after random time.
            {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> number_dist(1, 100);
                std::uniform_int_distribution<int> time_dist(1, 5);
                std::this_thread::sleep_for(std::chrono::seconds(time_dist(gen)));   // Sleep for random time.
                return number_dist(gen);                                             // Return random number.
            };
            results.push_back(thread_pool.post(task));
        }

        for (int i = 0; i < thread_count; i++)
        {
            std::cout << "Result " << i << ": " << results[i].get() << std::endl;
        }

        const auto end = std::chrono::high_resolution_clock::now();
        const auto time = std::chrono::duration_cast<std::chrono::seconds> (end - start).count();
        std::cout << "Time of operation: " << time << " s." << std::endl;
    }
    catch (const std::exception& e)
    {
		std::cout << "ERROR: " << e.what() << std::endl;
        return 1;
	}
    
    return 0;
}