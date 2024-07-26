/* driver library for cross-platform IoT dev.
 * ------Head-Only------, but may not, depends on the driver your need.
 * 
 * for unify variety interface provided by controllers, systems even computers.
 * 
 * it is just a skeleton now. Not suitable for the production environment unless you know this lib completely and know how to modify it.
 * it is just a skeleton now. Not suitable for the production environment unless you know this lib completely and know how to modify it.
 * 
 * Considering: use boost.asio for tcp/udp support, use boost.beast for http/ws support, use MQTT-CPP for mqtt support.
 *              use LVGL for display support
 * 
 * Licensed under the MIT License.
 * Looking forward to visiting https://github.com/UF4007/ to propose issues, pull your device driver, and make ioManager stronger and more universal.
*/
#pragma once



// choose your intend machine type in this line
//#include "freertos/FreeRTOS.h"



// includes and define
#include <stdio.h>
#include <stdint.h>
#include <cwchar>
#include <queue>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>
#include "ioManagerIncludes.h"



inline namespace io
{
    using byte_t = uint8_t;

    //error type of this lib
    enum err: bool{
        ok = false,
        failed = true,
    };

    using duration_ms = std::chrono::duration<unsigned long long,std::milli>;

    //Non-blocking sleep
    template <typename _Duration>
    inline void sleep_for(_Duration time)
    {
#if defined(IO_MULTI_THREAD)
        std::this_thread::sleep_for(time);
#else
        static_assert(!std::is_same_v<_Duration, _Duration>, "cannot find solution in this intend machine");
#endif
    }

    //Blocking precision delay
    template <typename _Duration>
    inline void delay_for(_Duration time)
    {
#if defined(ESP_PLATFORM)
        uint32_t time_ms = std::chrono::duration_cast<std::chrono::microseconds>(time).count();
        esp_rom_delay_us(time_ms);
#else
        static_assert(!std::is_same_v<_Duration, _Duration>, "cannot find solution in this intend machine");
#endif
    }

    static constexpr uint32_t noUsed = 0xffffffffu;   //denote that the class doesn't rely on specified substance to work

    struct gpioMode
    {
#if defined(ESP_PLATFORM)
        using pinMode = enum : uint8_t {
            disable = 0,
            functional = 1,
            interrupter = 2,
            iopin = 3
        };
        using pinDir = enum : std::underlying_type_t<gpio_mode_t>{
            input = GPIO_MODE_DEF_INPUT,
            output = GPIO_MODE_DEF_OUTPUT,
            both = ((GPIO_MODE_DEF_INPUT) | (GPIO_MODE_DEF_OUTPUT)),
        };
        using pinPull = enum : std::underlying_type_t<gpio_pull_mode_t> {
            PullUp = GPIO_PULLUP_ONLY,
            PullDown = GPIO_PULLDOWN_ONLY,
            PullBoth = GPIO_PULLUP_PULLDOWN,
            floating = GPIO_FLOATING
        };
        using pinRes = enum : uint8_t {
            ResUp = 1,
            ResDown = 1 << 1,
            ResBoth = 1 | 2,
            ResNone = 0
        };
#endif
    };
    template <uint32_t _Pin>
    class gpio
    {
        inline static volatile bool used = false;            // gpio is a single instance class, if a pin is used by someone, another one cannot creat it again.
    public:
        inline gpio(){
            if constexpr (_Pin == noUsed)
                return;
            assert((used == false) || !"this pin has created an instance or used it for another purpose.");
            used = true;
        }
        inline ~gpio(){
            if constexpr (_Pin == noUsed)
                return;
            used = false;
        }
        static void* operator new(size_t size) = delete;    // prohibit to allocate gpio on heap solely, restrict for reuse
        static void operator delete(void* pthis) = delete;
        inline static bool isUsing() {
            return used;
        }
    private:
#if defined(ESP_PLATFORM)
    public:
        static_assert(_Pin < gpio_num_t::GPIO_NUM_MAX || _Pin == noUsed, "Invaild pin.");
        using pinMode = gpioMode::pinMode;
        using pinDir = gpioMode::pinDir;
        using pinPull = gpioMode::pinPull;
        using pinRes = gpioMode::pinRes;
        inline static constexpr bool fulfilled = true;
        inline err open(pinMode mode, pinDir dir, pinPull pull, pinRes res)
        {
            switch(mode)
            {
                case pinMode::iopin:
                {
                    gpio_config_t io_conf;
                    io_conf.intr_type = GPIO_INTR_DISABLE;
                    io_conf.mode = (gpio_mode_t)dir;
                    io_conf.pin_bit_mask = (1ULL << _Pin);
                    io_conf.pull_down_en = (gpio_pulldown_t)((uint8_t)res & pinRes::ResDown);
                    io_conf.pull_up_en = (gpio_pullup_t)((uint8_t)res & pinRes::ResUp);
                    if (gpio_config(&io_conf))
                        return failed;
                    if (gpio_set_pull_mode((gpio_num_t)_Pin, (gpio_pull_mode_t)pull))
                        return failed;
                    return ok;
                }
                break;
                case pinMode::disable:
                {
                    gpio_config_t io_conf;
                    io_conf.intr_type = GPIO_INTR_DISABLE;
                    io_conf.mode = GPIO_MODE_DISABLE;
                    return (err)gpio_config(&io_conf);
                }
                break;
            }
        }
        inline void setLevel(bool level)
        {
            gpio_set_level((gpio_num_t)_Pin, level);
        }
        inline bool getLevel()
        {
            return gpio_get_level((gpio_num_t)_Pin);
        }
        inline err close()
        {
            gpio_config_t io_conf;
            io_conf.intr_type = GPIO_INTR_DISABLE;
            io_conf.mode = GPIO_MODE_DISABLE;
            return (err)gpio_config(&io_conf);
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    template <uint32_t _Timer>
    class timer
    {
        inline static uint8_t _memory[sizeof(timer<_Timer>)];         // timer can be reused by new and get timer pointer.
        inline static volatile bool used = false;            // timer is a single instance class, once a timer created, this program share it by pointer.
    public:
        inline timer(){
            if constexpr (_Timer == noUsed)
                return;
            assert((used == false) || !"this pin has created an instance or used it for another purpose.");
            used = true;
        }
        inline ~timer(){
            if constexpr (_Timer == noUsed)
                return;
            used = false;
        }
        inline static bool isUsing() {
            return used;
        }
        inline static void* operator new(size_t size)
        {
            return _memory;
        }
        inline static void operator delete(void* pthis)
        {
            return;
        }
    private:
#if defined(ESP_PLATFORM)
    public:
        // for an esp32 timer group as a timer
        static_assert(_Timer < timer_group_t::TIMER_GROUP_MAX || _Timer == noUsed, "Invaild timer.");
        template<uint8_t _Num>
        inline err setCounter()
        {}
        inline err open()
        {}
        inline err start()
        {}
        inline err reset()
        {}
        inline err stop()
        {}
        inline err close()
        {}
        inline static constexpr bool fulfilled = true;
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class watchDog
    {
#if (defined(ESP_PLATFORM) && defined(INC_FREERTOS_H))
    public:
        inline static constexpr bool fulfilled = true;
        inline static err set(bool isRunning, uint32_t timeout_ms)
        {
            if (isRunning)
            {
                if (esp_task_wdt_deinit())
                    return failed;
                esp_task_wdt_config_t conf;
                conf.timeout_ms = timeout_ms;
                conf.trigger_panic = isRunning;
                conf.idle_core_mask = 0;
                if (esp_task_wdt_init(&conf))
                    return failed;
                if (esp_task_wdt_add(xTaskGetCurrentTaskHandle()))
                    return failed;
                return ok;
            }
            else{
                return (err)esp_task_wdt_deinit();
            }
        }
        inline static void feed()
        {
            esp_task_wdt_reset();
        }
#else
    public:
        // this machine has no wdt, let's pretend it works well
        inline static constexpr bool fulfilled = false;
        inline static err set(bool isRunning, uint32_t timeout_ms) {return ok;}
        inline static void feed(){}
#endif
    };

    inline static uint64_t _pwmTimerUsed = 0;                    // pwm internal timer, for controllers which do not use pwm via the general timer.
    inline static uint64_t _pwmChannelUsed = 0;                  // pwm channel, pwm channel is restricted for reuse like gpio.
    template <uint32_t _Pin, uint32_t _Timer, uint32_t _Channel>
    class pwm
    {
#if defined(ESP_PLATFORM)
        gpio<_Pin> _gpioUsed;
    public:
        // pwm in esp32 does not use the general timer module.
        // if you wanna use motor controller pwm, go yourself because mcpwm includes too many relative functions that are not universal.
        // todo: make a specialized header "io-esp32.h" to add a 'advancedPwm' class fulfill the template of pwm class
        inline pwm()
        {

            assert(!(_pwmChannelUsed & 1 << _Channel) || !"this pwm channel has created an instance or used it for another purpose.");
            _pwmChannelUsed |= 1 << _Channel;
            assert(!(_pwmTimerUsed & 1 << _Timer) || !"this pwm internal timer has created an instance or used it for another purpose.");
            _pwmTimerUsed |= 1 << _Timer;
        }
        inline ~pwm()
        {
            _pwmChannelUsed &= ~(1 << _Channel);
            _pwmTimerUsed &= ~(1 << _Timer);
        }
        static void* operator new(size_t size) = delete;
        static void operator delete(void* pthis) = delete;
        static_assert(_Channel < ledc_channel_t::LEDC_CHANNEL_MAX, "Invaild pwm channel.");
        static_assert( _Timer < ledc_timer_t::LEDC_TIMER_MAX, "Invaild pwm internal timer.");
        inline err open(uint32_t freq_hz, uint32_t duty, uint32_t hpoint = 0)
        {
            ledc_timer_config_t ledc_timer = {
                .speed_mode       = LEDC_LOW_SPEED_MODE,
                .timer_num        = (ledc_timer_t)_Timer,
                .freq_hz          = freq_hz,
                .clk_cfg          = LEDC_AUTO_CLK
            };
            ledc_timer.duty_resolution = (ledc_timer_bit_t)(LEDC_TIMER_BIT_MAX - 1);
            if (ledc_timer_config(&ledc_timer))
                return failed;
            ledc_channel_config_t ledc_channel = {
                .speed_mode     = LEDC_LOW_SPEED_MODE,
                .channel        = (ledc_channel_t)_Channel,
                .timer_sel      = (ledc_timer_t)_Timer,
                .duty           = duty,
                .hpoint         = hpoint
            };
            ledc_channel.intr_type = LEDC_INTR_DISABLE;
            ledc_channel.gpio_num = _Pin;
            if (ledc_channel_config(&ledc_channel))
                return failed;
            if (ledc_fade_func_install(0))
                return failed;
            return ok;
        }
        inline err setDuty(uint32_t duty, uint32_t hpoint = 0)
        {
            return (err)ledc_set_duty_and_update(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_Channel, duty, hpoint);
        }
        inline err setFreq(uint32_t freq_hz)
        {
            return (err)ledc_set_freq(LEDC_LOW_SPEED_MODE, (ledc_timer_t)_Timer, freq_hz);
        }
        inline void close()
        {
            ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)_Channel, 0);
        }
        inline static uint32_t dutyMax()
        {
            return 1 << (LEDC_TIMER_BIT_MAX - 1);
        }
        inline static constexpr bool fulfilled = true;
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class interrupt     //immediate interrupt, must return in a transient.
    {
    public:
        using rParam = union {
            struct {
                uint8_t* bytes;
                uint32_t size;
            } network_rec_rec;
            struct {
                uint8_t* bytes;
                uint32_t size;
            } network_send_rec;
        };
        using isr = void(*)(rParam*);
    };

    class adc
    {
    };
    
    class dac
    {
    };

    class dma
    {
    };

    class uart
    {
    };

    class i2c
    {
    };

    class spi
    {
    };

    class can
    {
    };

    class camera
    {
    };

    class display
    {
    };

    // ----------------------device above use template, device below use inheritance.----------------------------------
    // ----------------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------------

    // disk, nvs or something else can storage data, especially for mcu does not support filesystem
    class nvs
    {
#if defined(_WIN32)
        HANDLE handle;

    public:
#ifdef UNICODE
        wchar_t *path;
        inline nvs(wchar_t *_path)
        {
            path = _path;
        }
#else
        char *path;
        inline nvs(char *_path)
        {
            path = _path;
        }
#endif
        inline static constexpr bool fulfilled = true;
        using openMode = enum : DWORD {
            read_only = FILE_GENERIC_READ,
            read_write = FILE_GENERIC_WRITE | FILE_GENERIC_READ
        };
        using shareMode = enum : DWORD {
            none = 0
        };
        using createMode = enum : DWORD {
            newOnly = CREATE_NEW,
            newAlways = CREATE_ALWAYS,
            openOnly = OPEN_EXISTING,
            openAlways = OPEN_ALWAYS,
            openTruncate = TRUNCATE_EXISTING
        };
        inline err open(openMode omode, shareMode smode, createMode cmode)
        {
            handle = CreateFile(path,
                                static_cast<DWORD>(omode),
                                static_cast<DWORD>(smode), NULL,
                                static_cast<DWORD>(cmode), FILE_ATTRIBUTE_NORMAL, NULL);
            return (err)!handle;
        }
        inline uint64_t get_size()
        {
            DWORD fileSizeH = 0, fileSizeL = 0;
            fileSizeL = GetFileSize(handle, &fileSizeH);
            uint64_t fileSize = fileSizeH;
            fileSize = (fileSize << 32);
            fileSize += fileSizeL;
            return fileSize;
        }
        inline void close()
        {
            CloseHandle(handle);
        }
        inline err read(void *dest, uint64_t &size)
        {
            DWORD sizeRead;
            err ret = (err)!ReadFile(handle, dest, size, &sizeRead, NULL);
            size = sizeRead;
            return ret;
        }
        inline err write(void *dest, uint64_t size)
        {
            return (err)!WriteFile(handle, dest, size, NULL, NULL);
        }
#ifdef UNICODE
        inline static err remove(wchar_t *path)
        {
            return (err)!DeleteFile(path);
        }
#else
        inline static err remove(char *path)
        {
            return (err)!DeleteFile(path);
        }
#endif
#elif defined(ESP_PLATFORM)
        nvs_handle handle;
        static inline bool isNvsInit = []{
            esp_err_t ret = nvs_flash_init();
            if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
                ESP_ERROR_CHECK(nvs_flash_erase());
                return !nvs_flash_init();
            } 
            return !ret;
            }();

    public:
        char *path;
        inline static constexpr bool fulfilled = true;
        using openMode = enum : std::underlying_type<nvs_open_mode_t>::type {
            read_only = NVS_READONLY,
            read_write = NVS_READWRITE
        };
        using shareMode = enum : uint8_t {
            none = 0
        };
        using createMode = enum : uint8_t {
            openAlways = 0
        };
        inline nvs(char *_path)
        {
            path = _path;
        }
        inline err open(openMode omode, shareMode smode, createMode cmode)
        {
            return (err)nvs_open(path, static_cast<nvs_open_mode_t>(omode), &handle);
        }
        inline void close()
        {
            nvs_close(handle);
        }
        inline uint64_t get_size()
        {
            uint64_t ret = 0;
            nvs_get_u64(handle, "s", &ret);
            return ret;
        }
        inline err read(void *dest, uint64_t &size)
        {
            size_t sizet = size;
            err ret = (err)nvs_get_blob(handle, "c", dest, &sizet);
            size = sizet;
            return ret;
        }
        inline err write(void *dest, uint64_t size)
        {
            err ret;
            if ((ret = (err)nvs_set_u64(handle, "s", size)) != ESP_OK)
                return ret;
            return (err)nvs_set_blob(handle, "c", dest, size);
        }
        inline static err remove(char *path)
        {
            nvs_handle handle2; // for behavior same as WIN API
            err ret = (err)nvs_open(path, NVS_READWRITE, &handle2);
            if (ret != ok)
                return ret;
            ret = (err)nvs_erase_all(handle2);
            nvs_close(handle2);
            return ret;
        }
#elif __has_include(<filesystem>)
    //todo with std::filesystem
    public:
        inline static constexpr bool fulfilled = false;
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    // for some mcu doesn't support freertos or std::thread, perhaps consider interrupt, or forbidden multi thread.
    class thread
    {
        // when the thread blocks over time, it triggers.
    };

    class mutex
    {
#if defined(_WIN32)
        HANDLE handle;

    public:
        inline static constexpr bool fulfilled = true;
        inline static constexpr uint32_t infinity = INFINITE;
        using retType = enum : DWORD {
            success = WAIT_OBJECT_0,
            abandon = WAIT_ABANDONED,
            timeout = WAIT_TIMEOUT,
            failed = WAIT_FAILED
        };
        inline mutex()
        {
            handle = CreateMutex(nullptr, NULL, nullptr);
        }
        inline ~mutex()
        {
            CloseHandle(handle);
        }
        inline retType wait(uint32_t time)
        {
            return static_cast<retType>(WaitForSingleObject(handle, time));
        }
        inline void release()
        {
            ReleaseMutex(handle);
        }
#elif defined(INC_FREERTOS_H)
        SemaphoreHandle_t handle;

    public:
        inline static constexpr bool fulfilled = true;
        inline static constexpr uint32_t infinity = portMAX_DELAY;
        using retType = enum : BaseType_t {
            success = pdTRUE,
            timeout = pdFALSE
        };
        using statusType = enum{
            idle,
            busy
        };
        inline mutex()
        {
            handle = xSemaphoreCreateMutex();
        }
        inline ~mutex()
        {
            vSemaphoreDelete(handle);
        }
        inline retType wait(uint32_t time_ms)
        {
            if (time_ms == infinity)
                return static_cast<retType>(xSemaphoreTake(handle, infinity));
            return static_cast<retType>(xSemaphoreTake(handle, time_ms / portTICK_PERIOD_MS));
        }
        inline void release()
        {
            xSemaphoreGive(handle);
        }
#elif defined(IO_MULTI_THREAD)
    //todo with std::mutex
    public:
        inline static constexpr bool fulfilled = true;
#else
    // intend machine has no multi-thread, let's pretend mutex works well
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class singal    //binary singal
    {
#if defined(_WIN32)
#elif defined(INC_FREERTOS_H)
        friend class sntp_cilent;
        friend class wifi_sta;
        friend class netif;
        SemaphoreHandle_t handle;

    public:
        inline static constexpr bool fulfilled = true;
        inline static constexpr uint32_t infinity = portMAX_DELAY;
        using retType = enum : BaseType_t {
            success = pdTRUE,
            timeout = pdFALSE
        };
        using statusType = enum : BaseType_t {
            idle = pdTRUE,
            occurred = pdFALSE
        };
        inline singal()
        {
            handle = xSemaphoreCreateBinary();
        }
        inline ~singal()
        {
            vSemaphoreDelete(handle);
        }
        inline retType wait(uint32_t time_ms)
        {
            if (time_ms == infinity)
                return static_cast<retType>(xSemaphoreTake(handle, infinity));
            return static_cast<retType>(xSemaphoreTake(handle, time_ms / portTICK_PERIOD_MS));
        }
        inline void send()
        {
            xSemaphoreGive(handle);
        }
        inline statusType getStatus()
        {
            if(xSemaphoreTake(handle, 0) == pdTRUE)
                return idle;
            return occurred;
        }
#elif defined(IO_MULTI_THREAD)
    //todo with std::condition_variables
    public:
        inline static constexpr bool fulfilled = true;
#else
    // intend machine has no multi-thread, let's pretend singal works well
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class multi_singal
    {};

    class atom
    {
#if defined(_WIN32)
#elif defined(INC_FREERTOS_H)
#elif defined(IO_MULTI_THREAD)
    //todo with std::mutex
    public:
        inline static constexpr bool fulfilled = true;
#else
    // intend machine has no multi-thread, let's pretend atom works well
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class async
    {
    };

    class pData
    {
        friend class ainterrupt;
        byte_t* _data;
        size_t _size;

    public:
        inline pData()
        {
            _data = nullptr;
            _size = 0;
        }
        inline pData(byte_t *rawData, size_t rawSize)
        {
            if (rawData && _data)
            {
                _size = rawSize;
                _data = (byte_t*)malloc(rawSize);
                memcpy(_data, rawData, rawSize);
            }
            else
            {
                _data = nullptr;
                _size = 0;
            }
        }
        inline void release() {
            if (_data)
                free(_data);
        }
        inline size_t getSize() { return _size; }
        inline byte_t* data(){ return _data; }
    };

    template <typename _Message>                //not only kind of data structure, it also blocks the thread.
    class queue
    {
#if defined(_WIN32)
#elif defined(INC_FREERTOS_H)
        QueueHandle_t xQueue;
    public:
        inline static constexpr bool fulfilled = true;
        inline queue(size_t queueLength = 10)
        {
            xQueue = xQueueCreate(queueLength, sizeof(_Message));
        }
        inline ~queue()
        {
            vQueueDelete(xQueue);
        }
        inline err postMessage(_Message _msg)
        {
            if (xQueueSend(xQueue, &_msg, portMAX_DELAY) == pdPASS)
            {
                return err::ok;
            }
            else
            {
                return err::failed;
            }
        }
        inline err getMessage(_Message &_msg_out)
        {
            if (xQueueReceive(xQueue, &_msg_out, portMAX_DELAY) == pdPASS)
            {
                return err::ok;
            }
            else
            {
                return err::failed;
            }
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class ainterrupt                            //asynchrono interrupt, driven by a main thread.
    {
    public:
        using aParam = struct {
            pData datas;
            uint32_t offset;
        };
        using aisr = void(*)(aParam);
    private:
        struct aitrpt_msg_content {
            aisr cb_func;
            aParam aparam;
        };
#if defined(_WIN32)
#elif defined(INC_FREERTOS_H)
        inline static void _thread(void*)
        {
            aitrpt_msg_content msg;
            while (1)
            {
                _threadQueue.getMessage(msg);
                if (msg.cb_func)
                    msg.cb_func(msg.aparam);
                msg.aparam.datas.release();
            }
        }
        bool isThreadInited = false;
        inline static io::queue<aitrpt_msg_content> _threadQueue;
    public:
        inline static constexpr bool fulfilled = true;
        inline static void init()
        {
            new std::thread(_thread, nullptr);
            //xTaskCreate(_thread, "ainterrupt task", 40960, NULL, 1, NULL);
        }
        inline static err send(aisr cbfunc, aParam para)
        {
            aitrpt_msg_content msg;
            msg.cb_func = cbfunc;
            msg.aparam = para;
            if (_threadQueue.postMessage(msg) == failed)
                return failed;
            else
                return ok;
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    // -----------------device above will block the current thread until user func execute complete.-------------------
    // manually use async is recommended, or just wait until overtime

    // abstract class, network interface, in a certain way to provide a stable network layer link.
    class netif
    {
#if defined(ESP_PLATFORM)
        friend class sntp_cilent;
        friend class wifi_sta;
        bool _is_this_created = false;          //is esp_netif_t created by this?
        static inline bool _is_init_netif = false;
        esp_netif_t *handle;
        esp_ping_handle_t ping_handle = nullptr;
        singal cb_singal;
    public:
        inline netif()
        {
            if(false == _is_init_netif)
                esp_netif_init();
        }
        inline ~netif()
        {
            esp_ping_delete_session(ping_handle);
        }
        inline static constexpr bool fulfilled = true;
        inline err ping(const char *test_host, int &delay, int timeout)
        {
            if (ping_handle == nullptr)
            {
                esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();
                ping_config.count = 1;
                ping_config.timeout_ms = timeout;
                ping_config.interval_ms = timeout;
                ping_config.interface = esp_netif_get_netif_impl_index(handle);
                esp_ping_callbacks_t cbs = {
                    .cb_args = nullptr,
                    .on_ping_success = on_ping_success_cb,
                    .on_ping_timeout = nullptr,
                    .on_ping_end = nullptr,
                };

                netif_ping_singal = cb_singal.handle;
                ip4_addr_t target_addr;
                inet_pton(AF_INET, test_host, &target_addr);
                ping_target_id_t targ = PING_TARGET_IP_ADDRESS;
                esp_ping_set_target(targ, &target_addr, sizeof(target_addr));

                ping_config.target_addr.u_addr.ip4 = target_addr;
                ping_config.target_addr.type = IPADDR_TYPE_V4;
                if (esp_ping_new_session(&ping_config, &cbs, &ping_handle) != ESP_OK)
                {
                    ping_handle = nullptr;
                    return failed;
                }
            }

            esp_ping_start(ping_handle);
            cb_singal.wait(timeout);
            esp_ping_stop(ping_handle);

            if (is_ping_successed)
            {
                is_ping_successed = false;
                delay = netif_ping_delay;
                return ok;
            }
            return failed;
        }
        // does your network way link to the Internet?
        inline err internetLinkTest(int &avrDelay, int timeout)
        {
            const char *test_hosts[] = {
                "119.29.29.29",    // DNSPod Public DNS
                "182.254.116.116", // DNSPod Public DNS
                "223.5.5.5",       // AliDNS
                "223.6.6.6",       // AliDNS
                "180.76.76.76",    // Baidu Public DNS
                "101.226.4.6",     // China Telecom DNS
                "123.125.81.6"     // Baidu
            };
            const int num_hosts = sizeof(test_hosts) / sizeof(test_hosts[0]);
            uint64_t delaySum = 0;
            int successfulPings = 0;
            int delay = 0;

            for (int i = 0; i < num_hosts; i++)
            {
                if (ping(test_hosts[i], delay, timeout) == ok)
                {
                    delaySum += delay;
                    successfulPings++;
                    delay = 0;
                }
                watchDog::feed();
            }

            if (successfulPings > 0)
            {
                avrDelay = delaySum / successfulPings;
                return ok;
            }
            else
            {
                return failed;
            }
        }
        // build a link to a certain custom network.
        inline err custom_buildNetwork(interrupt::isr sendIsr, interrupt::isr recIsr){}
        inline err custom_sendISR(uint8_t* bytes, size_t length)
        {
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    // asymmetric encryption by RSA algorithm
    class rsa
    {};

    class bluetooth_device
    {
#if defined(ESP_PLATFORM)
    protected:
        inline static uint _useCount = 0;
        inline static void global_bt_init()
        {
            esp_err_t ret;
            ESP_ERROR_CHECK(
                esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

            esp_bt_controller_config_t bt_cfg =
                BT_CONTROLLER_INIT_CONFIG_DEFAULT();
            ret = esp_bt_controller_init(&bt_cfg);
            if (ret)
            {
                ESP_LOGE("GATTS_TAG", "%s initialize controller failed", __func__);
                return;
            }

            ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
            if (ret)
            {
                ESP_LOGE("GATTS_TAG", "%s enable controller failed", __func__);
                return;
            }

            ret = esp_bluedroid_init();
            if (ret)
            {
                ESP_LOGE("GATTS_TAG", "%s init bluetooth failed", __func__);
                return;
            }
            ret = esp_bluedroid_enable();
            if (ret)
            {
                ESP_LOGE("GATTS_TAG", "%s enable bluetooth failed", __func__);
                return;
            }
        }
        inline static void global_bt_deinit() 
        {
            esp_bluedroid_deinit();
            esp_bt_controller_deinit();
        }
    public:
        inline static constexpr bool fulfilled = true;
        inline bluetooth_device()
        {
            if (_useCount == 0)
            {
                global_bt_init();
            }
            _useCount++;
        }
        inline ~bluetooth_device()
        {
            if (_useCount == 1)
            {
                global_bt_deinit();
            }
            _useCount--;
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class bluetooth_cilent : public bluetooth_device
    {
    };

    class bluetooth_server : public bluetooth_device
    {
#if defined(ESP_PLATFORM)
    public:
        inline static constexpr bool fulfilled = true;
        inline bluetooth_server()
        {
        }
        inline ~bluetooth_server()
        {
            this->close();
        }
        // todo: (Blocking) The call thread will hang up till the server open complete
        inline err open(char* deviceName)
        {
            return (err)ble_example_init(deviceName);
        }
        inline err addService()
        {
            return ok;
        }
        inline err addChar(ainterrupt::aisr func)
        {
            ble_set_server_char_rec_aisr((void*)func);
            return ok;
        }
        inline err sendCharNotify()
        {}
        inline err setCharReply()
        {}
        inline err removeChar()
        {}
        inline err removeService()
        {}
        inline err getMac(uint8_t(& outp)[6])
        {
            auto macScr = esp_bt_dev_get_address();
            if (macScr == nullptr)
                return failed;
            memcpy(&outp, macScr, sizeof(uint8_t) * 6);
            return ok;
        }
        inline void close()
        {
            ble_server_close();
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class wifi_device
    {
    public:
        using adConfig = struct {
            char SSID[32];
        };
        using linkConfig = struct {
            char SSID[32];
            char password[64];
        };
#if defined(ESP_PLATFORM)
    protected:
        inline static uint _useCount = 0;
        inline static void global_wifi_init()
        {
            esp_err_t ret = esp_event_loop_create_default();
            ESP_ERROR_CHECK(ret);
            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ret = esp_wifi_init(&cfg);
            ESP_ERROR_CHECK(ret);
        }
        inline static void global_wifi_deinit()
        {
            esp_wifi_deinit();
            esp_event_loop_delete_default();
        }

    public:
        inline static constexpr bool fulfilled = true;
        inline wifi_device()
        {
            if (_useCount == 0)
            {
                global_wifi_init();
            }
            _useCount++;
        }
        inline ~wifi_device()
        {
            if (_useCount == 1)
            {
                global_wifi_deinit();
            }
            _useCount--;
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class wifi_sta :public wifi_device
    {
#if defined(ESP_PLATFORM)
        int advertising_count = 0;
        int linkSuccessful = false;
    public:
        inline static constexpr bool fulfilled = true;
        duration_ms overtime_t = std::chrono::seconds(20);
        singal overtime_s;
        inline wifi_sta()
        {
            esp_netif_create_default_wifi_sta();
            esp_wifi_set_mode(WIFI_MODE_STA);
            wifi_example_regist_cb();
        }
        inline ~wifi_sta()
        {}
        inline err open()
        {
            return (err)esp_wifi_start();
        }
        inline err getAdvertisingBegin(wifi_device::adConfig &ad)
        {
        }
        inline err getAdvertisingContinue(wifi_device::adConfig& ad)
        {
        }
        inline err tryConnect(wifi_device::linkConfig &conf, netif &network_out)
        {
            wifi_config_t wifi_cfg = {};
            memcpy(wifi_cfg.sta.ssid, conf.SSID, strlen(conf.SSID));
            memcpy(wifi_cfg.sta.password, conf.password, strlen(conf.password));
            err ret = (err)esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
            if (ret != ok)
                return ret;

            wifi_example_on_connected(overtime_s.handle, &linkSuccessful);
            if (linkSuccessful)
                esp_wifi_disconnect();
            overtime_s.wait(std::chrono::milliseconds(100).count());
            ret = (err)esp_wifi_connect();
            if (ret == ok)
            {
                watchDog::feed();
                overtime_s.wait(overtime_t.count());
                if (linkSuccessful)
                {
                    network_out.handle = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
                    return ok;
                }
                else
                {
                    return failed;
                }
            }
            return ret;
        }
        inline err disconnect()
        {
            return (err)esp_wifi_disconnect();
        }
        inline bool isConnected(){ return linkSuccessful; }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class wifi_ap :public wifi_device
    {
    };

    class lte
    {};

    class ethernet
    {
    };

    // ----------------------config and access certain network link first before run things below.----------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------
    // -----------------------------------------------------------------------------------------------------------------------------

    // common protocol
    class tcp_prot
    {};

    class tcp_cilent
    {
    };
    
    class tcp_server
    {
    };

    class udp_prot
    {};

    class udp_cilent
    {
    };
    
    class udp_server
    {
    };

    class sntp_cilent                   // usually, we just need one sntp cilent.
    {
#if defined(ESP_PLATFORM)
        inline static singal cb_singal;
        inline static bool inited = false;
    public:
        inline static constexpr bool fulfilled = true;
        inline static err initService(netif &_netif, const char *ntp_server, uint32_t overtime)
        {
            if (inited == false)
            {
                sntp_setoperatingmode(SNTP_OPMODE_POLL);
                sntp_time_singal = cb_singal.handle;
                sntp_set_time_sync_notification_cb(time_sync_notification_cb);
                sntp_init();
                setenv("TZ", "CST-8", 1);
                tzset();
                inited = true;
                sntp_setservername(0, ntp_server);
                // sntp_set_netif_index(esp_netif_get_netif_impl_index(_netif.handle));
                if (cb_singal.wait(overtime) == singal::success)
                {
                    return ok;
                }
                else
                {
                    return failed;
                }
            }
            return ok;
        }
        inline static void getTime(struct tm &output)
        {
            time_t now;
            time(&now);
            localtime_r(&now, &output);
        }
#else
    public:
        inline static constexpr bool fulfilled = false;
#endif
    };

    class http_prot
    {};

    class http_cilent
    {
    };
    
    class http_server
    {
    };

    class https_prot : public http_prot
    {};

    class https_cilent
    {
    };
    
    class https_server
    {
    };

    class mqtt_prot
    {};

    class mqtt_cilent
    {
    };

    class mqtt_server
    {
    };

    class ota_cilent
    {
    };

    class ota_server
    {
    };



    //demo and trial code
#ifdef IO_DEBUG
    inline void testmain()
    {
        using test_item = enum{
            test_gpio,
            test_pwm,
            test_btserver       //bluetooth
        };
        test_item test_i = test_btserver;
        watchDog::set(true, 5000);
        switch (test_i)
        {
            case test_gpio:
            {
                constexpr uint32_t gpioNum = 12;
                if (io::gpio<gpioNum>::fulfilled)
                {
                    io::gpio<gpioNum> g;
                    std::cout << "iotest" << "gpio inited \n";
                    g.open(io::gpioMode::iopin, io::gpioMode::both, io::gpioMode::PullBoth, io::gpioMode::ResNone);
                    while(1)
                    {
                        g.setLevel(!g.getLevel());
                        std::cout << "iotest" << "gpio set:%d \n" << g.getLevel();
                        io::delay_for(std::chrono::seconds(1));
                        g.setLevel(!g.getLevel());
                        std::cout << "iotest" << "gpio set:%d \n" << g.getLevel();
                        io::sleep_for(std::chrono::seconds(1));
                        watchDog::feed();
                    }
                }
            }
            break;

            case test_pwm:
            {
                constexpr uint32_t pinNum = 12;
                constexpr uint32_t timerNum = 0;
                constexpr uint32_t channelNum = 0;
                watchDog::set(true, 10000);
                if (io::pwm<pinNum, timerNum, channelNum>::fulfilled)
                {
                    io::pwm<pinNum, timerNum, channelNum> p;
                    std::cout << "iotest" << "pwm inited \n";
                    p.open(1000, p.dutyMax() / 2);
                    while(1)
                    {
                        for (uint i = 0; i < p.dutyMax(); i++)
                        {
                            p.setDuty(p.dutyMax() - i);
                            io::delay_for(std::chrono::milliseconds(1));
                            watchDog::feed();
                        }
                    }
                }
            }
            break;

            case test_btserver:
            {
                io::bluetooth_server bts;
                uint8_t mac[6];
                bts.getMac(mac);
                std::string deviceName = "LED_";
                std::stringstream ss;
                ss << std::hex << std::uppercase << std::setfill('0');
                for (size_t i = 0; i < 6; ++i) {
                    std::cout << "mac " << i  << " : " << static_cast<unsigned>(mac[i]) << "\n";
                    ss << std::setw(2) << static_cast<unsigned>(mac[i]);
                }
                deviceName += ss.str();
                bts.open(deviceName.data());
                while(1)
                {
                    io::sleep_for(std::chrono::seconds(1));
                    watchDog::feed();
                }
            }
            break;
        }
    }
#endif
}