#include <avr/io.h>

enum USART_MODES { USART_ASYNC_MODE, USART_SYNC_MODE,
	USART_MASTER_SPI_MODE };

enum USART_PARITY_MODES { USART_PARITY_OFF_MODE,
	USART_EVEN_PARITY_MODE, USART_ODD_PARITY_MODE };

enum USART_BIT_MODES { USART_5_BIT_MODE, USART_6_BIT_MODE,
	USART_7_BIT_MODE, USART_8_BIT_MODE, USART_9_BIT_MODE };
