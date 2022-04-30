#include "enki.h"

extern int main(int argc, char** argv);

void enki_start() {
    struct process_args args;
    enki_process_args(&args);

    if (!main(args.argc, args.argv)) {
        // TODO: do something exit code
    }
}