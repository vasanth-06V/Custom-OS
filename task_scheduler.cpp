#include <iostream>
#include <string>
#include <thread>      // For std::thread
#include <mutex>       // For std::mutex and std::lock_guard
#include <chrono>      // For std::chrono
#include <sstream>     // For std::istringstream
#include <ctime>       // For std::localtime
#include <condition_variable> // For std::condition_variable
#include "task_scheduler.h"    
#include "command_parser.h"
#include "shell_executor.h"

// Define TaskScheduler class methods
TaskScheduler::TaskScheduler() : running(false), nextTaskId(1) {}

TaskScheduler::~TaskScheduler() {
    stop();
}

void TaskScheduler::start() {
    if (!running) {
        running = true;
        schedulerThread = std::thread(&TaskScheduler::schedulerLoop, this);
    }
}

void TaskScheduler::stop() {
    if (running) {
        running = false;
        taskCv.notify_all();
        if (schedulerThread.joinable()) {
            schedulerThread.join();
        }
    }
}

bool TaskScheduler::addTask(const std::string& taskSpec) {
    std::istringstream iss(taskSpec);
    std::string timeSpec;
    iss >> timeSpec;
    
    if (timeSpec.empty()) {
        std::cerr << "Error: Invalid task specification." << std::endl;
        return false;
    }
    
    std::string command;
    std::getline(iss, command);
    if (command.empty() || command[0] != ' ') {
        std::cerr << "Error: No command specified." << std::endl;
        return false;
    }
    command = command.substr(1);

    auto scheduledTime = parseTime(timeSpec);
    if (scheduledTime == std::chrono::system_clock::time_point()) {
        std::cerr << "Error: Invalid time specification." << std::endl;
        return false;
    }
    
    ScheduledTask task{command, scheduledTime, false, nextTaskId++};

    {
        std::lock_guard<std::mutex> lock(taskMutex);
        tasks.push_back(task);
    }

    taskCv.notify_one();
    std::cout << "Task scheduled with ID " << task.id << std::endl;
    return true;
}

void TaskScheduler::listTasks() {
    std::lock_guard<std::mutex> lock(taskMutex);

    if (tasks.empty()) {
        std::cout << "No scheduled tasks." << std::endl;
        return;
    }

    std::cout << "Scheduled tasks:\n";
    std::cout << "ID | Time | Command | Status\n";
    std::cout << "--------------------------------\n";

    for (const auto& task : tasks) {
        std::time_t timeT = std::chrono::system_clock::to_time_t(task.scheduledTime);
        std::tm timeInfo = *std::localtime(&timeT);
        char timeStr[9];
        std::strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeInfo);

        std::cout << task.id << " | " << timeStr << " | " << task.command 
                  << " | " << (task.executed ? "Executed" : "Pending") << std::endl;
    }
}

void TaskScheduler::schedulerLoop() {
    while (running) {
        ScheduledTask* nextTask = nullptr;
        auto now = std::chrono::system_clock::now();

        {
            std::lock_guard<std::mutex> lock(taskMutex);
            for (auto& task : tasks) {
                if (!task.executed && task.scheduledTime <= now) {
                    nextTask = &task;
                    break;
                }
            }
        }

        if (nextTask) {
            std::cout << "Executing scheduled task: " << nextTask->command << std::endl;

            CommandParser parser;
            ShellExecutor executor;
            auto args = parser.parse(nextTask->command);
            executor.execute(args);

            {
                std::lock_guard<std::mutex> lock(taskMutex);
                nextTask->executed = true;
            }
        } else {
            std::unique_lock<std::mutex> lock(taskMutex);
            auto nextTime = std::chrono::system_clock::time_point::max();

            for (const auto& task : tasks) {
                if (!task.executed && task.scheduledTime < nextTime) {
                    nextTime = task.scheduledTime;
                }
            }

            if (nextTime != std::chrono::system_clock::time_point::max()) {
                taskCv.wait_until(lock, nextTime);
            } else {
                taskCv.wait(lock);
            }
        }
    }
}

std::chrono::system_clock::time_point TaskScheduler::parseTime(const std::string& timeSpec) {
    auto now = std::chrono::system_clock::now();

    if (timeSpec[0] == '+') {
        try {
            int minutes = std::stoi(timeSpec.substr(1));
            return now + std::chrono::minutes(minutes);
        } catch (...) {
            return std::chrono::system_clock::time_point();
        }
    } else {
        try {
            std::time_t now_t = std::chrono::system_clock::to_time_t(now);
            std::tm currentTime = *std::localtime(&now_t);

            size_t colonPos = timeSpec.find(':');
            if (colonPos == std::string::npos) {
                return std::chrono::system_clock::time_point();
            }

            int hours = std::stoi(timeSpec.substr(0, colonPos));
            int minutes = std::stoi(timeSpec.substr(colonPos + 1));

            if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59) {
                return std::chrono::system_clock::time_point();
            }

            currentTime.tm_hour = hours;
            currentTime.tm_min = minutes;
            currentTime.tm_sec = 0;

            auto scheduledTime = std::chrono::system_clock::from_time_t(std::mktime(&currentTime));

            if (scheduledTime < now) {
                scheduledTime += std::chrono::hours(24);
            }

            return scheduledTime;
        } catch (...) {
            return std::chrono::system_clock::time_point();
        }
    }
}

