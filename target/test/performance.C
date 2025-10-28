#include <sys/wait.h>
#include <targeting/predicates/predicateattr.H>
#include <targeting/predicates/predicateattrval.H>
#include <targeting/target.H>
#include <targeting/xmltohb/attributeenums.H>
#include <targeting/xmltohb/attributestructs.H>
#include <targeting/xmltohb/attributetraits.H>
#include <targetsvc/target_service.H>
#include <time.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

using namespace TARGETING;

int main()
{
    constexpr const char* dtb_path = "/tmp/targeting_test.dtb";
    constexpr int NUM_RUNS = 10;

    double total_ms = 0.0;

    for (int i = 0; i < NUM_RUNS; ++i)
    {
        struct timespec start{}, end{};

        clock_gettime(CLOCK_MONOTONIC, &start);

        pid_t pid = fork();
        if (pid == 0)
        {
            // Child process: isolated initialization
            auto& ts = TargetService::instance();
            ts.init(dtb_path);
            PredicateAttrVal<ATTR_TYPE> pred(TYPE_PROC);
            auto top = ts.getTopLevelTarget();
            for (auto&& proc :
                 ts.getAssociated(top, AssociationType::childByPhysical,
                                  RecursionLevel::all, &pred))
            {}
            _exit(0);
        }
        else if (pid > 0)
        {
            // Parent waits for child to finish
            int status = 0;
            waitpid(pid, &status, 0);

            clock_gettime(CLOCK_MONOTONIC, &end);

            double elapsed_ms = (end.tv_sec - start.tv_sec) * 1000.0 +
                                (end.tv_nsec - start.tv_nsec) / 1.0e6;

            total_ms += elapsed_ms;
            std::cout << "Run " << (i + 1) << ": " << elapsed_ms << " ms"
                      << std::endl;
        }
        else
        {
            perror("fork");
            return 1;
        }
    }

    std::cout << "\nAverage init time: " << (total_ms / NUM_RUNS) << " ms"
              << std::endl;

    return 0;
}
