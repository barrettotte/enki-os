#include "enki.h"

extern int main(int argc, char** argv);

void enki_start() {
    struct process_args args;
    enki_process_args(&args);

    int result = main(args.argc, args.argv);
    if (result == 0) {
        // TODO: do something exit code
    }
}