/*
 * AtCoder Apps Native macOS Launcher (app/launcher/launcher_main.c)
 *
 * Lightweight zero-dependency C binary wrapped in AtCoder Apps.app.
 * - Ensures Spotlight/Dock GUI environment has full Python & compiler PATH.
 * - Checks if AtCoder Apps Portal is already running & healthy via /api/health.
 * - If running: opens its browser URL immediately (<10 ms) without spawning duplicates.
 * - If stopped: starts app/portal.py --no-browser detached in its own session,
 *   waits until /api/health passes, and opens the dashboard URL in the browser.
 * - Set ATCODER_NO_OPEN=1 to skip launching the browser/alert UI (for CLI testing).
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef APP_DIR
#define APP_DIR "."
#endif

#ifndef PORTAL_SCRIPT
#define PORTAL_SCRIPT "./portal.py"
#endif

#ifndef PYTHON_BIN
#define PYTHON_BIN "/Library/Frameworks/Python.framework/Versions/3.13/bin/python3"
#endif

#define STATE_FILE "/tmp/atcoder_apps_portal.json"
#define PORTAL_LOG "/tmp/atcoder_apps_portal.log"
#define DEFAULT_PORTAL_PORT 8000

static int is_no_open_mode(void) {
    const char *v = getenv("ATCODER_NO_OPEN");
    return (v != NULL && v[0] != '\0' && strcmp(v, "0") != 0);
}

static void setup_rich_path(void) {
    const char *prefix =
        "/Library/Frameworks/Python.framework/Versions/3.13/bin:"
        "/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin";
    const char *cur = getenv("PATH");
    if (cur == NULL || cur[0] == '\0') {
        setenv("PATH", prefix, 1);
        return;
    }
    size_t len = strlen(prefix) + 1 + strlen(cur) + 1;
    char *buf = (char *)malloc(len);
    if (buf != NULL) {
        snprintf(buf, len, "%s:%s", prefix, cur);
        setenv("PATH", buf, 1);
        free(buf);
    }
}

/* Parse "port": <num> from /tmp/atcoder_apps_portal.json. Returns port or 0 if unavailable. */
static int read_state_port(void) {
    FILE *fp = fopen(STATE_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
    char buf[1024];
    size_t n = fread(buf, 1, sizeof(buf) - 1, fp);
    fclose(fp);
    if (n == 0) {
        return 0;
    }
    buf[n] = '\0';

    const char *p = strstr(buf, "\"port\"");
    if (p == NULL) {
        return 0;
    }
    p = strchr(p, ':');
    if (p == NULL) {
        return 0;
    }
    p++;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
        p++;
    }
    char *endptr = NULL;
    long port = strtol(p, &endptr, 10);
    if (endptr == p || port < 1024 || port > 65535) {
        return 0;
    }
    return (int)port;
}

/* Returns 1 iff 127.0.0.1:port responds HTTP 200 to /api/health with "atcoder_portal". */
static int check_portal_health(int port) {
    if (port <= 0 || port > 65535) {
        return 0;
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        return 0;
    }

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 300000; /* 300 ms */
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    int flags = fcntl(fd, F_GETFL, 0);
    if (flags >= 0) {
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int rc = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0 && errno != EINPROGRESS) {
        close(fd);
        return 0;
    }
    if (rc < 0) {
        fd_set wfds;
        FD_ZERO(&wfds);
        FD_SET(fd, &wfds);
        struct timeval ctv = {0, 300000};
        if (select(fd + 1, NULL, &wfds, NULL, &ctv) <= 0) {
            close(fd);
            return 0;
        }
        int so_err = 0;
        socklen_t slen = sizeof(so_err);
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_err, &slen) < 0 || so_err != 0) {
            close(fd);
            return 0;
        }
    }

    if (flags >= 0) {
        fcntl(fd, F_SETFL, flags);
    }

    const char *req =
        "GET /api/health HTTP/1.1\r\n"
        "Host: 127.0.0.1\r\n"
        "Connection: close\r\n\r\n";
    ssize_t req_len = (ssize_t)strlen(req);
    if (send(fd, req, (size_t)req_len, 0) != req_len) {
        close(fd);
        return 0;
    }

    char resp[2048];
    size_t total = 0;
    while (total < sizeof(resp) - 1) {
        ssize_t r = recv(fd, resp + total, sizeof(resp) - 1 - total, 0);
        if (r <= 0) {
            break;
        }
        total += (size_t)r;
        if (strstr(resp, "atcoder_portal") != NULL) {
            break;
        }
    }
    close(fd);
    resp[total] = '\0';

    if (strstr(resp, "200 OK") != NULL && strstr(resp, "\"atcoder_portal\"") != NULL) {
        return 1;
    }
    return 0;
}

static int open_url(const char *url) {
    if (is_no_open_mode()) {
        return 0;
    }
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        char *const argv[] = {"/usr/bin/open", (char *)url, NULL};
        execv("/usr/bin/open", argv);
        _exit(127);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : -1;
}

static void show_error_alert(const char *msg) {
    fprintf(stderr, "[AtCoder Apps] Error: %s\n", msg);
    if (is_no_open_mode()) {
        return;
    }
    pid_t pid = fork();
    if (pid == 0) {
        char *const argv[] = {
            "/usr/bin/osascript",
            "-e", "on run argv",
            "-e", "display alert \"AtCoder Apps 起動エラー\" message (item 1 of argv) as critical",
            "-e", "end run",
            (char *)msg,
            NULL
        };
        execv("/usr/bin/osascript", argv);
        _exit(127);
    }
    if (pid > 0) {
        waitpid(pid, NULL, 0);
    }
}

static int find_healthy_portal_port(void) {
    int state_port = read_state_port();
    if (state_port > 0 && check_portal_health(state_port)) {
        return state_port;
    }
    if (state_port != DEFAULT_PORTAL_PORT && check_portal_health(DEFAULT_PORTAL_PORT)) {
        return DEFAULT_PORTAL_PORT;
    }
    return 0;
}

static pid_t spawn_portal_detached(void) {
    pid_t pid = fork();
    if (pid < 0) {
        return -1;
    }
    if (pid == 0) {
        if (setsid() < 0) {
            _exit(126);
        }
        int devnull = open("/dev/null", O_RDONLY);
        if (devnull >= 0) {
            dup2(devnull, STDIN_FILENO);
            if (devnull > STDERR_FILENO) {
                close(devnull);
            }
        }
        int logfd = open(PORTAL_LOG, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (logfd >= 0) {
            dup2(logfd, STDOUT_FILENO);
            dup2(logfd, STDERR_FILENO);
            if (logfd > STDERR_FILENO) {
                close(logfd);
            }
        }
        if (chdir(APP_DIR) != 0) {
            /* Continue even if chdir fails; PORTAL_SCRIPT is absolute */
        }

        if (access(PYTHON_BIN, X_OK) == 0) {
            char *const argv[] = {(char *)PYTHON_BIN, (char *)PORTAL_SCRIPT, "--no-browser", NULL};
            execv(PYTHON_BIN, argv);
        }
        char *const fallback_argv[] = {"python3", (char *)PORTAL_SCRIPT, "--no-browser", NULL};
        execvp("python3", fallback_argv);
        _exit(127);
    }
    return pid;
}

int main(void) {
    signal(SIGPIPE, SIG_IGN);
    setup_rich_path();

    int existing_port = find_healthy_portal_port();
    if (existing_port > 0) {
        char url[128];
        snprintf(url, sizeof(url), "http://127.0.0.1:%d", existing_port);
        if (open_url(url) != 0) {
            show_error_alert("ブラウザでポータルを開けませんでした。");
            return 1;
        }
        return 0;
    }

    if (access(PORTAL_SCRIPT, R_OK) != 0) {
        show_error_alert("ポータルスクリプト (app/portal.py) が見つかりません。");
        return 1;
    }

    pid_t child = spawn_portal_detached();
    if (child < 0) {
        show_error_alert("ポータルプロセスの起動 (fork) に失敗しました。");
        return 1;
    }

    int ready_port = 0;
    int child_exited_early = 0;
    for (int i = 0; i < 80; i++) {
        int port = read_state_port();
        if (port > 0 && check_portal_health(port)) {
            ready_port = port;
            break;
        }
        int status = 0;
        pid_t wp = waitpid(child, &status, WNOHANG);
        if (wp == child) {
            child_exited_early = 1;
            break;
        }
        usleep(50000); /* 50 ms * 80 = up to 4.0 s */
    }

    if (ready_port <= 0) {
        if (child_exited_early) {
            show_error_alert("ポータルサーバーが起動直後に終了しました。詳細: /tmp/atcoder_apps_portal.log");
        } else {
            show_error_alert("ポータルサーバーの起動がタイムアウトしました。詳細: /tmp/atcoder_apps_portal.log");
        }
        return 1;
    }

    char url[128];
    snprintf(url, sizeof(url), "http://127.0.0.1:%d", ready_port);
    if (open_url(url) != 0) {
        show_error_alert("ブラウザでポータルを開けませんでした。");
        return 1;
    }
    return 0;
}
