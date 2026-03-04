#include "il2cpp_dump.h"
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <fstream>
#include <unistd.h>

void il2cpp_api_init(void *handle) {}

void il2cpp_dump(const char *outDir) {
    // あなたのメタデータ 10.40 MB に合わせ、少し広めの範囲（9MB〜12MB）を探します
    const size_t target_min = 9 * 1024 * 1024;
    const size_t target_max = 12 * 1024 * 1024;

    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return;

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        uintptr_t start, end;
        char perms[5];
        if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s", &start, &end, perms) < 3) continue;
        
        size_t region_size = end - start;
        // 読み取り可能(r)で、サイズが10.40MBに近い領域をすべて抽出
        if (perms[0] == 'r' && region_size >= target_min && region_size <= target_max) {
            
            // 保存先を「ダウンロードフォルダ」に固定（Root不要で見れる場所）
            std::string outPath = "/sdcard/Download/global-metadata_" + std::to_string(count) + ".dat";
            std::ofstream out(outPath, std::ios::binary);
            if (out.is_open()) {
                out.write((char*)start, region_size);
                out.close();
                count++;
            }
        }
    }
    fclose(fp);
}
