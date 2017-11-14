#include "kernel.h"

extern "C" void setError(const char *message){
    Ilwis::kernel()->issues()->log(message);
}
