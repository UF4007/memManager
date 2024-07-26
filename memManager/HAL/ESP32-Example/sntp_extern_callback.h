#pragma once

extern "C"
{
    inline SemaphoreHandle_t sntp_time_singal = nullptr;
    inline void time_sync_notification_cb(struct timeval *tv)
    {
        if (sntp_time_singal)
            xSemaphoreGive(sntp_time_singal);
    }
}