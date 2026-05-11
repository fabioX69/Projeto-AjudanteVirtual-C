#include "../include/interface.h"
#include "../include/db.h"


int main(int argc, char *argv[]) {

    if (!db_connect()) return 1;

    db_close();
    
    iniciar_interface(argc, argv);
    return 0;
}
