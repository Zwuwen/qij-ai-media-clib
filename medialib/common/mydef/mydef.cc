#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include "typedef.h"
#include "mydef.h"

BOOL is_file_exist(const char* filename)
{
    if(access(filename,0) == 0)    
    {
        return true;
    }
    else
    {
        return false;
    }
}

void generate_rand_32bit(UINT32& rand_num)
{
    srand((unsigned)time(NULL));
    rand_num = (UINT32)rand();
}