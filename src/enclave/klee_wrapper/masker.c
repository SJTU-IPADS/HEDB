#include "prefix_udf.h"
#include <klee/klee.h>
#include <assert.h>

#define MAX_PREFIX_SIZE 100
void mask_prefix(prefix_range *a){
    char *ch = a->prefix;
    while(ch != '\0'){
        ch = '*';
        ch ++;
    }
}

void check_prefix_masked(prefix_range *a){
    char *ch = a->prefix;
    while (ch != '\0')
    {
        assert(ch == '*');
        ch ++;
    }
}
// ch == '*'
int main(){

    prefix_range *a = (prefix_range *)malloc(MAX_PREFIX_SIZE);
    klee_make_symbolic(a, MAX_PREFIX_SIZE, "prefix");

}
