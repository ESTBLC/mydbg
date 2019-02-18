#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "sig.h"
#include "dbg.h"
#include "register/regs.h"
#include "breakpoint/brk.h"

static void sig_trap(ctx_t *ctx);

int sig_catch(pid_t pid)
{
    int wstatus;
    waitpid(pid, &wstatus, 0);

    if (WIFEXITED(wstatus)) {
        printf("Debugged program exited normally\n");
        _Exit(0);
        return 0;
    }

    if (WIFSTOPPED(wstatus)) {
        return WSTOPSIG(wstatus);
    }

    return -1;
}

int sig_handler(ctx_t *ctx, int sig)
{
    switch (sig) {
        case SIGTRAP:
            sig_trap(ctx);
            return 0;
        default :
            printf("A signal has been caught but no action has been implemented\n");
            return -1;
    }

    return -1;
}

static void sig_trap(ctx_t *ctx)
{
    static brkp_t *last_brkp = NULL;

    /* If we where on a breakpoint the last time reset the interrupt */
    if (ctx->onbrk) {
        brkp_set(last_brkp);
        ctx->onbrk = false;
        last_brkp = NULL;
    }

    struct user_regs_struct regs;
    regs_get(ctx->pid, &regs);
    int rip = reg_get(&regs, rip);

    brkp_t *brkp = htab_find(ctx->brktab, &rip);

    if (brkp) {
        brkp_unset(brkp);

        /* Set rip to the breakpoint itself */
        reg_set(&regs, rip, --rip);
        regs_set(ctx->pid, &regs);

        ctx->onbrk = true;
        last_brkp = brkp;
    }
}
