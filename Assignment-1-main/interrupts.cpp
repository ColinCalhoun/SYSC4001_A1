/**
 *
 * @file interrupts.cpp
 * @author Sasisekhar Govind
 *
 * @author James Noel
 * @author Colin Calhoun
 */
#include "interrupts.hpp"

int main(int argc, char** argv) {

    //vectors is a C++ std::vector of strings that contain the address of the ISR
    //delays  is a C++ std::vector of ints that contain the delays of each device
    //the index of these elemens is the device number, starting from 0
    auto [vectors, delays] = parse_args(argc, argv);
    std::ifstream input_file(argv[1]);

    std::string trace;      //!< string to store single line of trace file
    std::string execution;  //!< string to accumulate the execution output

    

    /******************ADD YOUR VARIABLES HERE*************************/

    int current_time = 0; // total time (time up until current execution)

    int mode_switch_time = 1; // time it takes to switch between user and kernel mode
    int context_save_time = 10; // time it takes to save process context
    int find_isr_time = 1; // time it takes to find ISR in vector table
    int get_isr_time = 1; // time it takes to get ISR from vector table
    int isr_time = 300; // time it takes to execute ISR body
    int iret_time = 1; // time it takes to execute IRET

    /******************************************************************/

    //parse each line of the input trace file
    while(std::getline(input_file, trace)) {
        auto [activity, duration_intr] = parse_trace(trace);

        /******************ADD YOUR SIMULATION CODE HERE*************************/

        if (activity == "CPU") {

            execution += std::to_string(current_time) + ", " + std::to_string(duration_intr) + ", CPU burst complete\n";

            current_time += duration_intr;

        } else if (activity == "SYSCALL") {

            // intr_content is the type of activity for the line read ("switch to kernel mode", "context saved", etc.)
            // new_time is the NEW current total time of the entire simulation
            auto [exec_content, new_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);
            
            execution += exec_content; // add the execution content to the execution string
            current_time = new_time; // set the current time equal to the new time (current time + execution duration)

            execution += std::to_string(current_time) + ", " + std::to_string(isr_time) + ", execute ISR body\n";

            current_time += isr_time;

            execution += "...\n";

            execution += std::to_string(current_time) + ", " + std::to_string(iret_time) + ", IRET\n";

            current_time += iret_time;

            execution += std::to_string(current_time) + ", " + std::to_string(context_save_time) + ", Context restored\n";

            current_time += context_save_time;

            execution += std::to_string(current_time) + ", " + std::to_string(mode_switch_time) + " Switch to user mode\n";

            current_time += mode_switch_time;

        } else if (activity == "END_IO") {

            execution += std::to_string(current_time) + ", " + std::to_string(delays[duration_intr]) + 
            ", end of I/O " + std::to_string(duration_intr) + ": interrupt\n";

            current_time += delays[duration_intr];

            auto [exec_content, new_time] = intr_boilerplate(current_time, duration_intr, context_save_time, vectors);

            execution += exec_content; // add the execution content to the execution string
            current_time = new_time; // set the current time equal to the new time (current time + execution duration)

            execution += std::to_string(current_time) + ", " + std::to_string(isr_time) + ", execute ISR body\n";

            current_time += isr_time;

            execution += "...\n";

            execution += std::to_string(current_time) + ", " + std::to_string(iret_time) + ", IRET\n";

            current_time += iret_time;

            execution += std::to_string(current_time) + ", " + std::to_string(context_save_time) + ", Context restored\n";

            current_time += context_save_time;

            execution += std::to_string(current_time) + ", " + std::to_string(mode_switch_time) + " Switch to user mode\n";

            current_time += mode_switch_time;

        } else {
            execution += std::to_string(current_time) + ", 0, Unknown activity:" + activity + "\n";
        }
        
        /************************************************************************/

    }

    input_file.close();

    write_output(execution);

    return 0;
}
