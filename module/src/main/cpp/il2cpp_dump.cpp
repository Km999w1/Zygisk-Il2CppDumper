#include "il2cpp_dump.h"
#include <dlfcn.h>
#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include "xdl.h"
#include "log.h"

// ログ出力を無効化して検知リスクを最小限にする
#undef LOGI
#define LOGI(...) 

void il2cpp_api_init(void *handle) {
    // APIの初期化が必要な場合のみ残しますが、バイナリダンプのみなら空でも動作します
}

void il2cpp_dump(const char *outDir) {
    // あなたが教えてくれた 10.40 MB (約10,905,190バイト) に基づいた設定
    // 10MB以上、11MB以下のメモリ領域をターゲットにします
    const size_t target_min = 10 * 1024 * 1024;
    const size_t target_max = 11 * 1024 * 1024;

    // メモリの地図を開く
    FILE* fp = fopen("/proc/self/maps", "r");
    if (!fp) return;

    char line[512];
    bool dumped = false;

    while (fgets(line, sizeof(line), fp)) {
        uintptr_t start, end;
        char perms[5];
        if (sscanf(line, "%" PRIxPTR "-%" PRIxPTR " %4s", &start, &end, perms) < 3) continue;
        
        // 読み取り可能な領域 (r) かつ、サイズが 10.40 MB 近辺のものを探す
        size_t region_size = end - start;
        if (perms[0] == 'r' && region_size >= target_min && region_size <= target_max) {
            
            // メモリの先頭 4バイトがメタデータのマジックナンバー (0xAFB11BAF) か確認
            // リトルエンディアンのため 0xFAB11BAF をチェック
            uint32_t* magic_ptr = (uint32_t*)start;
            if (*magic_ptr == 0xFAB11BAF) {
                
                // 発見！検知される前に一気にファイルへ書き出す
                std::string outPath = std::string(outDir) + "/files/global-metadata.dat";
                std::ofstream out(outPath, std::ios::binary);
                if (out.is_open()) {
                    out.write((char*)start, region_size);
                    out.close();
                    dumped = true;
                    break; // 抜き出したら即座にループを抜ける
                }
            }
        }
    }
    fclose(fp);

    // 抽出に成功したかに関わらず、これ以上は何もせず終了（検知を避けるため）
}
