#include "LED.h"
#include "MKL46Z4.h"   

// Ðinh nghia các trang thái
typedef enum {
    STATE_IDLE, STATE_WAITING, STATE_ALARM, STATE_BELTED
} SystemState_t;

/* =======================================================
 * BIEN TOÀN CUC
 * ======================================================= */
volatile uint32_t g_ms_ticks = 0;

void SysTick_Handler(void)
{
    g_ms_ticks++;
}

int main(void){
	// Khoi tao phan cung
	InitLED();
	InitLED2();
	Initsw1();
	Initsw3();
	
	// Cài dat và bat SysTick (1ms)
	SysTick->LOAD = SystemCoreClock/1000;
	SysTick->CTRL = 7;// Bat timer, Bat ngat, Dùng clock CPU
	__enable_irq();
	
	// Bien cho máy trang thái
	SystemState_t currentState = STATE_IDLE;
	uint32_t gracePeriodStartTime = 0;
  uint32_t lastBlinkTime = 0;
	
	while (1)
	{
		uint8_t coNguoiNgoi = ((PTC->PDIR & (1u << 3)) == 0); 
    uint8_t daThatDay = ((PTC->PDIR & (1u << 12)) == 0); 
		
		uint32_t now = g_ms_ticks;
		
		switch (currentState)
        {
            case STATE_IDLE:
                PTE->PSOR = (1u << 29); // tat do
                PTD->PSOR = (1u << 5);  //tat xanh
            
                if (coNguoiNgoi) 
                {
                    if (daThatDay)
                        currentState = STATE_BELTED; //chuyen sang xanh
                    else
                    {
                        currentState = STATE_WAITING; //chuyen sang trang thai cho
                        gracePeriodStartTime = now;   // Ghi moc thoi gian
                    }
                }
                break;
                
            case STATE_WAITING:
                PTE->PSOR = (1u << 29); // Tat do
                PTD->PSOR = (1u << 5);  // Tat Xanh
            
                if (!coNguoiNgoi) // Ðung day
                    currentState = STATE_IDLE;
                else if (daThatDay) // That dây trong 5s
                    currentState = STATE_BELTED;
                else if (now - gracePeriodStartTime > 5000) // Het 5s
                {
                    currentState = STATE_ALARM; // Chuyen sang Báo dong
                    lastBlinkTime = now; // Reset moc nháy
                    PTE->PCOR = (1u << 29); // BaT LED do (0=ON)
                }
                break;

            case STATE_ALARM:
                PTD->PSOR = (1u << 5); // Tat Xanh

                if (now - lastBlinkTime > 500) // Ðã do 500ms
                {
                    lastBlinkTime = now; // Ghi lai moc thoi gian
                    PTE->PTOR = (1u << 29); // Ðao LED do
                }
            
                if (!coNguoiNgoi) // Ðung day
                    currentState = STATE_IDLE;
                else if (daThatDay) // That dây
                    currentState = STATE_BELTED;
                break;

            case STATE_BELTED:
                PTE->PSOR = (1u << 29); // Tat Ðo
                PTD->PCOR = (1u << 5);  // Bat Xanh (0=ON)
            
                if (!coNguoiNgoi) // Ra khoi xe
                    currentState = STATE_IDLE;
                else if (!daThatDay) // Tháo dây
                {
                    currentState = STATE_ALARM; // Báo dong ngay
                    lastBlinkTime = now; // Reset moc nháy
                    PTE->PCOR = (1u << 29); // BaT LED do (0=ON)
                }
                break;
        }
	}
}

