# Assignment 3 — Multithreaded Sensor Data Processor

Bài tập C++17 xử lý dữ liệu cảm biến đa luồng. Chương trình đọc dữ liệu từ file,
loại bỏ mẫu sai, tính thống kê, phát hiện cảnh báo và xuất báo cáo CSV. Kết quả
không thay đổi theo số lượng worker.

## 1. Build và chạy

Mở riêng thư mục Assignment3 2 bằng VS Code. Cài trình biên dịch hỗ trợ C++17.
Trên macOS/Linux dùng Terminal:

```sh
make
./build/sensor_processor data/sensor_small.txt 4
make test
bash tests/verify.sh
```

Trên Windows dùng MinGW-w64 (MSYS2 UCRT64) và thêm bin vào PATH:

```powershell
mingw32-make
.\build\sensor_processor.exe data/sensor_small.txt 4
mingw32-make test
```

Tham số thứ nhất là đường dẫn file (mặc định data/sensor_log.txt). Tham số thứ hai
là số worker (mặc định hardware_concurrency, tối thiểu 1). Sau khi hiển thị
báo cáo, nhập ID cảm biến như TEMP_01 rồi Enter; dòng trống thoát.
File report.csv được tạo tại thư mục làm việc hiện tại.

Trong VS Code: Ctrl+Shift+B chạy build; Terminal > Run Task > test chạy assert;
F5 khởi chạy debugger nếu đã cài C/C++ extension và debugger tương ứng.
Đặt breakpoint tại ThreadSafeQueue::pop hoặc SensorRegistry::add, mở Call Stack
để xem các worker. Trên Windows có cấu hình GDB cho MSYS2 UCRT64.

## 2. Các file mã nguồn

```text
Assignment3 2/
├── include/                    # Header và template
│   ├── Algorithms.h
│   ├── Errors.h
│   ├── LinkedQueue.h
│   ├── Processing.h
│   ├── RingBuffer.h
│   ├── Sensor.h
│   └── ThreadSafeQueue.h
├── src/                        # Mã nguồn chương trình
│   ├── main.cpp
│   ├── Processing.cpp
│   └── Sensor.cpp
├── data/                       # File dữ liệu đầu vào
│   ├── sensor_log.txt
│   ├── sensor_small.txt
│   ├── sensor_invalid.txt
│   ├── sensor_alarm.txt
│   ├── sensor_empty.txt
│   └── sensor_big.txt
├── tests/
│   ├── test_main.cpp
│   ├── verify.sh
│   └── expected/
│       ├── report_expected.csv
│       └── report_expected_big.csv
├── docs/
│   └── TEST_EVIDENCE.md
├── .vscode/
│   ├── launch.json
│   └── tasks.json
├── build/                      # Tự tạo khi build
├── report.csv                  # Tự tạo khi chạy
├── Makefile
└── README.md
```

| File | Chức năng |
|---|---|
| Errors.h | Error, ParseError, FileError, EmptyContainerError |
| LinkedQueue.h | Hàng đợi FIFO dùng danh sách liên kết đơn |
| RingBuffer.h | Bộ đệm vòng template, sức chứa N |
| Algorithms.h | Merge sort và binary search với comparator |
| ThreadSafeQueue.h | Hàng đợi đa luồng, chờ và đóng bằng condition_variable |
| Sensor.h / .cpp | Sample, Sensor, Group, SensorRegistry, Logger và thống kê |
| Processing.h / .cpp | Parser, producer, worker, báo cáo và tra cứu |
| main.cpp | Điểm bắt đầu chương trình |
| test_main.cpp | Kiểm thử đơn vị dùng assert |
| Makefile | Build, run, test |
| tests/verify.sh | Kiểm thử tích hợp, dùng file 100.000 dòng và so sánh CSV |

## 3. Kiến trúc

```text
                          ThreadSafeQueue<Sample>
Producer (file reader)  ->  LinkedQueue  -> Worker 1
                                        -> Worker 2  -> SensorRegistry
                                        -> Worker N       |
                                                   map<ID, shared_ptr<Sensor>>
                                                           |
                                      Group <--weak_ptr-- Sensor
                                      Group --shared_ptr-> Sensor

Main: join -> finalize -> mergeSort -> compare std::sort -> report.csv
                                          |
                                    sort by ID -> binarySearch
```

Namespace sdp chứa các thành phần chính; sdp::ds là cấu trúc dữ liệu,
sdp::algo là giải thuật và sdp::io xử lý nhập/xuất. Alias algorithms
được khai báo trong Processing.cpp. main toàn cục là ngoại lệ bắt buộc bởi C++.
