#pragma once


#ifdef __cplusplus
extern "C"
{
#endif
    void ble_set_server_char_rec_aisr(void* aParam);
    void ble_server_char_rec_isr(char* data, int size, int offset);
#ifdef __cplusplus
}
#endif