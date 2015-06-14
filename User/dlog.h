#ifndef OUR_UART_H
#define OUR_UART_H


#include <stdio.h>
 

/**@brief Logging function, used for formated output on the UART. */
/*
void uart_logf(const char *fmt, ...)
{
    uint16_t i = 0;
    static uint8_t logf_buf[150];
    va_list args;
    va_start(args, fmt);    
    i = vsnprintf((char*)logf_buf, sizeof(logf_buf) - 1, fmt, args);
    logf_buf[i] = 0x00; // Make sure its zero terminated
    uart_write_buf((uint8_t*)logf_buf, i);
    va_end(args);
}*/

#define USE_UART_LOG_INFO   /* Enable to print standard output to UART. */
#define USE_UART_LOG_DEBUG  /* Enable to print standard output to UART. */
#define USE_UART_LOG_DATA

#define uart_logf           printf
//void uart_logf(const char *fmt, ...);

#ifdef USE_UART_LOG_DEBUG    
    #define D_LOG(format, ...) (uart_logf("LINE:%4d, FILE:%s,\t\t" format "\r\n", __LINE__, __FILE__,  ##__VA_ARGS__))    
#else
    #define D_LOG(format, ...) (void)__NOP()
#endif

    
#ifdef USE_UART_LOG_INFO
    #define D_INFO(format, ...) (uart_logf(format "\r\n", ##__VA_ARGS__))
#else
    #define D_INFO(format, ...)(void)__NOP()
#endif
    

#ifdef USE_UART_LOG_DATA
    #define D_DATA      uart_logf
#else
    #define D_DATA(format, ...)(void)__NOP()
#endif    
    
    
    
    
    




#endif
    
    
    

    
    
    
    

