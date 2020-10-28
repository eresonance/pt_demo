#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define PT_USE_GOTO 1
#include "pt/pt.h"

// saved values for the loop
struct loop_ctx {
    int id;
    struct pt pt;
    int values[5];
    size_t values_idx;
};

// wait for something to finish, loop a few times to grab the results, average, then return
static void loop(struct loop_ctx *ctx, int *result)
{
    struct pt * const pt = &ctx->pt;
    pt_begin(pt);

    //initial setup
    memset(ctx->values, 0, sizeof(ctx->values));
    ctx->values_idx = 0;
    *result = 0;

    //grab values
    while(ctx->values_idx < sizeof(ctx->values)/sizeof(*ctx->values)) {
        int tmp;
        //wait until we get a suitable value from rand()
        pt_wait(pt, ((tmp = rand()) < RAND_MAX/4));

        //save the value
        ctx->values[ctx->values_idx] = tmp;
        printf("  loop[%d] grab %d=%d\n", ctx->id, ctx->values_idx, ctx->values[ctx->values_idx]);
        ctx->values_idx += 1;
    }

    ctx->values_idx = 0;

    //pretend that this average is computationally expensive, yielding each time
    printf("  loop[%d] start avg...\n", ctx->id);
    pt_loop(pt, ctx->values_idx < sizeof(ctx->values)/sizeof(*ctx->values)) {
        *result += ctx->values[ctx->values_idx];
        ctx->values_idx += 1;
    }
    *result /= ctx->values_idx;
    printf("  loop[%d] found %d\n", ctx->id, *result);

    pt_end(pt);
}

#define NUM_COROS 4
int main()
{
    struct loop_ctx context[NUM_COROS];
    int vals[NUM_COROS] = {0};

    //initialize the context
    for(int i=0; i<NUM_COROS; i++) {
        context[i].id = i;
        context[i].pt = (struct pt)pt_init();
    }

    // our polling loop, keep going until all the coroutines finish
    while(1) {
        bool done = true;
        printf("main tick\n");
        for(int i=0; i<NUM_COROS; i++) {
            loop(&context[i], &vals[i]);
            done &= (pt_status(&context[i].pt) == PT_STATUS_FINISHED);
        }
        if(done) break;
    }

    printf("vals:\n");
    for(int i=0; i<NUM_COROS; i++) {
        printf("  %d\n", vals[i]);
    }

    return 0;
}
