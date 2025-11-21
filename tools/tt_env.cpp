/*
    TickTockDB is an open-source Time Series Database, maintained by
    Yongtao You (yongtao.you@gmail.com) and Yi Lin (ylin30@gmail.com).

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <string>
#include <unistd.h>

#include "config.h"

int main(const int argc, char *argv[]) {
    std::string options[] = {
        CFG_APPEND_LOG_ENABLED,
        CFG_APPEND_LOG_FLUSH_FREQUENCY,
        CFG_CLUSTER_SERVERS,
        CFG_CLUSTER_PARTITIONS,
        CFG_CLUSTER_BACKLOG_ROTATION_SIZE,
        CFG_CONFIG_RELOAD_ENABLED,
        CFG_CONFIG_RELOAD_FREQUENCY,
        CFG_HTTP_LISTENER_COUNT,
        CFG_HTTP_REQUEST_FORMAT,
        CFG_HTTP_RESPONDERS_PER_LISTENER,
        CFG_HTTP_SERVER_PORT,
        CFG_LOG_FILE,
        CFG_LOG_LEVEL,
        CFG_LOG_RETENTION_COUNT,
        CFG_LOG_ROTATION_SIZE,
        CFG_MQTT_SETTINGS,
        CFG_STATS_FREQUENCY,
        CFG_TCP_CONNECTION_IDLE_TIMEOUT,
        CFG_TCP_LISTENER_COUNT,
        CFG_TCP_MAX_EPOLL_EVENTS,
        CFG_TCP_BUFFER_SIZE,
        CFG_TCP_MIN_FILE_DESCRIPTOR,
        CFG_TCP_MIN_HTTP_STEP,
        CFG_TCP_RESPONDERS_PER_LISTENER,
        CFG_TCP_RESPONDERS_QUEUE_SIZE,
        CFG_TCP_SERVER_ENABLED,
        CFG_TCP_SERVER_PORT,
        CFG_TCP_SOCKET_RCVBUF_SIZE,
        CFG_TCP_SOCKET_SNDBUF_SIZE,
        CFG_TICKTOCK_HOME,
        CFG_TIMER_GRANULARITY,
        CFG_TIMER_QUEUE_SIZE,
        CFG_TIMER_THREAD_COUNT,
        CFG_TS_ARCHIVE_THRESHOLD,
        CFG_TS_LOCK_PROBABILITY,
        CFG_TSDB_ARCHIVE_THRESHOLD,
        CFG_TSDB_COMPACT_BATCH_SIZE,
        CFG_TSDB_COMPACT_FREQUENCY,
        CFG_TSDB_COMPACT_THRESHOLD,
        CFG_TSDB_COMPRESSOR_PRECISION,
        CFG_TSDB_COMPRESSOR_VERSION,
        CFG_TSDB_DATA_DIR,
        CFG_TSDB_FLUSH_FREQUENCY,
        CFG_TSDB_GC_FREQUENCY,
        CFG_TSDB_MAX_DP_LINE,
        CFG_TSDB_METRIC_BUCKETS,
        CFG_TSDB_MIN_DISK_SPACE,
        CFG_TSDB_OFF_HOUR_BEGIN,
        CFG_TSDB_OFF_HOUR_END,
        CFG_TSDB_PAGE_COUNT,
        CFG_TSDB_PAGE_SIZE,
        CFG_TSDB_READ_ONLY_THRESHOLD,
        CFG_TSDB_RETENTION_THRESHOLD,
        CFG_TSDB_ROLLUP_BUCKETS,
        CFG_TSDB_ROLLUP_COMPRESSOR_PRECISION,
        CFG_TSDB_ROLLUP_COMPRESSOR_VERSION,
        CFG_TSDB_ROLLUP_LEVEL1_COMPRESSOR_VERSION,
        CFG_TSDB_ROLLUP_LEVEL2_COMPRESSOR_VERSION,
        CFG_TSDB_ROLLUP_ENABLED,
        CFG_TSDB_ROLLUP_FREQUENCY,
        CFG_TSDB_ROLLUP_PAUSE,
        CFG_TSDB_ROLLUP_THRESHOLD,
        CFG_TSDB_ROTATION_FREQUENCY,
        CFG_TSDB_SELF_METER_DESTINATION,
        CFG_TSDB_SELF_METER_ENABLED,
        CFG_TSDB_THRASHING_THRESHOLD,
        CFG_TSDB_TIMESTAMP_RESOLUTION,
        CFG_TSDB_TIMEZONE,
        CFG_UDP_LISTENER_COUNT,
        CFG_UDP_BATCH_SIZE,
        CFG_UDP_SERVER_ENABLED,
        CFG_UDP_SERVER_PORT,
    };

    std::string config_file_path;
    std::string exe_path = "tt";
    std::string env_prefix;

    while (true) {
        switch (getopt(argc, argv, "c:p:x:h")) {
            case 'c':
                config_file_path = optarg;
                continue;

            case 'p':
                env_prefix = optarg;
                continue;

            case 'x':
                exe_path = optarg;
                continue;

            case '?':
            case 'h':
            default:
                printf("Usage\n");
                printf("\n");
                printf("    %s [options]\n", argv[0]);
                printf("\n");
                printf("Options\n");
                printf("\n");
                printf(
                    "    -c <path-to-generated-config-file>     Explicitly specify the path of the generated config file.\n");
                printf("    -p <env-prefix>                        Environment variable prefix.\n");
                printf(
                    "    -x <path-to-the-tt-binary>             Explicitly specify the path of the tt binary file.\n");
                break;

            case -1:
                break;
        }
        break;
    }

    FILE *f;
    if (config_file_path.empty()) {
        char temp_file_path[] = "/tmp/tt.conf.XXXXXX";
        const int fd = mkstemp(temp_file_path);
        if (fd == -1) {
            std::cerr << "Failed to create temporary file" << std::endl;
            return 1;
        }
        f = fdopen(fd, "w");
        config_file_path = temp_file_path;
    } else {
        f = fopen(config_file_path.c_str(), "w");
    }

    for (const std::string &option: options) {
        std::string env_var_name = option;
        std::transform(env_var_name.begin(), env_var_name.end(), env_var_name.begin(), ::toupper);
        std::replace(env_var_name.begin(), env_var_name.end(), '.', '_');
        env_var_name.insert(0, env_prefix);
        char *env_var_value = getenv(env_var_name.c_str());
        if (env_var_value != nullptr) {
            fprintf(f, "%s = %s\n", option.c_str(), env_var_value);
        }
    }

    fclose(f);

    const int exe_argc = 2 + 2 + argc - optind;
    const auto exe_args = new char *[exe_argc];
    exe_args[0] = strdup(exe_path.c_str());
    exe_args[1] = strdup("-c");
    exe_args[2] = strdup(config_file_path.c_str());
    for (int i = optind; i < argc; i++) {
        exe_args[i - optind + 3] = strdup(argv[i]);
    }
    exe_args[3 + argc - optind] = nullptr;

    const auto err = execvp(exe_path.c_str(), exe_args);
    free(exe_args);
    perror("Exec error");
    return err;
}
