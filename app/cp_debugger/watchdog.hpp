#pragma once
/**
 * CP Debugger Watchdog Timer Header
 * 100% Deterministic & Contest-Safe Execution Profiler.
 * Uses POSIX setitimer and SIGALRM to capture the exact instruction pointer (PC)
 * and callstack when an infinite loop or execution timeout occurs.
 */

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <iostream>
#include <cstdlib>

namespace __cp_debugger_watchdog {

static void __handler(int sig, siginfo_t *info, void *ucontext) {
    ucontext_t *uc = (ucontext_t *)ucontext;
#if defined(__APPLE__) && defined(__arm64__)
    void *sig_pc = (void *)uc->uc_mcontext->__ss.__pc;
#elif defined(__APPLE__) && defined(__x86_64__)
    void *sig_pc = (void *)uc->uc_mcontext->__ss.__rip;
#elif defined(__linux__) && defined(__x86_64__)
    void *sig_pc = (void *)uc->uc_mcontext.gregs[REG_RIP];
#else
    void *sig_pc = nullptr;
#endif

    Dl_info dlinfo;
    if (sig_pc && dladdr(sig_pc, &dlinfo)) {
        std::cerr << "\n[WATCHDOG_TLE_PC] pc=" << sig_pc << " base=" << dlinfo.dli_fbase << " file=" << (dlinfo.dli_fname ? dlinfo.dli_fname : "") << "\n";
    }

    void *callstack[32];
    int frames = backtrace(callstack, 32);
    for (int i = 0; i < frames; ++i) {
        Dl_info frame_info;
        if (dladdr(callstack[i], &frame_info)) {
            std::cerr << "[WATCHDOG_FRAME " << i << "] pc=" << callstack[i] << " base=" << frame_info.dli_fbase << " file=" << (frame_info.dli_fname ? frame_info.dli_fname : "") << "\n";
        }
    }
    std::cerr.flush();
    _exit(124);
}

__attribute__((constructor))
static void __init() {
    const char *env_ms = getenv("WATCHDOG_TIMEOUT_MS");
    int ms = env_ms ? atoi(env_ms) : 2000;
    if (ms <= 0) ms = 2000;

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = __handler;
    sigaction(SIGALRM, &sa, nullptr);

    struct itimerval it;
    it.it_interval.tv_sec = 0;
    it.it_interval.tv_usec = 0;
    it.it_value.tv_sec = ms / 1000;
    it.it_value.tv_usec = (ms % 1000) * 1000;
    setitimer(ITIMER_REAL, &it, nullptr);
}

} // namespace __cp_debugger_watchdog
