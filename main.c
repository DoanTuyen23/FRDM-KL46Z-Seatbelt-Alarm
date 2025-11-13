#include "LED.h"     // 
#include "MKL46Z4.h" // 

// Định nghĩa các trạng thái của Máy trạng thái hữu hạn 
typedef enum {
    STATE_IDLE,      // Trạng thái chờ, không có người
    STATE_WAITING,   // Có người, đang chờ 5s 
    STATE_ALARM,     // Báo động, nháy LED đỏ
    STATE_BELTED     // Đã thắt dây, sáng LED xanh
} SystemState_t;

/* =======================================================
 * BIẾN TOÀN CỤC
 * ======================================================= */
volatile uint32_t g_ms_ticks = 0;

/**
 * @brief Hàm xử lý ngắt SysTick (ISR).
 */
void SysTick_Handler(void)
{
    g_ms_ticks++; // Tăng nhịp đếm hệ thống
}

/* =======================================================
 * HÀM MAIN
 * ======================================================= */
int main(void){
    // --- KHỞI TẠO PHẦN CỨNG ---
    InitLED(); 
    InitLED2(); 
    Initsw1(); 
    Initsw3(); 
    
    // --- CẤU HÌNH SYSTICK ---
    SysTick->LOAD = SystemCoreClock/1000; 
    SysTick->CTRL = 7;   // Bật SysTick, Bật ngắt SysTick, Dùng clock CPU
    __enable_irq();       // Cho phép tất cả các ngắt (bắt buộc)
    
    // --- KHỞI TẠO BIẾN FSM ---
    // Khởi tạo máy trạng thái 
    SystemState_t currentState = STATE_IDLE; // Trạng thái ban đầu
    uint32_t gracePeriodStartTime = 0; // Mốc thời gian bắt đầu chờ 5s
    uint32_t lastBlinkTime = 0; // Mốc thời gian để nháy 1Hz

    while (1)
    {
        uint8_t coNguoiNgoi = ((PTC->PDIR & (1u << 3)) == 0); 
        uint8_t daThatDay = ((PTC->PDIR & (1u << 12)) == 0); 
        
        uint32_t now = g_ms_ticks; 
        
        switch (currentState)
        {
            // Trạng thái: Chờ, không có người
            case STATE_IDLE:
                PTE->PSOR = (1u << 29); // Tắt LED Đỏ (ghi 1)
                PTD->PSOR = (1u << 5);  // Tắt LED Xanh (ghi 1)
            
                // Kiểm tra điều kiện chuyển trạng thái
                if (coNguoiNgoi) 
                {
                    if (daThatDay)
                        currentState = STATE_BELTED; // Có người, đã thắt
                    else
                    {
                        currentState = STATE_WAITING; // Có người, chưa thắt
                        gracePeriodStartTime = now; // Bắt đầu đếm 5s
                    }
                }
                break;
                
            // Trạng thái: Có người, đang trong 5s chờ
            case STATE_WAITING:
                PTE->PSOR = (1u << 29); // Vẫn tắt LED Đỏ
                PTD->PSOR = (1u << 5);  // Vẫn tắt LED Xanh
            
                // Kiểm tra điều kiện chuyển trạng thái
                if (!coNguoiNgoi) // Người dùng đứng dậy
                    currentState = STATE_IDLE;
                else if (daThatDay) // Thắt dây trong 5s
                    currentState = STATE_BELTED;
                else if (now - gracePeriodStartTime > 5000) // Hết 5s
                {
                    currentState = STATE_ALARM; // Chuyển sang báo động
                    lastBlinkTime = now; // Đặt mốc thời gian nháy
                    PTE->PCOR = (1u << 29); // Bật LED Đỏ (ghi 0)
                }
                break;

            // Trạng thái: Báo động
            case STATE_ALARM:
                PTD->PSOR = (1u << 5); // Luôn tắt LED Xanh

                // Logic nháy 1Hz (đảo trạng thái mỗi 500ms)
                if (now - lastBlinkTime > 500)
                {
                    lastBlinkTime = now; // Ghi lại mốc thời gian
                    PTE->PTOR = (1u << 29); // Đảo (toggle) LED Đỏ
                }
            
                // Kiểm tra điều kiện chuyển trạng thái
                if (!coNguoiNgoi) // Người dùng đứng dậy
                    currentState = STATE_IDLE;
                else if (daThatDay) // Người dùng thắt dây
                    currentState = STATE_BELTED;
                break;

            // Trạng thái: An toàn (đã thắt dây)
            case STATE_BELTED:
                PTE->PSOR = (1u << 29); // Tắt LED Đỏ
                PTD->PCOR = (1u << 5);  // Bật LED Xanh (ghi 0)
            
                // Kiểm tra điều kiện chuyển trạng thái
                if (!coNguoiNgoi) // Người dùng đứng dậy
                    currentState = STATE_IDLE;
                else if (!daThatDay) // Tháo dây
                {
                    // Báo động ngay lập tức, không chờ
                    currentState = STATE_ALARM; 
                    lastBlinkTime = now;
                    PTE->PCOR = (1u << 29); // Bật LED Đỏ
                }
                break;
        } 
    } 
} 
