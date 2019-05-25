/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* device.h includes */
#include "mt7687.h"
#include "system_mt7687.h"
#include "hal.h"

#include "sys_init.h"
#include "task_def.h"

#include "syslog.h"
#include "wifi_api.h"
#include "wifi_lwip_helper.h"
#include "httpclient.h"


#include "xfota_download_interface.h"
#include "fota.h"


#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */


#define WIFI_SSID                ("Sam&Betty")
#define WIFI_PASSWORD            ("0928666624")

#define BUF_SIZE        (1024 * 1)

// #define HTTPS_GET_URL   "https://raw.githubusercontent.com/pc0808f/mt7697_https_fota/master/image.bin"
#define HTTPS_GET_URL       "https://philio-tech.com:8088/fota.img"

#define BAIDUIE_CER                                                     \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIDtzCCAp+gAwIBAgIJAP2aliOjtukUMA0GCSqGSIb3DQEBCwUAMHExCzAJBgNV\r\n" \
"BAYTAlRXMRMwEQYDVQQIDApTb21lLVN0YXRlMQ8wDQYDVQQHDAZUYWlwZWkxDzAN\r\n" \
"BgNVBAoMBlBoaWxpbzELMAkGA1UECwwCUkQxDDAKBgNVBAMMA0I4NTEQMA4GCSqG\r\n" \
"SIb3DQEJARYBeDAgFw0xOTA1MTcwNTAzMDhaGA8yMTE5MDQyMzA1MDMwOFowcTEL\r\n" \
"MAkGA1UEBhMCVFcxEzARBgNVBAgMClNvbWUtU3RhdGUxDzANBgNVBAcMBlRhaXBl\r\n" \
"aTEPMA0GA1UECgwGUGhpbGlvMQswCQYDVQQLDAJSRDEMMAoGA1UEAwwDQjg1MRAw\r\n" \
"DgYJKoZIhvcNAQkBFgF4MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\r\n" \
"1lT5fYe6wh7Q6W+6uz9luAqbOaXAQDkTuPEdhqne4UmSgx4qO6dtAYXszGqEx20M\r\n" \
"e6+gJtQQTlerJEOYJWG+osn+DzvzGtoNXtOZkolgrih2dw+9FJpnjEJqUU9zlsIb\r\n" \
"gRjK52y95nkNYE3bBI+OyfQbygLA8X4hlvDg5QH3S73ZTx8mxZ9JjEkoLBK4z/3A\r\n" \
"zYiWS8o3+NZNccKez9hA8XLfJEde6T3R/fVHfA6oZoZHZs+hoAJOqY4yOlxL9sQq\r\n" \
"NY1aWn0Lft+O6AXgk7urhNFK+b+VjefZHHhZWDuToWlsFqa7I3opvVzcnt8rDXCg\r\n" \
"pRfhHq88Ut2OQPUk+QRdUwIDAQABo1AwTjAdBgNVHQ4EFgQUE0mZh2mnxE9xtVJM\r\n" \
"SYMIgBF5tUgwHwYDVR0jBBgwFoAUE0mZh2mnxE9xtVJMSYMIgBF5tUgwDAYDVR0T\r\n" \
"BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAH3nKzhcI0j9/nJzA6RPsIism9F+j\r\n" \
"lMkT4VH/zsc5HQ0i1xckEzTuv5ZcuiqXm6SNCprbtbfUiVvIMmw+ynHX5cY4/pwY\r\n" \
"o0/9pziSDvT4IL57BlMh49/Uiy7J+iZhIkEniFKnX53cFR/ttm6YpHXL6xQUM3j1\r\n" \
"pR0/B5VVu5eKfl1ai15mFO3MsAi/rum1JvkaaRs19NyXzbeWActEa+S+0XcP8Ocw\r\n" \
"TJcwKeTPTA4ei6CLbOxTiE/RSADqEGvSYhwy2r9l7Ep6Zg5+WK8C3MRQbgcxgEaH\r\n" \
"LdVe3XeRfT4XxFGTkfRc7xHHJGK+31ap+VUQ1AwnfE0JFqku5O9TnKbZOQ==\r\n" \
"-----END CERTIFICATE-----\r\n"
// "-----BEGIN CERTIFICATE-----\r\n"  \
// "MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\r\n"  \
// "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"  \
// "d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\r\n"  \
// "ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\r\n"  \
// "MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\r\n"  \
// "LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\r\n"  \
// "YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\r\n"  \
// "4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\r\n"  \
// "Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\r\n"  \
// "itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\r\n"  \
// "4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\r\n"  \
// "sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\r\n"  \
// "bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\r\n"  \
// "MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\r\n"  \
// "NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\r\n"  \
// "dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\r\n"  \
// "L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\r\n"  \
// "BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\r\n"  \
// "UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\r\n"  \
// "aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\r\n"  \
// "aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\r\n"  \
// "E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\r\n"  \
// "/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\r\n"  \
// "xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\r\n"  \
// "0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\r\n"  \
// "cPUeybQ=\r\n"  \
// "-----END CERTIFICATE-----\r\n"  



static const char baiduie_cer[] = BAIDUIE_CER;

log_create_module(http_client_get_example, PRINT_LEVEL_INFO);

log_create_module(freertos, PRINT_LEVEL_INFO);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
static void vTestTask(void *pvParameters);

#define mainCHECK_DELAY ( ( portTickType ) 1000 / portTICK_RATE_MS )

/**
* @brief       Task main function
* @param[in]   pvParameters: Pointer that will be used as the parameter for the task being created.
* @return      None.
*/
static void vTestTask(void *pvParameters)
{
    uint32_t idx = (int)pvParameters;
    portTickType xLastExecutionTime, xDelayTime;

    xLastExecutionTime = xTaskGetTickCount();
    xDelayTime = (1 << idx) * mainCHECK_DELAY;

    while (1) {
        vTaskDelayUntil(&xLastExecutionTime, xDelayTime);
        LOG_I(freertos, "Hello World from %d\n\r", idx);

        /* This is for auto-test */
        if (idx == 3) {
            LOG_I(freertos, "example project test success.\n\r");
        }
    }
}

void vApplicationIdleHook(void)
{
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
    systemhang_wdt_count = 0;
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
}

/**
  * @brief      Http client "get" method working flow.
  * @param      None
  * @return     None
  */
static void httpclient_test_get(void)
{
    // char *get_url = HTTP_GET_URL;    
    char *post_url = HTTPS_GET_URL;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    char *buf, *header;
    HTTPCLIENT_RESULT ret;
    int val_pos, val_len;
    
    LOG_I(http_client_get_example, "httpclient_test_get()");

    buf = pvPortMalloc(BUF_SIZE);    
    header = pvPortMalloc(BUF_SIZE);
    if (buf == NULL || header == NULL) {        
        LOG_I(http_client_get_example, "memory malloc failed.");
        return;
    }
    
    // Http "get"
    // client_data.header_buf = header;    
    // client_data.header_buf_len = BUF_SIZE;
    // client_data.response_buf = buf;
    // client_data.response_buf_len = BUF_SIZE;    
    // client_data.response_buf[0] = '\0';
    // ret = httpclient_get(&client, get_url, &client_data);
    // if (ret < 0)
    //     goto fail;   
    // LOG_I(http_client_get_example, "data received: %s", client_data.response_buf);

    // // get response header
    // if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-length", &val_pos, &val_len))
    //     LOG_I(http_client_get_example, "Content-length: %.*s", val_len, client_data.header_buf + val_pos);
    
    // Https "get"
    client_data.header_buf = header;    
    client_data.header_buf_len = BUF_SIZE;
    client_data.response_buf = buf;
    client_data.response_buf_len = BUF_SIZE;    
    client_data.response_buf[0] = '\0';    

    // char cer_buf[] ;
    // cer_buf= pvPortMalloc(sizeof(baiduie_cer));  
    // memset(cer_buf, 0, sizeof(baiduie_cer));
    // strcpy(cer_buf, baiduie_cer);

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    client.server_cert = baiduie_cer;
    client.server_cert_len = sizeof(baiduie_cer);
    LOG_I(http_client_get_example, "[http_client_get_example] server_cert_len=%d.\r\n " ,client.server_cert_len);
#endif


    ret = fota_download_by_https(post_url, baiduie_cer, sizeof(baiduie_cer));

    // ret = httpclient_get(&client, post_url, &client_data);  
    // if (ret < 0)
    //     goto fail;        
    // LOG_I(http_client_get_example, "data received: %s", client_data.response_buf);

    // // get response header
    // if(0 == httpclient_get_response_header_value(client_data.header_buf, "Content-length", &val_pos, &val_len))
    //     LOG_I(http_client_get_example, "Content-length: %.*s", val_len, client_data.header_buf + val_pos);

fail:
    // vPortFree(cer_buf);
    vPortFree(buf);
    vPortFree(header);

    // Print final log
    if (ret >= 0)    {
        LOG_I(http_client_get_example, "example project test success.");
        fota_trigger_update();
    }
    else        
        LOG_I(http_client_get_example, "fota_download_by_https fail, reason:%d.", ret);
}



/**
  * @brief      Create a task for http client get example
  * @param[in]  void *args: Not used
  * @return     None
  */
static void app_entry(void *args)
{
    lwip_net_ready();

    // Httpclient "get" feature test
    httpclient_test_get();

    while (1) {
        vTaskDelay(1000 / portTICK_RATE_MS); // release CPU
    }
}

int main(void)
{
    int idx;

    /* Do system initialization, eg: hardware, nvdm and random seed. */
    system_init();

    /* system log initialization.
     * This is the simplest way to initialize system log, that just inputs three NULLs
     * as input arguments. User can use advanved feature of system log along with NVDM.
     * For more details, please refer to the log dev guide under /doc folder or projects
     * under project/mtxxxx_hdk/apps/.
     */
    log_init(NULL, NULL, NULL);

    LOG_I(http_client_get_example, "FreeRTOS Running");

    wifi_config_t config = {0};
    config.opmode = WIFI_MODE_STA_ONLY;
    strcpy((char *)config.sta_config.ssid, WIFI_SSID);
    strcpy((char *)config.sta_config.password, WIFI_PASSWORD);
    config.sta_config.ssid_length = strlen(WIFI_SSID);
    config.sta_config.password_length = strlen(WIFI_PASSWORD);

    /* Initialize wifi stack and register wifi init complete event handler,
        * notes:  the wifi initial process will be implemented and finished while system task scheduler is running.
        */
    wifi_init(&config, NULL);

    /* Tcpip stack and net interface initialization,  dhcp client, dhcp server process initialization. */
    lwip_network_init(config.opmode);
    lwip_net_start(config.opmode);

    /* Create 4 tasks*/
    //for (idx = 0; idx < 4; idx++) {
    //    xTaskCreate(vTestTask, FREERTOS_EXAMPLE_TASK_NAME, FREERTOS_EXAMPLE_TASK_STACKSIZE / sizeof(portSTACK_TYPE), (void *)idx, FREERTOS_EXAMPLE_TASK_PRIO, NULL);
    //}

    xTaskCreate(app_entry, APP_TASK_NAME, APP_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, APP_TASK_PRIO, NULL);


    /* Call this function to indicate the system initialize done. */
    SysInitStatus_Set();

    /* Start the scheduler. */
    vTaskStartScheduler();

    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

