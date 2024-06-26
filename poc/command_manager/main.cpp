
#include "command_receiver.hpp"
#include "command_executor.hpp"
#include <thread>


int main() {
    
    command_receiver receiver("commands.db");

    std::thread t1(&command_receiver::receive_command, &receiver);
    std::thread t2(&command_receiver::dispatch_command, &receiver);

    // Esperar a que ambos hilos terminen su ejecución
    t1.join();
    t2.join();

    return 0;
}