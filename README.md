# 🚨 Dự án Hệ thống Cảnh báo Thắt dây An toàn (FRDM-KL46Z)

Đây là dự án cho môn học Lập trình nhúng và thời gian thực, thực hiện chức năng cảnh báo thắt dây an toàn trên bo mạch FRDM-KL46Z.

## 📝 Chức năng chính

Hệ thống sử dụng 2 nút bấm (SW1, SW3) và 2 LED (Đỏ, Xanh) để mô phỏng 4 kịch bản logic:
1.  **IDLE:** Không có người, đèn tắt.
2.  **WAITING:** Có người, chưa thắt dây. Chờ 5 giây.
3.  **ALARM:** Hết 5 giây hoặc tháo dây. LED Đỏ nháy 1Hz.
4.  **BELTED:** Có người và đã thắt dây. LED Xanh sáng.

## 🛠️ Phần cứng sử dụng

* Bo mạch: **FRDM-KL46Z**
* Cảm biến (Giả lập): **SW1** (PTC3) và **SW3** (PTC12)
* Hiển thị (Giả lập): **LED Đỏ** (PTE29) và **LED Xanh** (PTD5)

## 📁 Cấu trúc Code

Dự án sử dụng phương pháp "lai" (Ngắt SysTick + Hỏi vòng) với Máy trạng thái hữu hạn (FSM).

* `main.c`: Chứa hàm `main()`, `SysTick_Handler()`, và toàn bộ logic Máy trạng thái (FSM).
* `LED.c`: Chứa các hàm khởi tạo phần cứng `InitLED()`, `Initsw1()`, v.v.
* `LED.h`: File header chứa các khai báo chung.
