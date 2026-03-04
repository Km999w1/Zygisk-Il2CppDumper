#include "il2cpp_dump.h"
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <fstream>
#include <unistd.h>

void il2cpp_api_init(void *handle) {}

void il2cpp_dump(const char *outDir) {
    // ログを出さずに静かに全メモリをスキャンします
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return;

    char line[512];
    int count = 0;

    while (fgets(line, sizeof(line), fp)) {
        uintptr_t start, end;
        char perms[5];
        if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s", &start, &end, perms) < 3) continue;
        
        // 読み取り可能な全領域(r)を対象にする
        if (perms[0] == 'r') {
            size_t region_size = end - start;
            // メモリの先頭 4バイトが「Unity標準のマジックナンバー」かチェック
            // リトルエンディアン: 0xFAB11BAF (AF B1 1B FA)
            uint32_t* magic_ptr = (uint32_t*)start;
            
            // 注意：ゲームがメモリ上で展開した後は、このサインが現れるはずです
            if (*magic_ptr == 0xFAB11BAF) {
                // 発見！これが「復号済みの本物」である可能性が高いです
                std::string outPath = "/sdcard/Download/TRUE_metadata_" + std::to_string(count) + ".dat";
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
