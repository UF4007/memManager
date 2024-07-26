#include "../../ioManager.h"

static io::ainterrupt::aisr ble_server_char_rec_func = nullptr;

extern "C"
{
    void ble_set_server_char_rec_aisr(void* func)
    {
        ble_server_char_rec_func = (io::ainterrupt::aisr)func;
    }
    void ble_server_char_rec_isr(char* data, int size, int offset)
    {
        io::ainterrupt::aParam para;
        para.datas = pData((io::byte_t*)data, size + 1);
        para.offset = offset;
        io::ainterrupt::send((io::ainterrupt::aisr)ble_server_char_rec_func, para);
    }
}