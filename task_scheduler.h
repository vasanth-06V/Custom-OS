#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <chrono>

struct ScheduledTask {
    std::string command;
    std::chrono::system_clock::time_point scheduledTime;
    bool executed;
    int id;
};

class TaskScheduler {
public:
    TaskScheduler();
    ~TaskScheduler();
    
    // Start scheduler thread
    void start();
    
    // Stop scheduler thread
    void stop();
    
    // Add a new task
    bool addTask(const std::string& taskSpec);
    
    // List all tasks
    void listTasks();
    
private:
    // Scheduled tasks
    std::vector<ScheduledTask> tasks;
    
    // Mutex for thread safety
    std::mutex taskMutex;
    
    // Condition variable for waking up scheduler
    std::condition_variable taskCv;
    
    // Thread for scheduler
    std::thread schedulerThread;
    
    // Running flag
    bool running;
    
    // Next task ID
    int nextTaskId;
    
    // Scheduler thread function
    void schedulerLoop();
    
    // Parse time specification
    std::chrono::system_clock::time_point parseTime(const std::string& timeSpec);
};

#endif // TASK_SCHEDULER_H