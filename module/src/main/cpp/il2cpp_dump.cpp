#include "il2cpp_dump.h"
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <fstream>
#include <unistd.h>
#include <vector>

void il2cpp_api_init(void *handle) {}

// メモリ内の文字列検索関数
uintptr_t find_pattern(uintptr_t start, uintptr_t end, const char* pattern) {
    size_t len = strlen(pattern);
    for (uintptr_t i = start; i < end - len; i++) {
        if (memcmp((void*)i, pattern, len) == 0) {
            return i;
        }
    }
    return 0;
}

void il2cpp_dump(const char *outDir) {
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return;

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        uintptr_t start, end;
        char perms[5];
        if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s", &start, &end, perms) < 3) continue;
        
        // 読み取り可能な全領域(r--)および(rw-)を検索
        if (perms[0] == 'r') {
            // "mscorlib" という文字列を検索（Unityメタデータには必ず含まれる）
            uintptr_t found = find_pattern(start, end, "mscorlib");
            if (found) {
                size_t region_size = end - start;
                // 発見！この領域全体をダンプする
                // これが 10.40MB 前後であれば、それが「復号された後の本物」です
                std::string outPath = "/sdcard/Download/FOUND_BY_STRING_" + std::to_string(count) + ".dat";
                std::ofstream out(outPath, std::ios::binary);
                if (out.is_open()) {
                    out.write((char*)start, region_size);
                    out.close();
                    count++;
                }
            }
        }
    }
    fclose(fp);
}
