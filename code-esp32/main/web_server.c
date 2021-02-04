/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "esp_err.h"
#include "esp_log.h"

#include "esp_http_server.h"
#include "measure.h"
#include "ina226.h"
#include "speech.h"



static const char *TAG = "file_server";

/* Handler to redirect incoming GET request for /index.html to /
 * This can be overridden by uploading file with same name */
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

/* Handler to respond with an icon file embedded in flash.
 * Browsers expect to GET website icon at URI /favicon.ico.
 * This can be overridden by uploading file with same name */
static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[]   asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return ESP_OK;
}


/* Copies the full path into destination buffer and returns
 * pointer to path (skipping the preceding base path) */
static const char* get_path_from_uri(char *dest, const char *base_path, const char *uri, size_t destsize)
{
    const size_t base_pathlen = strlen(base_path);
    size_t pathlen = strlen(uri);

    // filter parameter
    const char *quest = strchr(uri, '?');
    if (quest) {
        pathlen = MIN(pathlen, quest - uri);
    }
    const char *hash = strchr(uri, '#');
    if (hash) {
        pathlen = MIN(pathlen, hash - uri);
    }

    if (base_pathlen + pathlen + 1 > destsize) {
        /* Full path string won't fit into destination buffer */
        return NULL;
    }

    /* Construct full path (base + path) */
    strcpy(dest, base_path);
    strlcpy(dest + base_pathlen, uri, pathlen + 1);

    /* Return pointer to path, skipping the base */
    return dest + base_pathlen;
}

static esp_err_t get_adc_handler(httpd_req_t *req)
{
    char str[32]={0};
    // float vol = ina226_readBusVoltage();
    float curr = ina226_readShuntVoltage()*10.0f;
    sprintf(str, "%.4f", curr*0.9262f);
    httpd_resp_set_status(req, HTTPD_200);
    httpd_resp_sendstr(req, str);  // Response body can be empty
    return ESP_OK;
}

/* Handler to download a file kept on the server */
static esp_err_t download_get_handler(httpd_req_t *req)
{
    char filepath[256];
    // ESP_LOGW(TAG, "download_get_handler\nuri=%s", req->uri);

    const char *filename = get_path_from_uri(filepath, "/base",
                                             req->uri, sizeof(filepath));
    if (!filename) {
        ESP_LOGE(TAG, "Filename is too long");
        /* Respond with 500 Internal Server Error */
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Filename too long");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "filename=%s", filename);

    if (strstr(filename, "getadc"))
    {
        return get_adc_handler(req);
    }

    else if (strstr(filename, "switch_mode"))
    {
        int32_t buf_len = httpd_req_get_hdr_value_len(req, "measure-mode") + 1;
        if (buf_len > 1) {
            if (httpd_req_get_hdr_value_str(req, "measure-mode", filepath, buf_len) == ESP_OK) {
                ESP_LOGI(TAG, "Found header => measure-mode: %s", filepath);
                if(strstr(filepath, "vol")){
                    // speech_start()
                }
            }
        }
        return httpd_resp_sendstr(req, filepath);
    }

    /* If name has trailing '/', respond with directory contents */
    else if (filename[strlen(filename) - 1] == '/') {
        /* Get handle to embedded file upload script */
        extern const unsigned char upload_script_start[] asm("_binary_index_html_start");
        extern const unsigned char upload_script_end[]   asm("_binary_index_html_end");
        const size_t upload_script_size = (upload_script_end - upload_script_start);

        /* Add file upload form and script which on execution sends a POST request to /upload */
        return httpd_resp_send(req, (const char *)upload_script_start, upload_script_size);
    }else if (strcmp(filename, "/index.html") == 0) {
        return index_html_get_handler(req);
    } else if (strcmp(filename, "/favicon.ico") == 0) {
        return favicon_get_handler(req);
    } else if (strcmp(filename, "/index.js") == 0) {
        extern const unsigned char js_start[] asm("_binary_index_js_start");
        extern const unsigned char js_end[]   asm("_binary_index_js_end");
        const size_t js_size = (js_end - js_start);
        return httpd_resp_send(req, (const char *)js_start, js_size);
    } else if (strcmp(filename, "/settings.html") == 0) {
        extern const unsigned char script_start[] asm("_binary_settings_html_start");
        extern const unsigned char script_end[]   asm("_binary_settings_html_end");
        const size_t script_size = (script_end - script_start);
        return httpd_resp_send(req, (const char *)script_start, script_size);
    }

    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "unsupport uri");
    return ESP_FAIL;
}

/* Function to start the file server */
httpd_handle_t start_web_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    /* Use the URI wildcard matching function in order to
     * allow the same handler to respond to multiple different
     * target URIs which match the wildcard scheme */
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start file server!");
        return NULL;
    }

    /* URI handler for getting uploaded files */
    httpd_uri_t file_download = {
        .uri       = "/*",  // Match all URIs of type /path/to/file
        .method    = HTTP_GET,
        .handler   = download_get_handler,
        .user_ctx  = NULL    // Pass server data as context
    };
    httpd_register_uri_handler(server, &file_download);

    return server;
}
