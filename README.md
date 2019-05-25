# mt7697_https_fota

***
2019/05/21 Sam's Test vs  philio's result~~~

### 1 First program start

![First program start](results-img/1%20program%20start.png "First program start")

```diff
- **在這邊有個嚴重的問題**
- 你們的程式說
- [I: fota_dl_api : fota_download_by_https : 158 ][FOTA DL] url length: 76
- [I: fota_dl_api : fota_download_by_https : 179 ][FOTA DL] server_cert_len=1717.
  但我的程式說
+ [I: fota_dl_api : fota_download_by_https : 158 ][FOTA DL] url length: 37
+ [I: fota_dl_api : fota_download_by_https : 179 ][FOTA DL] server_cert_len=1373.  
```
我去計算了一下，應該是我的才對喔，請檢查一下你們的程式
大小應該是URL "37"   和  CERT LEN "1373"
***
### 2 svr_cert varification
![svr_cert varification](results-img/2%20svr_cert%20varification.png "svr_cert varification")

這邊可以看到使用SSL連線成功了
***
### 3 download successful
![download successful](results-img/3%20download%20successful.png "download successful")

這邊可以看到下載成功
***
### 4 successful
![Total successful](results-img/4%20successful.png "Total successful")

最後程式回傳成功的結果

***
### feature.mk

Please add following feature

MTK_MBEDTLS_CONFIG_FILE = config-mtk-websocket.h
MTK_MBEDTLS_CONFIG_FILE             = config-mtk-websocket.h
MTK_HTTPCLIENT_SSL_ENABLE           = y
MTK_NVDM_ENABLE                     = y
MTK_FOTA_ENABLE	                    = y

### xfota_download_interface.h

Put the **xfota_download_interface.h** into **inc** folder

### xfota_download_interface.c

Put the **xfota_download_interface.c** into **src** folder

### Add the module in the makefile

add the following code in the **GCC/Makefile** for build module
```
#include mbedtls
include $(SOURCE_DIR)/middleware/third_party/mbedtls/module.mk

#fota_module
include $(SOURCE_DIR)/middleware/MTK/fota/module.mk

#tftp_module
include $(SOURCE_DIR)/middleware/MTK/tftp/module.mk
```
add the **APP_FILES**

```
APP_FILES += $(APP_PATH_SRC)/xfota_download_interface.c
```

### how to use it

add header file in **main.c**

```
#include "httpclient.h"
#include "xfota_download_interface.h"
#include "fota.h"
```

add url and ssl in **main.c**

for example

```
#define HTTPS_GET_URL   "https://raw.githubusercontent.com/pc0808f/mt7697_https_fota/master/image.bin"
#define BAIDUIE_CER                                                     \
"-----BEGIN CERTIFICATE-----\r\n"  \
"MIIEsTCCA5mgAwIBAgIQBOHnpNxc8vNtwCtCuF0VnzANBgkqhkiG9w0BAQsFADBs\r\n"  \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\r\n"  \
"d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j\r\n"  \
"ZSBFViBSb290IENBMB4XDTEzMTAyMjEyMDAwMFoXDTI4MTAyMjEyMDAwMFowcDEL\r\n"  \
"MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3\r\n"  \
"LmRpZ2ljZXJ0LmNvbTEvMC0GA1UEAxMmRGlnaUNlcnQgU0hBMiBIaWdoIEFzc3Vy\r\n"  \
"YW5jZSBTZXJ2ZXIgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQC2\r\n"  \
"4C/CJAbIbQRf1+8KZAayfSImZRauQkCbztyfn3YHPsMwVYcZuU+UDlqUH1VWtMIC\r\n"  \
"Kq/QmO4LQNfE0DtyyBSe75CxEamu0si4QzrZCwvV1ZX1QK/IHe1NnF9Xt4ZQaJn1\r\n"  \
"itrSxwUfqJfJ3KSxgoQtxq2lnMcZgqaFD15EWCo3j/018QsIJzJa9buLnqS9UdAn\r\n"  \
"4t07QjOjBSjEuyjMmqwrIw14xnvmXnG3Sj4I+4G3FhahnSMSTeXXkgisdaScus0X\r\n"  \
"sh5ENWV/UyU50RwKmmMbGZJ0aAo3wsJSSMs5WqK24V3B3aAguCGikyZvFEohQcft\r\n"  \
"bZvySC/zA/WiaJJTL17jAgMBAAGjggFJMIIBRTASBgNVHRMBAf8ECDAGAQH/AgEA\r\n"  \
"MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIw\r\n"  \
"NAYIKwYBBQUHAQEEKDAmMCQGCCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2Vy\r\n"  \
"dC5jb20wSwYDVR0fBEQwQjBAoD6gPIY6aHR0cDovL2NybDQuZGlnaWNlcnQuY29t\r\n"  \
"L0RpZ2lDZXJ0SGlnaEFzc3VyYW5jZUVWUm9vdENBLmNybDA9BgNVHSAENjA0MDIG\r\n"  \
"BFUdIAAwKjAoBggrBgEFBQcCARYcaHR0cHM6Ly93d3cuZGlnaWNlcnQuY29tL0NQ\r\n"  \
"UzAdBgNVHQ4EFgQUUWj/kK8CB3U8zNllZGKiErhZcjswHwYDVR0jBBgwFoAUsT7D\r\n"  \
"aQP4v0cB1JgmGggC72NkK8MwDQYJKoZIhvcNAQELBQADggEBABiKlYkD5m3fXPwd\r\n"  \
"aOpKj4PWUS+Na0QWnqxj9dJubISZi6qBcYRb7TROsLd5kinMLYBq8I4g4Xmk/gNH\r\n"  \
"E+r1hspZcX30BJZr01lYPf7TMSVcGDiEo+afgv2MW5gxTs14nhr9hctJqvIni5ly\r\n"  \
"/D6q1UEL2tU2ob8cbkdJf17ZSHwD2f2LSaCYJkJA69aSEaRkCldUxPUd1gJea6zu\r\n"  \
"xICaEnL6VpPX/78whQYwvwt/Tv9XBZ0k7YXDK/umdaisLRbvfXknsuvCnQsH6qqF\r\n"  \
"0wGjIChBWUMo0oHjqvbsezt3tkBigAVBRQHvFwY+3sAzm2fTYS5yh+Rp/BIAV0Ae\r\n"  \
"cPUeybQ=\r\n"  \
"-----END CERTIFICATE-----\r\n" 


static const char baiduie_cer[] = BAIDUIE_CER
```

The API function

fota_download_by_https(char *url, const char server_cert[],int server_cert_len)

The function will download the FOTA image @ url with SSL which is https method.

example

```
ret = fota_download_by_https(post_url, baiduie_cer, sizeof(baiduie_cer));
```

Use **fota_trigger_update()** if you want to trigger the FOTA
