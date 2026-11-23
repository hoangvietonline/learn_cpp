# Minh chứng kiểm thử Assignment 3

Ngày chạy: 19/11/2026. Môi trường macOS, C++17, Makefile.
Biên dịch bằng g++ với -Wall -Wextra -g -O0 -pthread, không có cảnh báo.

| Kiểm thử | Kết quả quan sát | Trạng thái |
|---|---|---|
| Unit tests | All unit tests passed | PASS |
| TC-01 | Small N=1, CSV khớp expected | PASS |
| TC-02 | Small N=2,4,8, CSV khớp từng byte | PASS |
| TC-03 | 6 dòng, 1 valid, 5 invalid, log số dòng | PASS |
| TC-04 | Thiếu file, Cannot open input file và exit code khác 0 | PASS |
| TC-05 | 3 cảnh báo, đúng ID và timestamp | PASS |
| TC-06 | 100.000 dòng N=8, 5 lần khớp expected từng byte | PASS |
| TC-07 | TEMP_01 tìm thấy, UNKNOWN không tìm thấy | PASS |
| TC-08 | File rỗng, tổng 0, valid 0, invalid 0 | PASS |

## Kết quả thực tế từ verify.sh

```text
All unit tests passed
Small N=1: PASS
Small N=2: PASS
Small N=4: PASS
Small N=8: PASS
Invalid: PASS
Missing file: PASS
Alarms: PASS
Big run 1: PASS (Workers: 8, processing time (ms): 951)
Big run 2: PASS (Workers: 8, processing time (ms): 1115)
Big run 3: PASS (Workers: 8, processing time (ms): 917)
Big run 4: PASS (Workers: 8, processing time (ms): 1179)
Big run 5: PASS (Workers: 8, processing time (ms): 1073)
Search: PASS
Empty input: PASS
```

## Báo cáo mẫu

```csv
id,type,group,count,min,max,average,moving_average,alarms
SPD_01,SPEED,SPEED,2,64.00,80.00,72.00,72.00,0
TEMP_01,TEMP,TEMP,2,36.50,40.50,38.50,38.50,0
VOLT_01,VOLT,VOLT,2,12.10,13.90,13.00,13.00,0
total,valid,invalid
6,6,0
```

Chưa kiểm tra trực quan Call Stack trong VS Code và chưa chạy
ThreadSanitizer/AddressSanitizer; đây là bước kiểm tra bổ sung.
